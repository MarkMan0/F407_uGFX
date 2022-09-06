#include "STHAL.h"
#include "FreeRTOS.h"
#include "task.h"
#include "pin_api.h"
#include "gfx.h"

SRAM_HandleTypeDef hsram1;


void MX_GPIO_Init();
void MX_FSMC_Init();

void blink_task(void*) {
  pin_mode(pins::LED0, pin_mode_t::OUT_PP);
  while (1) {
    toggle_pin(pins::LED0);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}


void blink_task2(void*) {
  pin_mode(pins::LED1, pin_mode_t::OUT_PP);
  while (1) {
    toggle_pin(pins::LED1);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void gfx_task(void*) {
  vTaskDelay(pdMS_TO_TICKS(500));
  gfxInit();
  while (1) {
  }
}

int main(void) {
  HAL_Init();
  SystemClock_Config();


  MX_GPIO_Init();
  MX_FSMC_Init();

  xTaskCreate(blink_task, "blink", 256, NULL, 10, NULL);
  xTaskCreate(blink_task2, "blink2", 256, NULL, 10, NULL);
  xTaskCreate(gfx_task, "GFX", 256, NULL, 10, NULL);

  vTaskStartScheduler();
  while (1) {
  }
}


void mandelbrot(float x1, float y1, float x2, float y2) {
  unsigned int i, j, width, height;
  gU16 iter;
  gColor color;
  float fwidth, fheight;

  float sy = y2 - y1;
  float sx = x2 - x1;
  const int MAX = 512;

  width = (unsigned int)gdispGetWidth();
  height = (unsigned int)gdispGetHeight();
  fwidth = width;
  fheight = height;

  for (i = 0; i < width; i++) {
    for (j = 0; j < height; j++) {
      float cy = j * sy / fheight + y1;
      float cx = i * sx / fwidth + x1;
      float x = 0.0f, y = 0.0f, xx = 0.0f, yy = 0.0f;
      for (iter = 0; iter <= MAX && xx + yy < 4.0f; iter++) {
        xx = x * x;
        yy = y * y;
        y = 2.0f * x * y + cy;
        x = xx - yy + cx;
      }
      color = ((iter << 8) | (iter & 0xFF));
      gdispDrawPixel(i, j, color);
    }
  }
}

extern "C" void uGFXMain() {
  float cx, cy;
  float zoom = 1.0f;
  cx = -0.086f;
  cy = 0.85f;
  while (1) {
    mandelbrot(-2.0f * zoom + cx, -1.5f * zoom + cy, 2.0f * zoom + cx, 1.5f * zoom + cy);

    zoom *= 0.7f;
    if (zoom <= 0.00000001f) zoom = 1.0f;
  }
}



void MX_GPIO_Init(void) {
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  pin_mode(pins::LCD_BL_PIN, pin_mode_t::OUT_PP);
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