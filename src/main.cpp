#include "STHAL.h"
#include "FreeRTOS.h"
#include "task.h"
#include "pin_api.h"
#include "gfx.h"
#include "usb_device.h"
#include "comm_api.h"
#include "mixer_gui.h"

#include "usb_uart.h"

#include "CDC_Adaptor.h"


/// @brief Will be called by CDC IRQ to put data into internal buffer
/// @param buff the incoming data
/// @param size length of data
static void USB_UART_receive(const void* buff, size_t size) {
  USB_UART::get_instance().receive(buff, size);
}

void USB_CDC_Receive_callback(uint8_t* buff, size_t size) {
  CDC_Adaptor::get_instance().receive(buff, size);
}


void monitor_task(void*) {
  vTaskDelay(pdMS_TO_TICKS(30000));

  const auto num_of_tasks = uxTaskGetNumberOfTasks();
  auto statuses = static_cast<TaskStatus_t*>(pvPortMalloc(num_of_tasks * sizeof(TaskStatus_t)));
  constexpr size_t memory_low_th{ 999999 };
  static char buff[100];
  while (1) {
    if (auto n = uxTaskGetSystemState(statuses, num_of_tasks, NULL)) {
      for (unsigned int i = 0; i < n; ++i) {
        if (statuses[i].usStackHighWaterMark < memory_low_th) {
          sprintf(buff, "MEM:%s:%d\n", statuses[i].pcTaskName, statuses[i].usStackHighWaterMark);
          CommAPI::get_instance().echo(buff);
        }
      }
    }

    if (xPortGetFreeHeapSize() < memory_low_th) {
      sprintf(buff, "HEAP:%u\t%u\n", static_cast<unsigned>(xPortGetFreeHeapSize()),
              static_cast<unsigned>(xPortGetMinimumEverFreeHeapSize()));
      CommAPI::get_instance().echo(buff);
    }
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

void uart_task(void*) {
  pin_mode(pins::LED1, pin_mode_t::OUT_PP);
  auto& uart = USB_UART::get_instance();

  CDC_Adaptor::get_instance().set_receive_cb(USB_UART_receive);
  uart.set_hw_msg(&(CDC_Adaptor::get_instance()));

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

#ifdef DEBUG
  xTaskCreate(monitor_task, "monitor", 256, NULL, 8, NULL);
#endif
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
