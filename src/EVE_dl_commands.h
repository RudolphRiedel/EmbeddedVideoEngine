/*
@file    EVE_dl_commands.h
@brief   contains FT8xx / BT8xx display list function prototypes
@version 6.0
@date    2025-09-20
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


*/

#ifndef EVE_DL_COMMANDS_H
#define EVE_DL_COMMANDS_H

#include "EVE.h"

extern volatile uint8_t g_cmd_burst; /* flag from EVE_commands.c to indicate cmd-burst is active */

/* ##################################################################
    display list command functions for use with the coprocessor
##################################################################### */

#if EVE_GEN > 2

void EVE_bitmap_ext_format(const uint16_t format);
void EVE_bitmap_ext_format_burst(const uint16_t format);
void EVE_bitmap_swizzle(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha);
void EVE_bitmap_swizzle_burst(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha);

#endif /* EVE_GEN > 2 */

void EVE_cmd_dl(const uint32_t command);
void EVE_cmd_dl_burst(const uint32_t command);

void EVE_alpha_func(const uint8_t func, const uint8_t ref);
void EVE_alpha_func_burst(const uint8_t func, const uint8_t ref);
void EVE_begin(const uint32_t prim);
void EVE_begin_burst(const uint32_t prim);
void EVE_bitmap_handle(const uint8_t handle);
void EVE_bitmap_handle_burst(const uint8_t handle);
void EVE_bitmap_layout(const uint8_t format, const uint16_t linestride, const uint16_t height);
void EVE_bitmap_layout_burst(const uint8_t format, const uint16_t linestride, const uint16_t height);
void EVE_bitmap_layout_h(const uint16_t linestride, const uint16_t height);
void EVE_bitmap_layout_h_burst(const uint16_t linestride, const uint16_t height);
void EVE_bitmap_size(const uint8_t filter, const uint8_t wrapx, const uint8_t wrapy, const uint16_t width, const uint16_t height);
void EVE_bitmap_size_burst(const uint8_t filter, const uint8_t wrapx, const uint8_t wrapy, const uint16_t width, const uint16_t height);
void EVE_bitmap_size_h(const uint16_t width, const uint16_t height);
void EVE_bitmap_size_h_burst(const uint16_t width, const uint16_t height);
void EVE_bitmap_source(const uint32_t addr);
void EVE_bitmap_source_burst(const uint32_t addr);
void EVE_blend_func(const uint8_t src, const uint8_t dst);
void EVE_blend_func_burst(const uint8_t src, const uint8_t dst);
void EVE_call(const uint16_t dest);
void EVE_call_burst(const uint16_t dest);
void EVE_cell(const uint8_t cell);
void EVE_cell_burst(const uint8_t cell);
void EVE_clear(const uint8_t color, const uint8_t stencil, const uint8_t tag);
void EVE_clear_burst(const uint8_t color, const uint8_t stencil, const uint8_t tag);
void EVE_clear_color_a(const uint8_t alpha);
void EVE_clear_color_a_burst(const uint8_t alpha);
void EVE_clear_color_rgb(const uint32_t color);
void EVE_clear_color_rgb_burst(const uint32_t color);
void EVE_clear_stencil(const uint8_t val);
void EVE_clear_stencil_burst(const uint8_t val);
void EVE_clear_tag(const uint8_t val);
void EVE_clear_tag_burst(const uint8_t val);
void EVE_color_rgb(const uint32_t color);
void EVE_color_rgb_burst(const uint32_t color);
void EVE_color_a(const uint8_t alpha);
void EVE_color_a_burst(const uint8_t alpha);
void EVE_color_mask(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha);
void EVE_color_mask_burst(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha);
void EVE_display(void);
void EVE_display_burst(void);
void EVE_end(void);
void EVE_end_burst(void);
void EVE_jump(const uint16_t dest);
void EVE_jump_burst(const uint16_t dest);
void EVE_line_width(const uint16_t width);
void EVE_line_width_burst(const uint16_t width);
void EVE_macro(const uint8_t macro);
void EVE_macro_burst(const uint8_t macro);
void EVE_nop(void);
void EVE_nop_burst(void);
void EVE_palette_source(const uint32_t addr);
void EVE_palette_source_burst(const uint32_t addr);
void EVE_point_size(const uint16_t size);
void EVE_point_size_burst(const uint16_t size);
void EVE_restore_context(void);
void EVE_restore_context_burst(void);
void EVE_return(void);
void EVE_return_burst(void);
void EVE_save_context(void);
void EVE_save_context_burst(void);
void EVE_scissor_size(const uint16_t width, const uint16_t height);
void EVE_scissor_size_burst(const uint16_t width, const uint16_t height);
void EVE_scissor_xy(const uint16_t xc0, const uint16_t yc0);
void EVE_scissor_xy_burst(const uint16_t xc0, const uint16_t yc0);
void EVE_stencil_func(const uint8_t func, const uint8_t ref, const uint8_t mask);
void EVE_stencil_func_burst(const uint8_t func, const uint8_t ref, const uint8_t mask);
void EVE_stencil_mask(const uint8_t mask);
void EVE_stencil_mask_burst(const uint8_t mask);
void EVE_stencil_op(const uint8_t sfail, const uint8_t spass);
void EVE_stencil_op_burst(const uint8_t sfail, const uint8_t spass);
void EVE_tag(const uint8_t tag);
void EVE_tag_burst(const uint8_t tag);
void EVE_tag_mask(const uint8_t mask);
void EVE_tag_mask_burst(const uint8_t mask);
void EVE_vertex2f(const int16_t xc0, const int16_t yc0);
void EVE_vertex2f_burst(const int16_t xc0, const int16_t yc0);
void EVE_vertex2ii(const uint16_t xc0, const uint16_t yc0, const uint8_t handle, const uint8_t cell);
void EVE_vertex2ii_burst(const uint16_t xc0, const uint16_t yc0, const uint8_t handle, const uint8_t cell);
void EVE_vertex_format(const uint8_t frac);
void EVE_vertex_format_burst(const uint8_t frac);
void EVE_vertex_translate_x(const int32_t xco);
void EVE_vertex_translate_x_burst(const int32_t xco);
void EVE_vertex_translate_y(const int32_t yco);
void EVE_vertex_translate_y_burst(const int32_t yco);

#endif /* EVE_DL_COMMANDS_H */
