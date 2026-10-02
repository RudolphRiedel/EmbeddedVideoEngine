/*
@file    EVE_lib_private.c
@brief   support functions to avoid code duplication
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

*/

#include "EVE.h"
#include "EVE_lib_private.h"

#if EVE_GEN > 4
#define MEM_WRITE ((uint32_t) 0x80000000UL) /* EVE Host Memory Write */
#endif

void eve_private_block_write(const uint8_t * const p_data, const uint16_t len)
{
    uint8_t padding;

    padding = (uint8_t) (len & 3U); /* 0, 1, 2 or 3 */
    padding = 4U - padding;         /* 4, 3, 2 or 1 */
    padding &= 3U;                  /* 3, 2, 1 or 0 */

    for (uint16_t count = 0U; count < len; count++)
    {
        spi_transmit(fetch_flash_byte(&p_data[count]));
    }

    while (padding > 0U)
    {
        spi_transmit((uint8_t) 0);
        padding--;
    }
}

void eve_private_block_write_burst(const uint8_t * const p_data, const uint16_t len)
{
    uint16_t const len_transfer = (uint16_t) ((len + 3U) / 4U); /* len in bytes, transfer in 32-bit words */
    uint16_t count = 0U;

    for (uint16_t index = 0U; index < len_transfer; index++)
    {
        uint32_t word = 0U;

        for (uint8_t byte_idx = 0U; byte_idx < 4U; byte_idx++)
        {
            uint8_t byte_val;

            if (count < len)
            {
                byte_val = fetch_flash_byte(&p_data[count]);
                count++;
            }
            else
            {
                byte_val = 0U; /* padding */
            }

            word |= ((uint32_t) byte_val) << (byte_idx * 8U);
        }

        spi_transmit_burst(word);
    }
}

void eve_block_transfer(const uint8_t * const p_data, const uint32_t len)
{
    uint32_t bytes_left;
    uint32_t offset = 0U;

    bytes_left = len;
    while (bytes_left > 0U)
    {
        uint32_t block_len;

        block_len = (bytes_left > 3840UL) ? 3840UL : bytes_left;

        EVE_cs_set();

#if EVE_GEN > 4
        spi_transmit_32_addr(MEM_WRITE | REG_CMDB_WRITE); /* EVE5 uses 32 bit adressing */
#else
        spi_transmit((uint8_t) 0xB0U); /* high-byte of REG_CMDB_WRITE + MEM_WRITE */
        spi_transmit((uint8_t) 0x25U); /* middle-byte of REG_CMDB_WRITE */
        spi_transmit((uint8_t) 0x78U); /* low-byte of REG_CMDB_WRITE */
#endif
        eve_private_block_write(&p_data[offset], (uint16_t) block_len);
        EVE_cs_clear();
        offset += block_len;
        bytes_left -= block_len;
        EVE_execute_cmd();
    }
}

/* write a string to coprocessor memory in context of a command: */
/* no chip-select, just plain SPI-transfers */
void eve_private_string_write(const char * const p_text)
{
    /* treat the array as bunch of bytes */
    const uint8_t *const p_bytes = (const uint8_t *)p_text;
    uint8_t exit_flag = 0U;

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

void eve_private_string_write_burst(const char * const p_text)
{
    const uint8_t *p_bytes = (const uint8_t *)p_text;
    uint8_t byte3 = 0;

    for (uint8_t index = 0U; index < 63U; index++)
    {
        const uint8_t byte0 = *p_bytes; if (byte0 != 0U) { p_bytes++; }
        const uint8_t byte1 = *p_bytes; if (byte1 != 0U) { p_bytes++; }
        const uint8_t byte2 = *p_bytes; if (byte2 != 0U) { p_bytes++; }
        byte3 = *p_bytes; if (byte3 != 0U) { p_bytes++; }
#if defined (EVE_DMA)
        uint32_t calc = (uint32_t)byte0 | ((uint32_t)byte1 << 8U) | ((uint32_t)byte2 << 16U) | ((uint32_t)byte3 << 24U);
        spi_transmit_burst(calc);
#else
        spi_transmit(byte0);
        spi_transmit(byte1);
        spi_transmit(byte2);
        spi_transmit(byte3);
#endif
        if (0U == byte3)
        {
            break; /* use break and not return to be BARR-C: 2018 compliant */
        }
    }

    if (0U != byte3)
    {
        spi_transmit_burst(0U);
    }
}
