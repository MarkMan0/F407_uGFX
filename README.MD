# VolumeMixer_board

Gadget, to change volume levels of windows applications, like in the GUI.

## Hardware

The MCU is and [STM32F407VET6](https://www.st.com/en/microcontrollers-microprocessors/stm32f407ve.html). It's seated on a [Black-F407VET6 board](https://stm32-base.org/boards/STM32F407VET6-STM32-F4VE-V2.0.html).

The display uses the *ILI9341* chip for the LCD and *ADS7843* for the resistive touch panel.

## Project Structure
The project is built in Platformio, and it's based on ST HAL libraries. No external library is used, and it's ready to build after download, Platformio will handle the dependencies. The application runs on FreeRTOS.

Communication to the PC application is via USB CDC and is verified by CRC at the end of messages. Porting to different communication method is possible. One would need to implement the `IHWMessage` interface and create a `comm_class` from it. For inspiration, look at `CDC_Adaptor` and `comm_class`.

The display is connected through FSMC, the GUI library is [µGFX](https://ugfx.io/). The library is easily portable and has support for many displays.

### Component list
Here I list the components, which are located in the `lib/` folder.

+ [FakeIt](https://github.com/eranpeer/FakeIt) - C++ mocking framework
+ IHWMessage - the interface to be implemented for a communication hardware (CDC, UART..). Has methods to init, transmit bytes and callback on receive.
+ CDC_Adaptor - `IHWMessage` implementation for USB CDC.
+ comm_class - Handles buffering and memory to type conversion from a serial interface through the `IHWMessage` interface. "Glueing" the interface to the instance of this class is done in `main.cpp`.
+ comm_api - the API to communicate with the PC application, and read/write mixer volumes
+ FreeRTOS - the official FreeRTOS as Platformio library
+ mixer_gui - the GUI task
+ pin_api - simple wrapper around HAL GPIO, allows the use of labels like *PA0*, with very little overhead
+ ring_buffer - C++ ring buffer implementation
+ sem_lock - RAII semaphore lock
+ STHAL - STM32 specific code, IRQ handlers, peripheral init functions etc.
+ ugfx - stripped version of the UGFX library. It originally uses Makefiles, this is a ported version to platformio, with only the needed files.
+ utility - simple utility functions, to make life easier

### Unit testing
The testing framework is Unity, and tests are run on the hardware.

Mocking is also supported, using FakeIt. Take a look at the [Comm Class tests](lib\comm_class\comm_class_tests.cpp) or [Mixer API test](lib\comm_api\comm_api_test.cpp) for reference.

Unit tests are run in a FreeRTOS environment. The FreeRTOS is started from [test_main](test/test_main.cpp). Each test is required to create a `void test_task(void*)` function, which will call the tests. This function must return, so unity can finish correctly.

To run the tests, the Platformio environment needs to be switched to `env:test`.

### Formatting
A `.clang_format` file is included with the project, along with a `.pre-commit-config.yaml`. [pre-commit](https://pre-commit.com/) should be enabled, to only allow formatted commits into the repo.

### Building
The project should build out of the box. The *test* environment cannot be built, but using the *test* button/command, it will work correctly.

## TO-DO
There is always room for improvement. These improvements only consider this Platformio project, without the PC app.

- [ ] Fix TODOs in the code
- [ ] Implement backlight control
- [x] Move the message interface one layer up
