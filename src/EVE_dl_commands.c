/*
@file    EVE_dl_commands.c
@brief   contains FT8xx / BT8xx display list functions
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

#include "EVE_dl_commands.h"

/* ##################################################################
    display list command functions for use with the coprocessor
##################################################################### */

#if EVE_GEN > 2

/**
 * @brief Specify the extended format of the bitmap.
 */
void EVE_bitmap_ext_format(const uint16_t format)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(BITMAP_EXT_FORMAT(format));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(BITMAP_EXT_FORMAT(format));
    }
}

/**
 * @brief Specify the extended format of the bitmap, only works in burst-mode.
 */
void EVE_bitmap_ext_format_burst(const uint16_t format)
{
    spi_transmit_burst(BITMAP_EXT_FORMAT(format));
}

/**
 * @brief Set the source for the red, green, blue and alpha channels of a bitmap.
 */
void EVE_bitmap_swizzle(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(BITMAP_SWIZZLE(red, green, blue, alpha));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(BITMAP_SWIZZLE(red, green, blue, alpha));
    }
}

/**
 * @brief Set the source for the red, green, blue and alpha channels of a bitmap, only works in burst-mode.
 */
void EVE_bitmap_swizzle_burst(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha)
{
    spi_transmit_burst(BITMAP_SWIZZLE(red, green, blue, alpha));
}

#endif /* EVE_GEN > 2 */

/**
 * @brief Generic function for display-list and coprocessor commands with no arguments.
 * @note - EVE_cmd_dl(CMD_DLSTART);
 * @note - EVE_cmd_dl(CMD_SWAP);
 * @note - EVE_cmd_dl(CMD_SCREENSAVER);
 * @note - EVE_cmd_dl(VERTEX2F(0,0));
 * @note - EVE_cmd_dl(DL_BEGIN | EVE_RECTS);
 * @note - use when keeping the binary size small is more important than beeing close to ESE / BRT_AN025
 */
void EVE_cmd_dl(const uint32_t command)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(command);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(command);
    }
}

/**
 * @brief Generic function for display-list and coprocessor commands with no arguments, only works in burst-mode.
 */
void EVE_cmd_dl_burst(const uint32_t command)
{
    spi_transmit_burst(command);
}

/**
 * @brief Specify the alpha test function.
 */
void EVE_alpha_func(const uint8_t func, const uint8_t ref)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(ALPHA_FUNC(func, ref));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(ALPHA_FUNC(func, ref));
    }
}

/**
 * @brief Specify the alpha test function, only works in burst-mode.
 */
void EVE_alpha_func_burst(const uint8_t func, const uint8_t ref)
{
    spi_transmit_burst(ALPHA_FUNC(func, ref));
}

/**
 * @brief Begin drawing a graphics primitive.
 */
void EVE_begin(const uint32_t prim)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(DL_BEGIN | prim);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_BEGIN | prim);
    }
}

/**
 * @brief Begin drawing a graphics primitive, only works in burst-mode.
 */
void EVE_begin_burst(const uint32_t prim)
{
    spi_transmit_burst(DL_BEGIN | prim);
}

/**
 * @brief Specify the bitmap handle.
 */
void EVE_bitmap_handle(const uint8_t handle)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(BITMAP_HANDLE(handle));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(BITMAP_HANDLE(handle));
    }
}

/**
 * @brief Specify the bitmap handle, only works in burst-mode.
 */
void EVE_bitmap_handle_burst(const uint8_t handle)
{
    spi_transmit_burst(BITMAP_HANDLE(handle));
}

/**
 * @brief Specify the source bitmap memory format and layout for the current handle.
 */
void EVE_bitmap_layout(const uint8_t format, const uint16_t linestride, const uint16_t height)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(BITMAP_LAYOUT(format , linestride, height));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(BITMAP_LAYOUT(format , linestride, height));
    }
}

/**
 * @brief Specify the source bitmap memory format and layout for the current handle, only works in burst-mode.
 * @note this is different to FTDIs implementation as this takes the original values as parameters and not only the upper bits
 */
void EVE_bitmap_layout_burst(const uint8_t format, const uint16_t linestride, const uint16_t height)
{
    spi_transmit_burst(BITMAP_LAYOUT(format , linestride, height));
}

/**
 * @brief Specify the 2 most significant bits of the source bitmap memory format and layout for the current handle.
 * @note this is different to FTDIs implementation as this takes the original values as parameters and not only the upper bits
 */
void EVE_bitmap_layout_h(const uint16_t linestride, const uint16_t height)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(BITMAP_LAYOUT_H(linestride, height));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(BITMAP_LAYOUT_H(linestride, height));
    }
}

/**
 * @brief Specify the 2 most significant bits of the source bitmap memory format and layout for the current handle, only works in burst-mode.
 */
void EVE_bitmap_layout_h_burst(const uint16_t linestride, const uint16_t height)
{
    spi_transmit_burst(BITMAP_LAYOUT_H(linestride, height));
}

/**
 * @brief Specify the screen drawing of bitmaps for the current handle.
 */
void EVE_bitmap_size(const uint8_t filter, const uint8_t wrapx, const uint8_t wrapy, const uint16_t width, const uint16_t height)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(BITMAP_SIZE(filter, wrapx, wrapy, width, height));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(BITMAP_SIZE(filter, wrapx, wrapy, width, height));
    }
}

/**
 * @brief Specify the screen drawing of bitmaps for the current handle, only works in burst-mode.
 */
void EVE_bitmap_size_burst(const uint8_t filter, const uint8_t wrapx, const uint8_t wrapy, const uint16_t width, const uint16_t height)
{
    spi_transmit_burst(BITMAP_SIZE(filter, wrapx, wrapy, width, height));
}

/**
 * @brief Specify the 2 most significant bits of bitmaps dimension for the current handle.
 * @note this is different to FTDIs implementation as this takes the original values as parameters and not only the upper bits
 */
void EVE_bitmap_size_h(const uint16_t width, const uint16_t height)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(BITMAP_SIZE_H(width, height));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(BITMAP_SIZE_H(width, height));
    }
}

/**
 * @brief Specify the 2 most significant bits of bitmaps dimension for the current handle, only works in burst-mode.
 * @note this is different to FTDIs implementation as this takes the original values as parameters and not only the upper bits
 */
void EVE_bitmap_size_h_burst(const uint16_t width, const uint16_t height)
{
    spi_transmit_burst(BITMAP_SIZE_H(width, height));
}

/**
 * @brief Specify the source address of bitmap data.
 */
void EVE_bitmap_source(const uint32_t addr)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(BITMAP_SOURCE(addr));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(BITMAP_SOURCE(addr));
    }
}

/**
 * @brief Specify the source address of bitmap data, only works in burst-mode.
 */
void EVE_bitmap_source_burst(const uint32_t addr)
{
    spi_transmit_burst(BITMAP_SOURCE(addr));
}

/**
 * @brief Specify how new color values are combined with the values already in the color buffer.
 */
void EVE_blend_func(const uint8_t src, const uint8_t dst)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(BLEND_FUNC(src, dst));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(BLEND_FUNC(src, dst));
    }
}

/**
 * @brief Specify how new color values are combined with the values already in the color buffer, only works in burst-mode.
 */
void EVE_blend_func_burst(const uint8_t src, const uint8_t dst)
{
    spi_transmit_burst(BLEND_FUNC(src, dst));
}

/**
 * @brief Execute a sequence of commands at another location in the display list.
 * @note valid range for dest is from zero to 2047
 */
void EVE_call(const uint16_t dest)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CALL(dest));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CALL(dest));
    }
}

/**
 * @brief Execute a sequence of commands at another location in the display list, only works in burst-mode.
 * @note valid range for dest is from zero to 2047
 */
void EVE_call_burst(const uint16_t dest)
{
    spi_transmit_burst(CALL(dest));
}

/**
 * @brief Set the bitmap cell number for the VERTEX2F command.
 */
void EVE_cell(const uint8_t cell)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CELL(cell));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CELL(cell));
    }
}

/**
 * @brief Set the bitmap cell number for the VERTEX2F command, only works in burst-mode.
 */
void EVE_cell_burst(const uint8_t cell)
{
    spi_transmit_burst(CELL(cell));
}

/**
 * @brief Clear buffers to preset values.
 */
void EVE_clear(const uint8_t color, const uint8_t stencil, const uint8_t tag)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CLEAR(color, stencil, tag));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CLEAR(color, stencil, tag));
    }
}

/**
 * @brief Clear buffers to preset values, only works in burst-mode.
 */
void EVE_clear_burst(const uint8_t color, const uint8_t stencil, const uint8_t tag)
{
    spi_transmit_burst(CLEAR(color, stencil, tag));
}

/**
 * @brief Set clear value for the alpha channel.
 */
void EVE_clear_color_a(const uint8_t alpha)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CLEAR_COLOR_A(alpha));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CLEAR_COLOR_A(alpha));
    }
}

/**
 * @brief Set clear value for the alpha channel, only works in burst-mode.
 */
void EVE_clear_color_a_burst(const uint8_t alpha)
{
    spi_transmit_burst(CLEAR_COLOR_A(alpha));
}

/**
 * @brief Specify clear values for red, green and blue channels.
 */
void EVE_clear_color_rgb(const uint32_t color)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(DL_CLEAR_COLOR_RGB | (color & 0x00ffffffUL));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_CLEAR_COLOR_RGB | (color & 0x00ffffffUL));
    }
}

/**
 * @brief Specify clear values for red, green and blue channels, only works in burst-mode.
 */
void EVE_clear_color_rgb_burst(const uint32_t color)
{
    spi_transmit_burst(DL_CLEAR_COLOR_RGB | (color & 0x00ffffffUL));
}

/**
 * @brief Set clear value for the stencil buffer.
 */
void EVE_clear_stencil(const uint8_t val)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CLEAR_STENCIL(val));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CLEAR_STENCIL(val));
    }
}

/**
 * @brief Set clear value for the stencil buffer, only works in burst-mode.
 */
void EVE_clear_stencil_burst(const uint8_t val)
{
    spi_transmit_burst(CLEAR_STENCIL(val));
}

/**
 * @brief Set clear value for the tag buffer.
 */
void EVE_clear_tag(const uint8_t val)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CLEAR_TAG(val));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CLEAR_TAG(val));
    }
}

/**
 * @brief Set clear value for the tag buffer, only works in burst-mode.
 */
void EVE_clear_tag_burst(const uint8_t val)
{
    spi_transmit_burst(CLEAR_TAG(val));
}


/**
 * @brief Set the current color red, green and blue.
 */
void EVE_color_rgb(const uint32_t color)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(DL_COLOR_RGB | (color & 0x00ffffffUL));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_COLOR_RGB | (color & 0x00ffffffUL));
    }
}

/**
 * @brief Set the current color red, green and blue, only works in burst-mode.
 */
void EVE_color_rgb_burst(const uint32_t color)
{
    spi_transmit_burst(DL_COLOR_RGB | (color & 0x00ffffffUL));
}

/**
 * @brief Set the current color alpha, green and blue.
 */
void EVE_color_a(const uint8_t alpha)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(DL_COLOR_A | ((uint32_t) alpha));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_COLOR_A | ((uint32_t) alpha));
    }
}

/**
 * @brief Set the current color alpha, green and blue, only works in burst-mode.
 */
void EVE_color_a_burst(const uint8_t alpha)
{
    spi_transmit_burst(DL_COLOR_A | ((uint32_t) alpha));
}

/**
 * @brief Enable or disable writing of color components.
 */
void EVE_color_mask(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(COLOR_MASK(red, green, blue, alpha));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(COLOR_MASK(red, green, blue, alpha));
    }
}

/**
 * @brief Enable or disable writing of color components, only works in burst-mode.
 */
void EVE_color_mask_burst(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha)
{
    spi_transmit_burst(COLOR_MASK(red, green, blue, alpha));
}

/**
 * @brief End the display list.
 */
void EVE_display(void)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(DL_DISPLAY);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_DISPLAY);
    }
}

/**
 * @brief End the display list, only works in burst-mode.
 */
void EVE_display_burst(void)
{
    spi_transmit_burst(DL_DISPLAY);
}

/**
 * @brief End drawing a graphics primitive.
 */
void EVE_end(void)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(DL_END);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_END);
    }
}

/**
 * @brief End drawing a graphics primitive, only works in burst-mode.
 */
void EVE_end_burst(void)
{
    spi_transmit_burst(DL_END);
}

/**
 * @brief Execute commands at another location in the display list.
 * @note valid range for dest is from zero to 2047
 */
void EVE_jump(const uint16_t dest)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(JUMP(dest));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(JUMP(dest));
    }
}

/**
 * @brief Execute commands at another location in the display list.
 * @note valid range for dest is from zero to 2047
 */
void EVE_jump_burst(const uint16_t dest)
{
    spi_transmit_burst(JUMP(dest));
}

/**
 * @brief Specify the width of lines to be drawn with primitive LINES in 1/16 pixel precision.
 */
void EVE_line_width(const uint16_t width)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(LINE_WIDTH(width));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(LINE_WIDTH(width));
    }
}

/**
 * @brief Specify the width of lines to be drawn with primitive LINES in 1/16 pixel precision, only works in burst-mode.
 */
void EVE_line_width_burst(const uint16_t width)
{
    spi_transmit_burst(LINE_WIDTH(width));
}

/**
 * @brief Execute a single command from a macro register.
 * @param macro Macro registers to read. 0 for REG_MACRO_0, 1 for REG_MACRO_1.
 */
void EVE_macro(const uint8_t macro)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(MACRO(macro));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(MACRO(macro));
    }
}

/**
 * @brief Execute a single command from a macro register, only works in burst-mode.
 * @param macro Macro registers to read. 0 for REG_MACRO_0, 1 for REG_MACRO_1.
 */
void EVE_macro_burst(const uint8_t macro)
{
    spi_transmit_burst(MACRO(macro));
}

/**
 * @brief No operation.
 * @note Does nothing. May be used as a spacer in display lists, if required.
 */
void EVE_nop(void)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(DL_NOP);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_NOP);
    }
}

/**
 * @brief No operation, only works in burst-mode.
 */
void EVE_nop_burst(void)
{
    spi_transmit_burst(DL_NOP);
}

/**
 * @brief Set the base address of the palette.
 * @note 2-byte alignment is required if pixel format is PALETTE4444 or PALETTE565.
 */
void EVE_palette_source(const uint32_t addr)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(PALETTE_SOURCE(addr));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(PALETTE_SOURCE(addr));
    }
}

/**
 * @brief Set the base address of the palette, only works in burst-mode.
 */
void EVE_palette_source_burst(const uint32_t addr)
{
    spi_transmit_burst(PALETTE_SOURCE(addr));
}

/**
 * @brief Specify the radius of points in 1/16 pixel precision.
 */
void EVE_point_size(const uint16_t size)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(POINT_SIZE(size));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(POINT_SIZE(size));
    }
}

/**
 * @brief Specify the radius of points in 1/16 pixel precision, only works in burst-mode.
 */
void EVE_point_size_burst(const uint16_t size)
{
    spi_transmit_burst(POINT_SIZE(size));
}

/**
 * @brief Restore the current graphics context from the context stack.
 */
void EVE_restore_context(void)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(DL_RESTORE_CONTEXT);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_RESTORE_CONTEXT);
    }
}

/**
 * @brief Restore the current graphics context from the context stack, only works in burst-mode.
 */
void EVE_restore_context_burst(void)
{
    spi_transmit_burst(DL_RESTORE_CONTEXT);
}

/**
 * @brief Return from a previous CALL command..
 */
void EVE_return(void)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(DL_RETURN);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_RETURN);
    }
}

/**
 * @brief Return from a previous CALL command..
 */
void EVE_return_burst(void)
{
    spi_transmit_burst(DL_RETURN);
}

/**
 * @brief Push the current graphics context on the context stack.
 */
void EVE_save_context(void)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(DL_SAVE_CONTEXT);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_SAVE_CONTEXT);
    }
}

/**
 * @brief Push the current graphics context on the context stack, only works in burst-mode.
 */
void EVE_save_context_burst(void)
{
    spi_transmit_burst(DL_SAVE_CONTEXT);
}

/**
 * @brief Set the size of the scissor clip rectangle.
 * @note valid range for width and height is from zero to 2048
 */
void EVE_scissor_size(const uint16_t width, const uint16_t height)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(SCISSOR_SIZE(width, height));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(SCISSOR_SIZE(width, height));
    }
}

/**
 * @brief Set the size of the scissor clip rectangle, only works in burst-mode.
 */
void EVE_scissor_size_burst(const uint16_t width, const uint16_t height)
{
    spi_transmit_burst(SCISSOR_SIZE(width, height));
}

/**
 * @brief Specify the top left corner of the scissor clip rectangle.
 * @note valid range for width and height is from zero to 2047
 */
void EVE_scissor_xy(const uint16_t xc0, const uint16_t yc0)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(SCISSOR_XY(xc0, yc0));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(SCISSOR_XY(xc0, yc0));
    }
}

/**
 * @brief Specify the top left corner of the scissor clip rectangle, only works in burst-mode.
 */
void EVE_scissor_xy_burst(const uint16_t xc0, const uint16_t yc0)
{
    spi_transmit_burst(SCISSOR_XY(xc0, yc0));
}

/**
 * @brief Set function and reference value for stencil testing.
 */
void EVE_stencil_func(const uint8_t func, const uint8_t ref, const uint8_t mask)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(STENCIL_FUNC(func, ref, mask));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(STENCIL_FUNC(func, ref, mask));
    }
}

/**
 * @brief Set function and reference value for stencil testing, only works in burst-mode.
 */
void EVE_stencil_func_burst(const uint8_t func, const uint8_t ref, const uint8_t mask)
{
    spi_transmit_burst(STENCIL_FUNC(func, ref, mask));
}

/**
 * @brief Control the writing of individual bits in the stencil planes.
 */
void EVE_stencil_mask(const uint8_t mask)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(STENCIL_MASK(mask));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(STENCIL_MASK(mask));
    }
}

/**
 * @brief Control the writing of individual bits in the stencil planes, only works in burst-mode.
 */
void EVE_stencil_mask_burst(const uint8_t mask)
{
    spi_transmit_burst(STENCIL_MASK(mask));
}

/**
 * @brief Set stencil test actions.
 */
void EVE_stencil_op(const uint8_t sfail, const uint8_t spass)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(STENCIL_OP(sfail, spass));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(STENCIL_OP(sfail, spass));
    }
}

/**
 * @brief Set stencil test actions, only works in burst-mode.
 */
void EVE_stencil_op_burst(const uint8_t sfail, const uint8_t spass)
{
    spi_transmit_burst(STENCIL_OP(sfail, spass));
}

/**
 * @brief Attach the tag value for the following graphics objects drawn on the screen.
 */
void EVE_tag(const uint8_t tag)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(DL_TAG | tag);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_TAG | tag);
    }
}

/**
 * @brief Attach the tag value for the following graphics objects drawn on the screen, only works in burst-mode.
 */
void EVE_tag_burst(const uint8_t tag)
{
    spi_transmit_burst(DL_TAG | tag);
}

/**
 * @brief Control the writing of the tag buffer.
 */
void EVE_tag_mask(const uint8_t mask)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(TAG_MASK(mask));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(TAG_MASK(mask));
    }
}

/**
 * @brief Control the writing of the tag buffer, only works in burst-mode.
 */
void EVE_tag_mask_burst(const uint8_t mask)
{
    spi_transmit_burst(TAG_MASK(mask));
}

/**
 * @brief Set coordinates for graphics primitve.
 */
void EVE_vertex2f(const int16_t xc0, const int16_t yc0)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(VERTEX2F(xc0, yc0));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(VERTEX2F(xc0, yc0));
    }
}

/**
 * @brief Set coordinates for graphics primitve, only works in burst-mode.
 */
void EVE_vertex2f_burst(const int16_t xc0, const int16_t yc0)
{
    spi_transmit_burst(VERTEX2F(xc0, yc0));
}

/**
 * @brief Set coordinates for graphics primitve.
 */
void EVE_vertex2ii(const uint16_t xc0, const uint16_t yc0, const uint8_t handle, const uint8_t cell)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(VERTEX2II(xc0, yc0, handle, cell));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(VERTEX2II(xc0, yc0, handle, cell));
    }
}

/**
 * @brief Set coordinates for graphics primitve, only works in burst-mode.
 */
void EVE_vertex2ii_burst(const uint16_t xc0, const uint16_t yc0, const uint8_t handle, const uint8_t cell)
{
    spi_transmit_burst(VERTEX2II(xc0, yc0, handle, cell));
}

/**
 * @brief Set the precision of VERTEX2F coordinates.
 */
void EVE_vertex_format(const uint8_t frac)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(VERTEX_FORMAT(frac));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(VERTEX_FORMAT(frac));
    }
}

/**
 * @brief Set the precision of VERTEX2F coordinates, only works in burst-mode.
 */
void EVE_vertex_format_burst(const uint8_t frac)
{
    spi_transmit_burst(VERTEX_FORMAT(frac));
}

/**
 * @brief Set the vertex transformations X translation component.
 */
void EVE_vertex_translate_x(const int32_t xco)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(VERTEX_TRANSLATE_X(xco));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(VERTEX_TRANSLATE_X(xco));
    }
}

/**
 * @brief Set the vertex transformations X translation component, only works in burst-mode.
 */
void EVE_vertex_translate_x_burst(const int32_t xco)
{
    spi_transmit_burst(VERTEX_TRANSLATE_X(xco));
}

/**
 * @brief Set the vertex transformations Y translation component.
 */
void EVE_vertex_translate_y(const int32_t yco)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(VERTEX_TRANSLATE_Y(yco));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(VERTEX_TRANSLATE_Y(yco));
    }
}

/**
 * @brief Set the vertex transformations Y translation component, only works in burst-mode.
 */
void EVE_vertex_translate_y_burst(const int32_t yco)
{
    spi_transmit_burst(VERTEX_TRANSLATE_Y(yco));
}
