#include "gfx.h"
#include "pin_api.h"


static constexpr uint32_t LCD_MEM_CMD = 0x60000000, LCD_MEM_DATA = 0x60080000;

static void LCD_write_cmd(uint16_t cmd) {
  *(volatile uint16_t*)(LCD_MEM_CMD) = cmd;
}
static void LCD_write_data(uint16_t data) {
  *(volatile uint16_t*)(LCD_MEM_DATA) = data;
}
static uint16_t LCD_read_data() {
  return *(volatile uint16_t*)(LCD_MEM_DATA);
}

#ifdef __cplusplus
extern "C" {
#endif

void init_board(GDisplay* g) {
  // FSMC init done in main.cpp
  return;
}

void post_init_board(GDisplay* g) {
  // not used
  return;
}

void setpin_reset(GDisplay* g, gBool state) {
  // soft reset only
  return;
}

void set_backlight(GDisplay* g, gU8 percent) {
  write_pin(pins::LCD_BL_PIN, 1);
}

void acquire_bus(GDisplay* g) {
  // nothing else uses bus
  return;
}

void release_bus(GDisplay* g) {
  // nothing else uses bus
  return;
}

void write_index(GDisplay* g, gU16 index) {
  LCD_write_cmd(index);
}

void write_data(GDisplay* g, gU16 data) {
  LCD_write_data(data);
}

void setreadmode(GDisplay* g) {
  // handled by MCU
  return;
}

void setwritemode(GDisplay* g) {
  // handled by MCU
  return;
}

gU16 read_data(GDisplay* g) {
  return LCD_read_data();
}



#ifdef __cplusplus
}
#endif