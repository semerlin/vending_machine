/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "m26.h"
#include "serial.h"
#include "global.h"
#include "trace.h"
#include "pinconfig.h"
#include "dbgserial.h"

#undef __TRACE_MODULE
#define __TRACE_MODULE "[m26]"

#define _PRINT_DETAIL 


/* mqtt driver */
static m26_driver g_driver;

TaskHandle_t task_m26 = NULL;

/* esp8266 work in block mode */
static struct
{
    const char *status_str;
    uint8_t code;
}status_code[] = 
{
    {"OK", M26_ERR_OK},
    {"ERROR", M26_ERR_FAIL},
    {"ALREADY CONNECT", M26_ERR_ALREADY},
    {"SEND OK", M26_ERR_OK},
    {"CLOSE OK", M26_ERR_OK},
};

static struct
{
    const char *pin_str;
    uint8_t code;
}pin_code[] = 
{
    {"READY", M26_PIN_READY},
    {"SIM PIN", M26_SIM_PIN},
    {"SIM PUK", M26_SIM_PUK},
    {"PH_SIM PIN", M26_PH_SIM_PIN},
    {"PH_SIM PUK", M26_PH_SIM_PUK},
    {"SIM PIN2", M26_SIM_PIN2},
    {"SIM PUK2", M26_SIM_PUK2},
};



typedef enum
{
    mode_at,
    mode_tcp_head,
    mode_tcp_data,
}work_mode;

static work_mode g_curmode = mode_at;

/* serial handle */
static serial *g_serial = NULL;

/* recive queue */
static xQueueHandle xStatusQueue = NULL;
static xQueueHandle xTcpQueue = NULL;
static xQueueHandle xAtQueue = NULL;
static xQueueHandle xSyncQueue = NULL;

#define M26_MAX_NODE_NUM              (6)
#define M26_MAX_MSG_SIZE_PER_LINE     (64)
#define M26_MAX_CONNECT_NUM           (5)

typedef struct
{
    uint16_t size;
    char data[M26_MAX_MSG_SIZE_PER_LINE];
}tcp_node;

/* timeout time(ms) */
#define DEFAULT_TIMEOUT      (3000 / portTICK_PERIOD_MS)


/**
 * @brief m26 net register callback
 */
static void m26_net_register(uint8_t code)
{
    UNUSED(code);
}

/**
 * @brief m26 gprs attach callback
 */
static void m26_gprs_attach(uint8_t code)
{
    UNUSED(code);
}

/**
 * @brief m26 server connect callback
 */
static void m26_server_connect(void)
{
}

/**
 * @brief m26 server disconnect callback
 */
static void m26_server_disconnect(void)
{
}

/**
 * @brief initialize esp8266 default driver
 */
static void init_m26_driver(void)
{
    g_driver.net_register = m26_net_register;
    g_driver.gprs_attach = m26_gprs_attach;
    g_driver.server_connect = m26_server_connect;
    g_driver.server_disconnect = m26_server_disconnect;
}

/**
 * @brief refresh driver
 */
static void refresh_driver(void)
{
    if (NULL == g_driver.net_register)
    {
        g_driver.net_register = m26_net_register;
    }

    if (NULL == g_driver.gprs_attach)
    {
        g_driver.gprs_attach = m26_gprs_attach;
    }

    if (NULL == g_driver.server_connect)
    {
        g_driver.server_connect = m26_server_connect;
    }

    if (NULL == g_driver.server_disconnect)
    {
        g_driver.server_disconnect = m26_server_disconnect;
    }
}

/**
 * @brief attach mqtt driver
 * @param driver - mqtt driver handle
 */
void m26_attach(const m26_driver *driver)
{
    assert_param(NULL != driver);
    g_driver.net_register = driver->net_register;
    g_driver.gprs_attach = driver->gprs_attach;
    g_driver.server_connect = driver->server_connect;
    g_driver.server_disconnect = driver->server_disconnect;
    refresh_driver();
}

/**
 * @brief detach mqtt driver
 */
void m26_detach(void)
{
    init_m26_driver();
}

/**
 * @brief send at command
 * @param cmd - at command
 * @param length - command length
 */
static void send_at_cmd(const char *cmd, uint32_t length)
{
    xQueueReset(xStatusQueue);
    xQueueReset(xAtQueue);
    TRACE("send: %s", cmd);
    serial_putstring(g_serial, cmd, length);
}

/* process function type define */
typedef bool (*process_func)(const char *data, uint8_t len);

/**
 * @brief process status
 * @param data - data to process
 * @param len - data length
 */
static bool try_process_status(const char *data, uint8_t len)
{
    uint8_t status = 0;
    int count = sizeof(status_code) / (sizeof(status_code[0]));
    for (int i = 0; i < count; ++i)
    {
        if (0 == strncmp(data, status_code[i].status_str, len - 2))
        {
            status = status_code[i].code;
            xQueueSend(xStatusQueue, &status, 0);
            return TRUE;
        }
    }

    return FALSE;
}

/**
 * @brief process server connect
 * @param data - data to process
 * @param len - data length
 */
static bool try_process_server_connect(const char *data, uint8_t len)
{
    if (0 == strncmp(data, "CONNECT OK", len - 2))
    {
        g_driver.server_connect();
        return TRUE;
    }
    else if (0 == strncmp(data, "CLOSED", len - 2))
    {
        g_driver.server_disconnect();
        return TRUE;
    }
    else if (0 == strncmp(data, "+PDP DEACT", len - 2))
    {
        g_driver.server_disconnect();
        return TRUE;
    }

    return FALSE;
}

/**
 * @brief process net register 
 * @param data - data to process
 * @param len - data length
 */
static bool try_process_net_register(const char *data, uint8_t len)
{
    if (0 == strncmp(data, "+CREG:", 6))
    {
        if ((1 == data[6]) || (5 == data[6]))
        {
            TRACE("net registered: %d\r\n", data[6]);
            g_driver.net_register(data[6]);
            return TRUE;
        }
    }

    return FALSE;
}

/**
 * @brief process gprs attach
 * @param data - data to process
 * @param len - data length
 */
static bool try_process_gprs_attach(const char *data, uint8_t len)
{
    if (0 == strncmp(data, "+CGREG:", 7))
    {
        if ((1 == data[7]) || (5 == data[7]))
        {
            TRACE("gprs attached: %d\r\n", data[7]);
            g_driver.gprs_attach(data[7]);
            return TRUE;
        }
    }

    return FALSE;
}

/**
 * @brief process default 
 * @param data - data to process
 * @param len - data length
 */
static bool try_process_default(const char *data, uint8_t len)
{
    /* at command parameter */
    xQueueSend(xAtQueue, data, 0);

    return TRUE;
}

/* process functions list */
static process_func process_funcs[] = 
{
    try_process_status,
    try_process_server_connect,
    try_process_net_register,
    try_process_gprs_attach,
    try_process_default,
    NULL
};

/**
 * @brief process line data
 * @param data - line data
 */
static void process_line(const char *data, uint8_t len)
{
    if (len > 2)
    {
        for (int i = 0; NULL != process_funcs[i]; ++i)
        {
            if (process_funcs[i](data, len))
            {
                break;
            }
        }
    }
}

/**
 * @brief process at data
 * @param data - node data
 * @param len - node length
 */
static int process_at_data(const char *data, uint8_t len)
{
    if (len == 2)
    {
        uint8_t flag = 0;
        if (0 == strncmp(data, "at", 2))
        {
            xQueueSend(xSyncQueue, &flag, 0);
            return 1;
        }
    }
    
    if (len == 3)
    {
        if (0 == strncmp(data, "IPD", 3))
        {
            /* mode changed */
            g_curmode = mode_tcp_head;
            return 1;
        }
    }
                
    if (0 == strncmp(data + len - 2, "\r\n", 2))
    {
        /* get line data */
        process_line(data, len);
        return 1;
    }

    return 0;
}

/**
 * @brief process tcp head
 * @param data - data buffer
 * @param len - data length
 * @param length - tcp data length
 */
static int process_tcp_head(const char *data, uint8_t len, uint16_t *length)
{
    assert_param(NULL != length);
    uint16_t val = 0;
    if (':' == data[len - 1])
    {
        for (int i = 0; i < len - 1; ++i)
        {
            if (':' == data[i])
            {
                *length = val;
                if (0 == *length)
                {
                    g_curmode = mode_at;
                }
                else
                {
                    g_curmode = mode_tcp_data;
                }
                return 1;
            }
            else
            {
                val *= 10;
                val += (data[i] - '0');
            }
        }
    }

    return 0;
}

/**
 * @brief process received tcp data
 * @param data - data buffer
 * @param len - data length
 */
static int process_tcp_data(char *data, uint16_t len)
{
    tcp_node node;
    node.size = len;
    strncpy(node.data, data, len);
    xQueueSend(xTcpQueue, &node, 100 / portTICK_PERIOD_MS);
    
    return 0;
}

/**
 * @brief m26 data process task
 * @param serial handle
 */
static void vM26Response(void *pvParameters)
{
    serial *pserial = pvParameters;
    char node_data[M26_MAX_MSG_SIZE_PER_LINE];
    uint8_t node_size = 0;
    char *pData = node_data;
    uint16_t tcp_size = 0;
    char data;
    TickType_t xDelay = 50 / portTICK_PERIOD_MS;
    for (;;)
    {
        if (serial_getchar(pserial, &data, portMAX_DELAY))
        {
            g_curmode = mode_at;
            tcp_size = 0;
            node_size = 0;
            pData = node_data;
            /* receive data */
            *pData++ = data;
            node_size ++;
#ifdef _PRINT_DETAIL
            dbg_putchar(data);
#endif
            while (serial_getchar(pserial, &data, xDelay))
            {
#ifdef _PRINT_DETAIL
                dbg_putchar(data);
#endif
                /* receive data */
                *pData++ = data;
                node_size ++;
                switch (g_curmode)
                {
                case mode_at:
                    if (process_at_data(node_data, node_size) > 0)
                    {
                        pData = node_data;
                        node_size = 0;
                    }
                    break;
                case mode_tcp_head:
                    if (process_tcp_head(node_data, node_size,
                                         &tcp_size) > 0)
                    {
                        pData = node_data;
                        node_size = 0;
                    }
                    break;
                case mode_tcp_data:
                    tcp_size --;
                    if (0 == tcp_size)
                    {
                        process_tcp_data(node_data, node_size);
                        pData = node_data;
                        node_size = 0;
                        /* reset mode */
                        g_curmode = mode_at;
                    }
                    else
                    {
                        if (node_size >= M26_MAX_MSG_SIZE_PER_LINE)
                        {
                            process_tcp_data(node_data, node_size);
                            pData = node_data;
                            node_size = 0;
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }
}

/**
 * @brief initialize esp8266
 * @return 0 means success, otherwise error code
 */
bool m26_init(void)
{
    TRACE("initialize m26...\r\n");
    pin_reset("GPRS_PWR");
    vTaskDelay(500 / portTICK_PERIOD_MS);
    pin_set("GPRS_PWR");
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    pin_reset("GPRS_PWR");
    
    g_serial = serial_request(COM3);
    if (NULL == g_serial)
    {
        TRACE("initialize failed, can't open serial \'COM2\'\r\n");
        return FALSE;
    }
    serial_open(g_serial);

    xStatusQueue = xQueueCreate(M26_MAX_NODE_NUM, M26_MAX_NODE_NUM);
    xAtQueue = xQueueCreate(M26_MAX_NODE_NUM, M26_MAX_MSG_SIZE_PER_LINE);
    xTcpQueue = xQueueCreate(M26_MAX_NODE_NUM * 2, 
                             sizeof(tcp_node) / sizeof(char));
    xSyncQueue = xQueueCreate(1, 1);

    if ((NULL == xStatusQueue) || 
        (NULL == xAtQueue) || 
        (NULL == xTcpQueue) || 
        (NULL == xStatusQueue))
    {
        TRACE("initialize failed, can't create queue\'COM2\'\r\n");
        serial_release(g_serial);
        g_serial = NULL;
        return FALSE;
    }
    
    xTaskCreate(vM26Response, "M26Response", M26_STACK_SIZE, 
            g_serial, M26_PRIORITY, &task_m26);
     
    return TRUE;
}

/**
 * @brief m26 module send command
 * @param cmd - cmd to send
 * @param time - timeout time
 * @return 0 means connect success, otherwise failed
 */
int m26_send_ok(const char *cmd, TickType_t time)
{
    assert_param(NULL != g_serial);

    uint8_t status;
    send_at_cmd(cmd, strlen(cmd));
    int ret = M26_ERR_OK;

    if (pdPASS == xQueueReceive(xStatusQueue, &status, time))
    {
        ret = -status;
    }
    else
    {
        ret = -M26_ERR_TIMEOUT;
    }

    if (0 != ret)
    {
        TRACE("status: %d\r\n", -ret);
    }
    return ret;
}

/**
 * @brief sync baudrate
 */
int m26_sync(void)
{
    TRACE("sync baudrate\r\n");
    int ret = -M26_ERR_FAIL;
    uint8_t flag = 0;
    for (int i = 0; i < 10; ++i)
    {
        serial_putstring(g_serial, "at", 2);
        if (pdPASS == xQueueReceive(xSyncQueue, &flag, 
                                    500 / portTICK_PERIOD_MS))
        {
            TRACE("sync success\r\n");
            ret = M26_ERR_OK;
            break;
        }
    }
    
    return ret;
}

/**
 * @brief parse pin code status
 * @param pin - pin code string
 * @return pin code status
 */
uint8_t parse_pin_code(const char *pin)
{
    assert_param(NULL != pin);
    const char *pdata = pin;
    while (('\0' != *pdata) && (':' != *pdata))
    {
        pdata ++;
    }

    if (*pdata != '\0')
    {
        pdata++;
        for (uint8_t i = 0; i < sizeof(pin_code) / sizeof(pin_code[0]); ++i)
        {
            if (0 == strcmp(pdata, pin_code[i].pin_str))
            {
                return pin_code[i].code;
            }
        }
    }
    
    return M26_SIM_UNKNOWN;
}

/**
 * @brief get m26 pin code status
 * @param time - timeout time
 * @return pin code status
 */
uint8_t m26_pin_status(TickType_t time)
{
    assert_param(NULL != g_serial);

    uint8_t status;
    uint8_t buf[M26_MAX_MSG_SIZE_PER_LINE];
    send_at_cmd("AT+CPIN?\r\n", 10);
    uint8_t code = M26_SIM_UNKNOWN;

    if (pdPASS == xQueueReceive(xStatusQueue, &status, time))
    {
        if (M26_ERR_OK == status)
        {
            xQueueReceive(xAtQueue, buf, 0);
            code = parse_pin_code((const char *)buf);
        }
    }

    TRACE("code: %d\r\n", code);
    return code;
}

/**
 * @brief connect remote server
 * @param mode - connect mode
 * @param ip - remote ip address
 * @param port - remote port
 * @param time - timeout time
 */
int m26_connect(const char *mode, const char *ip, const char *port,
        TickType_t time)
{
    assert_param(NULL != g_serial);

    char str_mode[64];
    sprintf(str_mode, "AT+QIOPEN=\"%s\",\"%s\",\"%s\"\r\n", mode, ip, port);
    
    return m26_send_ok(str_mode, time);
}

/**
 * @brief prepare send tcp data
 * @param length - send length
 * @param time - timeout time
 */
int m26_prepare_send(uint16_t length, TickType_t time)
{
    char str_mode[20];
    sprintf(str_mode, "AT+QISEND=%d\r\n", length);
    
    return m26_send_ok(str_mode, time);
}

/**
 * @brief write data
 * @param data - data
 * @param length - data length
 */
int m26_write(const char *data, uint32_t length, TickType_t time)
{
    uint8_t status;
    serial_putstring(g_serial, data, length);
    int ret = M26_ERR_OK;

    if (pdPASS == xQueueReceive(xStatusQueue, &status, time))
    {
        ret = -status;
    }
    else
    {
        ret = -M26_ERR_TIMEOUT;
    }

    if (0 != ret)
    {
        TRACE("status: %d\r\n", -ret);
    }
    return ret;
}

/**
 * @brief get tcp data
 * @param data - tcp data
 * @param len - data length
 * @param xBlockTime - timeout time
 */
int m26_recv(char *data, uint16_t *len, TickType_t xBlockTime)
{
    assert_param(NULL != xTcpQueue);
    
    tcp_node node;

    if (xQueueReceive(xTcpQueue, &node, xBlockTime))
    {
        strncpy(data, node.data, node.size);
        *len = node.size;
        return M26_ERR_OK;
    }
    else
    {
        return -M26_ERR_TIMEOUT;
    }
}

/**
 * @brief disconnect tcp,udp,ssl connection
 * @param time - timeout time
 */
int m26_disconnect(TickType_t time)
{
    char str_mode[22];
    sprintf(str_mode, "AT+QICLOSE\r\n");
    
    return m26_send_ok(str_mode, time);
}

/**
 * @brief shutdown m26
 */
void m26_shutdown(void)
{
    if (NULL != task_m26)
    {
        vTaskDelete(task_m26);
        task_m26 = NULL;
    }
}



