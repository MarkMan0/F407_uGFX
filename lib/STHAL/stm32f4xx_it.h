#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define ATTR_USED __attribute__((used))


void NMI_Handler(void) ATTR_USED;
void HardFault_Handler(void) ATTR_USED;
void MemManage_Handler(void) ATTR_USED;
void BusFault_Handler(void) ATTR_USED;
void UsageFault_Handler(void) ATTR_USED;
void SVC_Handler(void) ATTR_USED;
void DebugMon_Handler(void) ATTR_USED;
void PendSV_Handler(void) ATTR_USED;
void SysTick_Handler(void) ATTR_USED;
void WWDG_IRQHandler(void) ATTR_USED;
void PVD_IRQHandler(void) ATTR_USED;
void TAMP_STAMP_IRQHandler(void) ATTR_USED;
void RTC_WKUP_IRQHandler(void) ATTR_USED;
void FLASH_IRQHandler(void) ATTR_USED;
void RCC_IRQHandler(void) ATTR_USED;
void EXTI0_IRQHandler(void) ATTR_USED;
void EXTI1_IRQHandler(void) ATTR_USED;
void EXTI2_IRQHandler(void) ATTR_USED;
void EXTI3_IRQHandler(void) ATTR_USED;
void EXTI4_IRQHandler(void) ATTR_USED;
void DMA1_Stream0_IRQHandler(void) ATTR_USED;
void DMA1_Stream1_IRQHandler(void) ATTR_USED;
void DMA1_Stream2_IRQHandler(void) ATTR_USED;
void DMA1_Stream3_IRQHandler(void) ATTR_USED;
void DMA1_Stream4_IRQHandler(void) ATTR_USED;
void DMA1_Stream5_IRQHandler(void) ATTR_USED;
void DMA1_Stream6_IRQHandler(void) ATTR_USED;
void ADC_IRQHandler(void) ATTR_USED;
void CAN1_TX_IRQHandler(void) ATTR_USED;
void CAN1_RX0_IRQHandler(void) ATTR_USED;
void CAN1_RX1_IRQHandler(void) ATTR_USED;
void CAN1_SCE_IRQHandler(void) ATTR_USED;
void EXTI9_5_IRQHandler(void) ATTR_USED;
void TIM1_BRK_TIM9_IRQHandler(void) ATTR_USED;
void TIM1_UP_TIM10_IRQHandler(void) ATTR_USED;
void TIM1_TRG_COM_TIM11_IRQHandler(void) ATTR_USED;
void TIM1_CC_IRQHandler(void) ATTR_USED;
void TIM2_IRQHandler(void) ATTR_USED;
void TIM3_IRQHandler(void) ATTR_USED;
void TIM4_IRQHandler(void) ATTR_USED;
void I2C1_EV_IRQHandler(void) ATTR_USED;
void I2C1_ER_IRQHandler(void) ATTR_USED;
void I2C2_EV_IRQHandler(void) ATTR_USED;
void I2C2_ER_IRQHandler(void) ATTR_USED;
void SPI1_IRQHandler(void) ATTR_USED;
void SPI2_IRQHandler(void) ATTR_USED;
void USART1_IRQHandler(void) ATTR_USED;
void USART2_IRQHandler(void) ATTR_USED;
void USART3_IRQHandler(void) ATTR_USED;
void EXTI15_10_IRQHandler(void) ATTR_USED;
void RTC_Alarm_IRQHandler(void) ATTR_USED;
void OTG_FS_WKUP_IRQHandler(void) ATTR_USED;
void TIM8_BRK_TIM12_IRQHandler(void) ATTR_USED;
void TIM8_UP_TIM13_IRQHandler(void) ATTR_USED;
void TIM8_TRG_COM_TIM14_IRQHandler(void) ATTR_USED;
void TIM8_CC_IRQHandler(void) ATTR_USED;
void DMA1_Stream7_IRQHandler(void) ATTR_USED;
void FSMC_IRQHandler(void) ATTR_USED;
void SDIO_IRQHandler(void) ATTR_USED;
void TIM5_IRQHandler(void) ATTR_USED;
void SPI3_IRQHandler(void) ATTR_USED;
void UART4_IRQHandler(void) ATTR_USED;
void UART5_IRQHandler(void) ATTR_USED;
void TIM6_DAC_IRQHandler(void) ATTR_USED;
void TIM7_IRQHandler(void) ATTR_USED;
void DMA2_Stream0_IRQHandler(void) ATTR_USED;
void DMA2_Stream1_IRQHandler(void) ATTR_USED;
void DMA2_Stream2_IRQHandler(void) ATTR_USED;
void DMA2_Stream3_IRQHandler(void) ATTR_USED;
void DMA2_Stream4_IRQHandler(void) ATTR_USED;
void ETH_IRQHandler(void) ATTR_USED;
void ETH_WKUP_IRQHandler(void) ATTR_USED;
void CAN2_TX_IRQHandler(void) ATTR_USED;
void CAN2_RX0_IRQHandler(void) ATTR_USED;
void CAN2_RX1_IRQHandler(void) ATTR_USED;
void CAN2_SCE_IRQHandler(void) ATTR_USED;
void OTG_FS_IRQHandler(void) ATTR_USED;
void DMA2_Stream5_IRQHandler(void) ATTR_USED;
void DMA2_Stream6_IRQHandler(void) ATTR_USED;
void DMA2_Stream7_IRQHandler(void) ATTR_USED;
void USART6_IRQHandler(void) ATTR_USED;
void I2C3_EV_IRQHandler(void) ATTR_USED;
void I2C3_ER_IRQHandler(void) ATTR_USED;
void OTG_HS_EP1_OUT_IRQHandler(void) ATTR_USED;
void OTG_HS_EP1_IN_IRQHandler(void) ATTR_USED;
void OTG_HS_WKUP_IRQHandler(void) ATTR_USED;
void OTG_HS_IRQHandler(void) ATTR_USED;
void DCMI_IRQHandler(void) ATTR_USED;
void HASH_RNG_IRQHandler(void) ATTR_USED;
void FPU_IRQHandler(void) ATTR_USED;

#undef ATTR_USED

#ifdef __cplusplus
}
#endif
