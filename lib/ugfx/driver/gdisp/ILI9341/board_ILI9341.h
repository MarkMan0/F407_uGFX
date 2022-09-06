/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.io/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

// functions defined in project src/.


#ifdef __cplusplus
extern "C" {
#endif

static const uint32_t LCD_MEM_CMD = 0x60000000, LCD_MEM_DATA = 0x60080000;

static GFXINLINE void LCD_write_cmd(uint16_t cmd) {
  *(volatile uint16_t*)(LCD_MEM_CMD) = cmd;
}
static GFXINLINE void LCD_write_data(uint16_t data) {
  *(volatile uint16_t*)(LCD_MEM_DATA) = data;
}
static GFXINLINE uint16_t LCD_read_data() {
  return *(volatile uint16_t*)(LCD_MEM_DATA);
}


extern void LCD_set_backlight(GDisplay* g, gU8 percent);


static GFXINLINE void set_backlight(GDisplay* g, gU8 percent) {
    LCD_set_backlight(g, percent);
}
static GFXINLINE void write_index(GDisplay* g, gU16 index){
  LCD_write_cmd(index);
}
static GFXINLINE void write_data(GDisplay* g, gU16 data){
  LCD_write_data(data);
}
static GFXINLINE gU16 read_data(GDisplay* g){
  return LCD_read_data();
}

// Rest is unused

static GFXINLINE void init_board(GDisplay* g) {
  // FSMC init done in main.cpp
  return;
}
static GFXINLINE void post_init_board(GDisplay* g){
  // not used
  return;
}
static GFXINLINE void setpin_reset(GDisplay* g, gBool state) {
    // no reset pin
    return;
}
static GFXINLINE void acquire_bus(GDisplay* g){
  // nothing else uses bus
  return;
}
static GFXINLINE void release_bus(GDisplay* g){
  // nothing else uses bus
  return;
}
static GFXINLINE void setreadmode(GDisplay* g){
  // handled by MCU
  return;}
static GFXINLINE void setwritemode(GDisplay* g){
  // handled by MCU
  return;
}




#ifdef __cplusplus
}
#endif

#endif /* _GDISP_LLD_BOARD_H */