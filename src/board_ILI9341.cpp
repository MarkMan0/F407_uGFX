#include "gfx.h"
#include "pin_api.h"




#ifdef __cplusplus
extern "C" {
#endif

void LCD_set_backlight(GDisplay* g, gU8 percent) {
  write_pin(pins::LCD_BL_PIN, percent > 0);
}


#ifdef __cplusplus
}
#endif