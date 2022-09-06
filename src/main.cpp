#include "STHAL.h"
#include "FreeRTOS.h"
#include "task.h"
#include "pin_api.h"


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

int main(void) {
  HAL_Init();
  SystemClock_Config();
  xTaskCreate(blink_task, "blink", 256, NULL, 10, NULL);
  xTaskCreate(blink_task2, "blink2", 256, NULL, 10, NULL);

  vTaskStartScheduler();
  while (1) {
  }
}
