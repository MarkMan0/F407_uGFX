/**
 * @file unity_config.cpp
 * @brief unity test framework connector with board and with PC via UART2
 */
#include "unity_config.h"
#include "STHAL.h"
#include <unity.h>
#include "comm_class.h"
#include "CDC_Adaptor.h"

CommClass uart;

#ifdef __cplusplus
extern "C" {
#endif



static void dummy_task(void*) {
  while (1) {
    xTaskNotifyWait(0, UINT32_MAX, nullptr, portMAX_DELAY);
  }
}
TaskHandle_t handle;

static void callback(const void* ptr, size_t sz) {
  uart.receive(ptr, sz);
}
void unityOutputStart() {
  CDC_Adaptor::get_instance().set_receive_cb(callback);
  uart.set_hw_msg(&CDC_Adaptor::get_instance());

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