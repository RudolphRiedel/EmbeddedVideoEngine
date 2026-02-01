/*
@file    EVE_target_Test.h
@brief   target specific includes, definitions and functions
@version 6.0
@date    2026-02-01
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
- modified for BT820
- rewritten, moved all the functions to EVE_target.c

*/

#ifndef EVE_TARGET_TEST_H
#define EVE_TARGET_TEST_H

#if defined (SOFTWARE_TEST)

#include <stdint.h>

#define EVE_DELAY_1MS 4U  /* no real delay needed for the software tests */
#define EVE_TEST_BUFFER_SIZE 8192U

extern uint8_t EVE_spi_test_buffer[];
extern uint16_t EVE_spi_test_buffer_index;
extern uint8_t EVE_spi_test_buffer_receive[];
typedef struct
{
    uint32_t called;
} test_call_counter_t;

extern test_call_counter_t Test_EVE_pdn_set;
extern test_call_counter_t Test_EVE_pdn_clear;
extern test_call_counter_t Test_EVE_cs_set;
extern test_call_counter_t Test_EVE_cs_clear;
extern test_call_counter_t Test_EVE_spi_transmit;
extern test_call_counter_t Test_EVE_spi_transmit_32;
extern test_call_counter_t Test_EVE_spi_transmit_32_addr;
extern test_call_counter_t Test_EVE_spi_transmit_burst;
extern test_call_counter_t Test_EVE_spi_receive;
extern test_call_counter_t Test_EVE_fetch_flash_byte;

/* mock function declarations */
void DELAY_MS(uint16_t val);
void EVE_pdn_set(void);
void EVE_pdn_clear(void);
void EVE_cs_set(void);
void EVE_cs_clear(void);
void spi_transmit(uint8_t data);
void spi_transmit_32_addr(uint32_t data);
void spi_transmit_32(uint32_t data);
void spi_transmit_burst(uint32_t data);
uint8_t spi_receive(uint8_t data);
uint8_t fetch_flash_byte(const uint8_t *p_data);

/* Test helper functions */
void EVE_test_reset_counters(void);
void EVE_test_print_report(void);

#endif /* SOFTWARE_TEST */

#endif /* EVE_TARGET_TEST_H */
