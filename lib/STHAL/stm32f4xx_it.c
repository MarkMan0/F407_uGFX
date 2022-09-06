#include "STHAL.h"
#include "stm32f4xx_it.h"


/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void) {
  while (1) {
  }
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void) {
  while (1) {
  }
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void) {
  while (1) {
  }
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */
void BusFault_Handler(void) {
  while (1) {
  }
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void) {
  while (1) {
  }
}


/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void) {
}


/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/


void TIM7_IRQHandler(void) {
  extern TIM_HandleTypeDef htim7;
  HAL_TIM_IRQHandler(&htim7);
}


/******************************************************************************/
/* Unused interrupt handlers                                                  */
/******************************************************************************/

// clang-format off

// void NMI_Handler(void) { while(1) {} }
// void HardFault_Handler(void) { while(1) {} }
// void MemManage_Handler(void) { while(1) {} }
// void BusFault_Handler(void) { while(1) {} }
// void UsageFault_Handler(void) { while(1) {} }
// void SVC_Handler(void) { while(1) {} }
// void DebugMon_Handler(void) { while(1) {} }
// void PendSV_Handler(void) { while(1) {} }
// void SysTick_Handler(void) { while(1) {} }
void WWDG_IRQHandler(void) { while(1) {} }
void PVD_IRQHandler(void) { while(1) {} }
void TAMP_STAMP_IRQHandler(void) { while(1) {} }
void RTC_WKUP_IRQHandler(void) { while(1) {} }
void FLASH_IRQHandler(void) { while(1) {} }
void RCC_IRQHandler(void) { while(1) {} }
void EXTI0_IRQHandler(void) { while(1) {} }
void EXTI1_IRQHandler(void) { while(1) {} }
void EXTI2_IRQHandler(void) { while(1) {} }
void EXTI3_IRQHandler(void) { while(1) {} }
void EXTI4_IRQHandler(void) { while(1) {} }
void DMA1_Stream0_IRQHandler(void) { while(1) {} }
void DMA1_Stream1_IRQHandler(void) { while(1) {} }
void DMA1_Stream2_IRQHandler(void) { while(1) {} }
void DMA1_Stream3_IRQHandler(void) { while(1) {} }
void DMA1_Stream4_IRQHandler(void) { while(1) {} }
void DMA1_Stream5_IRQHandler(void) { while(1) {} }
void DMA1_Stream6_IRQHandler(void) { while(1) {} }
void ADC_IRQHandler(void) { while(1) {} }
void CAN1_TX_IRQHandler(void) { while(1) {} }
void CAN1_RX0_IRQHandler(void) { while(1) {} }
void CAN1_RX1_IRQHandler(void) { while(1) {} }
void CAN1_SCE_IRQHandler(void) { while(1) {} }
void EXTI9_5_IRQHandler(void) { while(1) {} }
void TIM1_BRK_TIM9_IRQHandler(void) { while(1) {} }
void TIM1_UP_TIM10_IRQHandler(void) { while(1) {} }
void TIM1_TRG_COM_TIM11_IRQHandler(void) { while(1) {} }
void TIM1_CC_IRQHandler(void) { while(1) {} }
void TIM2_IRQHandler(void) { while(1) {} }
void TIM3_IRQHandler(void) { while(1) {} }
void TIM4_IRQHandler(void) { while(1) {} }
void I2C1_EV_IRQHandler(void) { while(1) {} }
void I2C1_ER_IRQHandler(void) { while(1) {} }
void I2C2_EV_IRQHandler(void) { while(1) {} }
void I2C2_ER_IRQHandler(void) { while(1) {} }
void SPI1_IRQHandler(void) { while(1) {} }
void SPI2_IRQHandler(void) { while(1) {} }
void USART1_IRQHandler(void) { while(1) {} }
void USART2_IRQHandler(void) { while(1) {} }
void USART3_IRQHandler(void) { while(1) {} }
void EXTI15_10_IRQHandler(void) { while(1) {} }
void RTC_Alarm_IRQHandler(void) { while(1) {} }
void OTG_FS_WKUP_IRQHandler(void) { while(1) {} }
void TIM8_BRK_TIM12_IRQHandler(void) { while(1) {} }
void TIM8_UP_TIM13_IRQHandler(void) { while(1) {} }
void TIM8_TRG_COM_TIM14_IRQHandler(void) { while(1) {} }
void TIM8_CC_IRQHandler(void) { while(1) {} }
void DMA1_Stream7_IRQHandler(void) { while(1) {} }
void FSMC_IRQHandler(void) { while(1) {} }
void SDIO_IRQHandler(void) { while(1) {} }
void TIM5_IRQHandler(void) { while(1) {} }
void SPI3_IRQHandler(void) { while(1) {} }
void UART4_IRQHandler(void) { while(1) {} }
void UART5_IRQHandler(void) { while(1) {} }
void TIM6_DAC_IRQHandler(void) { while(1) {} }
// void TIM7_IRQHandler(void) { while(1) {} }
void DMA2_Stream0_IRQHandler(void) { while(1) {} }
void DMA2_Stream1_IRQHandler(void) { while(1) {} }
void DMA2_Stream2_IRQHandler(void) { while(1) {} }
void DMA2_Stream3_IRQHandler(void) { while(1) {} }
void DMA2_Stream4_IRQHandler(void) { while(1) {} }
void ETH_IRQHandler(void) { while(1) {} }
void ETH_WKUP_IRQHandler(void) { while(1) {} }
void CAN2_TX_IRQHandler(void) { while(1) {} }
void CAN2_RX0_IRQHandler(void) { while(1) {} }
void CAN2_RX1_IRQHandler(void) { while(1) {} }
void CAN2_SCE_IRQHandler(void) { while(1) {} }
void OTG_FS_IRQHandler(void) { while(1) {} }
void DMA2_Stream5_IRQHandler(void) { while(1) {} }
void DMA2_Stream6_IRQHandler(void) { while(1) {} }
void DMA2_Stream7_IRQHandler(void) { while(1) {} }
void USART6_IRQHandler(void) { while(1) {} }
void I2C3_EV_IRQHandler(void) { while(1) {} }
void I2C3_ER_IRQHandler(void) { while(1) {} }
void OTG_HS_EP1_OUT_IRQHandler(void) { while(1) {} }
void OTG_HS_EP1_IN_IRQHandler(void) { while(1) {} }
void OTG_HS_WKUP_IRQHandler(void) { while(1) {} }
void OTG_HS_IRQHandler(void) { while(1) {} }
void DCMI_IRQHandler(void) { while(1) {} }
void HASH_RNG_IRQHandler(void) { while(1) {} }
void FPU_IRQHandler(void) { while(1) {} }

// clang-format on