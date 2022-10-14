#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"


extern SPI_HandleTypeDef hspi2;
extern SRAM_HandleTypeDef hsram1;
extern TIM_HandleTypeDef htim7;
extern CRC_HandleTypeDef hcrc;

void Error_Handler(void);
void SystemClock_Config(void);

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line);
#endif


// MSP
void HAL_MspInit(void);
// GPIO
void MX_GPIO_Init(void);
// TIM
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority);
void HAL_SuspendTick(void);
void HAL_ResumeTick(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim);
// FSMC
void MX_FSMC_Init(void);
void HAL_FSMC_MspInit(void);
void HAL_SRAM_MspInit(SRAM_HandleTypeDef* hsram);
void HAL_FSMC_MspDeInit(void);
void HAL_SRAM_MspDeInit(SRAM_HandleTypeDef* hsram);
// SPI
void MX_SPI2_Init(void);
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi);
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi);
// CRC
void MX_CRC_Init(void);
void CRC_begin();
void CRC_tick(uint32_t);
uint32_t CRC_end();

#ifdef __cplusplus
}
#endif
