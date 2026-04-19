/*
@file    EVE_commands_BT82x
@brief   contains BT82 functions
@version 6.0
@date    2026-04-28
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

*/

#include "EVE_commands_BT82x.h"


/* BT820 */
#if EVE_GEN > 4

#define DUMMY_BYTE ((uint8_t) 0x00U)
#define FIFO_BIT_MASK ((uint16_t)0x3fffU)
#define MEM_WRITE ((uint32_t) 0x80000000L) /* EVE Host Memory Write */

/* define NULL if it not already is */
#ifndef NULL
#include <stddef.h>
#endif

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

        uint32_t length = (len | 0x03UL);

        for (uint32_t count = 0U; count < length; count++)
        {
            spi_transmit(fetch_flash_byte(&p_data[count]));
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

        uint32_t length = (len | 0x03UL);

        for (uint32_t count = 0U; count < length; count++)
        {
            spi_transmit(p_data[count]);
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
            uint32_t length = (len | 0x03UL);

            for (uint32_t count = 0U; count < length; count++)
            {
                p_data[count] = spi_receive(DUMMY_BYTE); /* read data byte by sending dummy bytes */
            }
        }

        EVE_cs_clear();
    }
}

/* write a string to coprocessor memory in context of a command: */
/* no chip-select, just plain SPI-transfers */
static void private_string_write(const char * const p_text); /* prototype to comply with MISRA */

static void private_string_write(const char * const p_text)
{
    /* treat the array as bunch of bytes */
    const uint8_t *const p_bytes = (const uint8_t *)p_text;
    uint8_t exit_flag = 0U;

    if (0U == g_cmd_burst)
    {
        for (uint8_t textindex = 0U; (textindex < 249U) && (0U == exit_flag); textindex += 4U)
        {
            uint32_t calc = 0U;

            for (uint8_t index = 0U; index < 4U; index++)
            {
                uint8_t data;

                data = p_bytes[textindex + index];

                if (0U == data)
                {
                    exit_flag = 1U; /* leave outer loop */
                    break; /* leave inner loop */
                }
                calc += ((uint32_t)data) << (index * 8U);
            }

            spi_transmit_32(calc);
        }

        if(0U == exit_flag) /* left outer loop because the string is too long, send zeroes to terminate the string */
        {
            spi_transmit_32(0U);
        }
    }
    else /* we are in burst mode, so every transfer is 32 bits */
    {
        for (uint8_t textindex = 0U; (textindex < 249U) && (0U == exit_flag); textindex += 4U)
        {
            uint32_t calc = 0U;

            for (uint8_t index = 0U; index < 4U; index++)
            {
                uint8_t data;

                data = p_bytes[textindex + index];

                if (0U == data)
                {
                    exit_flag = 1U; /* leave outer loop */
                    break; /* leave inner loop */
                }
                calc += ((uint32_t)data) << (index * 8U);
            }

            spi_transmit_burst(calc);
        }

        if(0U == exit_flag) /* left outer loop because the string is too long, send zeroes to terminate the string */
        {
            spi_transmit_burst(0U);
        }
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
    private_string_write(p_path);
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
    private_string_write(p_name);
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
    private_string_write(p_name);
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
    private_string_write(p_name);
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

    if (0UL == options) /* direct data, not by Media-FIFO, Flash or SD */
    {
        if (p_data != NULL)
        {
            block_transfer(p_data, len);
        }
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

    if (0UL == options) /* direct data, not by Media-FIFO, Flash or SD */
    {
        if (p_data != NULL)
        {
            block_transfer(p_data, len);
        }
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
        (0UL == (options & EVE_OPT_FS))) /* direct data, neither by Media-FIFO or from Flash */
    {
        if (p_data != NULL)
        {
            block_transfer(p_data, len);
        }
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

    if (0UL == options) /* direct data, not by Media-FIFO, Flash or SD */
    {
        if (p_data != NULL)
        {
            block_transfer(p_data, len);
        }
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

    if (0UL == options) /* direct data, not by Media-FIFO, Flash or SD */
    {
        if (p_data != NULL)
        {
            block_transfer(p_data, len);
        }
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

    if (0UL == options) /* direct data, not by Media-FIFO, Flash or SD */
    {
        if (p_data != NULL)
        {
            block_transfer(p_data, len);
        }
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
    spi_transmit_32(i16_i16_to_u32(format, wid));
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
    private_string_write(p_text);
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
    private_string_write(p_name);
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
    private_string_write(p_name);
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
    private_string_write(p_name);
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
    private_string_write(p_name);
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
    private_string_write(p_text);
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
        block_transfer(p_data, len);
    }
}


/* ##################################################################
    init functions
##################################################################### */


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
    EVE_memWrite32(REG_SO_MODE, EVE_SO_MODE_2); /* 2-pixel per clock for single LVDS channel mode */

    EVE_memWrite32(REG_RE_DEST, EVE_SWAPCHAIN_0);
    EVE_memWrite32(REG_RE_FORMAT, EVE_RGB8);
    EVE_memWrite32(REG_RE_W, EVE_HSIZE); /* CMD_RENDERTARGET: Render target width in pixels and must be a multiple of 16. */
    EVE_memWrite32(REG_RE_H, EVE_VSIZE); /* CMD_RENDERTARGET: Render target height in pixels. w × h must be a multiple of 128 */
    EVE_memWrite32(REG_RE_DITHER, 0UL);
    EVE_memWrite32(REG_RE_ACTIVE, 1UL);

    EVE_memWrite32(REG_LVDSTX_CTRL_CH0, 2); /* VESA/Format 2 Mapping for 24-bit, Single Pixel per Clock */

    /* target: 51.2MHz LVDSTX clock for 1024x600 panel*/
    //EVE_memRead32(REG_LVDSTX_PLLCFG);
    EVE_memWrite32(REG_LVDSTX_PLLCFG, setlvdspll_value(PLL_LOCK_PERIOD, 1u, 5u)); /* scanclk_2x -> 576MHz / 6 = 96MHz -> LVDSTX = 48MHz */

    EVE_memWrite32(REG_LVDSTX_EN, LVDS_CH0_EN);
    DELAY_MS(10);

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
    EVE_memWrite32(REG_DISP, 1UL); /* enable backlight */

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
    EVE_cmdWrite(EVE_SETSYSCLKDIV, 0x17U); /* set SYSCLK_DIV to the default value of 7 for a the system clock of 72MHz. */
    EVE_cmdWrite(EVE_ACTIVE, 0U); /* start EVE */

    DELAY_MS(50U); /* give EVE a moment of silence to power up, a BT820 answers about 34ms after ACTIVE and booting takes about 27ms */

    ret = wait_boot();
    if (E_OK == ret)
    {
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

    g_cmd_burst = 42U;
    EVE_dma_buffer[0U] = 0x000001ffUL; /* REG_CMDB_WRITE + MEM_WRITE low mid hi */
    EVE_dma_buffer_index = 1U;
#else
    g_cmd_burst = 42U;
    EVE_cs_set();
    spi_transmit_32_addr(MEM_WRITE | REG_CMDB_WRITE);
#endif
}

/**
 * @brief Draw a circular arc with rounded caps.
 */
void EVE_cmd_arc(const int16_t xc0, const int16_t yc0, const uint16_t rad0, const uint16_t rad1, const uint16_t angle0, const uint16_t angle1)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CMD_ARC);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(rad0, rad1));
        spi_transmit_32(u16_u16_to_u32(angle0, angle1));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ARC);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(u16_u16_to_u32(rad0, rad1));
        spi_transmit_burst(u16_u16_to_u32(angle0, angle1));
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
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CMD_CGRADIENT);
        spi_transmit_32(shape);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(wid, hgt));
        spi_transmit_32(rgb0);
        spi_transmit_32(rgb1);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_CGRADIENT);
        spi_transmit_burst(shape);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(i16_i16_to_u32(wid, hgt));
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
    spi_transmit_burst(i16_i16_to_u32(wid, hgt));
    spi_transmit_burst(rgb0);
    spi_transmit_burst(rgb1);
}

/**
 * @brief Enable or disable render optomization for widgets.
 */
void EVE_cmd_enableregion(const uint32_t enable)
{
    if (0U == g_cmd_burst)
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
    if (0U == g_cmd_burst)
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
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CMD_GLOW);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(wid, hgt));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_GLOW);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(i16_i16_to_u32(wid, hgt));
    }
}

/**
 * @brief Draws an additive glow effect centered in a rectangle, using the current color, only works in burst-mode.
 */
void EVE_cmd_glow_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt)
{
    spi_transmit_burst(CMD_GLOW);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(i16_i16_to_u32(wid, hgt));
}

/**
 * @brief Waits until the render engine is idle.
 * @note - This is not be used within a display-list, in the given example this is placed after CMD_SWAP.
 */
void EVE_cmd_graphicsfinish(void)
{
    if (0U == g_cmd_burst)
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
    if (0U == g_cmd_burst)
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
    if (0U == g_cmd_burst)
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
    if (0U == g_cmd_burst)
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
    if (0U == g_cmd_burst)
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
    if (0U == g_cmd_burst)
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
    if (0U == g_cmd_burst)
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
    if (0U == g_cmd_burst)
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
    if (0U == g_cmd_burst)
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
    if (0U == g_cmd_burst)
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

/* the following commands require a patch loaded with CMD_LOADPATCH */

/**
 * @brief Start a Region section.
 */
void EVE_cmd_region(void)
{
    if (0U == g_cmd_burst)
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
 * @brief Start a Region section, only works in burst-mode.
 */
void EVE_cmd_region_burst(void)
{
    spi_transmit_burst(CMD_REGION);
}

/**
 * @brief Stop a Region section.
 */
void EVE_cmd_endregion(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CMD_ENDREGION);
        spi_transmit_32(i16_i16_to_u32(xco, yco));
        spi_transmit_32(u16_u16_to_u32(wid, hgt));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ENDREGION);
        spi_transmit_burst(i16_i16_to_u32(xco, yco));
        spi_transmit_burst(u16_u16_to_u32(wid, hgt));
    }
}

/**
 * @brief Stop a Region section, only works in burst-mode.
 */
void EVE_cmd_endregion_burst(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt)
{
    spi_transmit_burst(CMD_ENDREGION);
    spi_transmit_burst(i16_i16_to_u32(xco, yco));
    spi_transmit_burst(u16_u16_to_u32(wid, hgt));
}

/**
 * @brief Draw scaled text.
 */
void EVE_cmd_textscale(const int16_t xco, const int16_t yco, const uint16_t font, const uint16_t options, const uint32_t scale, const char * const p_text)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CMD_TEXTSCALE);
        spi_transmit_32(i16_i16_to_u32(xco, yco));
        spi_transmit_32(u16_u16_to_u32(font, options));
        spi_transmit_32(scale);
        private_string_write(p_text);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_TEXTSCALE);
        spi_transmit_burst(i16_i16_to_u32(xco, yco));
        spi_transmit_burst(u16_u16_to_u32(font, options));
        spi_transmit_burst(scale);
        private_string_write(p_text);
    }
}

/**
 * @brief SDraw scaled text, only works in burst-mode.
 */
void EVE_cmd_textscale_burst(const int16_t xco, const int16_t yco, const uint16_t font, const uint16_t options, const uint32_t scale, const char * const p_text)
{
    spi_transmit_burst(CMD_TEXTSCALE);
    spi_transmit_burst(i16_i16_to_u32(xco, yco));
    spi_transmit_burst(u16_u16_to_u32(font, options));
    spi_transmit_burst(scale);
    private_string_write(p_text);
}

/**
 * @brief Draw text at an angle.
 */
void EVE_cmd_textangle(const int16_t xco, const int16_t yco, const uint16_t font, const uint16_t options, const uint32_t angle, const char * const p_text)
{
if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CMD_TEXTANGLE);
        spi_transmit_32(i16_i16_to_u32(xco, yco));
        spi_transmit_32(u16_u16_to_u32(font, options));
        spi_transmit_32(angle);
        private_string_write(p_text);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_TEXTANGLE);
        spi_transmit_burst(i16_i16_to_u32(xco, yco));
        spi_transmit_burst(u16_u16_to_u32(font, options));
        spi_transmit_burst(angle);
        private_string_write(p_text);
    }
}

/**
 * @brief Draw text at an angle, only works in burst-mode.
 */
void EVE_cmd_textangle_burst(const int16_t xco, const int16_t yco, const uint16_t font, const uint16_t options, const uint32_t angle, const char * const p_text)
{
    spi_transmit_burst(CMD_TEXTANGLE);
    spi_transmit_burst(i16_i16_to_u32(xco, yco));
    spi_transmit_burst(u16_u16_to_u32(font, options));
    spi_transmit_burst(angle);
    private_string_write(p_text);
}

/**
 * @brief Draw text within a box and scroll the text smoothly.
 */
void EVE_cmd_textticker(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt, const uint16_t font, const uint16_t options, const uint32_t offset, const char * const p_text)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CMD_TEXTTICKER);
        spi_transmit_32(i16_i16_to_u32(xco, yco));
        spi_transmit_32(u16_u16_to_u32(wid, hgt));
        spi_transmit_32(u16_u16_to_u32(font, options));
        spi_transmit_32(offset);
        private_string_write(p_text);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_TEXTTICKER);
        spi_transmit_burst(i16_i16_to_u32(xco, yco));
        spi_transmit_burst(u16_u16_to_u32(wid, hgt));
        spi_transmit_burst(u16_u16_to_u32(font, options));
        spi_transmit_burst(offset);
        private_string_write(p_text);
    }
}

/**
 * @brief Draw text within a box and scroll the text smoothly, only works in burst-mode.
 */
void EVE_cmd_textticker_burst(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt, const uint16_t font, const uint16_t options, const uint32_t offset, const char * const p_text)
{
    spi_transmit_burst(CMD_TEXTTICKER);
    spi_transmit_burst(i16_i16_to_u32(xco, yco));
    spi_transmit_burst(u16_u16_to_u32(wid, hgt));
    spi_transmit_burst(u16_u16_to_u32(font, options));
    spi_transmit_burst(offset);
    private_string_write(p_text);
}


/**
 * @brief Draw a seven segment display for decimal numbers from 0 to 9.
 */
void EVE_cmd_sevenseg(const int16_t xc0, const int16_t yc0, const uint16_t size, const uint16_t number)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CMD_SEVENSEG);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(size, number));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SEVENSEG);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(u16_u16_to_u32(size, number));
    }
}

/**
 * @brief Draw a seven segment display for decimal numbers from 0 to 9, only works in burst-mode.
 */
void EVE_cmd_sevenseg_burst(const int16_t xc0, const int16_t yc0, const uint16_t size, const uint16_t number)
{
    spi_transmit_burst(CMD_SEVENSEG);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(u16_u16_to_u32(size, number));
}

/**
 * @brief Display a multiline message box.
 */
void EVE_cmd_messagebox(const uint16_t font, const uint16_t options, const char * const p_text)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CMD_MESSAGEBOX);
        spi_transmit_32(u16_u16_to_u32(font, options));
        private_string_write(p_text);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_MESSAGEBOX);
        spi_transmit_burst(u16_u16_to_u32(font, options));
        private_string_write(p_text);
    }
}

/**
 * @brief Display a multiline message box, only works in burst-mode.
 */
void EVE_cmd_messagebox_burst(const uint16_t font, const uint16_t options, const char * const p_text)
{
    spi_transmit_burst(CMD_MESSAGEBOX);
    spi_transmit_burst(u16_u16_to_u32(font, options));
    private_string_write(p_text);
}

/**
 * @brief Display a multiline tooltip box.
 */
void EVE_cmd_tooltip(const int16_t xco, const int16_t yco, const uint16_t font, const uint16_t options, const char * const p_text)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CMD_TOOLTIP);
        spi_transmit_32(i16_i16_to_u32(xco, yco));
        spi_transmit_32(u16_u16_to_u32(font, options));
        private_string_write(p_text);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_TOOLTIP);
        spi_transmit_burst(i16_i16_to_u32(xco, yco));
        spi_transmit_burst(u16_u16_to_u32(font, options));
        private_string_write(p_text);
    }
}

/**
 * @brief Display a multiline tooltip box, only works in burst-mode.
 */
void EVE_cmd_tooltip_burst(const int16_t xco, const int16_t yco, const uint16_t font, const uint16_t options, const char * const p_text)
{
    spi_transmit_burst(CMD_TOOLTIP);
    spi_transmit_burst(i16_i16_to_u32(xco, yco));
    spi_transmit_burst(u16_u16_to_u32(font, options));
    private_string_write(p_text);
}

/**
 * @brief Draw a keyboard or keypad.
 */
void EVE_cmd_keyboard(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt, const uint16_t font, const uint16_t options, const char * const p_text)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CMD_KEYBOARD);
        spi_transmit_32(i16_i16_to_u32(xco, yco));
        spi_transmit_32(u16_u16_to_u32(wid, hgt));
        spi_transmit_32(u16_u16_to_u32(font, options));
        private_string_write(p_text);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_KEYBOARD);
        spi_transmit_burst(i16_i16_to_u32(xco, yco));
        spi_transmit_burst(u16_u16_to_u32(wid, hgt));
        spi_transmit_burst(u16_u16_to_u32(font, options));
        private_string_write(p_text);
    }
}

/**
 * @brief Draw a keyboard or keypad, only works in burst-mode.
 */
void EVE_cmd_keyboard_burst(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt, const uint16_t font, const uint16_t options, const char * const p_text)
{
    spi_transmit_burst(CMD_KEYBOARD);
    spi_transmit_burst(i16_i16_to_u32(xco, yco));
    spi_transmit_burst(u16_u16_to_u32(wid, hgt));
    spi_transmit_burst(u16_u16_to_u32(font, options));
    private_string_write(p_text);
}

/**
 * @brief Create a blurred image of the current screen.
 */
void EVE_cmd_blurscreen(void)
{
    if (0U == g_cmd_burst)
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
 * @brief Create a blurred image of the current screen, only works in burst-mode.
 */
void EVE_cmd_blurscreen_burst(void)
{
    spi_transmit_burst(CMD_BLURSCREEN);
}

/**
 * @brief Draw a previously blurred image of the screen generated by cmd_blurscreen.
 */
void EVE_cmd_blurdraw(void)
{
    if (0U == g_cmd_burst)
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
 * @brief Draw a previously blurred image of the screen generated by cmd_blurscreen, only works in burst-mode.
 */
void EVE_cmd_blurdraw_burst(void)
{
    spi_transmit_burst(CMD_BLURDRAW);
}

/**
 * @brief Draws an LED-style graphic to simulate a round LED.
 */
void EVE_cmd_ledround(const int16_t xco, const int16_t yco, const uint16_t radius, const uint16_t options)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CMD_LEDROUND);
        spi_transmit_32(i16_i16_to_u32(xco, yco));
        spi_transmit_32(u16_u16_to_u32(radius, options));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_LEDROUND);
        spi_transmit_burst(i16_i16_to_u32(xco, yco));
        spi_transmit_burst(u16_u16_to_u32(radius, options));
    }
}

/**
 * @brief DDraws an LED-style graphic to simulate a round LED, only works in burst-mode.
 */
void EVE_cmd_ledround_burst(const int16_t xco, const int16_t yco, const uint16_t radius, const uint16_t options)
{
    spi_transmit_burst(CMD_LEDROUND);
    spi_transmit_burst(i16_i16_to_u32(xco, yco));
    spi_transmit_burst(u16_u16_to_u32(radius, options));
}

/**
 * @brief Draws an LED-style graphic to simulate a rectangular LED.
 */
void EVE_cmd_ledrect(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt, const uint16_t options)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CMD_LEDRECT);
        spi_transmit_32(i16_i16_to_u32(xco, yco));
        spi_transmit_32(u16_u16_to_u32(wid, hgt));
        spi_transmit_32(u16_u16_to_u32(options, 0U));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_LEDRECT);
        spi_transmit_burst(i16_i16_to_u32(xco, yco));
        spi_transmit_burst(u16_u16_to_u32(wid, hgt));
        spi_transmit_burst(u16_u16_to_u32(options, 0U));
    }
}

/**
 * @brief Draws an LED-style graphic to simulate a rectangular LED, only works in burst-mode.
 */
void EVE_cmd_ledrect_burst(const int16_t xco, const int16_t yco, const uint16_t wid, const uint16_t hgt, const uint16_t options)
{
    spi_transmit_burst(CMD_LEDRECT);
    spi_transmit_burst(i16_i16_to_u32(xco, yco));
    spi_transmit_burst(u16_u16_to_u32(wid, hgt));
    spi_transmit_burst(u16_u16_to_u32(options, 0U));
}

/**
 * @brief Draws a feedback emoji.
 */
void EVE_cmd_feedbackicon(const int16_t xco, const int16_t yco, const uint16_t rad1, const uint16_t rad2, const int16_t sentiment)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CMD_FEEDBACKICON);
        spi_transmit_32(i16_i16_to_u32(xco, yco));
        spi_transmit_32(u16_u16_to_u32(rad1, rad2));
        spi_transmit_32(u16_u16_to_u32(sentiment, 0U));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_FEEDBACKICON);
        spi_transmit_burst(i16_i16_to_u32(xco, yco));
        spi_transmit_burst(u16_u16_to_u32(rad1, rad2));
        spi_transmit_burst(u16_u16_to_u32(sentiment, 0U));
    }
}

/**
 * @brief Draws a feedback emoji, only works in burst-mode.
 */
void EVE_cmd_feedbackicon_burst(const int16_t xco, const int16_t yco, const uint16_t rad1, const uint16_t rad2, const int16_t sentiment)
{
    spi_transmit_burst(CMD_FEEDBACKICON);
    spi_transmit_burst(i16_i16_to_u32(xco, yco));
    spi_transmit_burst(u16_u16_to_u32(rad1, rad2));
    spi_transmit_burst(u16_u16_to_u32(sentiment, 0U));
}

/**
 * @brief Change data for a BARGRAPH bitmap into a VERTEX2F points for a LINESTRIP.
 */
void EVE_cmd_plotdraw(const uint32_t source, const uint16_t len, const uint16_t opt, const int16_t xco, const int16_t yco, const uint32_t xscale, const uint32_t yscale, const uint32_t threshold)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CMD_PLOTDRAW);
        spi_transmit_32(source);
        spi_transmit_32(u16_u16_to_u32(len, opt));
        spi_transmit_32(i16_i16_to_u32(xco, yco));
        spi_transmit_32(xscale);
        spi_transmit_32(yscale);
        spi_transmit_32(threshold);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_PLOTDRAW);
        spi_transmit_burst(source);
        spi_transmit_burst(u16_u16_to_u32(len, opt));
        spi_transmit_burst(i16_i16_to_u32(xco, yco));
        spi_transmit_burst(xscale);
        spi_transmit_burst(yscale);
        spi_transmit_burst(threshold);
    }
}

/**
 * @brief Change data for a BARGRAPH bitmap into a VERTEX2F points for a LINESTRIP, only works in burst-mode.
 */
void EVE_cmd_plotdraw_burst(const uint32_t source, const uint16_t len, const uint16_t opt, const int16_t xco, const int16_t yco, const uint32_t xscale, const uint32_t yscale, const uint32_t threshold)
{
    spi_transmit_burst(CMD_PLOTDRAW);
    spi_transmit_burst(source);
    spi_transmit_burst(u16_u16_to_u32(len, opt));
    spi_transmit_burst(i16_i16_to_u32(xco, yco));
    spi_transmit_burst(xscale);
    spi_transmit_burst(yscale);
    spi_transmit_burst(threshold);
}

/**
 * @brief Stream data in BARGRAPH bitmap format into a VERTEX2F points for a LINESTRIP.
 */
void EVE_cmd_plotstream(const uint16_t len, const uint16_t opt, const int16_t xco, const int16_t yco, const uint32_t xscale, const uint32_t yscale, const uint32_t threshold, const uint8_t * const p_data)
{
    uint16_t len_transfer;
    len_transfer = (len + 3U) / 4U; /* len in bytes, transfer in 32-bit words */

    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CMD_PLOTSTREAM);
        spi_transmit_32(u16_u16_to_u32(len, opt));
        spi_transmit_32(i16_i16_to_u32(xco, yco));
        spi_transmit_32(xscale);
        spi_transmit_32(yscale);
        spi_transmit_32(threshold);
        for(uint16_t index = 0U; index < len_transfer; index++)
        {
            spi_transmit_32(((uint32_t *)p_data)[index]);
        }
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_PLOTSTREAM);
        spi_transmit_burst(u16_u16_to_u32(len, opt));
        spi_transmit_burst(i16_i16_to_u32(xco, yco));
        spi_transmit_burst(xscale);
        spi_transmit_burst(yscale);
        spi_transmit_burst(threshold);
        for(uint16_t index = 0U; index < len_transfer; index++)
        {
            spi_transmit_burst(((uint32_t *)p_data)[index]);
        }
    }
}

/**
 * @brief Stream data in BARGRAPH bitmap format into a VERTEX2F points for a LINESTRIP, only works in burst-mode.
 */
void EVE_cmd_plotstream_burst(const uint16_t len, const uint16_t opt, const int16_t xco, const int16_t yco, const uint32_t xscale, const uint32_t yscale, const uint32_t threshold,  const uint8_t * const p_data)
{
    uint16_t len_transfer;
    len_transfer = (len + 3U) / 4U; /* len in bytes, transfer in 32-bit words */
    spi_transmit_burst(CMD_PLOTSTREAM);
    spi_transmit_burst(u16_u16_to_u32(len, opt));
    spi_transmit_burst(i16_i16_to_u32(xco, yco));
    spi_transmit_burst(xscale);
    spi_transmit_burst(yscale);
    spi_transmit_burst(threshold);
    for(uint16_t index = 0U; index < len_transfer; index++)
    {
        spi_transmit_burst(((uint32_t *)p_data)[index]);
    }
}

/**
 * @brief Apply an offset to the current touch coordinates.
 */
void EVE_cmd_touchoffset(const int16_t xco, const int16_t yco)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CMD_TOUCHOFFSET);
        spi_transmit_32(i16_i16_to_u32(xco, yco));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_TOUCHOFFSET);
        spi_transmit_burst(i16_i16_to_u32(xco, yco));
    }
}

/**
 * @brief Apply an offset to the current touch coordinates, only works in burst-mode.
 */
void EVE_cmd_touchoffset_burst(const int16_t xco, const int16_t yco)
{
    spi_transmit_burst(CMD_TOUCHOFFSET);
    spi_transmit_burst(i16_i16_to_u32(xco, yco));
}

/**
 * @brief Ends the touch offset mode started by CMD_TOUCHOFFSET
 */
void EVE_cmd_endtouchoffset(void)
{
    if (0U == g_cmd_burst)
    {
        eve_begin_cmd(CMD_ENDTOUCHOFFSET);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ENDTOUCHOFFSET);
    }
}

/**
 * @brief ends the touch offset mode started by CMD_TOUCHOFFSET, only works in burst-mode.
 */
void EVE_cmd_endtouchoffset_burst(void)
{
    spi_transmit_burst(CMD_ENDTOUCHOFFSET);
}


#endif
