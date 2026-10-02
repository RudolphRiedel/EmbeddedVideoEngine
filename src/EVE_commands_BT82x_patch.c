/*
@file    EVE_commands_BT82x_patch.c
@brief   BT82x functions for patches
@version 6.0
@date    2026-10-02
@author  Rudolph Riedel

@section info

At least for Arm Cortex-M0 and Cortex-M4, the fastest observed execution is with -O2.
The c-standard is C99.

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
- split from EVE_commands_BT82x.c
- removed the ..._burst() functions

*/

#include "EVE.h"
#include "EVE_lib_private.h"

/* BT820 */
#if EVE_GEN > 4

#define DUMMY_BYTE ((uint8_t) 0x00U)
#define FIFO_BIT_MASK ((uint16_t)0x3fffU)


/* ##################################################################
    coprocessor commands that are not used in displays lists,
    most of these are not to be used with burst transfers
################################################################### */


/* the following commands require a patch loaded with CMD_LOADPATCH */

/**
 * @brief Write to an existing file on the SD card file system.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_fswrite(const uint32_t addr, const char * const p_name)
{
    eve_begin_cmd(CMD_FSWRITE);
    spi_transmit_32(addr);
    eve_private_string_write(p_name);
    return EVE_execute_cmd_and_get_result();
}

/**
 * @brief Add or resize files on the SD card file system.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_fsfile(const uint32_t size, const char * const p_name)
{
    eve_begin_cmd(CMD_FSFILE);
    spi_transmit_32(size);
    eve_private_string_write(p_name);
    return EVE_execute_cmd_and_get_result();
}

/**
 * @brief Write a bitmap screenshot to the SD card file system.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_fssnapshot(const uint32_t addr, const char * const p_name)
{
    eve_begin_cmd(CMD_FSSNAPSHOT);
    spi_transmit_32(addr);
    eve_private_string_write(p_name);
    return EVE_execute_cmd_and_get_result();
}

/**
 * @brief Write a portion of the screen to the SD card file system.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_fscropshot(const uint32_t addr, const char * const p_name, const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt)
{
    eve_begin_cmd(CMD_FSCROPSHOT);
    spi_transmit_32(addr);
    eve_private_string_write(p_name);
    spi_transmit_32(i16_i16_to_u32(xco, yco));
    spi_transmit_32(u16_u16_to_u32(wid, hgt));
    return EVE_execute_cmd_and_get_result();
}

/**
 * @brief Initialise RAM_G memory for reusable allocation by the coprocessor.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_memoryinit(const uint32_t addr, const uint32_t size)
{
    eve_begin_cmd(CMD_MEMORYINIT);
    spi_transmit_32(addr);
    spi_transmit_32(size);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Reserve a fixed size of RAM_G memory from the coprocessor.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_memorymalloc(const uint32_t size)
{
    eve_begin_cmd(CMD_MEMORYMALLOC);
    spi_transmit_32(size);
    return EVE_execute_cmd_and_get_result();
}

/**
 * @brief Free a previously reserved area of RAM_G memory from the coprocessor.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_memoryfree(const uint32_t addr, const uint32_t size)
{
    eve_begin_cmd(CMD_MEMORYFREE);
    spi_transmit_32(addr);
    spi_transmit_32(size);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Setup REG_LVDSRX_CTRL and REG_LVDSRX_SETUP.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_lvdssetup(const uint16_t setup, const uint16_t ctrl)
{
    eve_begin_cmd(CMD_LVDSSETUP);
    spi_transmit_32(u16_u16_to_u32(setup, ctrl));
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Test for an active LVDS connection to the LVDS RX channel.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_lvdsconn(void)
{
    eve_begin_cmd(CMD_LVDSCONN);
    return EVE_execute_cmd_and_get_result();
}

/**
 * @brief Stop LVDS decoding.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_lvdsstop(void)
{
    eve_begin_cmd(CMD_LVDSSTOP);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Activate LVDS decoding.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_lvdsstart(void)
{
    eve_begin_cmd(CMD_LVDSSTART);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Create a blurred copy of an image.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_blurimage(const uint32_t source, const uint32_t dest, const uint16_t format, const uint16_t width, const uint16_t height)
{
    eve_begin_cmd(CMD_BLURIMAGE);
    spi_transmit_32(source);
    spi_transmit_32(dest);
    spi_transmit_32(u16_u16_to_u32(format, width));
    spi_transmit_32(u16_u16_to_u32(height, 0U));
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Reserve an area of RAM_G memory from the coprocessor to fit a bitmap of specified size and format.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_memorybitmap(const uint16_t format, const uint16_t width, const uint16_t height, const uint16_t addn)
{
    eve_begin_cmd(CMD_MEMORYBITMAP);
    spi_transmit_32(u16_u16_to_u32(format, width));
    spi_transmit_32(u16_u16_to_u32(height, addn));
    return EVE_execute_cmd_and_get_result();
}

/**
 * @brief Calculate the exact on-screen width and height in pixels of a multiline message.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_textsize(const uint16_t font, const uint16_t options, const char * const p_text, uint16_t * const p_width, uint16_t * const p_height)
{
    uint32_t result;

    eve_begin_cmd(CMD_TEXTSIZE);
    spi_transmit_32(u16_u16_to_u32(font, options));
    eve_private_string_write(p_text);
    result = EVE_execute_cmd_and_get_result();

    if (p_width != NULL)
    {
        *p_width = (uint16_t)(result & 0xFFFFU);
    }

    if (p_height != NULL)
    {
        *p_height = (uint16_t)((result >> 16U) & 0xFFFFU);
    }
}

/**
 * @brief Stream data in BARGRAPH bitmap format into a buffer in RAM_G.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_plotbitmap(const uint32_t address, const uint16_t len, const uint16_t opt, const uint32_t handle, const uint8_t * const p_data)
{
    eve_begin_cmd(CMD_PLOTBITMAP);
    spi_transmit_32(address);
    spi_transmit_32(u16_u16_to_u32(len, opt));
    spi_transmit_32(handle);
    EVE_cs_clear();
    if (p_data != NULL)
    {
        eve_block_transfer(p_data, len);
    }
    else
    {
        EVE_execute_cmd();
    }
}

/**
 * @brief Erase a single block of NAND flash.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_nflasheraseblock(const uint32_t block)
{
    eve_begin_cmd(CMD_NFLASHERASEBLOCK);
    spi_transmit_32(block);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Report the status of a flash block to determine if it is marked as bad.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_nflashisblockbad(const uint32_t block)
{
    eve_begin_cmd(CMD_NFLASHISBLOCKBAD);
    spi_transmit_32(block);
    return EVE_execute_cmd_and_get_result();
}

/**
 * @brief Read the parameter page from the NAND device and copy it to an address in RAM_G.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_nflashparameters(const uint32_t addr)
{
    eve_begin_cmd(CMD_NFLASHPARAMETERS);
    spi_transmit_32(addr);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Report the status of the last flash operation.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_nflashstatus(void)
{
    eve_begin_cmd(CMD_NFLASHSTATUS);
    return EVE_execute_cmd_and_get_result();
}

/**
 * @brief Map a linear address in Flash into a Block and Page.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_nflashmap(const uint32_t addr)
{
    eve_begin_cmd(CMD_NFLASHMAP);
    spi_transmit_32(addr);
    return EVE_execute_cmd_and_get_result();
}

/**
 * @brief Mark a block as known bad.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_nflashsetblockbad(const uint32_t block)
{
    eve_begin_cmd(CMD_NFLASHSETBLOCKBAD);
    spi_transmit_32(block);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Scan an image without writing it to RAM_G.
 * @note - Use CMD_GETIMAGE or CMD_GETPROPS to access the results.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_queryimage(const uint32_t options, const uint8_t * const p_data, const uint32_t len)
{
    eve_begin_cmd(CMD_QUERYIMAGE);
    spi_transmit_32(options);
    EVE_cs_clear();

    if ((0UL == (options & EVE_OPT_MEDIAFIFO)) &&
        (0UL == (options & EVE_OPT_FLASH)) &&
        (0UL == (options & EVE_OPT_FS)) && /* direct data, neither by Media-FIFO or from Flash */
        (p_data != NULL))
    {
        eve_block_transfer(p_data, len);
    }
    else
    {
        EVE_execute_cmd();
    }
}

/**
 * @brief Scan a relocatable asset without writing it to RAM_G.
 * @note - Use CMD_GETIMAGE or CMD_GETPROPS to access the results.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_queryasset(const uint32_t options, const uint8_t * const p_data, const uint32_t len)
{
    eve_begin_cmd(CMD_QUERYASSET);
    spi_transmit_32(options);
    EVE_cs_clear();

    if ((0UL == (options & EVE_OPT_MEDIAFIFO)) &&
        (0UL == (options & EVE_OPT_FLASH)) &&
        (0UL == (options & EVE_OPT_FS)) && /* direct data, neither by Media-FIFO or from Flash */
        (p_data != NULL))
    {
        eve_block_transfer(p_data, len);
    }
    else
    {
        EVE_execute_cmd();
    }

}

/* ##################################################################
    functions for display lists
##################################################################### */

/* the following commands require a patch loaded with CMD_LOADPATCH */

/**
 * @brief Start a Region section.
 */
void EVE_cmd_region(void)
{
    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_REGION);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_REGION);
    }
}

/**
 * @brief Stop a Region section.
 */
void EVE_cmd_endregion(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt)
{
    const uint32_t param0 = i16_i16_to_u32(xco, yco);
    const uint32_t param1 = u16_u16_to_u32(wid, hgt);

    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_ENDREGION);
        spi_transmit_32(param0);
        spi_transmit_32(param1);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ENDREGION);
        spi_transmit_burst(param0);
        spi_transmit_burst(param1);
    }
}

/**
 * @brief Draw scaled text.
 */
void EVE_cmd_textscale(const int16_t xco, const int16_t yco, const uint16_t font, const uint16_t options, const uint32_t scale, const char * const p_text)
{
    const uint32_t param0 = i16_i16_to_u32(xco, yco);
    const uint32_t param1 = u16_u16_to_u32(font, options);

    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_TEXTSCALE);
        spi_transmit_32(param0);
        spi_transmit_32(param1);
        spi_transmit_32(scale);
        eve_private_string_write(p_text);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_TEXTSCALE);
        spi_transmit_burst(param0);
        spi_transmit_burst(param1);
        spi_transmit_burst(scale);
        eve_private_string_write_burst(p_text);
    }
}

/**
 * @brief Draw text at an angle.
 */
void EVE_cmd_textangle(const int16_t xco, const int16_t yco, const uint16_t font, const uint16_t options, const uint32_t angle, const char * const p_text)
{
    const uint32_t param0 = i16_i16_to_u32(xco, yco);
    const uint32_t param1 = u16_u16_to_u32(font, options);

    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_TEXTANGLE);
        spi_transmit_32(param0);
        spi_transmit_32(param1);
        spi_transmit_32(angle);
        eve_private_string_write(p_text);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_TEXTANGLE);
        spi_transmit_burst(param0);
        spi_transmit_burst(param1);
        spi_transmit_burst(angle);
        eve_private_string_write_burst(p_text);
    }
}

/**
 * @brief Draw text within a box and scroll the text smoothly.
 */
void EVE_cmd_textticker(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt, const uint16_t font, const uint16_t options, const uint32_t offset, const char * const p_text)
{
    const uint32_t param0 = i16_i16_to_u32(xco, yco);
    const uint32_t param1 = u16_u16_to_u32(wid, hgt);
    const uint32_t param2 = u16_u16_to_u32(font, options);

    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_TEXTTICKER);
        spi_transmit_32(param0);
        spi_transmit_32(param1);
        spi_transmit_32(param2);
        spi_transmit_32(offset);
        eve_private_string_write(p_text);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_TEXTTICKER);
        spi_transmit_burst(param0);
        spi_transmit_burst(param1);
        spi_transmit_burst(param2);
        spi_transmit_burst(offset);
        eve_private_string_write_burst(p_text);
    }
}

/**
 * @brief Draw a seven segment display for decimal numbers from 0 to 9.
 */
void EVE_cmd_sevenseg(const int16_t xc0, const int16_t yc0, const uint16_t size, const uint16_t number)
{
    const uint32_t param0 = i16_i16_to_u32(xc0, yc0);
    const uint32_t param1 = u16_u16_to_u32(size, number);

    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_SEVENSEG);
        spi_transmit_32(param0);
        spi_transmit_32(param1);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SEVENSEG);
        spi_transmit_burst(param0);
        spi_transmit_burst(param1);
    }
}

/**
 * @brief Display a multiline message box.
 */
void EVE_cmd_messagebox(const uint16_t font, const uint16_t options, const char * const p_text)
{
    const uint32_t param0 = u16_u16_to_u32(font, options);

    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_MESSAGEBOX);
        spi_transmit_32(param0);
        eve_private_string_write(p_text);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_MESSAGEBOX);
        spi_transmit_burst(param0);
        eve_private_string_write_burst(p_text);
    }
}

/**
 * @brief Display a multiline tooltip box.
 */
void EVE_cmd_tooltip(const int16_t xco, const int16_t yco, const uint16_t font, const uint16_t options, const char * const p_text)
{
    const uint32_t param0 = i16_i16_to_u32(xco, yco);
    const uint32_t param1 = u16_u16_to_u32(font, options);

    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_TOOLTIP);
        spi_transmit_32(param0);
        spi_transmit_32(param1);
        eve_private_string_write(p_text);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_TOOLTIP);
        spi_transmit_burst(param0);
        spi_transmit_burst(param1);
        eve_private_string_write_burst(p_text);
    }
}

/**
 * @brief Draw a keyboard or keypad.
 */
void EVE_cmd_keyboard(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt, const uint16_t font, const uint16_t options, const char * const p_text)
{
    const uint32_t param0 = i16_i16_to_u32(xco, yco);
    const uint32_t param1 = u16_u16_to_u32(wid, hgt);
    const uint32_t param2 = u16_u16_to_u32(font, options);

    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_KEYBOARD);
        spi_transmit_32(param0);
        spi_transmit_32(param1);
        spi_transmit_32(param2);
        eve_private_string_write(p_text);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_KEYBOARD);
        spi_transmit_burst(param0);
        spi_transmit_burst(param1);
        spi_transmit_burst(param2);
        eve_private_string_write_burst(p_text);
    }
}

/**
 * @brief Create a blurred image of the current screen.
 */
void EVE_cmd_blurscreen(void)
{
    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_BLURSCREEN);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_BLURSCREEN);
    }
}

/**
 * @brief Draw a previously blurred image of the screen generated by cmd_blurscreen.
 */
void EVE_cmd_blurdraw(void)
{
    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_BLURDRAW);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_BLURDRAW);
    }
}

/**
 * @brief Draws an LED-style graphic to simulate a round LED.
 */
void EVE_cmd_ledround(const int16_t xco, const int16_t yco, const uint16_t radius, const uint16_t options)
{
    const uint32_t param0 = i16_i16_to_u32(xco, yco);
    const uint32_t param1 = u16_u16_to_u32(radius, options);

    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_LEDROUND);
        spi_transmit_32(param0);
        spi_transmit_32(param1);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_LEDROUND);
        spi_transmit_burst(param0);
        spi_transmit_burst(param1);
    }
}

/**
 * @brief Draws an LED-style graphic to simulate a rectangular LED.
 */
void EVE_cmd_ledrect(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt, const uint16_t options)
{
    const uint32_t param0 = i16_i16_to_u32(xco, yco);
    const uint32_t param1 = u16_u16_to_u32(wid, hgt);
    const uint32_t param2 = u16_u16_to_u32(options, 0U);

    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_LEDRECT);
        spi_transmit_32(param0);
        spi_transmit_32(param1);
        spi_transmit_32(param2);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_LEDRECT);
        spi_transmit_burst(param0);
        spi_transmit_burst(param1);
        spi_transmit_burst(param2);
    }
}

/**
 * @brief Draws a feedback emoji.
 */
void EVE_cmd_feedbackicon(const int16_t xco, const int16_t yco, const uint16_t rad1, const uint16_t rad2, const int16_t sentiment)
{
    const uint32_t param0 = i16_i16_to_u32(xco, yco);
    const uint32_t param1 = u16_u16_to_u32(rad1, rad2);
    const uint32_t param2 = i16_i16_to_u32(sentiment, 0);

    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_FEEDBACKICON);
        spi_transmit_32(param0);
        spi_transmit_32(param1);
        spi_transmit_32(param2);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_FEEDBACKICON);
        spi_transmit_burst(param0);
        spi_transmit_burst(param1);
        spi_transmit_burst(param2);
    }
}

/**
 * @brief Change data for a BARGRAPH bitmap into a VERTEX2F points for a LINESTRIP.
 */
void EVE_cmd_plotdraw(const uint32_t source, const uint16_t len, const uint16_t opt, const int16_t xco, const int16_t yco, const uint32_t xscale, const uint32_t yscale, const uint32_t threshold)
{
    const uint32_t param0 = u16_u16_to_u32(len, opt);
    const uint32_t param1 = i16_i16_to_u32(xco, yco);

    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_PLOTDRAW);
        spi_transmit_32(source);
        spi_transmit_32(param0);
        spi_transmit_32(param1);
        spi_transmit_32(xscale);
        spi_transmit_32(yscale);
        spi_transmit_32(threshold);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_PLOTDRAW);
        spi_transmit_burst(source);
        spi_transmit_burst(param0);
        spi_transmit_burst(param1);
        spi_transmit_burst(xscale);
        spi_transmit_burst(yscale);
        spi_transmit_burst(threshold);
    }
}

/* BT82x max screen resolution is 2048 x 2048 regardless of pixel precision;
 * BARGRAPH indexes y-values by x-coordinate, so more input points than
 * screen width could ever be rendered are meaningless. */
#define PLOTSTREAM_MAX_LEN ((uint16_t) 2048U)

/**
 * @brief Stream data in BARGRAPH bitmap format into a VERTEX2F points for a LINESTRIP.
 */
void EVE_cmd_plotstream(const uint16_t len, const uint16_t opt, const int16_t xco, const int16_t yco, const uint32_t xscale, const uint32_t yscale, const uint32_t threshold, const uint8_t * const p_data)
{
    const uint16_t len_transfer = (len > PLOTSTREAM_MAX_LEN) ? PLOTSTREAM_MAX_LEN : len;
    const uint32_t param0 = u16_u16_to_u32(len_transfer, opt);
    const uint32_t param1 = i16_i16_to_u32(xco, yco);

    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_PLOTSTREAM);
        spi_transmit_32(param0);
        spi_transmit_32(param1);
        spi_transmit_32(xscale);
        spi_transmit_32(yscale);
        spi_transmit_32(threshold);

        if (p_data != NULL)
        {
            eve_private_block_write(p_data, len_transfer);
        }

        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_PLOTSTREAM);
        spi_transmit_burst(param0);
        spi_transmit_burst(param1);
        spi_transmit_burst(xscale);
        spi_transmit_burst(yscale);
        spi_transmit_burst(threshold);

        if (p_data != NULL)
        {
            eve_private_block_write_burst(p_data, len_transfer);
        }
    }
}

/**
 * @brief Apply an offset to the current touch coordinates.
 */
void EVE_cmd_touchoffset(const int16_t xco, const int16_t yco)
{
    const uint32_t param0 = i16_i16_to_u32(xco, yco);

    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_TOUCHOFFSET);
        spi_transmit_32(param0);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_TOUCHOFFSET);
        spi_transmit_burst(param0);
    }
}

/**
 * @brief Ends the touch offset mode started by CMD_TOUCHOFFSET
 */
void EVE_cmd_endtouchoffset(void)
{
    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_ENDTOUCHOFFSET);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ENDTOUCHOFFSET);
    }
}

// fixme - set more meaningfull parameter names when the docs are available
/**
 * @brief Turn x,y coordinates in RAM_G into VERTEX2F commands in the display list.
 */
void EVE_cmd_vxy(const uint32_t p1, const uint32_t p2, const uint32_t p3, const uint32_t p4)
{
    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_VXY);
        spi_transmit_32(p1);
        spi_transmit_32(p2);
        spi_transmit_32(p3);
        spi_transmit_32(p4);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_VXY);
        spi_transmit_burst(p1);
        spi_transmit_burst(p2);
        spi_transmit_burst(p3);
        spi_transmit_burst(p4);
    }
}


#endif
