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

extern void init_board(GDisplay* g);
extern void post_init_board(GDisplay* g);
extern void setpin_reset(GDisplay* g, gBool state);
extern void set_backlight(GDisplay* g, gU8 percent);
extern void acquire_bus(GDisplay* g);
extern void release_bus(GDisplay* g);
extern void write_index(GDisplay* g, gU16 index);
extern void write_data(GDisplay* g, gU16 data);
extern void setreadmode(GDisplay* g);
extern void setwritemode(GDisplay* g);
extern gU16 read_data(GDisplay* g);



#ifdef __cplusplus
}
#endif

#endif /* _GDISP_LLD_BOARD_H */