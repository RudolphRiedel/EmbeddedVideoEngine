/*
@file    EVE_commands_BT82x.h
@brief   contains BT82x function prototypes
@version 6.0
@date    2025-09-21
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2025 Rudolph Riedel

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

@section History

6.0
- split from EVE_commands.h
- implemented the remaining BT82x extension commands

*/

#ifndef EVE_COMMANDS_BT82X_H
#define EVE_COMMANDS_BT82X_H

#include "EVE.h"

extern volatile uint8_t g_cmd_burst; /* flag from EVE_commands.c to indicate cmd-burst is active */

/* ##################################################################
    commands and functions to be used outside of display-lists
##################################################################### */

/* BT820 */
#if EVE_GEN > 4

void EVE_cmd_copylist(uint32_t dest);
void EVE_cmd_ddrshutdown(void);
void EVE_cmd_ddrstartup(void);
void EVE_cmd_fsoptions(const uint32_t options);
uint32_t EVE_cmd_fsdir(const uint32_t dest, const uint32_t num, const char * const p_path);
uint32_t EVE_cmd_fsread(const uint32_t dest, const char * const p_path);
uint32_t EVE_cmd_fssize(const char * const p_name);
uint32_t EVE_cmd_fssource(const char * const p_name);
void EVE_cmd_i2sstartup(uint32_t freq);
void EVE_cmd_inflate(const uint32_t ptr, const uint32_t options, const uint8_t * const p_data, const uint32_t len);
void EVE_cmd_loadasset(const uint32_t ptr, const uint32_t options, const uint8_t * const p_data, const uint32_t len);
void EVE_cmd_loadpatch(const uint32_t options, const uint8_t * const p_data, const uint32_t len);
void EVE_cmd_loadwav(const uint32_t ptr, const uint32_t options, const uint8_t * const p_data, const uint32_t len);
void EVE_cmd_playwav(const uint32_t ptr, const uint32_t options, const uint8_t * const p_data, const uint32_t len);
void EVE_cmd_rendertarget(const uint32_t dest, const uint16_t format, const uint16_t wid, const uint16_t hgt);
uint32_t EVE_cmd_sdattach(const uint32_t options);
uint32_t EVE_cmd_sdblockread(const uint32_t dest, const uint32_t source, const uint32_t num);
void EVE_cmd_textdim(const uint32_t ptr, const uint16_t font, const uint16_t options, const char * const p_text);
void EVE_cmd_videostart(const uint32_t options);

/* the following commands require a patch loaded with CMD_LOADPATCH */
uint32_t EVE_cmd_fswrite(const uint32_t addr, const char * const p_name);
uint32_t EVE_cmd_fsfile(const uint32_t size, const char * const p_name);
uint32_t EVE_cmd_fssnapshot(const uint32_t addr, const char * const p_name);
uint32_t EVE_cmd_fscropshot(const uint32_t addr, const char * const p_name, const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt);
void EVE_cmd_memoryinit(const uint32_t addr, const uint32_t size);
uint32_t EVE_cmd_memorymalloc(const uint32_t size);
void EVE_cmd_memoryfree(const uint32_t addr, const uint32_t size);
void EVE_cmd_lvdssetup(const uint16_t setup, const uint16_t ctrl);
uint32_t EVE_cmd_lvdsconn(void);
void EVE_cmd_lvdsstop(void);
void EVE_cmd_lvdsstart(void);
void EVE_cmd_blurimage(const uint32_t source, const uint32_t dest, const uint16_t format, const uint16_t width, const uint16_t height);
uint32_t EVE_cmd_memorybitmap(const uint16_t format, const uint16_t width, const uint16_t height, const uint16_t addn);
void EVE_cmd_textsize(const uint16_t font, const uint16_t options, const char * const p_text, uint16_t * const p_width, uint16_t * const p_height);
void EVE_cmd_plotbitmap(const uint32_t address, const uint16_t len, const uint16_t opt, const uint32_t handle, const uint8_t * const p_data);


/* ##################################################################
    command co-processor functions for display lists
##################################################################### */

void EVE_cmd_arc(const int16_t xc0, const int16_t yc0, const uint16_t rad0, const uint16_t rad1, const uint16_t angle0, const uint16_t angle1);
void EVE_cmd_arc_burst(const int16_t xc0, const int16_t yc0, const uint16_t rad0, const uint16_t rad1, const uint16_t angle0, const uint16_t angle1);
void EVE_cmd_cgradient(const uint32_t shape, const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt, const uint32_t rgb0, const uint32_t rgb1);
void EVE_cmd_cgradient_burst(const uint32_t shape, const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt, const uint32_t rgb0, const uint32_t rgb1);
void EVE_cmd_enableregion(const uint32_t enable);
void EVE_cmd_enableregion_burst(const uint32_t enable);
void EVE_cmd_fence(void);
void EVE_cmd_fence_burst(void);
void EVE_cmd_glow(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt);
void EVE_cmd_glow_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt);
void EVE_cmd_graphicsfinish(void);
void EVE_cmd_graphicsfinish_burst(void);
void EVE_cmd_result(const uint32_t dest);
void EVE_cmd_result_burst(const uint32_t dest);
void EVE_cmd_regwrite(const uint32_t dest, const uint32_t value);
void EVE_cmd_regwrite_burst(const uint32_t dest, const uint32_t value);
void EVE_cmd_restorecontext(void);
void EVE_cmd_restorecontext_burst(void);
void EVE_cmd_savecontext_burst(void);
void EVE_cmd_savecontext(void);
void EVE_cmd_setfont(const uint32_t font, const uint32_t ptr, const uint32_t firstchar);
void EVE_cmd_setfont_burst(const uint32_t font, const uint32_t ptr, const uint32_t firstchar);
void EVE_cmd_skipcond(const uint32_t adr, const uint32_t func, const uint32_t ref, const uint32_t mask, const uint32_t num);
void EVE_cmd_skipcond_burst(const uint32_t adr, const uint32_t func, const uint32_t ref, const uint32_t mask, const uint32_t num);
void EVE_cmd_waitchange(const uint32_t adr);
void EVE_cmd_waitchange_burst(const uint32_t adr);
void EVE_cmd_waitcond(const uint32_t adr, const uint32_t func, const uint32_t ref, const uint32_t mask);
void EVE_cmd_waitcond_burst(const uint32_t adr, const uint32_t func, const uint32_t ref, const uint32_t mask);
void EVE_cmd_watchdog(const uint32_t init_val);
void EVE_cmd_watchdog_burst(const uint32_t init_val);

/* the following commands require a patch loaded with CMD_LOADPATCH */
void EVE_cmd_region(void);
void EVE_cmd_region_burst(void);
void EVE_cmd_endregion(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt);
void EVE_cmd_endregion_burst(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt);
void EVE_cmd_textscale(const int16_t xco, const int16_t yco, const uint16_t font, const uint16_t options, const uint32_t scale, const char * const p_text);
void EVE_cmd_textscale_burst(const int16_t xco, const int16_t yco, const uint16_t font, const uint16_t options, const uint32_t scale, const char * const p_text);
void EVE_cmd_textangle(const int16_t xco, const int16_t yco, const uint16_t font, const uint16_t options, const uint32_t angle, const char * const p_text);
void EVE_cmd_textangle_burst(const int16_t xco, const int16_t yco, const uint16_t font, const uint16_t options, const uint32_t angle, const char * const p_text);
void EVE_cmd_textticker(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt, const uint16_t font, const uint16_t options, uint32_t offset, const char * const p_text);
void EVE_cmd_textticker_burst(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt, const uint16_t font, const uint16_t options, const uint32_t offset, const char * const p_text);
void EVE_cmd_sevenseg(const int16_t xc0, const int16_t yc0, const uint16_t size, const uint16_t number);
void EVE_cmd_sevenseg_burst(const int16_t xc0, const int16_t yc0, const uint16_t size, const uint16_t number);
void EVE_cmd_messagebox(const uint16_t font, const uint16_t options, const char * const p_text);
void EVE_cmd_messagebox_burst(const uint16_t font, const uint16_t options, const char * const p_text);
void EVE_cmd_tooltip(const int16_t xco, const int16_t yco, const uint16_t font, const uint16_t options, const char * const p_text);
void EVE_cmd_tooltip_burst(const int16_t xco, const int16_t yco, const uint16_t font, const uint16_t options, const char * const p_text);
void EVE_cmd_keyboard(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt, const uint16_t font, const uint16_t options, const char * const p_text);
void EVE_cmd_keyboard_burst(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt, const uint16_t font, const uint16_t options, const char * const p_text);
void EVE_cmd_blurscreen(void);
void EVE_cmd_blurscreen_burst(void);
void EVE_cmd_blurdraw(void);
void EVE_cmd_blurdraw_burst(void);
void EVE_cmd_ledround(const int16_t xco, const int16_t yco, const uint16_t radius, const uint16_t options);
void EVE_cmd_ledround_burst(const int16_t xco, const int16_t yco, const uint16_t radius, const uint16_t options);
void EVE_cmd_ledrect(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt, const uint16_t options);
void EVE_cmd_ledrect_burst(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt, const uint16_t options);
void EVE_cmd_feedbackicon(const int16_t xco, const int16_t yco, const uint16_t rad1, const uint16_t rad2, const int16_t sentiment);
void EVE_cmd_feedbackicon_burst(const int16_t xco, const int16_t yco, const uint16_t rad1, const uint16_t rad2, const int16_t sentiment);
void EVE_cmd_plotdraw(const uint32_t source, const uint16_t len, const uint16_t opt, const int16_t xco, const int16_t yco, const uint32_t xscale, const uint32_t yscale, const uint32_t threshold);
void EVE_cmd_plotdraw_burst(const uint32_t source, const uint16_t len, const uint16_t opt, const int16_t xco, const int16_t yco, const uint32_t xscale, const uint32_t yscale, const uint32_t threshold);
void EVE_cmd_plotstream(const uint16_t len, const uint16_t opt, const int16_t xco, const int16_t yco, const uint32_t xscale, const uint32_t yscale, const uint32_t threshold, const uint8_t * const p_data);
void EVE_cmd_plotstream_burst(const uint16_t len, const uint16_t opt, const int16_t xco, const int16_t yco, const uint32_t xscale, const uint32_t yscale, const uint32_t threshold,  const uint8_t * const p_data);
void EVE_cmd_touchoffset(const int16_t xco, const int16_t yco);
void EVE_cmd_touchoffset_burst(const int16_t xco, const int16_t yco);
void EVE_cmd_endtouchoffset(void);
void EVE_cmd_endtouchoffset_burst(void);


#endif

#endif /* EVE_COMMANDS_BT82X_H */
