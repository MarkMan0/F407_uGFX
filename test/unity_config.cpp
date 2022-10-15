/**
 * @file unity_config.cpp
 * @brief unity test framework connector with board and with PC via UART2
 */
#include "unity_config.h"
#include "STHAL.h"
#include <unity.h>
#include "usb_uart.h"

#ifdef __cplusplus
extern "C" {
#endif


static USB_UART uart;

void unityOutputStart() {
  uart.init();
  HAL_Delay(2000);
}

void unityOutputChar(char c) {
  uart.putc(c);
}

void unityOutputFlush() {
  uart.send_task();
  HAL_Delay(10);
}

void unityOutputComplete() {
  uart.send_task();
  HAL_Delay(500);
}



#ifdef __cplusplus
}
#endif