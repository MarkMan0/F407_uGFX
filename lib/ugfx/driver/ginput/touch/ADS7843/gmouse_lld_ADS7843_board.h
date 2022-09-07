/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.io/license.html
 */

#ifndef _GINPUT_LLD_MOUSE_BOARD_H
#define _GINPUT_LLD_MOUSE_BOARD_H

// Resolution and Accuracy Settings
#define GMOUSE_ADS7843_PEN_CALIBRATE_ERROR		8
#define GMOUSE_ADS7843_PEN_CLICK_ERROR			6
#define GMOUSE_ADS7843_PEN_MOVE_ERROR			4
#define GMOUSE_ADS7843_FINGER_CALIBRATE_ERROR	14
#define GMOUSE_ADS7843_FINGER_CLICK_ERROR		18
#define GMOUSE_ADS7843_FINGER_MOVE_ERROR		14

// How much extra data to allocate at the end of the GMouse structure for the board's use
#define GMOUSE_ADS7843_BOARD_DATA_SIZE			0

extern gBool TOUCH_init_board();
static GFXINLINE gBool init_board(GMouse* m, unsigned driverinstance) {
    return TOUCH_init_board();
}

extern gBool TOUCH_getpin_pressed();
static GFXINLINE gBool getpin_pressed(GMouse* m) {
  return TOUCH_getpin_pressed();
}

extern void TOUCH_aquire_bus();
static GFXINLINE void aquire_bus(GMouse* m) {
  return TOUCH_aquire_bus();
}

extern void TOUCH_release_bus();
static GFXINLINE void release_bus(GMouse* m) {
  return TOUCH_release_bus();
}

extern gU16 TOUCH_read_value(gU16 reg);
static GFXINLINE gU16 read_value(GMouse* m, gU16 port) {
  return TOUCH_read_value(port);
}

#endif /* _GINPUT_LLD_MOUSE_BOARD_H */
