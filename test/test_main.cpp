#include "STHAL.h"

#include "FreeRTOS.h"
#include "task.h"
#include "unity.h"
#include "fakeit.hpp"

/**
 * @brief Tests have to implement this task
 * @details Body should be like this:
 *   void test_task(void*) {
 *     RUN_TEST(test_1);
 *     RUN_TEST(test_2);
 *     ...
 *   }
 */
extern void test_task(void*);

/// If implemented, will be called before starting the scheduler
extern void pre_test();


static void test_task_wrap(void* arg) {
  UNITY_BEGIN();
  test_task(arg);
  UNITY_END();
  while (1) {
  }
}


int main() {
  HAL_Init();
  SystemClock_Config();


  pre_test();

  TaskHandle_t handle;
  xTaskCreate(test_task_wrap, "test task", 512, nullptr, 10, &handle);

  vTaskStartScheduler();
}


void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
  assert_param(0);
}
extern "C" void vApplicationMallocFailedHook(void) {
  while (1) {
  }
}

__weak void setUp() {
}
__weak void tearDown() {
}
__weak void pre_test() {
}
