/*
@file    EVE_commands_BT82x
@brief   BT82 functions
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
- split from EVE_commands.c
- implemented the remaining BT82x extension commands
- split eve_private_string_write() and made the new eve_private_string_write_burst() about 20% faster
- compliance: fixed linter warnings
- added touch patch for VM820B10A

*/

#include "EVE.h"
#include "EVE_lib_private.h"

/* BT820 */
#if EVE_GEN > 4

#define DUMMY_BYTE ((uint8_t) 0x00U)
#define FIFO_BIT_MASK ((uint16_t)0x3fffU)
#define MEM_WRITE ((uint32_t) 0x80000000UL) /* EVE Host Memory Write */


/* ##################################################################
    helper functions
##################################################################### */

/**
 * @brief Send a host command.
 */
void EVE_cmdWrite(uint8_t const command, uint8_t const parameter)
{
    EVE_cs_set();
    if (EVE_ACTIVE == command)
    {
        spi_transmit(0x00);
        spi_transmit(0x00);
        spi_transmit(0x00);
    }
    else
    {
      spi_transmit(0xFF);
      spi_transmit(command);
      spi_transmit(parameter);
    }

    spi_transmit(0x00);
    spi_transmit(0x00);

    EVE_cs_clear();
}

#define READ_TIMEOUT ((uint8_t) 0x10U)

/**
 * @brief Implementation of rd8() function, reads 8 bits.
 * @note the address must be 4 byte aligned, the last two bits are ignored
 */
uint8_t EVE_memRead8(uint32_t const ft_address)
{
    uint8_t data;
    uint8_t timeout;

    EVE_cs_set();
    spi_transmit_32_addr(ft_address);

    /* BT82x read protocoll: read data untill either 0x01 is returned or a timeout is reached */
    for (timeout = 0U; timeout < READ_TIMEOUT; timeout++)
    {
        data = spi_receive(DUMMY_BYTE);

        if (0x01 == data)
        {
            break;
        }
    }

    if (timeout < READ_TIMEOUT)
    {
        data = spi_receive(DUMMY_BYTE); /* read data byte by sending dummy byte */
    }
    else
    {
        data = 0x00; // issue?, how to indicate to the calling function that reading has failed?
    }

    EVE_cs_clear();
    return (data);
}

/**
 * @brief Implementation of rd16() function, reads 16 bits.
 * @note the address must be 4 byte aligned, the last two bits are ignored
 */
uint16_t EVE_memRead16(uint32_t const ft_address)
{
    uint16_t data;
    uint8_t timeout;

    EVE_cs_set();
    spi_transmit_32_addr(ft_address);

    for (timeout = 0U; timeout < READ_TIMEOUT; timeout++)
    {
        data = spi_receive(DUMMY_BYTE);

        if (0x01 == data)
        {
            break;
        }
    }

    /* BT82x read protocoll: read data untill either 0x01 is returned or a timeout is reached */
    if (timeout < READ_TIMEOUT)
    {
        uint8_t const lowbyte = spi_receive(DUMMY_BYTE); /* read low byte */
        uint8_t const hibyte = spi_receive(DUMMY_BYTE); /* read high byte */
        data = ((uint16_t) hibyte * 256U) | lowbyte;
    }
    else
    {
        data = 0x00; // issue?, how to indicate to the calling function that reading has failed?
    }

    EVE_cs_clear();
    return (data);
}

/**
 * @brief Implementation of rd32() function, reads 32 bits.
 */
uint32_t EVE_memRead32(uint32_t const ft_address)
{
    uint32_t data;
    uint8_t timeout;

    EVE_cs_set();
    spi_transmit_32_addr(ft_address);

    /* BT82x read protocoll: read data untill either 0x01 is returned or a timeout is reached */
    for (timeout = 0U; timeout < READ_TIMEOUT; timeout++)
    {
        data = spi_receive(DUMMY_BYTE);

        if (0x01 == data)
        {
            break;
        }
    }

    if (timeout < READ_TIMEOUT)
    {
        data = ((uint32_t) spi_receive(DUMMY_BYTE)); /* read low byte */
        data = ((uint32_t) spi_receive(DUMMY_BYTE) << 8U) | data;
        data = ((uint32_t) spi_receive(DUMMY_BYTE) << 16U) | data;
        data = ((uint32_t) spi_receive(DUMMY_BYTE) << 24U) | data; /* read high byte */
    }
    else
    {
        data = 0x00; // issue?, how to indicate to the calling function that reading has failed?
    }

    EVE_cs_clear();
    return (data);
}

/* note: EVE 5 does only support 32 bit writes */

/**
 * @brief Implementation of wr32() function, writes 32 bits.
 */
void EVE_memWrite32(uint32_t const ft_address, uint32_t const ft_data)
{
    EVE_cs_set();
    spi_transmit_32_addr(MEM_WRITE | ft_address);
    spi_transmit_32(ft_data);
    EVE_cs_clear();
}

/**
 * @brief Helper function, write a block of memory from the FLASH of the host controller to EVE.
 * @ note: for EVE 5 the size must be a multiple of 4
 */
void EVE_memWrite_flash_buffer(uint32_t const ft_address, const uint8_t * const p_data, uint32_t const len)
{
    if (p_data != NULL)
    {
        EVE_cs_set();
        spi_transmit_32_addr(MEM_WRITE | ft_address);

        for (uint32_t count = 0U; count < len; count++)
        {
            spi_transmit(fetch_flash_byte(&p_data[count]));
        }

        uint8_t padding;

        padding = (uint8_t) (len & 3U); /* 0, 1, 2 or 3 */
        padding = 4U - padding;         /* 4, 3, 2 or 1 */
        padding &= 3U;                  /* 3, 2, 1 or 0 */

        while (padding > 0U)
        {
            spi_transmit(0U);
            padding--;
        }

        EVE_cs_clear();
    }
}

/**
 * @brief Helper function, write a block of memory from the SRAM of the host controller to EVE.
 * @ note: for EVE 5 the size must be a multiple of 4
 */
void EVE_memWrite_sram_buffer(uint32_t const ft_address, const uint8_t * const p_data, uint32_t const len)
{
    if (p_data != NULL)
    {
        EVE_cs_set();
        spi_transmit_32_addr(MEM_WRITE | ft_address);

        for (uint32_t count = 0U; count < len; count++)
        {
            spi_transmit(p_data[count]);
        }

        uint8_t padding;

        padding = (uint8_t) (len & 3U); /* 0, 1, 2 or 3 */
        padding = 4U - padding;         /* 4, 3, 2 or 1 */
        padding &= 3U;                  /* 3, 2, 1 or 0 */

        while (padding > 0U)
        {
            spi_transmit(0U);
            padding--;
        }

        EVE_cs_clear();
    }
}

/**
 * @brief Helper function, read a block of memory from EVE to the SRAM of the host controller.
 * @note the address must be 4 byte aligned, the last two bits are ignored
 * @ note: make sure the buffer is large enough!
 */
void EVE_memRead_sram_buffer(uint32_t const ft_address, uint8_t * const p_data, uint32_t const len)
{
    uint8_t timeout;
    uint8_t data;

    if (p_data != NULL)
    {
        EVE_cs_set();
        spi_transmit_32_addr(ft_address);

        /* BT82x read protocoll: read data untill either 0x01 is returned or a timeout is reached */
        for (timeout = 0U; timeout < READ_TIMEOUT; timeout++)
        {
            data = spi_receive(DUMMY_BYTE);

            if (0x01 == data)
            {
                break;
            }
        }

        if (timeout < READ_TIMEOUT)
        {
            for (uint32_t count = 0U; count < len; count++)
            {
                p_data[count] = spi_receive(DUMMY_BYTE); /* read data byte by sending dummy bytes */
            }
        }

        EVE_cs_clear();
    }
}


/* ##################################################################
    coprocessor commands that are not used in displays lists,
    most of these are not to be used with burst transfers
################################################################### */

/**
 * @brief Copies the current display list to RAM_G.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_copylist(uint32_t dest)
{
    eve_begin_cmd(CMD_COPYLIST);
    spi_transmit_32(dest);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Deactive the DDR interface in preparation to enter SLEEP state.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_ddrshutdown(void)
{
    eve_begin_cmd(CMD_DDRSHUTDOWN);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Activate the DDR interface to bring DDR out of SLEEP state.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_ddrstartup(void)
{
    eve_begin_cmd(CMD_DDRSTARTUP);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Configures options affecting the behaviour of the FAT subsystem.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_fsoptions(const uint32_t options)
{
    eve_begin_cmd(CMD_FSOPTIONS);
    spi_transmit_32(options);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Write a list of the files in a SDcard directory to memory.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_fsdir(const uint32_t dest, const uint32_t num, const char * const p_path)
{
    eve_begin_cmd(CMD_FSDIR);
    spi_transmit_32(dest);
    spi_transmit_32(num);
    eve_private_string_write(p_path);
    return EVE_execute_cmd_and_get_result();
}

/**
 * @brief Reads the named file into RAM_G.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_fsread(const uint32_t dest, const char * const p_name)
{
    eve_begin_cmd(CMD_FSREAD);
    spi_transmit_32(dest);
    eve_private_string_write(p_name);
    return EVE_execute_cmd_and_get_result();
}

/**
 * @brief Returns the size of the named file, in bytes.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_fssize(const char * const p_name)
{
    eve_begin_cmd(CMD_FSSIZE);
    eve_private_string_write(p_name);
    return EVE_execute_cmd_and_get_result();
}

/**
 * @brief Set source file for a future load.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_fssource(const char * const p_name)
{
    eve_begin_cmd(CMD_FSSOURCE);
    eve_private_string_write(p_name);
    return EVE_execute_cmd_and_get_result();
}

/**
 * @brief Fills half the I2S output FIFO with zeroes, writes the given frequency to REG_I2S_FREQ and enables I2S by writing 1 to REG_I2S_EN.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_i2sstartup(uint32_t freq)
{
    eve_begin_cmd(CMD_I2SSTARTUP);
    spi_transmit_32(freq);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Decompress data into RAM_G.
 * @note - The data must be correct and complete.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_inflate(const uint32_t ptr, const uint32_t options, const uint8_t * const p_data, const uint32_t len)
{
    eve_begin_cmd(CMD_INFLATE);
    spi_transmit_32(ptr);
    spi_transmit_32(options);
    EVE_cs_clear();

    if ((0UL == options) && /* direct data, not by Media-FIFO, Flash or SD */
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
 * @brief Loads an asset in .reloc format to the given address.
 * @note - The data must be correct and complete.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_loadasset(const uint32_t ptr, const uint32_t options, const uint8_t * const p_data, const uint32_t len)
{
    eve_begin_cmd(CMD_LOADASSET);
    spi_transmit_32(ptr);
    spi_transmit_32(options);
    EVE_cs_clear();

    if ((0UL == options) && /* direct data, not by Media-FIFO, Flash or SD */
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
 * @brief Loads and decodes a JPEG/PNG image into RAM_G.
 * @note - Decoding PNG images takes significantly more time than decoding JPEG images.
 * @note - In doubt use the EVE Asset Builder to check if PNG/JPEG files are compatible.
 * @note - If the image is in PNG format, the top 42kiB of RAM_G will be overwritten.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_loadimage(const uint32_t ptr, const uint32_t options, const uint8_t * const p_data, const uint32_t len)
{
    eve_begin_cmd(CMD_LOADIMAGE);
    spi_transmit_32(ptr);
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
 * @brief Loads a patch file to provide updates to the EVE firmware or new features.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_loadpatch(const uint32_t options, const uint8_t * const p_data, const uint32_t len)
{
    eve_begin_cmd(CMD_LOADPATCH);
    spi_transmit_32(options);
    EVE_cs_clear();

   if ((0UL == options) && /* direct data, not by Media-FIFO, Flash or SD */
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
 * @brief Loads a WAV file into memory so that it can be played or looped asynchronously.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_loadwav(const uint32_t ptr, const uint32_t options, const uint8_t * const p_data, const uint32_t len)
{
    eve_begin_cmd(CMD_LOADWAV);
    spi_transmit_32(ptr);
    spi_transmit_32(options);
    EVE_cs_clear();

   if ((0UL == options) && /* direct data, not by Media-FIFO, Flash or SD */
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
 * @brief Play back an audio sample.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_playwav(const uint32_t ptr, const uint32_t options, const uint8_t * const p_data, const uint32_t len)
{
    eve_begin_cmd(CMD_PLAYWAV);
    spi_transmit_32(ptr);
    spi_transmit_32(options);
    EVE_cs_clear();

   if ((0UL == options) && /* direct data, not by Media-FIFO, Flash or SD */
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
 * @brief Set REG_RE_DEST, REG_RE_FORMAT, REG_RE_W and REG_RE_H.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_rendertarget(const uint32_t dest, const uint16_t format, const uint16_t wid, const uint16_t hgt)
{
    eve_begin_cmd(CMD_RENDERTARGET);
    spi_transmit_32(dest);
    spi_transmit_32(u16_u16_to_u32(format, wid));
    spi_transmit_32(u16_u16_to_u32(hgt, 0U));
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Try to connect to an attached SD card or EMMC.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_sdattach(const uint32_t options)
{
    eve_begin_cmd(CMD_SDATTACH);
    spi_transmit_32(options);
    return EVE_execute_cmd_and_get_result();
}

/**
 * @brief Read 512-byte blocks from SD into main memory.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_sdblockread(const uint32_t dest, const uint32_t source, const uint32_t num)
{
    eve_begin_cmd(CMD_SDBLOCKREAD);
    spi_transmit_32(dest);
    spi_transmit_32(source);
    spi_transmit_32(num);
    return EVE_execute_cmd_and_get_result();
}

/**
 * @brief Compute the size of a UTF-8 text.
 * @note - The data must be correct and complete.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_textdim(const uint32_t ptr, const uint16_t font, const uint16_t options, const char * const p_text)
{
    eve_begin_cmd(CMD_TEXTDIM);
    spi_transmit_32(ptr);
    spi_transmit_32(u16_u16_to_u32(font, options));
    eve_private_string_write(p_text);
    EVE_cs_clear();
}

/**
 * @brief Initialize video frame decoder for video provided according to options.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_videostart(const uint32_t options)
{
    eve_begin_cmd(CMD_VIDEOSTART);
    spi_transmit_32(options);
    EVE_cs_clear();
    EVE_execute_cmd();
}


/* ##################################################################
    init functions
##################################################################### */

#if defined (EVE_PATCH_TOUCH)

#if defined (__AVR__)
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif

static const uint8_t touch_patch[2908] PROGMEM =
{
    0x7c, 0xda, 0x00, 0x50, 0x00, 0x20, 0x54, 0x0b, 0x78, 0x9c, 0xed, 0x57, 0x7d, 0x74, 0x54, 0xc5, 0x15, 0x7f, 0xfb, 0x66, 0xbf, 0xb3, 0x09, 0xfb, 0x5a, 0xd4, 0x50, 0xd9, 0x40, 0x39, 0x24, 0xeb,
    0x07, 0x5f, 0x6a, 0x20, 0x22, 0x14, 0xe7, 0xed, 0xee, 0x7b, 0x64, 0x43, 0x83, 0x5f, 0x54, 0xd4, 0xc3, 0xd2, 0x5d, 0x93, 0x90, 0xc4, 0x84, 0x24, 0x86, 0xf7, 0x36, 0x60, 0x95, 0x4e, 0x14, 0x95,
    0xc2, 0x96, 0x84, 0x4f, 0x63, 0x14, 0x8c, 0x94, 0x83, 0x5a, 0x21, 0x72, 0x28, 0x16, 0x4e, 0x45, 0xd8, 0x17, 0x3e, 0xa2, 0x6e, 0x5c, 0xa5, 0x34, 0x04, 0xca, 0x01, 0xb7, 0x2a, 0x8a, 0x9e, 0x56,
    0x72, 0xaa, 0xe2, 0x17, 0xf8, 0x7a, 0x67, 0xde, 0x4b, 0xe1, 0xd8, 0xfe, 0xd5, 0xff, 0x7a, 0xce, 0xce, 0x9e, 0x99, 0xfb, 0x9b, 0x7b, 0xef, 0xcc, 0x9b, 0xb9, 0xf7, 0xce, 0x9d, 0x59, 0xae, 0x83,
    0x3b, 0x29, 0x24, 0xa7, 0xaf, 0xe5, 0x4e, 0x06, 0x93, 0x48, 0xe3, 0x5a, 0x57, 0x86, 0x1c, 0xca, 0xe1, 0x90, 0x47, 0xf9, 0x7a, 0x8a, 0xae, 0x6f, 0x0f, 0x79, 0x9a, 0x62, 0x4a, 0x45, 0xcd, 0x2f,
    0x95, 0x46, 0xb5, 0xa2, 0x66, 0x9a, 0x9e, 0x48, 0xea, 0xfa, 0x87, 0x8c, 0x6f, 0xbd, 0x61, 0xc2, 0x14, 0xa3, 0xff, 0xae, 0x55, 0xd7, 0xb9, 0x53, 0x5b, 0xd7, 0x70, 0x27, 0x9b, 0x92, 0xdc, 0x51,
    0xee, 0x34, 0xea, 0xb1, 0x14, 0x3c, 0x74, 0xdb, 0xd6, 0x3b, 0x26, 0x91, 0xc4, 0x1d, 0x3c, 0x69, 0x3b, 0x50, 0x3d, 0xe7, 0x0a, 0x65, 0x4a, 0x59, 0x6e, 0x66, 0x7b, 0xe8, 0xc7, 0x77, 0x85, 0x46,
    0x2b, 0xcd, 0xb1, 0x86, 0x45, 0x0b, 0xaa, 0x9a, 0x47, 0x2b, 0xb5, 0x0b, 0xab, 0x1a, 0x55, 0xa5, 0x3f, 0xc4, 0x93, 0x49, 0x64, 0xbd, 0xa0, 0xeb, 0x17, 0x43, 0x42, 0x8f, 0x90, 0xe4, 0xba, 0xb8,
    0x53, 0x93, 0x30, 0x27, 0x51, 0xca, 0xf5, 0xae, 0xc9, 0xd5, 0xf5, 0x82, 0x70, 0x41, 0x78, 0x8c, 0xb2, 0xc9, 0x5a, 0xd9, 0x4a, 0xbf, 0x90, 0xe8, 0xbb, 0x4e, 0x11, 0x92, 0xbd, 0x01, 0x94, 0x04,
    0xaa, 0x4d, 0xc0, 0xdc, 0x61, 0xda, 0xa2, 0x03, 0x5f, 0x9d, 0x43, 0xda, 0x7d, 0x76, 0xaf, 0xc6, 0x91, 0x18, 0x36, 0xda, 0xd5, 0x4e, 0x5d, 0x97, 0x09, 0xb7, 0x03, 0xa5, 0xff, 0xca, 0x8f, 0x98,
    0x5f, 0x8b, 0x17, 0x14, 0xbc, 0xc8, 0x5b, 0xb9, 0xe8, 0xad, 0xd2, 0x52, 0xf7, 0x95, 0x36, 0x32, 0x13, 0xe3, 0x42, 0x14, 0x13, 0xea, 0xdd, 0x38, 0x5c, 0x28, 0xd4, 0xcb, 0x85, 0xd7, 0x5a, 0x12,
    0xd1, 0x1a, 0x7d, 0x5b, 0xc8, 0xaf, 0x6d, 0x0b, 0x9d, 0x2e, 0xf7, 0xa5, 0xda, 0x7a, 0xb8, 0x76, 0xd4, 0x33, 0xc8, 0xeb, 0x7a, 0x50, 0x91, 0x15, 0xca, 0xf5, 0xa5, 0xbc, 0xda, 0xf3, 0xe2, 0x0b,
    0x21, 0x94, 0x6c, 0xeb, 0x41, 0xe4, 0xd3, 0x76, 0x94, 0xbe, 0x18, 0x29, 0x55, 0x4a, 0x95, 0x91, 0x2f, 0x20, 0x12, 0x85, 0x5e, 0x13, 0x71, 0xcc, 0x1f, 0x0b, 0xb8, 0x0b, 0xf0, 0x48, 0xc0, 0x37,
    0xed, 0xe2, 0x89, 0xf7, 0x71, 0x94, 0x9e, 0xaa, 0x8c, 0x03, 0xe4, 0x7e, 0x9a, 0xa2, 0x12, 0x40, 0xab, 0x3a, 0x86, 0xd0, 0x16, 0x8a, 0x8a, 0xb7, 0x6d, 0xe0, 0x4e, 0x1e, 0xb7, 0xe8, 0x3a, 0xa5,
    0x60, 0xc1, 0xbe, 0x2e, 0xa0, 0x6d, 0x3d, 0x46, 0xdf, 0x89, 0x47, 0x62, 0xca, 0xf3, 0x12, 0x8e, 0xcc, 0xc1, 0x42, 0x92, 0xcf, 0x44, 0x15, 0x6a, 0x73, 0xbc, 0xd6, 0xd0, 0xbd, 0x5f, 0xa1, 0x3e,
    0xb3, 0x92, 0x05, 0x8a, 0x13, 0xec, 0xeb, 0x6c, 0x8c, 0x57, 0x35, 0x2f, 0xa8, 0x6f, 0x6c, 0xd1, 0xfb, 0x43, 0x89, 0x24, 0x22, 0xdc, 0x61, 0x83, 0xef, 0x52, 0x1b, 0x2a, 0x0d, 0x41, 0x7f, 0xe8,
    0x7e, 0x85, 0x8e, 0xc3, 0xcc, 0xdb, 0x6d, 0xda, 0xa0, 0xb4, 0xb0, 0xf8, 0x63, 0xe9, 0x13, 0xc9, 0x81, 0xf9, 0x4c, 0x22, 0xd9, 0x2c, 0xb9, 0x33, 0xa1, 0x76, 0x63, 0xe6, 0xe9, 0x26, 0x75, 0x0d,
    0x96, 0x9a, 0x08, 0x9b, 0x54, 0xe8, 0xb5, 0x65, 0x5c, 0x98, 0x83, 0x15, 0x79, 0x07, 0xa9, 0x9d, 0x31, 0x69, 0x91, 0x56, 0x8a, 0x5e, 0xad, 0x45, 0x8a, 0x30, 0x9c, 0x10, 0x05, 0x0d, 0x0d, 0xdc,
    0x4a, 0xd0, 0x81, 0x39, 0xad, 0xc6, 0x88, 0x06, 0xdc, 0x1d, 0x18, 0x5a, 0x31, 0xfd, 0xaa, 0xae, 0xc7, 0x95, 0xd7, 0xa5, 0x75, 0xe1, 0x75, 0xe1, 0x85, 0x18, 0x74, 0x7b, 0xfe, 0x24, 0xd1, 0x76,
    0xb7, 0x44, 0x8a, 0x9b, 0x60, 0xcd, 0x5c, 0xaf, 0x33, 0x83, 0xb4, 0xed, 0x21, 0x7b, 0x73, 0x55, 0x75, 0x6d, 0x63, 0x83, 0xde, 0x67, 0x6d, 0x64, 0x5c, 0x17, 0xe3, 0xba, 0xaa, 0x1a, 0x2a, 0x0d,
    0x41, 0x9f, 0xd5, 0x5b, 0xfc, 0xbb, 0x62, 0x88, 0xbd, 0x24, 0x97, 0xcc, 0x27, 0x88, 0x5c, 0x58, 0x8d, 0xd2, 0x6d, 0xa6, 0x0f, 0x94, 0x35, 0x28, 0xdd, 0x0e, 0x18, 0x91, 0xbd, 0x80, 0x7a, 0x20,
    0x36, 0xef, 0x02, 0x3b, 0x45, 0x8b, 0x33, 0xe5, 0xe5, 0x7c, 0xa6, 0x7c, 0x0e, 0xff, 0x5e, 0xf9, 0x2f, 0x80, 0xde, 0x0d, 0x75, 0x2e, 0xe0, 0x7b, 0xf8, 0xc7, 0xf0, 0x6b, 0xc5, 0x7b, 0x8b, 0x5f,
    0x2f, 0xde, 0x57, 0xbc, 0xbf, 0x38, 0x09, 0x73, 0xfa, 0xb5, 0x45, 0xb2, 0x85, 0x78, 0x09, 0x3a, 0x32, 0x18, 0x5c, 0x29, 0xfa, 0x52, 0x9a, 0x92, 0x67, 0x43, 0x64, 0xf8, 0x5a, 0x94, 0xee, 0xca,
    0x1d, 0x07, 0xf3, 0x5f, 0x03, 0x68, 0x14, 0x71, 0x14, 0x0c, 0xc5, 0x97, 0x4c, 0xac, 0x26, 0x75, 0x9a, 0xd4, 0x63, 0x52, 0x2f, 0xa3, 0x77, 0x62, 0x3a, 0x17, 0xd4, 0x37, 0xa9, 0xc5, 0x72, 0xdc,
    0xd4, 0x12, 0x0e, 0x72, 0x71, 0x3f, 0x4a, 0xeb, 0x50, 0xe9, 0xee, 0xac, 0x99, 0xf5, 0xe1, 0x9c, 0xb2, 0x1c, 0xb7, 0x5f, 0xf3, 0x60, 0x5f, 0x6a, 0x7d, 0xd8, 0xab, 0x2d, 0xc4, 0x27, 0x94, 0x13,
    0xb0, 0x16, 0xaa, 0x4b, 0xfb, 0x54, 0x1a, 0x35, 0x2d, 0xfa, 0x0e, 0xf3, 0x7b, 0xa5, 0xd9, 0x3b, 0xc2, 0x7a, 0x35, 0x66, 0xef, 0x28, 0xeb, 0xd5, 0x9b, 0xbd, 0x7e, 0xd6, 0x6b, 0x32, 0x7b, 0x03,
    0xac, 0xd7, 0x19, 0x76, 0xb4, 0xe6, 0x94, 0xd1, 0xb9, 0x4e, 0x29, 0xf3, 0x4c, 0x09, 0x47, 0x50, 0x6f, 0x5e, 0xa6, 0x92, 0xf1, 0x6a, 0x58, 0x5b, 0xcf, 0xda, 0x26, 0xd6, 0xe6, 0x0e, 0x52, 0xc9,
    0x59, 0x26, 0x39, 0xcb, 0x24, 0x67, 0x99, 0xe4, 0xac, 0xe2, 0xe2, 0x38, 0x92, 0x53, 0x46, 0xe7, 0x7b, 0x47, 0x39, 0xab, 0x1c, 0x81, 0x7a, 0x14, 0x6a, 0x3f, 0xd4, 0x01, 0xa8, 0x20, 0x73, 0x53,
    0xff, 0x5c, 0x03, 0x36, 0xdb, 0x06, 0x36, 0xeb, 0x06, 0x9f, 0x50, 0x5f, 0xe1, 0x75, 0x28, 0xfd, 0x0a, 0xd8, 0xcf, 0x93, 0x14, 0x92, 0x13, 0xb1, 0x2d, 0xf3, 0xee, 0x6c, 0xc8, 0x3c, 0x6f, 0x7d,
    0xab, 0xd3, 0xd8, 0x18, 0xf5, 0x2a, 0x4f, 0x2a, 0x5f, 0xa2, 0x27, 0x43, 0xd7, 0x17, 0xe0, 0x56, 0xcc, 0x63, 0xbf, 0x26, 0x1c, 0x72, 0x65, 0x7c, 0xa9, 0xa6, 0x63, 0x42, 0x9f, 0x57, 0xf3, 0x6b,
    0x5e, 0x2d, 0x78, 0xfa, 0x2b, 0xdd, 0x93, 0x74, 0x71, 0x7e, 0xed, 0x83, 0x80, 0x90, 0xf2, 0xab, 0x1f, 0x04, 0x7c, 0x29, 0x7a, 0x2e, 0x26, 0xaa, 0x1e, 0x5c, 0x08, 0xe7, 0x9e, 0xeb, 0x9a, 0x37,
    0xd3, 0x95, 0xf9, 0x4c, 0x3e, 0x2f, 0xf3, 0x10, 0x87, 0x28, 0x33, 0xaf, 0x94, 0x1f, 0xbc, 0x49, 0x4d, 0x24, 0x5b, 0x67, 0xcd, 0x90, 0x3b, 0x21, 0xee, 0xe0, 0x6c, 0xec, 0x43, 0xe9, 0x9f, 0x48,
    0xc7, 0x66, 0x2e, 0x9d, 0x35, 0x59, 0x1d, 0x81, 0x7f, 0x93, 0xa3, 0xeb, 0x01, 0x99, 0xec, 0xe1, 0x4e, 0x1d, 0x98, 0xb5, 0x51, 0x7c, 0x5b, 0x1a, 0x81, 0xe3, 0xb3, 0xfc, 0xda, 0xcb, 0xd2, 0x46,
    0xf0, 0x7d, 0xa2, 0x54, 0x9f, 0x99, 0x97, 0x99, 0x2e, 0x6f, 0x14, 0x2d, 0x10, 0x87, 0x42, 0x72, 0x93, 0xb8, 0xa2, 0x74, 0x1a, 0xf4, 0xac, 0xc4, 0xc0, 0x53, 0xe5, 0x15, 0xa5, 0xdd, 0x92, 0x3e,
    0x13, 0x65, 0x3c, 0xf8, 0xc1, 0x59, 0x1b, 0x86, 0xf2, 0xe0, 0x13, 0xe8, 0xe0, 0x06, 0x33, 0xee, 0xd6, 0x96, 0xae, 0x2e, 0x35, 0xbe, 0xf3, 0x3e, 0x7c, 0xe7, 0xcd, 0xf0, 0x3e, 0xd7, 0x1b, 0x50,
    0x11, 0x69, 0x5a, 0x8f, 0xd2, 0x88, 0xfc, 0x1d, 0x6c, 0xe1, 0x66, 0xb1, 0xaa, 0xb0, 0xbe, 0x13, 0xda, 0x1c, 0xb0, 0x8d, 0x71, 0x7a, 0x84, 0xbe, 0x3c, 0x5c, 0x10, 0xf6, 0x6b, 0x97, 0xaf, 0x79,
    0x7d, 0xce, 0x9b, 0x6c, 0xfc, 0x2e, 0xa6, 0xdf, 0x01, 0xed, 0x32, 0xd0, 0xcf, 0x83, 0x93, 0x7e, 0x85, 0xda, 0xb0, 0x48, 0x6d, 0x6a, 0x6a, 0x6c, 0x56, 0xaa, 0x2a, 0x47, 0x97, 0xdd, 0x2e, 0xcd,
    0x1c, 0xbd, 0xa8, 0xf6, 0xa1, 0x2a, 0x9a, 0x0d, 0x74, 0x7d, 0x18, 0xc8, 0xaf, 0xfa, 0x0f, 0x79, 0x45, 0x4d, 0x73, 0xe3, 0xc2, 0x18, 0xd5, 0x58, 0x54, 0x35, 0x32, 0xf3, 0xae, 0x19, 0x0d, 0xc3,
    0xc0, 0x6a, 0xe3, 0x71, 0xb3, 0xfa, 0x17, 0xb3, 0x6f, 0x31, 0xfb, 0xc7, 0xcd, 0xfe, 0x98, 0xcd, 0x90, 0x9d, 0x37, 0xc3, 0xd7, 0x7b, 0xb9, 0xc3, 0x0f, 0xa9, 0x96, 0x61, 0x8f, 0x82, 0xcf, 0xdc,
    0x4f, 0x19, 0x3e, 0xf3, 0x40, 0x94, 0x8f, 0x22, 0xdc, 0x1e, 0x38, 0x8d, 0xbd, 0x90, 0xcf, 0xc1, 0xc3, 0xeb, 0x54, 0x94, 0x71, 0x05, 0xd1, 0x20, 0x1d, 0x99, 0x48, 0x16, 0x84, 0x3b, 0xd4, 0x42,
    0x97, 0x91, 0xd7, 0x21, 0x53, 0x9b, 0x72, 0x1e, 0xe4, 0x34, 0x36, 0xc1, 0x47, 0x58, 0x38, 0xe4, 0xc8, 0x14, 0x84, 0x8b, 0xc2, 0x82, 0xc6, 0x07, 0xb9, 0xd3, 0xdf, 0xe8, 0x2e, 0x8e, 0xea, 0x0a,
    0x1a, 0xd5, 0xdb, 0xa2, 0xf2, 0x83, 0xdf, 0x87, 0xe9, 0x7e, 0x68, 0x5c, 0x50, 0x5d, 0x37, 0x8d, 0x8c, 0xd3, 0x46, 0x64, 0x18, 0x23, 0xce, 0xb3, 0xd8, 0x30, 0xe6, 0xa7, 0xa3, 0xd8, 0xc8, 0x83,
    0x56, 0x98, 0xb3, 0x5b, 0x45, 0x83, 0x05, 0xe1, 0x4d, 0xcc, 0x1e, 0x05, 0xe1, 0xa7, 0x54, 0xbf, 0xd6, 0x1b, 0x38, 0x5a, 0xfa, 0x94, 0x2a, 0xa4, 0xc6, 0xe1, 0xef, 0xce, 0x79, 0x58, 0x6e, 0xea,
    0x50, 0xd1, 0x01, 0x2b, 0x6e, 0xc8, 0x5d, 0x54, 0xde, 0x1b, 0xd8, 0x28, 0x1e, 0x54, 0x2f, 0x9e, 0x5b, 0x0a, 0xf8, 0xa0, 0x4a, 0xc7, 0x5f, 0x1f, 0xa6, 0xe3, 0xe9, 0x2a, 0x75, 0x1d, 0xfc, 0xb7,
    0x01, 0xa5, 0xaf, 0x22, 0x82, 0x82, 0xc8, 0x62, 0x40, 0x4f, 0x30, 0xb4, 0x05, 0x50, 0x3e, 0x43, 0xc7, 0x01, 0x5d, 0xcd, 0xd0, 0xd7, 0x80, 0xaa, 0x19, 0xca, 0x07, 0x1b, 0x2d, 0x66, 0x68, 0x2c,
    0xa0, 0x04, 0x11, 0xc0, 0x5e, 0x43, 0x11, 0x73, 0x7b, 0x17, 0x3a, 0x40, 0xf3, 0x70, 0x51, 0xb8, 0x97, 0xaf, 0x69, 0x37, 0x7c, 0xbf, 0x0c, 0xf7, 0xe5, 0xbd, 0x91, 0x77, 0x30, 0x0f, 0x72, 0xad,
    0x46, 0xad, 0xa3, 0xcd, 0xae, 0x07, 0x49, 0x50, 0xcb, 0xa8, 0x3a, 0xdc, 0xac, 0xba, 0x7e, 0x46, 0x75, 0x42, 0x5e, 0x39, 0xa3, 0xd2, 0xfb, 0xe2, 0x8c, 0x9a, 0xcf, 0x30, 0xcd, 0x34, 0xa8, 0xc7,
    0x90, 0x50, 0x4a, 0x65, 0x7e, 0x8d, 0x27, 0xc3, 0xbb, 0x51, 0xda, 0x42, 0xc0, 0xb3, 0x47, 0x84, 0xa4, 0x6d, 0xbe, 0x90, 0x42, 0x90, 0x73, 0x06, 0x83, 0x90, 0x79, 0xe7, 0x5f, 0x8f, 0x85, 0xde,
    0xf3, 0xe7, 0x7c, 0xa9, 0xa5, 0xb9, 0x0d, 0x98, 0xde, 0xbb, 0x6d, 0x07, 0x1b, 0x38, 0xd8, 0x5f, 0xdc, 0xab, 0xd1, 0xfa, 0xcd, 0x64, 0x5d, 0xb7, 0x01, 0xb5, 0x99, 0x18, 0x91, 0x75, 0xb0, 0x76,
    0xfc, 0x9a, 0x3d, 0x0e, 0xd9, 0x15, 0x50, 0x1f, 0x43, 0x6f, 0x00, 0x3a, 0xce, 0x10, 0x01, 0xd4, 0xb1, 0xe5, 0x12, 0x72, 0xc1, 0x7b, 0xc1, 0x97, 0x9a, 0x52, 0xc6, 0x6e, 0x28, 0x78, 0x01, 0x54,
    0x35, 0x37, 0x37, 0x36, 0xeb, 0x46, 0x4c, 0x8e, 0x88, 0xc7, 0xaf, 0x1c, 0x11, 0x2f, 0x83, 0xba, 0xe0, 0xda, 0x11, 0xf1, 0xf9, 0x40, 0x17, 0x5e, 0x37, 0x22, 0xde, 0x06, 0x38, 0x02, 0xf4, 0x05,
    0xa0, 0x9d, 0xd7, 0x22, 0xf2, 0x30, 0xdc, 0x93, 0x65, 0xcc, 0x6a, 0x04, 0x50, 0x2b, 0x43, 0xcb, 0x00, 0xfd, 0x94, 0xa1, 0xe5, 0x80, 0x1e, 0x65, 0x28, 0x01, 0xe8, 0x71, 0x86, 0xda, 0x01, 0x4d,
    0x66, 0x68, 0x1d, 0xa0, 0x25, 0x0c, 0x75, 0x00, 0x9a, 0xc6, 0xd0, 0xb3, 0x80, 0x6e, 0x61, 0xb6, 0x2f, 0x08, 0xd3, 0x5b, 0x08, 0x69, 0x83, 0xc1, 0x1c, 0x96, 0xff, 0xf2, 0xe0, 0xed, 0xb0, 0x65,
    0x36, 0xbd, 0xdb, 0x38, 0x32, 0x1c, 0xff, 0x48, 0x16, 0x30, 0xd2, 0x2c, 0xe4, 0x2a, 0x99, 0x1f, 0xec, 0x9e, 0x6d, 0xf8, 0xfc, 0x02, 0x8c, 0xfd, 0x19, 0x1b, 0x2b, 0xd7, 0xf4, 0xf2, 0x88, 0xdc,
    0x0c, 0xf7, 0x3a, 0x4f, 0xf2, 0x9f, 0x47, 0x69, 0x3b, 0xec, 0x73, 0xd8, 0x03, 0x94, 0xf7, 0x73, 0xc6, 0x9b, 0xfe, 0xa2, 0xc1, 0x8b, 0x9a, 0xf7, 0xfa, 0xd8, 0x64, 0x2d, 0xa6, 0x98, 0xfa, 0xf1,
    0x91, 0x88, 0x2f, 0x65, 0xe0, 0x36, 0xed, 0x9e, 0x38, 0x95, 0xde, 0x27, 0xaf, 0xf2, 0xe9, 0xfa, 0x21, 0xb1, 0x85, 0x3c, 0x29, 0xfa, 0xb5, 0xa6, 0xc8, 0x0c, 0x79, 0x5b, 0xb9, 0x71, 0x03, 0x2d,
    0x91, 0x46, 0x3f, 0xb6, 0x52, 0x14, 0x92, 0x7f, 0x2c, 0xff, 0xa4, 0xbc, 0x62, 0x56, 0x77, 0xa4, 0x5b, 0x72, 0xc3, 0xea, 0x5c, 0x84, 0xeb, 0xe5, 0x33, 0x3b, 0x23, 0x42, 0xea, 0xe9, 0xa7, 0xd1,
    0x00, 0x22, 0xe8, 0x6d, 0x41, 0x03, 0xf4, 0x36, 0x1a, 0x68, 0xa5, 0xb7, 0x25, 0x59, 0x1c, 0xa9, 0x8f, 0x08, 0x5a, 0x34, 0xce, 0x1d, 0x17, 0x7a, 0x3f, 0x3d, 0xe7, 0xc2, 0x3b, 0x22, 0x67, 0x24,
    0x43, 0x17, 0x4e, 0x2e, 0x7d, 0x5b, 0x81, 0x86, 0x9b, 0xdc, 0x13, 0xb7, 0x90, 0x8e, 0xc8, 0xc7, 0x20, 0xb9, 0xe9, 0x0f, 0xdc, 0x29, 0x63, 0x9e, 0x3f, 0x13, 0x63, 0x74, 0x0e, 0x89, 0xc6, 0xbb,
    0x61, 0x7e, 0xc8, 0x3b, 0x04, 0x4e, 0xfa, 0x80, 0x15, 0x64, 0x1c, 0xd1, 0x41, 0x36, 0x49, 0xe6, 0x88, 0x07, 0xa4, 0xd3, 0x64, 0x9d, 0xf8, 0x52, 0x8d, 0x58, 0x27, 0x11, 0x8c, 0x06, 0x26, 0xc9,
    0x2f, 0x4b, 0xd4, 0x6e, 0x13, 0xe5, 0x25, 0x52, 0x2d, 0x7e, 0x18, 0xea, 0x44, 0x19, 0xdb, 0x75, 0xfd, 0xfe, 0xc8, 0xbc, 0xf5, 0xff, 0x08, 0xac, 0x0e, 0x73, 0x64, 0xf9, 0x1d, 0x48, 0xa3, 0xb2,
    0x43, 0xe2, 0xe8, 0xc7, 0x9e, 0x14, 0x87, 0x6c, 0x41, 0x35, 0xff, 0x49, 0xea, 0x23, 0x68, 0xa0, 0x02, 0x57, 0x9a, 0xbc, 0xc5, 0x30, 0x8f, 0x90, 0xac, 0x60, 0xab, 0xbf, 0x00, 0xab, 0x5f, 0x11,
    0xae, 0xf1, 0xd1, 0xf3, 0x00, 0x96, 0x3a, 0xe1, 0x85, 0x58, 0xf6, 0x6a, 0x23, 0xd9, 0xf9, 0x36, 0xce, 0xf8, 0x2e, 0x73, 0x94, 0x57, 0xdb, 0x61, 0x22, 0x9a, 0x17, 0x72, 0x32, 0xb4, 0xf7, 0xfb,
    0xf8, 0x2e, 0xd6, 0xbe, 0x2f, 0xd2, 0x5b, 0xe3, 0x4b, 0xc8, 0x25, 0xf4, 0x3e, 0xa2, 0x6f, 0xb6, 0x3d, 0x9d, 0x28, 0xfd, 0x1a, 0xe4, 0x4d, 0xf3, 0x55, 0x38, 0xdb, 0xc6, 0x97, 0x4b, 0x33, 0x1c,
    0x4e, 0x2e, 0x0a, 0x57, 0x72, 0x45, 0x22, 0xba, 0xd4, 0x3d, 0xfd, 0xbf, 0xbf, 0x12, 0x4b, 0x02, 0x25, 0x81, 0x67, 0x63, 0x8e, 0x56, 0xae, 0x7a, 0x5e, 0x61, 0x99, 0xf4, 0xa1, 0x58, 0x26, 0x7d,
    0x04, 0xf5, 0x33, 0xa8, 0x67, 0xa1, 0x5e, 0x10, 0x4b, 0x02, 0x28, 0xd6, 0x16, 0xd3, 0x75, 0x07, 0xb1, 0xec, 0x85, 0xbb, 0x1d, 0x5e, 0x83, 0x6d, 0x3d, 0x41, 0xe5, 0x58, 0xfc, 0x87, 0x6f, 0x48,
    0x2b, 0x31, 0xfc, 0xdf, 0x01, 0x31, 0x42, 0xef, 0x47, 0xf2, 0x0c, 0x4a, 0x8f, 0x87, 0xf5, 0xe4, 0xc2, 0xeb, 0x7b, 0x4c, 0xb0, 0x5c, 0x41, 0xda, 0x35, 0xf1, 0x1d, 0x4a, 0x9f, 0x92, 0xaf, 0xce,
    0x57, 0xc7, 0xaa, 0x0d, 0xea, 0x6a, 0x35, 0xad, 0x7a, 0xe2, 0xb3, 0xe2, 0xb7, 0xc5, 0xe7, 0xc4, 0xff, 0x16, 0xdf, 0x1b, 0xe7, 0xc9, 0x72, 0x62, 0xec, 0x16, 0x3c, 0xf4, 0x0c, 0x7d, 0x8d, 0x9e,
    0x88, 0x1b, 0x3f, 0xb8, 0x4b, 0x4d, 0x19, 0xcd, 0x67, 0xdf, 0x95, 0xd0, 0xf7, 0xba, 0x9b, 0xbd, 0xe0, 0xc7, 0xdf, 0x30, 0xe1, 0x46, 0xfa, 0x6a, 0x5f, 0xc6, 0xf3, 0x9b, 0xe4, 0xe5, 0xb4, 0x49,
    0xd0, 0xdd, 0x07, 0xc4, 0x0d, 0x3c, 0xdf, 0x21, 0x6f, 0xe2, 0xf9, 0x75, 0xf2, 0x56, 0xde, 0xbe, 0x46, 0xa6, 0x06, 0xd9, 0x41, 0x59, 0xfb, 0x28, 0xeb, 0x23, 0x9e, 0xdf, 0x20, 0x7f, 0x41, 0xbb,
    0xcc, 0x48, 0xc4, 0xc6, 0x77, 0xca, 0xb7, 0xd8, 0xa1, 0x99, 0x61, 0xe7, 0x9f, 0x95, 0x4b, 0x69, 0x73, 0x99, 0xe1, 0xee, 0x74, 0xf0, 0xab, 0xa4, 0xc5, 0x0e, 0x0b, 0xf7, 0xa8, 0xc3, 0x62, 0x59,
    0xe1, 0xe0, 0x53, 0x52, 0x87, 0x83, 0xe7, 0xa2, 0x3b, 0x1d, 0xce, 0x9d, 0xd8, 0x57, 0x17, 0x91, 0xfc, 0xb1, 0x29, 0x4e, 0xf8, 0x26, 0x47, 0x3e, 0x72, 0x5a, 0x2b, 0x6d, 0x0d, 0xb6, 0x1b, 0x5d,
    0xb9, 0x47, 0x6d, 0xc7, 0x6c, 0x47, 0x79, 0x9b, 0xfd, 0x5e, 0xfb, 0x7b, 0x96, 0xb9, 0xd8, 0xc5, 0xbf, 0xc4, 0x17, 0xb8, 0xf9, 0x57, 0xe4, 0xa5, 0x6e, 0xfd, 0x7f, 0x77, 0x41, 0x52, 0xf6, 0xc7,
    0x84, 0xba, 0x2f, 0x45, 0xa1, 0xce, 0x16, 0xf0, 0xd5, 0x25, 0xa2, 0x9b, 0x80, 0x2b, 0x44, 0x21, 0x29, 0xd6, 0x76, 0x8a, 0x9d, 0x80, 0x3b, 0xc5, 0x5a, 0xcb, 0x26, 0xf1, 0x12, 0xa7, 0x56, 0xdc,
    0x22, 0xbe, 0x24, 0xbe, 0x0a, 0xdc, 0xdd, 0xc0, 0xf5, 0xc7, 0xec, 0xc0, 0xcf, 0x0f, 0x3e, 0x17, 0xa4, 0x75, 0x2a, 0xe9, 0x17, 0x7d, 0x75, 0xf0, 0xa4, 0xad, 0x70, 0xe3, 0x95, 0x85, 0x9f, 0x07,
    0x7e, 0x6b, 0xdb, 0x86, 0x51, 0x6c, 0xaa, 0x45, 0x88, 0x96, 0x12, 0x54, 0xbd, 0xb6, 0x70, 0x89, 0x64, 0x21, 0x09, 0xd8, 0x11, 0x7c, 0x31, 0x6a, 0x21, 0x42, 0x1d, 0x7a, 0x90, 0x6a, 0x76, 0x15,
    0x0a, 0x75, 0x9f, 0xc3, 0x38, 0x2b, 0xb6, 0x11, 0xae, 0x7a, 0x43, 0x61, 0x5b, 0x8c, 0x1e, 0x0f, 0x7f, 0x6c, 0xb7, 0xe8, 0x8f, 0xe5, 0xc3, 0xbc, 0x42, 0x1d, 0xfd, 0xc6, 0xfe, 0x42, 0x6f, 0xec,
    0xb9, 0x20, 0xad, 0xc6, 0x17, 0xfc, 0xb1, 0x9d, 0x98, 0xae, 0xdb, 0x07, 0xeb, 0xee, 0x79, 0x24, 0x47, 0x87, 0x4f, 0xf9, 0xea, 0x50, 0x6c, 0x68, 0x9a, 0x17, 0xc1, 0x18, 0xeb, 0xa5, 0x42, 0xd2,
    0x6f, 0x7e, 0xfc, 0x48, 0xe1, 0x56, 0xc9, 0x1b, 0xa3, 0x9f, 0x1f, 0xd7, 0xea, 0x26, 0xb2, 0xe5, 0x3a, 0x40, 0x5b, 0xc4, 0xfc, 0xe0, 0x70, 0x7c, 0xba, 0x90, 0x27, 0x25, 0x5c, 0x22, 0x7a, 0x5c,
    0x1a, 0x8e, 0x51, 0x11, 0xfd, 0x2c, 0xdc, 0x0f, 0xd5, 0x5c, 0xd1, 0x6e, 0x11, 0x01, 0x36, 0x16, 0x61, 0x6c, 0xcf, 0x57, 0xe7, 0xc6, 0x5f, 0xc2, 0xb6, 0xbe, 0x62, 0xdb, 0xca, 0x0f, 0x1a, 0xd5,
    0x57, 0xe7, 0x21, 0x25, 0xb8, 0x5f, 0x44, 0x76, 0x5f, 0x1d, 0xf7, 0x40, 0xbf, 0x38, 0x55, 0x3c, 0x60, 0xe9, 0x0b, 0xbc, 0x27, 0xdd, 0x8d, 0x6d, 0x76, 0x63, 0xd6, 0xb9, 0xff, 0x9e, 0x75, 0x0e,
    0xcc, 0x3a, 0x1c, 0xcf, 0x2e, 0x12, 0xa2, 0x36, 0x32, 0x11, 0x5f, 0x5d, 0x84, 0x62, 0x36, 0xd8, 0xa8, 0xb1, 0xdd, 0xfc, 0xe0, 0xd0, 0x76, 0xe1, 0x15, 0x51, 0x31, 0xb9, 0xe8, 0xd2, 0x76, 0xa9,
    0xe9, 0x2f, 0xdf, 0xee, 0xcd, 0x76, 0x63, 0xaf, 0x97, 0x56, 0x60, 0xc5, 0x0f, 0x7b, 0x74, 0xa1, 0x8e, 0xab, 0x2e, 0x28, 0xa2, 0xdb, 0xa6, 0x2c, 0x5f, 0x1d, 0x6c, 0x0b, 0x7b, 0x63, 0x6e, 0xd2,
    0x8d, 0xe9, 0xf2, 0xee, 0xb2, 0x7f, 0x2f, 0x1a, 0xbc, 0x7e, 0x71, 0xee, 0x65, 0x4b, 0x3d, 0x14, 0x70, 0xcb, 0x77, 0xe3, 0x7b, 0xed, 0xd4, 0x2f, 0x5c, 0x75, 0x25, 0x8c, 0x1f, 0x0f, 0xa6, 0x81,
    0x27, 0x5c, 0xf5, 0x03, 0x80, 0x27, 0x00, 0xe6, 0x01, 0x37, 0x01, 0x9e, 0x08, 0x18, 0xc5, 0x26, 0x11, 0x43, 0x73, 0x09, 0x70, 0xa6, 0x98, 0x9a, 0xbf, 0x06, 0x5c, 0x62, 0x6a, 0x3e, 0x0e, 0xf8,
    0x66, 0xa6, 0x39, 0x15, 0x5a, 0x07, 0xe1, 0xea, 0xbd, 0x31, 0x54, 0x8b, 0x48, 0x5b, 0xc5, 0x72, 0x79, 0xbe, 0x7d, 0xb9, 0xbc, 0xc8, 0xce, 0x91, 0x55, 0x50, 0xdb, 0xed, 0xf0, 0x17, 0x0f, 0x6a,
    0xbb, 0xdd, 0x0a, 0xd4, 0x0a, 0xd4, 0x0e, 0xd4, 0x0e, 0x34, 0x17, 0xc6, 0x6c, 0x2f, 0x2a, 0x09, 0x38, 0x5b, 0x51, 0x35, 0xa5, 0x0e, 0x72, 0x2f, 0x4e, 0x44, 0x37, 0xc3, 0x0a, 0x3d, 0xc4, 0x29,
    0xee, 0x29, 0xa2, 0x73, 0xba, 0xf1, 0xee, 0xa2, 0xcd, 0xf2, 0x7e, 0xbb, 0x81, 0xf7, 0x17, 0x7d, 0x48, 0x1c, 0xbf, 0xca, 0xf5, 0x81, 0x62, 0x97, 0xdd, 0x09, 0xe1, 0x7e, 0xa8, 0x88, 0x86, 0xb6,
    0x0b, 0x50, 0x8a, 0xa9, 0xdf, 0x12, 0x68, 0x8b, 0x5d, 0xfe, 0x9f, 0xf5, 0xf2, 0x9c, 0x33, 0xd8, 0xf2, 0x79, 0xcb, 0x0f, 0x73, 0xce, 0x37, 0xf1, 0xc3, 0x21, 0x23, 0x6f, 0x8c, 0xc2, 0x88, 0x7c,
    0xc1, 0xb2, 0xc6, 0x28, 0xe2, 0xd7, 0xce, 0xb7, 0xf8, 0x52, 0xdc, 0x5b, 0x42, 0xd2, 0xaf, 0x39, 0xf0, 0xb7, 0xf0, 0x12, 0x80, 0x9c, 0xa2, 0xd9, 0x98, 0xde, 0x18, 0x78, 0x09, 0x64, 0x4b, 0xb6,
    0x64, 0x4b, 0xb6, 0x64, 0x4b, 0xb6, 0x64, 0x4b, 0xb6, 0x64, 0x4b, 0xb6, 0x64, 0x4b, 0xb6, 0x64, 0x4b, 0xb6, 0x64, 0xcb, 0xff, 0x7b, 0xf9, 0x17, 0x7c, 0x8e, 0x31, 0x18,
};
#endif


void configure_lvds(void)
{
    EVE_memWrite32(REG_SO_EN, 0UL);
    EVE_memWrite32(REG_RE_ACTIVE, 0UL);
    EVE_memWrite32(REG_LVDSTX_EN, 0UL);

    /* place the swapchain-buffers at the end of the memory */
    /* 1920 x 1200 as assumed maximum resolution */
    /* 2304000 pixel with 24 bits per pixel in RGB8 = 6912000 bytes, 6750kiB, 6.59MiB */
    /* using 8MiB per buffer should be generous*/
    /* top is 125.5MiB (1Gib DDR3L)*/
    /* -> top buffer at 117 MiB */

    /* Swap Chain 0 : Render Engine */
    EVE_memWrite32(REG_SC0_RESET, 1UL);
    EVE_memWrite32(REG_SC0_SIZE, 2UL);
    EVE_memWrite32(REG_SC0_PTR0, 117UL << 20UL); /* place buffer at address of 117MiB */
    EVE_memWrite32(REG_SC0_PTR1, 109UL << 20UL);

    /* the JPEG Engine outputs upto 32 bits per pixel in ARGB8 mode */
    /* 1920 x 1200 x 4 = 9216000 = 9000kiB = 8.8MiB -> use 9MiB*/

    /* Swap Chain 1 : JPEG Engine */
    EVE_memWrite32(REG_SC1_RESET, 1UL);
    EVE_memWrite32(REG_SC1_SIZE, 2);
    EVE_memWrite32(REG_SC1_PTR0, 100UL << 20UL);
    EVE_memWrite32(REG_SC1_PTR1, 91UL << 20UL);

    /* Swap Chain 2 : LVDS RX */
    EVE_memWrite32(REG_SC2_RESET, 1UL);
    EVE_memWrite32(REG_SC2_SIZE, 2);
    EVE_memWrite32(REG_SC2_PTR0, 83UL << 20UL);
    EVE_memWrite32(REG_SC2_PTR1, 75UL << 20UL); /* place buffer at address of 75MiB */

    /* yes, this configuration "wastes" several MiBs, but it leaves 75MiB to work with */

    EVE_memWrite32(REG_SO_SOURCE, EVE_SWAPCHAIN_0);
    EVE_memWrite32(REG_SO_FORMAT, EVE_RGB8);
    EVE_memWrite32(REG_SO_MODE, EVE_LVDS_SO_MODE);

    EVE_memWrite32(REG_RE_DEST, EVE_SWAPCHAIN_0);
    EVE_memWrite32(REG_RE_FORMAT, EVE_RGB8);
    EVE_memWrite32(REG_RE_W, EVE_HSIZE); /* CMD_RENDERTARGET: Render target width in pixels and must be a multiple of 16. */
    EVE_memWrite32(REG_RE_H, EVE_VSIZE); /* CMD_RENDERTARGET: Render target height in pixels. w × h must be a multiple of 128 */
    EVE_memWrite32(REG_RE_DITHER, 0UL);
    EVE_memWrite32(REG_RE_ACTIVE, 1UL);

    EVE_memWrite32(REG_LVDSTX_CTRL_CH0, EVE_LVDS_MODE); /* set mode defined by display configuration */
    EVE_memWrite32(REG_LVDSTX_CTRL_CH1, EVE_LVDS_MODE); /* set mode defined by display configuration */

    EVE_memWrite32(REG_LVDSTX_PLLCFG, setlvdspll_value(PLL_LOCK_PERIOD, EVE_LVDS_PLL_CKS, EVE_LVDS_PLL_DIV));

#if defined (EVE_LVDS_SINGLE_CHANNEL)
    EVE_memWrite32(REG_LVDSTX_EN, LVDS_CH0_EN);
#else
    EVE_memWrite32(REG_LVDSTX_EN, LVDS_CH0_EN | LVDS_CH1_EN);
#endif

    DELAY_MS(10U);

    EVE_memWrite32(REG_SO_EN, 1UL); /* enable scanout */

//    EVE_memWrite32(REG_DISP, 1);


// Audio config
//    EVE_memWrite32(REG_I2S_CTL, 0x2);
//    EVE_memWrite32(REG_I2S_CFG, 0x400);
//    EVE_memWrite32(REG_I2S_EN, 1);
//    EVE_memWrite32(REG_I2S_FREQ, 0x3CF0);

}


/**
 * @brief Waits for either REG_BOOT_STATUS to indicate that the boot sequence is complete,
 * or untill a timeout of 50ms has passed.
 * @return Returns E_OK in case of success, EVE_FAIL_BOOT_TIMEOUT if the timeout is reached.
 */
static uint8_t wait_boot(void)
{
    uint8_t ret = EVE_FAIL_BOOT_TIMEOUT;
    uint32_t bootstatus = 0U;

    for (uint16_t timeout = 0U; timeout < 100U; timeout++)
    {
        bootstatus = EVE_memRead32(REG_BOOT_STATUS);

        if (0x522E2E2EU == bootstatus) /* EVE reports boot is done - "normal running" */
        {
            ret = E_OK;
            break;
        }

        DELAY_MS(1U);
    }

    return (ret);
}

/**
 * @brief Writes all parameters defined for the display selected in EVE_config.h.
 * to the corresponding registers.
 * Used by EVE_init() and can be used to refresh the register values if needed.
 */
void EVE_write_display_parameters(void)
{
    /* Initialize Display */
    EVE_memWrite32(REG_HSIZE, EVE_HSIZE);      /* active display width */
    EVE_memWrite32(REG_HCYCLE, EVE_HCYCLE);    /* total number of clocks per line, incl front/back porch */
    EVE_memWrite32(REG_HOFFSET, EVE_HOFFSET);  /* start of active line */
    EVE_memWrite32(REG_HSYNC0, EVE_HSYNC0);    /* start of horizontal sync pulse */
    EVE_memWrite32(REG_HSYNC1, EVE_HSYNC1);    /* end of horizontal sync pulse */
    EVE_memWrite32(REG_VSIZE, EVE_VSIZE);      /* active display height */
    EVE_memWrite32(REG_VCYCLE, EVE_VCYCLE);    /* total number of lines per screen, including pre/post */
    EVE_memWrite32(REG_VOFFSET, EVE_VOFFSET);  /* start of active screen */
    EVE_memWrite32(REG_VSYNC0, EVE_VSYNC0);    /* start of vertical sync pulse */
    EVE_memWrite32(REG_VSYNC1, EVE_VSYNC1);    /* end of vertical sync pulse */
    EVE_memWrite32(REG_PCLK_POL, EVE_PCLKPOL); /* LCD data is clocked in on this PCLK edge */
    EVE_memWrite32(REG_DISP, 1UL); /* at this point only enables the backlight, the LVDS-TX still needs to be configured */

    /* no need to configure Touch, auto-discovery and continous mode is reset default */
    //EVE_memWrite32(REG_TOUCH_CONFIG, 0UL); /* trigger auto-discovery for touch controller with 400kHz I2C */
    //EVE_memWrite32(REG_TOUCH_MODE, EVE_TMODE_CONTINUOUS); /* enable touch */
    // there is no REG_TOUCH_RZTHRESH in EVE5

#if defined (EVE_ROTATE)
    EVE_memWrite32(REG_RE_ROTATE, EVE_ROTATE & 7U); /* bit0 = invert, bit2 = portrait, bit3 = mirrored */
    /* reset default value is 0x0 - not inverted, landscape, not mirrored */
#endif
}

/**
 * @brief Initializes EVE according to the selected configuration from EVE_config.h.
 * @return E_OK in case of success
 * @note - Has to be executed with the SPI setup to 11 MHz or less as required by FT8xx / BT8xx!
 * @note - Additional settings can be made through extra macros.
 * @note - (EVE_TOUCH_RZTHRESH - configure the sensitivity of resistive touch, defaults to 1200.) - not on EVE5
 * @note - EVE_ROTATE - set the screen rotation: bit0 = invert, bit1 = portrait, bit2 = mirrored.
 * @note - needs a set of calibration values for the selected rotation since this rotates before calibration!
 * @note - EVE_BACKLIGHT_FREQ - configure the backlight frequency, default is not writing it which results in 250Hz.
 * @note - EVE_BACKLIGHT_PWM - configure the backlight pwm, defaults to 0x20 / 25%.
 * @note - EVE_SOFT_RESET - if defined the host command RST_PULSE is send
 */
uint8_t EVE_init(void)
{
    uint8_t ret;

    /* note: using the RST_N pin is recommended by Bridgetek! */
    EVE_pdn_set();
    DELAY_MS(6U); /* minimum time for reset-down is 214us and the voltage rails need to be stable for 5ms */
    EVE_pdn_clear();
    DELAY_MS(2U); /* BT820 does not specifiy a minimum time to pass after raising RST_N */

#if defined (EVE_SOFT_RESET)
    EVE_cmdWrite(EVE_RESET_PULSE,0U); /* reset, only required for warm-start if RST_N line is not used */
#endif

    EVE_cmdWrite(EVE_BOOTCFGEN, (BOOTCFGEN_BOOT_USER_SETTING | BOOTCFGEN_DDRTYPE_USER_SETTING | BOOTCFGEN_ALLOW)); /* turn on user setting switch */
    EVE_cmdWrite(EVE_SETBOOTCFG, (SETBOOTCFG_DDR_EN | SETBOOTCFG_TOUCH_EN));
    //EVE_cmdWrite(EVE_SETBOOTCFG, (SETBOOTCFG_DDR_EN|SETBOOTCFG_TOUCH_EN|SETBOOTCFG_AUDIO_EN));
    EVE_cmdWrite(EVE_SETDDRTYPE, setddrtype_value(SETDDRTYPE_SPEED_1333, SETDDRTYPE_TYPE_DDR3L, SETDDRTYPE_SIZE_1024));
    EVE_cmdWrite(EVE_BOOTCFGEN, (BOOTCFGEN_BOOT_USER_SETTING | BOOTCFGEN_DDRTYPE_USER_SETTING)); /* turn off user setting switch */
    EVE_cmdWrite(EVE_SETPLLSP1, 15U); /* set SYSPLL_NS to the default value of 15 */
    EVE_cmdWrite(EVE_SETSYSCLKDIV, 0x17U); /* set SYSCLK_DIV to the default value of 0x10 + 0x07 for a the system clock of 72MHz. */
    EVE_cmdWrite(EVE_ACTIVE, 0U); /* start EVE */

    DELAY_MS(60U); /* give EVE a moment of silence to power up, a BT820 answers about 34ms after ACTIVE and booting takes about 27ms */

    ret = wait_boot();
    if (E_OK == ret)
    {
#if defined (EVE_PATCH_TOUCH)
        EVE_cmd_loadpatch(0, touch_patch, sizeof(touch_patch));
#endif

#if defined (EVE_BACKLIGHT_FREQ)
        EVE_memWrite32(REG_PWM_HZ, EVE_BACKLIGHT_FREQ); /* set backlight frequency to configured value */
#endif

#if defined (EVE_BACKLIGHT_PWM)
        EVE_memWrite32(REG_PWM_DUTY, EVE_BACKLIGHT_PWM); /* set backlight pwm to user requested level */
#else
        EVE_memWrite32(REG_PWM_DUTY, 0x20U); /* turn on backlight pwm to 25% for any other module */
#endif

        EVE_write_display_parameters();

        /* write a basic display-list to get things started */
        EVE_memWrite32(EVE_RAM_DL, DL_CLEAR_COLOR_RGB);
        EVE_memWrite32(EVE_RAM_DL + 4U, (DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG));
        EVE_memWrite32(EVE_RAM_DL + 8U, DL_DISPLAY); /* end of display list */
        EVE_memWrite32(REG_DLSWAP, EVE_DLSWAP_FRAME);
        /* nothing is being displayed yet... the pixel clock is still off */

        configure_lvds();

        DELAY_MS(1U);
        EVE_execute_cmd(); /* just to be safe, wait for EVE to not be busy */

#if defined (EVE_DMA)
        EVE_init_dma(); /* prepare DMA */
#endif
    }

    return (ret);
}


/* ##################################################################
    functions for display lists
##################################################################### */

/**
 * @brief Begin a sequence of commands or prepare a DMA transfer if applicable.
 * @note - Needs to be used with EVE_end_cmd_burst().
 * @note - Do not use any functions in the sequence that do not address the command-fifo as for example any of EVE_mem...() functions.
 * @note - Do not use any of the functions that do not support burst-mode.
 */
void EVE_start_cmd_burst(void)
{
#if defined (EVE_DMA)
    if (EVE_dma_busy)
    {
        EVE_execute_cmd(); /* this is a safe-guard to protect segmented display-list building with DMA from overlapping */
    }

    g_cmd_burst = EVE_BURST_ACTIVE;
    EVE_dma_buffer[0U] = 0x000001ffUL; /* sends FF 01 00 00 to EVE - REG_CMDB_WRITE + MEM_WRITE - low mid hi */
    EVE_dma_buffer_index = 1U;
#else
    g_cmd_burst = EVE_BURST_ACTIVE;
    EVE_cs_set();
    spi_transmit_32_addr(MEM_WRITE | REG_CMDB_WRITE);
#endif
}

/**
 * @brief Draw a circular arc with rounded caps.
 */
void EVE_cmd_arc(const int16_t xc0, const int16_t yc0, const uint16_t rad0, const uint16_t rad1, const uint16_t angle0, const uint16_t angle1)
{
    const uint32_t param0 = i16_i16_to_u32(xc0, yc0);
    const uint32_t param1 = u16_u16_to_u32(rad0, rad1);
    const uint32_t param2 = u16_u16_to_u32(angle0, angle1);

    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_ARC);
        spi_transmit_32(param0);
        spi_transmit_32(param1);
        spi_transmit_32(param2);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ARC);
        spi_transmit_burst(param0);
        spi_transmit_burst(param1);
        spi_transmit_burst(param2);
    }
}

/**
 * @brief Draw a circular arc with rounded caps, only works in burst-mode.
 */
void EVE_cmd_arc_burst(const int16_t xc0, const int16_t yc0, const uint16_t rad0, const uint16_t rad1, const uint16_t angle0, const uint16_t angle1)
{
    spi_transmit_burst(CMD_ARC);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(u16_u16_to_u32(rad0, rad1));
    spi_transmit_burst(u16_u16_to_u32(angle0, angle1));
}

/**
 * @brief Draw a rectangle with a circular gradient.
 */
void EVE_cmd_cgradient(const uint32_t shape, const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt, const uint32_t rgb0, const uint32_t rgb1)
{
    const uint32_t param0 = i16_i16_to_u32(xc0, yc0);
    const uint32_t param1 = u16_u16_to_u32(wid, hgt);

    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_CGRADIENT);
        spi_transmit_32(shape);
        spi_transmit_32(param0);
        spi_transmit_32(param1);
        spi_transmit_32(rgb0);
        spi_transmit_32(rgb1);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_CGRADIENT);
        spi_transmit_burst(shape);
        spi_transmit_burst(param0);
        spi_transmit_burst(param1);
        spi_transmit_burst(rgb0);
        spi_transmit_burst(rgb1);
    }
}

/**
 * @brief Draw a rectangle with a circular gradient, only works in burst-mode.
 */
void EVE_cmd_cgradient_burst(const uint32_t shape, const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt, const uint32_t rgb0, const uint32_t rgb1)
{
    spi_transmit_burst(CMD_CGRADIENT);
    spi_transmit_burst(shape);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(u16_u16_to_u32(wid, hgt));
    spi_transmit_burst(rgb0);
    spi_transmit_burst(rgb1);
}

/**
 * @brief Enable or disable render optomization for widgets.
 */
void EVE_cmd_enableregion(const uint32_t enable)
{
    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_ENABLEREGION);
        spi_transmit_32(enable);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ENABLEREGION);
        spi_transmit_burst(enable);
    }
}

/**
 * @brief Enable or disable render optomization for widgets, only works in burst-mode.
 */
void EVE_cmd_enableregion_burst(const uint32_t enable)
{
    spi_transmit_burst(CMD_ENABLEREGION);
    spi_transmit_burst(enable);
}

/**
 * @brief Pause execution to wait for outstanding writes.
 */
void EVE_cmd_fence(void)
{
    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_FENCE);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_FENCE);
    }
}

/**
 * @brief Pause execution to wait for outstanding writes, only works in burst-mode.
 */
void EVE_cmd_fence_burst(void)
{
    spi_transmit_burst(CMD_FENCE);
}

/**
 * @brief Draws an additive glow effect centered in a rectangle, using the current color.
 */
void EVE_cmd_glow(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt)
{
    const uint32_t param0 = i16_i16_to_u32(xc0, yc0);
    const uint32_t param1 = u16_u16_to_u32(wid, hgt);

    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_GLOW);
        spi_transmit_32(param0);
        spi_transmit_32(param1);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_GLOW);
        spi_transmit_burst(param0);
        spi_transmit_burst(param1);
    }
}

/**
 * @brief Draws an additive glow effect centered in a rectangle, using the current color, only works in burst-mode.
 */
void EVE_cmd_glow_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt)
{
    spi_transmit_burst(CMD_GLOW);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(u16_u16_to_u32(wid, hgt));
}

/**
 * @brief Waits until the render engine is idle.
 * @note - This is not be used within a display-list, in the given example this is placed after CMD_SWAP.
 */
void EVE_cmd_graphicsfinish(void)
{
    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_GRAPHICSFINISH);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_GRAPHICSFINISH);
    }
}

/**
 * @brief Waits until the render engine is idle, only works in burst-mode.
 */
void EVE_cmd_graphicsfinish_burst(void)
{
    spi_transmit_burst(CMD_GRAPHICSFINISH);
}

/**
 * @brief Write a value to a core register.
 */
void EVE_cmd_regwrite(const uint32_t dest, const uint32_t value)
{
    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_REGWRITE);
        spi_transmit_32(dest);
        spi_transmit_32(value);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_REGWRITE);
        spi_transmit_burst(dest);
        spi_transmit_burst(value);
    }
}

/**
 * @brief Write a value to a core register, only works in burst-mode.
 */
void EVE_cmd_regwrite_burst(const uint32_t dest, const uint32_t value)
{
    spi_transmit_burst(CMD_REGWRITE);
    spi_transmit_burst(dest);
    spi_transmit_burst(value);
}

/**
 * @brief Copies the result field of the preceding command into memory.
 */
void EVE_cmd_result(const uint32_t dest)
{
    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_RESULT);
        spi_transmit_32(dest);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_RESULT);
        spi_transmit_burst(dest);
    }
}

/**
 * @brief Copies the result field of the preceding command into memory, only works in burst-mode.
 */
void EVE_cmd_result_burst(const uint32_t dest)
{
    spi_transmit_burst(CMD_RESULT);
    spi_transmit_burst(dest);
}

/**
 * @brief Adds a RESTORE_CONTEXT to the display list and  restores the coprocessor graphics state from the state stack.
 */
void EVE_cmd_restorecontext(void)
{
    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_RESTORECONTEXT);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_RESTORECONTEXT);
    }
}

/**
 * @brief Adds a RESTORE_CONTEXT to the display list and  restores the coprocessor graphics state from the state stack, only works in burst-mode.
 */
void EVE_cmd_restorecontext_burst(void)
{
    spi_transmit_burst(CMD_RESTORECONTEXT);
}

/**
 * @brief Adds a SAVE_CONTEXT to the display list and preserves the coprocessor graphics state on the state stack.
 */
void EVE_cmd_savecontext(void)
{
    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_SAVECONTEXT);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SAVECONTEXT);
    }
}

/**
 * @brief Adds a SAVE_CONTEXT to the display list and preserves the coprocessor graphics state on the state stack, only works in burst-mode.
 */
void EVE_cmd_savecontext_burst(void)
{
    spi_transmit_burst(CMD_SAVECONTEXT);
}

/**
 * @brief Skip following command bytes if a given condition is true.
 */
void EVE_cmd_skipcond(const uint32_t adr, const uint32_t func, const uint32_t ref, const uint32_t mask, const uint32_t num)
{
    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_SKIPCOND);
        spi_transmit_32(adr);
        spi_transmit_32(func);
        spi_transmit_32(ref);
        spi_transmit_32(mask);
        spi_transmit_32(num);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SKIPCOND);
        spi_transmit_burst(adr);
        spi_transmit_burst(func);
        spi_transmit_burst(ref);
        spi_transmit_burst(mask);
        spi_transmit_burst(num);
    }
}

/**
 * @brief Skip following command bytes if a given condition is true, only works in burst-mode.
 */
void EVE_cmd_skipcond_burst(const uint32_t adr, const uint32_t func, const uint32_t ref, const uint32_t mask, const uint32_t num)
{
    spi_transmit_burst(CMD_SKIPCOND);
    spi_transmit_burst(adr);
    spi_transmit_burst(func);
    spi_transmit_burst(ref);
    spi_transmit_burst(mask);
    spi_transmit_burst(num);
}

/**
 * @brief Register one custom font into the coprocessor engine.
 * @note - does not set up the bitmap parameters of the font
 */
void EVE_cmd_setfont(const uint32_t font, const uint32_t ptr, const uint32_t firstchar)
{
    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_SETFONT);
        spi_transmit_32(font);
        spi_transmit_32(ptr);
        spi_transmit_32(firstchar);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SETFONT);
        spi_transmit_burst(font);
        spi_transmit_burst(ptr);
        spi_transmit_burst(firstchar);
    }
}

/**
 * @brief Register one custom font into the coprocessor engine, only works in burst-mode.
 * @note - does not set up the bitmap parameters of the font
 */
void EVE_cmd_setfont_burst(const uint32_t font, const uint32_t ptr, const uint32_t firstchar)
{
    spi_transmit_burst(CMD_SETFONT);
    spi_transmit_burst(font);
    spi_transmit_burst(ptr);
    spi_transmit_burst(firstchar);
}

/**
 * @brief Wait for the given register value to change.
 */
void EVE_cmd_waitchange(const uint32_t adr)
{
    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_WAITCHANGE);
        spi_transmit_32(adr);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_WAITCHANGE);
        spi_transmit_burst(adr);
    }
}

/**
 * @brief Wait for the given register value to change, only works in burst-mode.
 */
void EVE_cmd_waitchange_burst(const uint32_t adr)
{
    spi_transmit_burst(CMD_WAITCHANGE);
    spi_transmit_burst(adr);
}

/**
 * @brief Wait until the given condition is true.
 */
void EVE_cmd_waitcond(const uint32_t adr, const uint32_t func, const uint32_t ref, const uint32_t mask)
{
    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_WAITCOND);
        spi_transmit_32(adr);
        spi_transmit_32(func);
        spi_transmit_32(ref);
        spi_transmit_32(mask);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_WAITCOND);
        spi_transmit_burst(adr);
        spi_transmit_burst(func);
        spi_transmit_burst(ref);
        spi_transmit_burst(mask);
    }
}

/**
 * @brief Wait until the given condition is true, only works in burst-mode.
 */
void EVE_cmd_waitcond_burst(const uint32_t adr, const uint32_t func, const uint32_t ref, const uint32_t mask)
{
    spi_transmit_burst(CMD_WAITCOND);
    spi_transmit_burst(adr);
    spi_transmit_burst(func);
    spi_transmit_burst(ref);
    spi_transmit_burst(mask);
}

/**
 * @brief Enable the watchdog timer and set the watchdog reset interval in clocks.
 */
void EVE_cmd_watchdog(const uint32_t init_val)
{
    if (EVE_BURST_INACTIVE == g_cmd_burst)
    {
        eve_begin_cmd(CMD_WATCHDOG);
        spi_transmit_32(init_val);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_WATCHDOG);
        spi_transmit_burst(init_val);
    }
}

/**
 * @brief Enable the watchdog timer and set the watchdog reset interval in clocks, only works in burst-mode.
 */
void EVE_cmd_watchdog_burst(const uint32_t init_val)
{
    spi_transmit_burst(CMD_WATCHDOG);
    spi_transmit_burst(init_val);
}

#endif
