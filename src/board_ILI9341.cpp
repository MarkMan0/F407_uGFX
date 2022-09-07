#include "gfx.h"
#include "pin_api.h"
#include "STHAL.h"




#ifdef __cplusplus
extern "C" {
#endif

void LCD_init_board(GDisplay* g) {
  pin_mode(pins::LCD_BL_PIN, pin_mode_t::OUT_PP);
  MX_GPIO_Init();
  MX_FSMC_Init();
}

void LCD_set_backlight(GDisplay* g, gU8 percent) {
  write_pin(pins::LCD_BL_PIN, percent > 0);
}


#ifdef __cplusplus
}
#endif