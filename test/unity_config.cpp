/**
 * @file unity_config.cpp
 * @brief unity test framework connector with board and with PC via UART2
 */
#include "unity_config.h"
#include "STHAL.h"
#include <unity.h>
#include "usb_uart.h"

USB_UART uart;

#ifdef __cplusplus
extern "C" {
#endif



static void dummy_task(void*) {
  while (1) {
    xTaskNotifyWait(0, UINT32_MAX, nullptr, portMAX_DELAY);
  }
}
TaskHandle_t handle;


void unityOutputStart() {
  uart.init();
  xTaskCreate(dummy_task, "dummy", 64, nullptr, 1, &handle);
  uart.set_tx_task(handle);
  HAL_Delay(2000);
}

void unityOutputChar(char c) {
  uart.write(c);
}

void unityOutputFlush() {
  uart.flush();
  HAL_Delay(10);
}

void unityOutputComplete() {
  uart.flush();
  HAL_Delay(500);
}



#ifdef __cplusplus
}
#endif