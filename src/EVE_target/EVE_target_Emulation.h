/*
@file    EVE_target_Emulation.h
@brief   target specific includes, definitions and functions for BT8xx Emulator
@version 1.0
@date    2026-09-27
@author  Rudolph Riedel / adapted for BT8xx emulator

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
*/

#ifndef EVE_TARGET_EMULATION_H
#define EVE_TARGET_EMULATION_H

#include <stdint.h>
#include <windows.h>

#include "bt8xxemu.h"

/* The global emulator instance pointer.
   Must be set by the application before calling any EVE_Library functions. */
extern BT8XXEMU_Emulator *emulator;

#define DELAY_MS(ms) Sleep(ms)

static inline void EVE_pdn_set(void)
{
    /* No-op for the emulator: the emulated EVE is always powered on. */
}

static inline void EVE_pdn_clear(void)
{
    /* No-op for the emulator. */
}

static inline void EVE_cs_set(void)
{
    BT8XXEMU_chipSelect(emulator, 1);
}

static inline void EVE_cs_clear(void)
{
    BT8XXEMU_chipSelect(emulator, 0);
}

static inline void spi_transmit(uint8_t data)
{
    BT8XXEMU_transfer(emulator, data);
}

/* SPI Transmit 32-bit address (MSB first, used for EVE5) */
static inline void spi_transmit_32_addr(uint32_t data)
{
    spi_transmit((uint8_t)(data >> 24U));
    spi_transmit((uint8_t)(data >> 16U));
    spi_transmit((uint8_t)(data >> 8U));
    spi_transmit((uint8_t)(data & 0x000000ffUL));
}

/* SPI Transmit 32-bit data (little-endian, used for register access) */
static inline void spi_transmit_32(uint32_t data)
{
    spi_transmit((uint8_t)(data & 0x000000ffUL));
    spi_transmit((uint8_t)(data >> 8U));
    spi_transmit((uint8_t)(data >> 16U));
    spi_transmit((uint8_t)(data >> 24U));
}

static inline void spi_transmit_burst(uint32_t data)
{
    spi_transmit_32(data);
}

static inline uint8_t spi_receive(uint8_t data)
{
    return BT8XXEMU_transfer(emulator, data);
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#endif /* EVE_TARGET_EMULATION_H */
