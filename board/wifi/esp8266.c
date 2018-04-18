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
#include "esp8266.h"
#include "serial.h"
#include "global.h"
#include "trace.h"
#include "pinconfig.h"
#include "dbgserial.h"

#undef __TRACE_MODULE
#define __TRACE_MODULE "[esp8266]"

#define _PRINT_DETAIL 

/* esp8266 work in block mode */
static struct
{
    const char *status_str;
    uint8_t code;
}status_code[] = 
{
    {"OK", ESP_ERR_OK},
    {"FAIL", ESP_ERR_FAIL},
    {"ERROR", ESP_ERR_FAIL},
    {"ALREADY CONNECTED", ESP_ERR_ALREADY},
    {"SEND OK", ESP_ERR_OK},
};

typedef enum
{
    mode_at,
    mode_tcp_head,
    mode_tcp_data,
}work_mode;

static work_mode g_curmode = mode_at;

/* serial handle */
serial *g_serial = NULL;

/* recive queue */
static xQueueHandle xStatusQueue = NULL;
static xQueueHandle xInTcpQueue = NULL;
static xQueueHandle xOutTcpQueue = NULL;
static xQueueHandle xAtQueue = NULL;

#define ESP_MAX_NODE_NUM              (4)
#define ESP_MAX_MSG_SIZE_PER_LINE     (64)
#define ESP_MAX_CONNECT_NUM           (5)

typedef struct
{
    uint16_t link_id;
    uint16_t size;
    char data[ESP_MAX_MSG_SIZE_PER_LINE];
}tcp_node;


/* connect information */
typedef struct
{
    bool is_valid;
    uint8_t direction;
    uint16_t id;
    bool is_working;
}connect_info;

/* support at most 10 connects */
connect_info connects[10];

/* timeout time(ms) */
#define DEFAULT_TIMEOUT      (3000 / portTICK_PERIOD_MS)


/**
 * @brief add connect information to list
 * @param direction - connect direction
 * @param id - connect id
 */
static void add_connect(esp8266_condir direction, uint16_t id)
{
    uint8_t count = sizeof(connects) / sizeof(connects[0]);
    for (int i = 0; i < count; ++i)
    {
        if (!connects[i].is_valid)
        {
            connects[i].is_valid = TRUE;
            connects[i].direction = direction;
            connects[i].id = id;
            connects[i].is_working = FALSE;
            break;
        }
    }
}

/**
 * @brief remove connect from list
 * @param id - connect id
 */
static void remove_connect(uint16_t id)
{
    uint8_t count = sizeof(connects) / sizeof(connects[0]);
    for (int i = 0; i < count; ++i)
    {
        if (connects[i].is_valid && (connects[i].id == id))
        {
            connects[i].is_valid = FALSE;
            break;
        }
    }
}

/**
 * @brief set id working
 * @param id - connect id
 */
static void set_id_working(uint16_t id)
{
    uint8_t count = sizeof(connects) / sizeof(connects[0]);
    for (int i = 0; i < count; ++i)
    {
        if (connects[i].id == id)
        {
            connects[i].is_working = TRUE;
            break;
        }
    }
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
            status = ESP_ERR_OK;
            xQueueSend(xStatusQueue, &status, 0);
            return TRUE;
        }
    }

    return FALSE;
}

/**
 * @brief parse id from data
 * @param data - data to parse
 */
static uint16_t parse_id(const char *data)
{
    const char *pdata = data;
    uint16_t val = 0;
    while (',' != *pdata)
    {
        val *= 10;
        val += (*pdata - '0'); 
    }
    
    return val;
}

/**
 * @brief process connect
 * @param data - data to process
 * @param len - data length
 */
static bool try_process_connect(const char *data, uint8_t len)
{
    /* find ',' first */
    const char *pdata = data;
    uint16_t id = 0;
    while (('\0' != *pdata) && (',' != *pdata))
    {
        pdata ++;
    }

    if (*pdata != '\0')
    {
        pdata++;
        if (0 == strncmp(pdata, "CONNECT", 7))
        {
            id = parse_id(data);
            add_connect(in, id);
            return TRUE;
        }
        else if (0 == strncmp(pdata, "CLOSED", 6))
        {
            id = parse_id(data);
            remove_connect(id);
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
process_func process_funcs[4] = 
{
    try_process_status,
    try_process_connect,
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
int process_at_data(const char *data, uint8_t len)
{
    if (len >= 4)
    {
        if (0 == strncmp(data, "+IPD", 4))
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
 * @param id - tcp link id
 * @param length - tcp data length
 */
int process_tcp_head(const char *data, uint8_t len, uint16_t *id, 
                     uint16_t *length)
{
    assert_param(NULL != id);
    assert_param(NULL != length);
    bool calc_id = FALSE;
    bool calc_len = FALSE;
    uint16_t val = 0;
    if (':' == data[len - 1])
    {
        const char *pdata = data + 1;
        calc_id = TRUE;
        for (int i = 0; i < len - 1; ++i)
        {
           if (calc_id)
           {
               if (',' == pdata[i])
               {
                   calc_id = FALSE;
                   calc_len = TRUE;
                   *id = val;
                   val = 0;
               }
               else
               {
                   val *= 10;
                   val += (pdata[i] - '0');
               }
           }
           else if (calc_len)
           {
               if (':' == pdata[i])
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
                   val += (pdata[i] - '0');
               }
           }
        }
    }

    return 0;
}

/**
 * @brief process reveived tcp data
 * @param id - link id
 * @param data - data buffer
 * @param len - data length
 */
static int process_tcp_data(uint16_t id, char *data, uint16_t len)
{
    tcp_node node;
    node.link_id = id;
    node.size = len;
    strncpy(node.data, data, len);
    xQueueSend(xTcpQueue, &node, 0);
    return 0;
}

/**
 * @brief esp8266 data process task
 * @param serial handle
 */
static void vESP8266Response(void *pvParameters)
{
    serial *pserial = pvParameters;
    char node_data[ESP_MAX_MSG_SIZE_PER_LINE];
    uint8_t node_size = 0;
    char *pData = node_data;
    uint16_t link_id = 0;
    uint16_t tcp_size = 0;
    char data;
    TickType_t xDelay = 50 / portTICK_PERIOD_MS;
    for (;;)
    {
        if (serial_getchar(pserial, &data, portMAX_DELAY))
        {
            g_curmode = mode_at;
            link_id = 0;
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
                    if (process_tcp_head(node_data, node_size, &link_id, 
                                         &tcp_size) > 0)
                    {
                        set_id_working(link_id);
                        pData = node_data;
                        node_size = 0;
                    }
                    break;
                case mode_tcp_data:
                    tcp_size --;
                    if (0 == tcp_size)
                    {
                        process_tcp_data(link_id, node_data, node_size);
                        pData = node_data;
                        node_size = 0;
                        /* reset mode */
                        g_curmode = mode_at;
                    }
                    else
                    {
                        if (node_size >= ESP_MAX_MSG_SIZE_PER_LINE)
                        {
                            process_tcp_data(link_id, node_data, node_size);
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
bool esp8266_init(void)
{
    TRACE("initialize esp8266...\n");
    pin_reset("WIFI_EN");
    vTaskDelay(100 / portTICK_PERIOD_MS);
    pin_set("WIFI_EN");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    g_serial = serial_request(COM2);
    if (NULL == g_serial)
    {
        TRACE("initialize failed, can't open serial \'COM2\'\n");
        return FALSE;
    }
    serial_open(g_serial);

    xStatusQueue = xQueueCreate(ESP_MAX_NODE_NUM, ESP_MAX_NODE_NUM);
    xAtQueue = xQueueCreate(ESP_MAX_NODE_NUM, ESP_MAX_MSG_SIZE_PER_LINE);
    xInTcpQueue = xQueueCreate(ESP_MAX_NODE_NUM * 2, 
                             sizeof(tcp_node) / sizeof(char));
    xOutTcpQueue = xQueueCreate(ESP_MAX_NODE_NUM * 2, 
                             sizeof(tcp_node) / sizeof(char));

    if ((NULL == xStatusQueue) || (NULL == xAtQueue) || (NULL == xTcpQueue))
    {
        TRACE("initialize failed, can't create queue\'COM2\'\n");
        serial_release(g_serial);
        g_serial = NULL;
        return FALSE;
    }
    
    xTaskCreate(vESP8266Response, "ESP8266Response", ESP8266_STACK_SIZE, 
            g_serial, ESP8266_PRIORITY, NULL);
     
    return TRUE;
}

/**
 * @brief test esp8266 module
 * @param cmd - cmd to send
 * @param time - timeout time
 * @return 0 means connect success, otherwise failed
 */
int esp8266_send_ok(const char *cmd, TickType_t time)
{
    assert_param(NULL != g_serial);

    uint8_t status;
    send_at_cmd(cmd, strlen(cmd));
    int ret = ESP_ERR_OK;

    if (pdPASS == xQueueReceive(xStatusQueue, &status, time))
    {
        ret = -status;
    }
    else
    {
        ret = -ESP_ERR_TIMEOUT;
    }

    if (0 != ret)
    {
        TRACE("status: %d\n", -ret);
    }
    return ret;
}

/**
 * @brief write data
 * @param data - data
 * @param length - data length
 */
int esp8266_write(const char *data, uint32_t length, TickType_t time)
{
    uint8_t status;
    serial_putstring(g_serial, data, length);
    int ret = ESP_ERR_OK;

    if (pdPASS == xQueueReceive(xStatusQueue, &status, time))
    {
        ret = -status;
    }
    else
    {
        ret = -ESP_ERR_TIMEOUT;
    }

    if (0 != ret)
    {
        TRACE("status: %d\n", -ret);
    }
    return ret;
}

/**
 * @brief set esp8266 work mode
 * @param mode - work mode
 * @param time - timeout time
 * @return 0 means connect success, otherwise failed
 */
int esp8266_setmode(esp8266_mode mode, TickType_t time)
{
    assert_param(NULL != g_serial);

    char str_mode[16];
    sprintf(str_mode, "AT+CWMODE_CUR=%d\r\n", mode);
    return esp8266_send_ok(str_mode, time);
}

/**
 * @brief get esp8266 mode
 * @param time - timeout time
 * @return esp8266 current work mode
 */
esp8266_mode esp8266_getmode(TickType_t time)
{
    assert_param(NULL != g_serial);

    uint8_t status;
    uint8_t buf[ESP_MAX_MSG_SIZE_PER_LINE];
    buf[0] = UNKNOWN + '0';
    send_at_cmd("AT+CWMODE_CUR?\r\n", 16);
    esp8266_mode mode = UNKNOWN;

    if (pdPASS == xQueueReceive(xStatusQueue, &status, time))
    {
        if (ESP_ERR_OK == status)
        {
            xQueueReceive(xAtQueue, buf, 0);
            mode = (esp8266_mode)(buf[0] - '0');
        }
    }

    TRACE("mode: %d\n", mode);
    return mode;
}

/**
 * @brief connect ap
 * @param ssid - ap ssid
 * @param pwd - ap password
 * @param time - connect timeout time
 * @return 0 means connect success, otherwise failed
 */
int esp8266_connect_ap(const char *ssid, const char *pwd, TickType_t time)
{
    assert_param(NULL != g_serial);

    char str_mode[64];
    sprintf(str_mode, "AT+CWJAP_CUR=\"%s\",\"%s\"\r\n", ssid, pwd);
    send_at_cmd(str_mode, strlen(str_mode));
    int ret = ESP_ERR_OK;

    uint8_t status;
    uint8_t buf[ESP_MAX_MSG_SIZE_PER_LINE];
    buf[0] = ESP_ERR_FAIL + '0';
    if (pdPASS == xQueueReceive(xStatusQueue, &status, time))
    {
        if (ESP_ERR_OK != status)
        {
            ret = -ESP_ERR_FAIL;
            xQueueReceive(xAtQueue, buf, 0);
            for (int i = 0; i < ESP_MAX_MSG_SIZE_PER_LINE; ++i)
            {
                if (':' == buf[i])
                {
                    ret = -(buf[i + 1] - '0');
                }
            }
        }
    }
    else
    {
        ret = -ESP_ERR_TIMEOUT;
    }

    if (0 != ret)
    {
        TRACE("status: %d\n", -ret);
    }
    return ret;
}

/**
 * @brief disconnect ap
 * @param time - disconnect timeout time
 */
void esp8266_disconnect_ap(TickType_t time)
{
    assert_param(NULL != g_serial);

    esp8266_send_ok("AT+CWQAP\r\n", time);
}

/**
 * @brief set software ap parameter
 * @param ssid - ap ssid
 * @param pwd - ap password
 * @param chl - ap channel
 * @param ecn - password encode type
 * @return 0 means connect success, otherwise failed
 */
int esp8266_set_softap(const char *ssid, const char *pwd, uint8_t chl, 
                       esp8266_ecn ecn, TickType_t time)
{
    assert_param(NULL != g_serial);

    char str_mode[64];
    sprintf(str_mode, "AT+CWSAP_CUR=\"%s\",\"%s\",%d,%d\r\n", 
            ssid, pwd, chl, ecn);
    
    return esp8266_send_ok(str_mode, time);
}

/**
 * @brief set software ap address
 * @param ssid - ap ssid
 * @param pwd - ap password
 * @param chl - ap channel
 * @param ecn - password encode type
 * @return 0 means connect success, otherwise failed
 */
int esp8266_set_apaddr(const char *ip, const char *gateway, const char *netmask,
                       TickType_t time)
{
    assert_param(NULL != g_serial);

    char str_mode[64];
    sprintf(str_mode, "AT+CIPAP_CUR=\"%s\",\"%s\",\"%s\"\r\n", 
            ip, gateway, netmask);
    
    return esp8266_send_ok(str_mode, time);
}

/**
 * @brief connect remote server
 * @param mode - connect mode
 * @param ip - remote ip address
 * @param port - remote port
 * @param time - timeout time
 */
int esp8266_connect(uint16_t id, const char *mode, const char *ip, uint16_t port,
                    TickType_t time)
{
    assert_param(NULL != g_serial);

    char str_mode[64];
    sprintf(str_mode, "AT+CIPSTART=%d,\"%s\",\"%s\",%d\r\n", id, mode, ip, port);
    
    int ret = esp8266_send_ok(str_mode, time);
    if (ESP_ERR_OK == ret)
    {
        add_connect(out, id);
        set_id_working(id);
    }
}

/**
 * @brief set esp8266 transport mode
 * @param mode - transport mode
 * @param time - timeout time
 * @return error code
 */
int esp8266_set_transmode(esp8266_transmode mode, TickType_t time)
{
    assert_param(NULL != g_serial);
    
    char str_mode[16];
    sprintf(str_mode, "AT+CIPMODE=%d\r\n", mode);

    return esp8266_send_ok(str_mode, time);
}

/**
 * @brief create server
 * @param port - listen port
 * @param time - timeout time
 * @return error code
 */
int esp8266_listen(uint16_t port, TickType_t time)
{
    assert_param(NULL != g_serial);
    
    char str_mode[24];
    sprintf(str_mode, "AT+CIPSERVER=1,%d\r\n", port);

    return esp8266_send_ok(str_mode, time);
}

/**
 * @brief close server
 * @param time - timeout time
 * @return error code
 */
int esp8266_close(uint16_t port, TickType_t time)
{
    assert_param(NULL != g_serial);
    
    char str_mode[24];
    sprintf(str_mode, "AT+CIPSERVER=0,%d\r\n", port);

    return esp8266_send_ok(str_mode, time);
}

/**
 * @brief get tcp data
 * @param data - tcp data
 * @param len - data length
 * @param xBlockTime - timeout time
 */
int esp8266_recv(esp8266_condir dir, char *data, uint16_t *len, TickType_t xBlockTime)
{
    assert_param(NULL != xTcpQueue);
    
    tcp_node node;
    xQueueHandle tcp_queue = NULL;
    if (in == dir)
    {
        tcp_queue = xInTcpQueue;
    }
    else
    {
        tcp_queue = xOutTcpQueue;
    }

    if (xQueueReceive(tcp_queue, &node, xBlockTime))
    {
        strncpy(data, node.data, node.size);
        *len = node.size;
        return ESP_ERR_OK;
    }
    else
    {
        return -ESP_ERR_TIMEOUT;
    }
}

/**
 * @brief prepare send tcp data
 * @param chl - connected channel
 * @param length - send length
 * @param time - timeout time
 */
int esp8266_prepare_send(uint16_t chl, uint16_t length, TickType_t time)
{
    char str_mode[20];
    sprintf(str_mode, "AT+CIPSEND=%d,%d\r\n", chl, length);
    
    return esp8266_send_ok(str_mode, time);
}

/**
 * @brief get tcp id
 * @param dir - connect direction
 * @return tcp id
 */
uint16_t esp8266_tcp_id(esp8266_condir dir)
{
    uint8_t count = sizeof(connects) / sizeof(connects[0]);
    for (int i = 0; i < count; ++i)
    {
        if ((connects[i].direction == dir) &&
            connects[i].is_working)
        {
            return connects[i].id;
        }
    }
    return 0xffff;
}

/**
 * @brief disconnect tcp,udp,ssl connection
 * @param id - link id
 * @param time - timeout time
 */
int esp8266_disconnect(uint16_t id, TickType_t time)
{
    char str_mode[22];
    sprintf(str_mode, "AT+CIPCLOSE=%d\r\n", id);
    
    return esp8266_send_ok(str_mode, time);
}

/**
 * @brief set tcp timeout time
 * @param time - timeout time
 * @param time - timeout time
 */
int esp8266_set_tcp_timeout(uint16_t timeout, TickType_t time)
{
    char str_mode[22];
    sprintf(str_mode, "AT+CIPSTO=%d\r\n", timeout);
    
    return esp8266_send_ok(str_mode, time);
}

