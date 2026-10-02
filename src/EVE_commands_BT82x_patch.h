/*
@file    EVE_commands_BT82x_patch.h
@brief   BT82x macros and function prototypes for patches
@version 6.0
@date    2026-10-02
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2026 Rudolph Riedel

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
- split from EVE_commands_BT82x.h
- removed the ..._burst() functions


*/

#ifndef EVE_COMMANDS_BT82X_PATCH_H
#define EVE_COMMANDS_BT82X_PATCH_H


#if EVE_GEN > 4

/* from patch-dialogs */
#define EVE_OPT_MSGBGALPHA      ((uint16_t) 0x00FFU)
#define EVE_OPT_MSGTOP          ((uint16_t) 0x0200U)
#define EVE_OPT_MSGBOTTOM       ((uint16_t) 0x0400U)
#define EVE_OPT_MSGEDGE         ((uint16_t) 0x0800U)

/* from patch-keyboard */
#define EVE_KEY_RETURN          ((uint16_t) 0x000DU)
#define EVE_KEY_DEL             ((uint16_t) 0x0008U)
#define EVE_KEY_TAB             ((uint16_t) 0x0009U)
#define EVE_KEY_ESC             ((uint16_t) 0x001BU)
#define EVE_KEY_SHIFT           ((uint16_t) 0x0001U)
#define EVE_KEY_ALPHA           ((uint16_t) 0x0002U)
#define EVE_KEY_NUMBER          ((uint16_t) 0x0003U)
#define EVE_KEY_SYMBOLNUM       ((uint16_t) 0x0004U)
#define EVE_KEY_SYMBOLS         ((uint16_t) 0x0005U)
#define EVE_KEY_CAPS            ((uint16_t) 0x0006U)
#define EVE_KEY_ALT             ((uint16_t) 0x000BU)
#define EVE_KEY_CTRL            ((uint16_t) 0x000CU)
#define EVE_OPT_EXTEND_EDGE     ((uint16_t) 0x1000U)
#define EVE_OPT_NO_EXTEND_SPACE ((uint16_t) 0x2000U)
#define EVE_OPT_MAP_SPECIAL_KEYS ((uint16_t) 0x4000U)
#define EVE_OPT_INVERT_SPECIAL  ((uint16_t) 0x8000U)
#define EVE_OPT_PRESSED         ((uint16_t) 0x00FFU)

/* from patch-plotgraph */
#define EVE_OPT_PLOTFILTER      ((uint16_t) 0x2000U)
#define EVE_OPT_PLOTHORIZONTAL  ((uint16_t) 0x0000U)
#define EVE_OPT_PLOTVERTICAL    ((uint16_t) 0x1000U)
#define EVE_OPT_PLOTINVERT      ((uint16_t) 0x4000U)
#define EVE_OPT_PLOTOVERLAY     ((uint16_t) 0x8000U)
#define EVE_OPT_PLOTWIDTH       ((uint16_t) 0x000FU)

/* from patch-sevenseg */
#define EVE_OPT_DECIMAL         ((uint16_t) 0x0010U)
#define EVE_OPT_TIMECOLON       ((uint16_t) 0x0020U)
#define EVE_OPT_NUMBER          ((uint16_t) 0x000FU)
#define EVE_OPT_SEGMENTMASK     ((uint16_t) 0x2030U)


#define CMD_REGION          ((uint32_t) 0xFFFFFF8FUL)   /* base */
#define CMD_ENDREGION       ((uint32_t) 0xFFFFFF90UL)   /* base */
#define CMD_FSWRITE         ((uint32_t) 0xFFFFFF91UL)   /* fssnapshot / fswrite */
#define CMD_FSFILE          ((uint32_t) 0xFFFFFF92UL)   /* fssnapshot / fswrite */
#define CMD_FSSNAPSHOT      ((uint32_t) 0xFFFFFF93UL)   /* fssnapshot */
#define CMD_FSCROPSHOT      ((uint32_t) 0xFFFFFF94UL)   /* fssnapshot */
#define CMD_TEXTSCALE       ((uint32_t) 0xFFFFFF95UL)   /* textscale */
#define CMD_TEXTANGLE       ((uint32_t) 0xFFFFFF96UL)   /* textangle*/
#define CMD_TEXTTICKER      ((uint32_t) 0xFFFFFF97UL)   /* textticker */
#define CMD_SEVENSEG        ((uint32_t) 0xFFFFFF98UL)   /* sevenseg */
#define CMD_MESSAGEBOX      ((uint32_t) 0xFFFFFF99UL)   /* dialog */
#define CMD_TOOLTIP         ((uint32_t) 0xFFFFFF9AUL)   /* dialog */
#define CMD_KEYBOARD        ((uint32_t) 0xFFFFFF9BUL)   /* keyboard  */
#define CMD_MEMORYINIT      ((uint32_t) 0xFFFFFF9CUL)   /* memory */
#define CMD_MEMORYMALLOC    ((uint32_t) 0xFFFFFF9DUL)   /* memory */
#define CMD_MEMORYFREE      ((uint32_t) 0xFFFFFF9EUL)   /* memory */
#define CMD_LVDSSETUP       ((uint32_t) 0xFFFFFF9FUL)   /* lvds */
#define CMD_LVDSCONN        ((uint32_t) 0xFFFFFFA0UL)   /* lvds */
#define CMD_LVDSSTOP        ((uint32_t) 0xFFFFFFA1UL)   /* lvds */
#define CMD_LVDSSTART       ((uint32_t) 0xFFFFFFA2UL)   /* lvds */
#define CMD_BLURIMAGE       ((uint32_t) 0xFFFFFFA3UL)   /* blur */
#define CMD_BLURSCREEN      ((uint32_t) 0xFFFFFFA4UL)   /* blur */
#define CMD_BLURDRAW        ((uint32_t) 0xFFFFFFA5UL)   /* blur */
#define CMD_LEDROUND        ((uint32_t) 0xFFFFFFA6UL)   /* led */
#define CMD_LEDRECT         ((uint32_t) 0xFFFFFFA7UL)   /* led */
#define CMD_FEEDBACKICON    ((uint32_t) 0xFFFFFFA8UL)   /* feedbackicon */
#define CMD_MEMORYBITMAP    ((uint32_t) 0xFFFFFFA9UL)   /* memory */
#define CMD_TEXTSIZE        ((uint32_t) 0xFFFFFFAAUL)   /* dialog */
#define CMD_PLOTDRAW        ((uint32_t) 0xFFFFFFABUL)   /* plotgraph */
#define CMD_PLOTSTREAM      ((uint32_t) 0xFFFFFFACUL)   /* plotgraph */
#define CMD_PLOTBITMAP      ((uint32_t) 0xFFFFFFADUL)   /* plotgraph */
#define CMD_TOUCHOFFSET     ((uint32_t) 0xFFFFFFAEUL)   /* base */
#define CMD_ENDTOUCHOFFSET  ((uint32_t) 0xFFFFFFAFUL)   /* base */


#define CMD_NFLASHERASEBLOCK    ((uint32_t) 0xFFFFFFB0UL)   /* nand */
#define CMD_NFLASHISBLOCKBAD    ((uint32_t) 0xFFFFFFB1UL)   /* nand */
#define CMD_NFLASHPARAMETERS    ((uint32_t) 0xFFFFFFB2UL)   /* nand */
#define CMD_NFLASHSTATUS        ((uint32_t) 0xFFFFFFB3UL)   /* nand */
#define CMD_NFLASHMAP           ((uint32_t) 0xFFFFFFB4UL)   /* nand */
#define CMD_NFLASHSETBLOCKBAD   ((uint32_t) 0xFFFFFFB5UL)   /* nand */


#define CMD_VXY             ((uint32_t) 0xFFFFFFBBUL)   /* base */
#define CMD_QUERYIMAGE      ((uint32_t) 0xFFFFFFBCUL)   /* queryassets */
#define CMD_QUERYASSET      ((uint32_t) 0xFFFFFFBDUL)   /* queryassets */



/* ##################################################################
    commands and functions to be used outside of display-lists
##################################################################### */

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

void EVE_cmd_nflasheraseblock(const uint32_t block);
uint32_t EVE_cmd_nflashisblockbad(const uint32_t block);
void EVE_cmd_nflashparameters(const uint32_t addr);
uint32_t EVE_cmd_nflashstatus(void);
uint32_t EVE_cmd_nflashmap(const uint32_t addr);
void EVE_cmd_nflashsetblockbad(const uint32_t block);

void EVE_cmd_queryimage(const uint32_t options, const uint8_t * const p_data, const uint32_t len);
void EVE_cmd_queryasset(const uint32_t options, const uint8_t * const p_data, const uint32_t len);


/* ##################################################################
    command co-processor functions for display lists
##################################################################### */

void EVE_cmd_region(void);
void EVE_cmd_endregion(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt);
void EVE_cmd_textscale(const int16_t xco, const int16_t yco, const uint16_t font, const uint16_t options, const uint32_t scale, const char * const p_text);
void EVE_cmd_textangle(const int16_t xco, const int16_t yco, const uint16_t font, const uint16_t options, const uint32_t angle, const char * const p_text);
void EVE_cmd_textticker(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt, const uint16_t font, const uint16_t options, uint32_t offset, const char * const p_text);
void EVE_cmd_sevenseg(const int16_t xc0, const int16_t yc0, const uint16_t size, const uint16_t number);
void EVE_cmd_messagebox(const uint16_t font, const uint16_t options, const char * const p_text);
void EVE_cmd_tooltip(const int16_t xco, const int16_t yco, const uint16_t font, const uint16_t options, const char * const p_text);
void EVE_cmd_keyboard(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt, const uint16_t font, const uint16_t options, const char * const p_text);
void EVE_cmd_blurscreen(void);
void EVE_cmd_blurdraw(void);
void EVE_cmd_ledround(const int16_t xco, const int16_t yco, const uint16_t radius, const uint16_t options);
void EVE_cmd_ledrect(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt, const uint16_t options);
void EVE_cmd_feedbackicon(const int16_t xco, const int16_t yco, const uint16_t rad1, const uint16_t rad2, const int16_t sentiment);
void EVE_cmd_plotdraw(const uint32_t source, const uint16_t len, const uint16_t opt, const int16_t xco, const int16_t yco, const uint32_t xscale, const uint32_t yscale, const uint32_t threshold);
void EVE_cmd_plotstream(const uint16_t len, const uint16_t opt, const int16_t xco, const int16_t yco, const uint32_t xscale, const uint32_t yscale, const uint32_t threshold, const uint8_t * const p_data);
void EVE_cmd_touchoffset(const int16_t xco, const int16_t yco);
void EVE_cmd_endtouchoffset(void);

void EVE_cmd_vxy(const uint32_t p1, const uint32_t p2, const uint32_t p3, const uint32_t p4);

#endif

#endif /* EVE_COMMANDS_BT82X_PATCH_H */
