/**
 * @file STHAL.cpp
 * @brief File containint STM32F4, and HAL callbacks
 */
#include "STHAL.h"



SPI_HandleTypeDef hspi2;
SRAM_HandleTypeDef hsram1;
TIM_HandleTypeDef htim7;
CRC_HandleTypeDef hcrc;


/**
 * Initializes the Global MSP.
 */
void HAL_MspInit(void) {
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}



/// @brief  Configure the clocks for max frequency
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
  RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
    Error_Handler();
  }
}


/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  __disable_irq();
  while (1) {
  }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
  __disable_irq();
  volatile bool b = true;  // so we can exit in debug
  while (b) {
  }
}
#endif /* USE_FULL_ASSERT */



/**
 * @brief  This function configures the TIM7 as a time base source.
 *         The time source is configured  to have 1ms time base with a dedicated
 *         Tick interrupt priority.
 * @note   This function is called  automatically at the beginning of program after
 *         reset by HAL_Init() or at any time when clock is configured, by HAL_RCC_ClockConfig().
 * @param  TickPriority: Tick interrupt priority.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority) {
  RCC_ClkInitTypeDef clkconfig;
  uint32_t uwTimclock, uwAPB1Prescaler = 0U;

  uint32_t uwPrescalerValue = 0U;
  uint32_t pFLatency;
  HAL_StatusTypeDef status;

  /* Enable TIM7 clock */
  __HAL_RCC_TIM7_CLK_ENABLE();

  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

  /* Get APB1 prescaler */
  uwAPB1Prescaler = clkconfig.APB1CLKDivider;
  /* Compute TIM7 clock */
  if (uwAPB1Prescaler == RCC_HCLK_DIV1) {
    uwTimclock = HAL_RCC_GetPCLK1Freq();
  } else {
    uwTimclock = 2UL * HAL_RCC_GetPCLK1Freq();
  }

  /* Compute the prescaler value to have TIM7 counter clock equal to 1MHz */
  uwPrescalerValue = (uint32_t)((uwTimclock / 1000000U) - 1U);

  /* Initialize TIM7 */
  htim7.Instance = TIM7;

  /* Initialize TIMx peripheral as follow:
  + Period = [(TIM7CLK/1000) - 1]. to have a (1/1000) s time base.
  + Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
  + ClockDivision = 0
  + Counter direction = Up
  */
  htim7.Init.Period = (1000000U / 1000U) - 1U;
  htim7.Init.Prescaler = uwPrescalerValue;
  htim7.Init.ClockDivision = 0;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  status = HAL_TIM_Base_Init(&htim7);
  if (status == HAL_OK) {
    /* Start the TIM time Base generation in interrupt mode */
    status = HAL_TIM_Base_Start_IT(&htim7);
    if (status == HAL_OK) {
      /* Enable the TIM7 global Interrupt */
      HAL_NVIC_EnableIRQ(TIM7_IRQn);
      /* Configure the SysTick IRQ priority */
      if (TickPriority < (1UL << __NVIC_PRIO_BITS)) {
        /* Configure the TIM IRQ priority */
        HAL_NVIC_SetPriority(TIM7_IRQn, TickPriority, 0U);
        uwTickPrio = TickPriority;
      } else {
        status = HAL_ERROR;
      }
    }
  }

  /* Return function status */
  return status;
}

/**
 * @brief  Suspend Tick increment.
 * @note   Disable the tick increment by disabling TIM7 update interrupt.
 * @param  None
 * @retval None
 */
void HAL_SuspendTick(void) {
  /* Disable TIM7 update Interrupt */
  __HAL_TIM_DISABLE_IT(&htim7, TIM_IT_UPDATE);
}

/**
 * @brief  Resume Tick increment.
 * @note   Enable the tick increment by Enabling TIM7 update interrupt.
 * @param  None
 * @retval None
 */
void HAL_ResumeTick(void) {
  /* Enable TIM7 Update interrupt */
  __HAL_TIM_ENABLE_IT(&htim7, TIM_IT_UPDATE);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
  if (htim->Instance == TIM7) {
    HAL_IncTick();
  }
}


static uint32_t FSMC_Initialized = 0;

void HAL_FSMC_MspInit(void) {
  GPIO_InitTypeDef GPIO_InitStruct = { 0 };
  if (FSMC_Initialized) {
    return;
  }
  FSMC_Initialized = 1;

  /* Peripheral clock enable */
  __HAL_RCC_FSMC_CLK_ENABLE();

  /** FSMC GPIO Configuration
  PE7   ------> FSMC_D4
  PE8   ------> FSMC_D5
  PE9   ------> FSMC_D6
  PE10   ------> FSMC_D7
  PE11   ------> FSMC_D8
  PE12   ------> FSMC_D9
  PE13   ------> FSMC_D10
  PE14   ------> FSMC_D11
  PE15   ------> FSMC_D12
  PD8   ------> FSMC_D13
  PD9   ------> FSMC_D14
  PD10   ------> FSMC_D15
  PD13   ------> FSMC_A18
  PD14   ------> FSMC_D0
  PD15   ------> FSMC_D1
  PD0   ------> FSMC_D2
  PD1   ------> FSMC_D3
  PD4   ------> FSMC_NOE
  PD5   ------> FSMC_NWE
  PD7   ------> FSMC_NE1
  */
  GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 |
                        GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_0 | GPIO_PIN_1 |
                        GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

void HAL_SRAM_MspInit(SRAM_HandleTypeDef* hsram) {
  HAL_FSMC_MspInit();
}

static uint32_t FSMC_DeInitialized = 0;

void HAL_FSMC_MspDeInit(void) {
  if (FSMC_DeInitialized) {
    return;
  }
  FSMC_DeInitialized = 1;
  /* Peripheral clock enable */
  __HAL_RCC_FSMC_CLK_DISABLE();

  /** FSMC GPIO Configuration
  PE7   ------> FSMC_D4
  PE8   ------> FSMC_D5
  PE9   ------> FSMC_D6
  PE10   ------> FSMC_D7
  PE11   ------> FSMC_D8
  PE12   ------> FSMC_D9
  PE13   ------> FSMC_D10
  PE14   ------> FSMC_D11
  PE15   ------> FSMC_D12
  PD8   ------> FSMC_D13
  PD9   ------> FSMC_D14
  PD10   ------> FSMC_D15
  PD13   ------> FSMC_A18
  PD14   ------> FSMC_D0
  PD15   ------> FSMC_D1
  PD0   ------> FSMC_D2
  PD1   ------> FSMC_D3
  PD4   ------> FSMC_NOE
  PD5   ------> FSMC_NWE
  PD7   ------> FSMC_NE1
  */
  HAL_GPIO_DeInit(GPIOE, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 |
                             GPIO_PIN_14 | GPIO_PIN_15);

  HAL_GPIO_DeInit(GPIOD, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_0 |
                             GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
}

void HAL_SRAM_MspDeInit(SRAM_HandleTypeDef* hsram) {
  HAL_FSMC_MspDeInit();
}



void MX_GPIO_Init(void) {
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
}

/* FSMC initialization function */
void MX_FSMC_Init(void) {
  FSMC_NORSRAM_TimingTypeDef Timing = { 0 };


  /** Perform the SRAM1 memory initialization sequence
   */
  hsram1.Instance = FSMC_NORSRAM_DEVICE;
  hsram1.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram1.Init */
  hsram1.Init.NSBank = FSMC_NORSRAM_BANK1;
  hsram1.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
  hsram1.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
  hsram1.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram1.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
  hsram1.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram1.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
  hsram1.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
  hsram1.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
  hsram1.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
  hsram1.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
  hsram1.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram1.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
  hsram1.Init.PageSize = FSMC_PAGE_SIZE_NONE;
  /* Timing */
  Timing.AddressSetupTime = 1;
  Timing.AddressHoldTime = 15;
  Timing.DataSetupTime = 5;
  Timing.BusTurnAroundDuration = 0;
  Timing.CLKDivision = 16;
  Timing.DataLatency = 17;
  Timing.AccessMode = FSMC_ACCESS_MODE_A;
  /* ExtTiming */

  if (HAL_SRAM_Init(&hsram1, &Timing, NULL) != HAL_OK) {
    Error_Handler();
  }
}


void MX_SPI2_Init(void) {
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK) {
    Error_Handler();
  }
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi) {
  if (hspi->Instance == SPI2) {
    __HAL_RCC_SPI2_CLK_ENABLE();
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi) {
  if (hspi->Instance == SPI2) {
    __HAL_RCC_SPI2_CLK_DISABLE();
  }
}

void MX_CRC_Init() {
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK) {
    Error_Handler();
  }
}

void HAL_CRC_MspInit(CRC_HandleTypeDef* hcrc) {
  __CRC_CLK_ENABLE();
}

void CRC_begin() {
  hcrc.State = HAL_CRC_STATE_BUSY;
  __HAL_CRC_DR_RESET(&hcrc);
}

void CRC_tick(uint32_t val) {
  hcrc.Instance->DR = val;
}

uint32_t CRC_end() {
  auto tmp = hcrc.Instance->DR;
  hcrc.State = HAL_CRC_STATE_READY;
  return tmp;
}