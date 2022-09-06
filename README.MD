# F407_starter

Empty project for STM32F407VET6 MCU.

## Features
+ platformio only
+ uses official FreeRTOS as a library
+ automatic formatting using clang-format
+ pre-commit check for formatting (pre-commit install)
+ MIT license
+ my *pin_api* library, to simplify GPIO manipulation with very little overhead
+ uses Full_Assert in debug build

## TODO
Configure testing. It's already working for STM32F3, so should be relatively easy, but I didn't need it yet.