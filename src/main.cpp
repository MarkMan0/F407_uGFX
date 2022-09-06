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


#define COLOR_SIZE 20
#define PEN_SIZE   20
#define OFFSET     3

#define COLOR_BOX(a)  (ev.x >= a && ev.x <= a + COLOR_SIZE)
#define PEN_BOX(a)    (ev.y >= a && ev.y <= a + COLOR_SIZE)
#define GET_COLOR(a)  (COLOR_BOX(a * COLOR_SIZE + OFFSET))
#define GET_PEN(a)    (PEN_BOX(a * 2 * PEN_SIZE + OFFSET))
#define DRAW_COLOR(a) (a * COLOR_SIZE + OFFSET)
#define DRAW_PEN(a)   (a * 2 * PEN_SIZE + OFFSET)
#define DRAW_AREA(x, y)                                                                                                \
  (x >= PEN_SIZE + OFFSET + 3 && x <= gdispGetWidth() && y >= COLOR_SIZE + OFFSET + 3 && y <= gdispGetHeight())

void drawScreen() {
  gdispClear(GFX_RED);

  char* msg = "uGFX";
  gFont font1, font2;

  font1 = gdispOpenFont("DejaVuSans24*");
  font2 = gdispOpenFont("DejaVuSans12*");

  gdispClear(GFX_WHITE);
  gdispDrawString(gdispGetWidth() - gdispGetStringWidth(msg, font1) - 3, 3, msg, font1, GFX_BLACK);

  /* colors */
  gdispFillArea(0 * COLOR_SIZE + 3, 3, COLOR_SIZE, COLOR_SIZE, GFX_BLACK);  /* Black */
  gdispFillArea(1 * COLOR_SIZE + 3, 3, COLOR_SIZE, COLOR_SIZE, GFX_RED);    /* Red */
  gdispFillArea(2 * COLOR_SIZE + 3, 3, COLOR_SIZE, COLOR_SIZE, GFX_YELLOW); /* Yellow */
  gdispFillArea(3 * COLOR_SIZE + 3, 3, COLOR_SIZE, COLOR_SIZE, GFX_GREEN);  /* Green */
  gdispFillArea(4 * COLOR_SIZE + 3, 3, COLOR_SIZE, COLOR_SIZE, GFX_BLUE);   /* Blue */
  gdispDrawBox(5 * COLOR_SIZE + 3, 3, COLOR_SIZE, COLOR_SIZE, GFX_WHITE);   /* White */

  /* pens */
  gdispFillStringBox(OFFSET * 2, DRAW_PEN(1), PEN_SIZE, PEN_SIZE, "1", font2, GFX_WHITE, GFX_BLACK, gJustifyCenter);
  gdispFillStringBox(OFFSET * 2, DRAW_PEN(2), PEN_SIZE, PEN_SIZE, "2", font2, GFX_WHITE, GFX_BLACK, gJustifyCenter);
  gdispFillStringBox(OFFSET * 2, DRAW_PEN(3), PEN_SIZE, PEN_SIZE, "3", font2, GFX_WHITE, GFX_BLACK, gJustifyCenter);
  gdispFillStringBox(OFFSET * 2, DRAW_PEN(4), PEN_SIZE, PEN_SIZE, "4", font2, GFX_WHITE, GFX_BLACK, gJustifyCenter);
  gdispFillStringBox(OFFSET * 2, DRAW_PEN(5), PEN_SIZE, PEN_SIZE, "5", font2, GFX_WHITE, GFX_BLACK, gJustifyCenter);

  gdispCloseFont(font1);
  gdispCloseFont(font2);
}

extern "C" void uGFXMain() {
  while (1) {
    drawScreen();
    vTaskDelay(pdMS_TO_TICKS(3000));
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