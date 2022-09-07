
#include "STHAL.h"
#include <stdbool.h>
#include "pin_api.h"
#include "gfx.h"


#ifdef __cplusplus
extern "C" {
#endif



gBool TOUCH_init_board() {
  pin_mode(pins::T_CS, pin_mode_t::OUT_PP);
  pin_mode(pins::T_CLK, pin_mode_t::ALTERNATE_PP, GPIO_AF5_SPI2);
  pin_mode(pins::T_MOSI, pin_mode_t::ALTERNATE_PP, GPIO_AF5_SPI2);
  pin_mode(pins::T_MISO, pin_mode_t::ALTERNATE_PP, GPIO_AF5_SPI2);
  pin_mode(pins::T_PEN, pin_mode_t::INPUT_PU);
  MX_SPI2_Init();
  return gTrue;
}

gBool TOUCH_getpin_pressed() {
  return not read_pin(pins::T_PEN);
}

void TOUCH_aquire_bus() {
  write_pin(pins::T_CS, 0);
}
void TOUCH_release_bus() {
  write_pin(pins::T_CS, 1);
}
gU16 TOUCH_read_value(gU16 reg) {
  gU8 txbuf[3] = { 0, 0, 0 };
  gU8 rxbuf[3] = { 0, 0, 0 };


  txbuf[0] = reg;
  HAL_SPI_TransmitReceive(&hspi2, txbuf, rxbuf, 3, 1000);

  return (rxbuf[1] << 5) | (rxbuf[2] >> 3);
}


#ifdef __cplusplus
}
#endif