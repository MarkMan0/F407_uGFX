#include "STHAL.h"
#include "FreeRTOS.h"
#include "task.h"
#include "pin_api.h"
#include "gfx.h"
#include "usb_device.h"
#include "comm_api.h"
#include "mixer_gui.h"

#include "usb_uart.h"



void USB_CDC_Receive_callback(uint8_t* buff, size_t size) {
  USB_UART::get_instance().rx_buffer_.push(buff, size);
}


void blink_task(void*) {
  pin_mode(pins::LED0, pin_mode_t::OUT_PP);
  vTaskDelay(1000);
  while (1) {
    toggle_pin(pins::LED0);
    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}


void uart_task(void*) {
  pin_mode(pins::LED1, pin_mode_t::OUT_PP);
  auto& uart = USB_UART::get_instance();
  uart.init();
  uart.set_tx_task(xTaskGetCurrentTaskHandle());
  CommAPI::get_instance().init(&uart);
  while (1) {
    toggle_pin(pins::LED1);
    uart.send_task();
  }
}

void gfx_task(void*) {
  vTaskDelay(pdMS_TO_TICKS(500));
  gfxInit();
  while (1) {
  }
}

extern "C" void uGFXMain() {
  mixer_gui_task();
}

int main(void) {
  HAL_Init();
  SystemClock_Config();

  MX_CRC_Init();


  xTaskCreate(blink_task, "blink", 256, NULL, 10, NULL);
  xTaskCreate(uart_task, "uart", 256, NULL, 9, NULL);
  xTaskCreate(gfx_task, "GFX", 256, NULL, 10, NULL);

  vTaskStartScheduler();
  while (1) {
  }
}

extern "C" void vApplicationStackOverflowHook(xTaskHandle pxTask, char* pcTaskName) {
  vPortEnterCritical();
  Error_Handler();
}

extern "C" void vApplicationMallocFailedHook(void) {
  vPortEnterCritical();
  Error_Handler();
}
