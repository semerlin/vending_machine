#ifndef _STM32F10X_MAP_H_
  #define _STM32F10X_MAP_H_

/* register address map */

/* sram and peripheral bit band start address */
#define SRAM_BB_BASE     ((unsigned int)0x22000000)
#define PERIPH_BB_BASE   ((unsigned int)0x42000000)

/* sram and peripheral start address */
#define SRAM_BASE        ((unsigned int)0x20000000)
#define PERIPH_BASE      ((unsigned int)0x40000000)


/* APB1,APB2,AHB bus base address */
#define APB1PHERIP_BASE  PERIPH_BASE
#define APB2PHERIP_BASE  (PERIPH_BASE + 0x10000)
#define AHBPHERIP_BASE   (PERIPH_BASE + 0x18000)

/* APB1 bus peripheral address */
#define TIM2_BASE        (APB1PHERIP_BASE + 0x0000)
#define TIM3_BASE        (APB1PHERIP_BASE + 0x0400)
#define TIM4_BASE        (APB1PHERIP_BASE + 0x0800)
#define TIM5_BASE        (APB1PHERIP_BASE + 0x0C00)
#define TIM6_BASE        (APB1PHERIP_BASE + 0x1000)
#define TIM7_BASE        (APB1PHERIP_BASE + 0x1400)
#define RTC_BASE         (APB1PHERIP_BASE + 0x2800)
#define WWDG_BASE        (APB1PHERIP_BASE + 0x2C00)
#define IWDG_BASE        (APB1PHERIP_BASE + 0x3000)
#define SPI2_BASE        (APB1PHERIP_BASE + 0x3800)
#define SPI3_BASE        (APB1PHERIP_BASE + 0x3C00)
#define USART2_BASE      (APB1PHERIP_BASE + 0x4400)
#define USART3_BASE      (APB1PHERIP_BASE + 0x4800)
#define UART4_BASE       (APB1PHERIP_BASE + 0x4C00)
#define UART5_BASE       (APB1PHERIP_BASE + 0x5000)
#define I2C1_BASE        (APB1PHERIP_BASE + 0x5400)
#define I2C2_BASE        (APB1PHERIP_BASE + 0x5800)
#define USB_BASE         (APB1PHERIP_BASE + 0x5C00)
#define CAN_BASE         (APB1PHERIP_BASE + 0x6000)
#define BxCAN1_BASE      (APB1PHERIP_BASE + 0x6400)
#define BxCAN2_BASE      (APB1PHERIP_BASE + 0x6800)
#define BKP_BASE         (APB1PHERIP_BASE + 0x6C00)
#define PWR_BASE         (APB1PHERIP_BASE + 0x7000)
#define DAC_BASE         (APB1PHERIP_BASE + 0x7400)


/* APB2 bus peripheral address */
#define AFIO_BASE        (APB2PHERIP_BASE + 0x0000)
#define EXIT_BASE        (APB2PHERIP_BASE + 0x0400)
#define GPIOA_BASE       (APB2PHERIP_BASE + 0x0800)
#define GPIOB_BASE       (APB2PHERIP_BASE + 0x0C00)
#define GPIOC_BASE       (APB2PHERIP_BASE + 0x1000)
#define GPIOD_BASE       (APB2PHERIP_BASE + 0x1400)
#define GPIOE_BASE       (APB2PHERIP_BASE + 0x1800)
#define GPIOF_BASE       (APB2PHERIP_BASE + 0x1C00)
#define GPIOG_BASE       (APB2PHERIP_BASE + 0x2000)
#define ADC1_BASE        (APB2PHERIP_BASE + 0x2400)
#define ADC2_BASE        (APB2PHERIP_BASE + 0x2800)
#define TIM1_BASE        (APB2PHERIP_BASE + 0x2C00)
#define SPI1_BASE        (APB2PHERIP_BASE + 0x3000)
#define TIM8_BASE        (APB2PHERIP_BASE + 0x3400)
#define USART1_BASE      (APB2PHERIP_BASE + 0x3800)
#define ADC3_BASE        (APB2PHERIP_BASE + 0x3C00)


/* AHB bus peripheral address */
#define SDIO_BASE        (AHBPHERIP_BASE + 0x0000)
#define DMA1_BASE        (AHBPHERIP_BASE + 0x8000)
#define DMA2_BASE        (AHBPHERIP_BASE + 0x8400)
#define RCC_BASE         (AHBPHERIP_BASE + 0x9000)
#define FLASH_BASE       (AHBPHERIP_BASE + 0xA000)
#define CRC_BASE         (AHBPHERIP_BASE + 0xB000)
#define NET_BASE         (AHBPHERIP_BASE + 0x10000)
#define OTG_BASE         (AHBPHERIP_BASE + 0xFFE8000)


/* scb address */
#define SCB_BASE         (0xE000ED00)
#define NVIC_BASE        (0xE000E100)
#define SYSTICK_BASE     (0xE000E010)




#endif
