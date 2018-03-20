#include "stm32f10x_it.h"
#include "stm32f10x_cfg.h"

/* irq default implementation, user application can replace it*/

/**
 * @brief This function handles NMI exception.
 */
__weak void NMIException(void)
{
}

/**
 * @brief This function handles Hard Fault exception.
 */
__weak void HardFaultException(void)
{
}

/**
 * @brief This function handles Memory Manage exception.
 */
__weak void MemManageException(void)
{
}

/**
 * @brief This function handles Bus Fault exception.
 */
__weak void BusFaultException(void)
{
}

/**
 * @brief This function handles Usage Fault exception.
 */
__weak void UsageFaultException(void)
{
}

/**
 * @brief This function handles Debug Monitor exception.
 */
__weak void DebugMonitor(void)
{
}

/**
 * @brief This function handles SVCall exception.
 */
__weak void SVCHandler(void)
{
}

/**
* @brief This function handles PendSVC exception.
*/
__weak void PendSVHandler(void)
{
}

/**
 * @brief This function handles SysTick Handler.
 */
__weak void SysTickHandler(void)
{
}

/**
 * @brief This function handles WWDG interrupt request.
 */
__weak void WWDG_IRQHandler(void)
{
}

/**
 * @brief This function handles PVD interrupt request.
 */
__weak void PVD_IRQHandler(void)
{
}

/**
 * @brief This function handles Tamper interrupt request.
 */
__weak void TAMPER_IRQHandler(void)
{
}

/**
 * @brief This function handles RTC global interrupt request.
 */
__weak void RTC_IRQHandler(void)
{
}

/**
 * @brief This function handles Flash interrupt request.
 */
__weak void FLASH_IRQHandler(void)
{
}

/**
 * @brief This function handles RCC interrupt request.
 */
__weak void RCC_IRQHandler(void)
{
}

/**
 * @brief This function handles External interrupt Line 0 request.
 */
__weak void EXTI0_IRQHandler(void)
{

}

/**
 * @brief This function handles External interrupt Line 1 request.
 */
__weak void EXTI1_IRQHandler(void)
{

}

/**
 * @brief This function handles External interrupt Line 2 request.
 */
__weak void EXTI2_IRQHandler(void)
{
}

/**
 * @brief This function handles External interrupt Line 3 request.
 */
__weak void EXTI3_IRQHandler(void)
{
}

/**
 * @brief This function handles External interrupt Line 4 request.
 */
__weak void EXTI4_IRQHandler(void)
{
}

/**
 * @brief This function handles DMA Stream 1 interrupt request.
 */
__weak void DMAChannel1_IRQHandler(void)
{
}

/**
 * @brief This function handles DMA Stream 2 interrupt request.
 */
__weak void DMAChannel2_IRQHandler(void)
{
}

/**
 * @brief This function handles DMA Stream 3 interrupt request.
 */
__weak void DMAChannel3_IRQHandler(void)
{
}

/**
 * @brief This function handles DMA Stream 4 interrupt request.
 */
__weak void DMAChannel4_IRQHandler(void)
{
}

/**
 * @brief This function handles DMA Stream 5 interrupt request.
 */
__weak void DMAChannel5_IRQHandler(void)
{
}

/**
 * @brief This function handles DMA Stream 6 interrupt request.
 */
__weak void DMAChannel6_IRQHandler(void)
{
}

/**
 * @brief This function handles DMA Stream 7 interrupt request.
 */
__weak void DMAChannel7_IRQHandler(void)
{
}

/**
 * @brief This function handles ADC global interrupt request.
 */
__weak void ADC_IRQHandler(void)
{
}

/**
 * @brief This function handles USB High Priority or CAN TX interrupts
 */
__weak void USB_HP_CAN_TX_IRQHandler(void)
{
}

/**
 * @brief This function handles USB Low Priority or CAN RX0 interrupts
 */
__weak void USB_LP_CAN_RX0_IRQHandler(void)
{
}

/**
 * @brief This function handles CAN RX1 interrupt request.
 */
__weak __weak void CAN_RX1_IRQHandler(void)
{
}

/**
 * @brief This function handles CAN SCE interrupt request.
 */
__weak void CAN_SCE_IRQHandler(void)
{
}

/**
 * @brief This function handles External lines 9 to 5 interrupt request.
 */
__weak void EXTI9_5_IRQHandler(void)
{
}

/**
 * @brief This function handles TIM1 Break interrupt request.
 */
__weak void TIM1_BRK_IRQHandler(void)
{
}

/**
 * @brief This function handles TIM1 overflow and update interrupt
 */
__weak void TIM1_UP_IRQHandler(void)
{
}

/**
 * @brief This function handles TIM1 Trigger and Commutation interrupts
 */
__weak void TIM1_TRG_COM_IRQHandler(void)
{
}

/**
 * @brief This function handles TIM1 capture compare interrupt request.
 */
__weak void TIM1_CC_IRQHandler(void)
{
}

/**
 * @brief This function handles TIM2 global interrupt request.
 */
__weak void TIM2_IRQHandler(void)
{
}

/**
 * @brief This function handles TIM3 global interrupt request.
 */
__weak void TIM3_IRQHandler(void)
{
}

/**
 * @brief  This function handles TIM4 global interrupt request.
 */
__weak void TIM4_IRQHandler(void)
{
}

/**
 * @brief  This function handles I2C1 Event interrupt request.
 */
__weak void I2C1_EV_IRQHandler(void)
{
}

/**
 * @brief  This function handles I2C1 Error interrupt request.
 */
__weak void I2C1_ER_IRQHandler(void)
{
}

/**
 * @brief  This function handles I2C2 Event interrupt request.
 */
__weak void I2C2_EV_IRQHandler(void)
{
}

/**
 * @brief  This function handles I2C2 Error interrupt request.
 */
__weak void I2C2_ER_IRQHandler(void)
{
}

/**
 * @brief  This function handles SPI1 global interrupt request.
 */
__weak void SPI1_IRQHandler(void)
{
}

/**
 * @brief  This function handles SPI2 global interrupt request.
 */
__weak void SPI2_IRQHandler(void)
{
}

/**
 * @brief  This function handles USART1 global interrupt request.
 */
__weak void USART1_IRQHandler(void)
{
}

/**
 * @brief  This function handles USART2 global interrupt request.
 */
__weak void USART2_IRQHandler(void)
{
}

/**
 * @brief  This function handles USART3 global interrupt request.
 */
__weak void USART3_IRQHandler(void)
{
}

/**
 * @brief  This function handles External lines 15 to 10 interrupt request.
 */
__weak void EXTI15_10_IRQHandler(void)
{
}

/**
 * @brief  This function handles RTC Alarm interrupt request.
 */
__weak void RTCAlarm_IRQHandler(void)
{
}

/**
 * @brief This function handles USB WakeUp interrupt request.
 */
__weak void USBWakeUp_IRQHandler(void)
{
}

/**
 * @brief This function handles TIM8 BRK exception.
 */
__weak void TIM8_BRK_IRQHandler(void)
{
}

/**
 * @brief This function handles TIM8 UP exception.
 */
__weak void TIM8_UP_IRQHandler(void)
{
}

/**
 * @brief This function handles TIM8 TRG exception.
 */
__weak void TIM8_TRG_COM_IRQHandler(void)
{
}

/**
 * @brief This function handles TIM8 CC exception.
 */
__weak void TIM8_CC_IRQHandler(void)
{
}

/**
 * @brief This function handles ADC3 exception.
 */
__weak void ADC3_IRQHandler(void)
{
}

/**
 * @brief This function handles FSMC exception.
 */
__weak void FSMC_IRQHandler(void)
{
}

/**
 * @brief This function handles SDIO exception.
 */
__weak void SDIO_IRQHandler(void)
{
}

/**
 * @brief This function handles TIM5 exception.
 */
__weak void TIM5_IRQHandler(void)
{
}

/**
 * @brief This function handles SPI3 exception.
 */
__weak void SPI3_IRQHandler(void)
{
}

/**
 * @brief This function handles UART4 exception.
 */
__weak void UART4_IRQHandler(void)
{
}

/**
 * @brief This function handles UART5 exception.
 */
__weak void UART5_IRQHandler(void)
{
}

/**
 * @brief This function handles TIM6 exception.
 */
__weak void TIM6_IRQHandler(void)
{
}

/**
 * @brief This function handles TIM7 exception.
 */
__weak void TIM7_IRQHandler(void)
{
}

/**
 * @brief This function handles DMA2 Channel1 exception.
 */
__weak void DMA2_Channel1_IRQHandler(void)
{
}

/**
 * @brief This function handles DMA2 Channel2 exception.
 */
__weak void DMA2_Channel2_IRQHandler(void)
{
}

/**
 * @brief This function handles DMA2 Channel3 exception.
 */
__weak void DMA2_Channel3_IRQHandler(void)
{
}

/**
 * @brief This function handles DMA2 Channel4_5 exception.
 */
__weak void DMA2_Channel4_5_IRQHandler(void)
{
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
