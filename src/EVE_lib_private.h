/*
@file    EVE_lib_private.h
@brief   support function function prototypes
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


*/

#ifndef EVE_LIB_PRIVATE_H
#define EVE_LIB_PRIVATE_H

extern volatile uint8_t g_cmd_burst; /* flag from EVE_commands.c to indicate cmd-burst is active */

void eve_private_block_write(const uint8_t * const p_data, const uint16_t len);
void eve_private_block_write_burst(const uint8_t * const p_data, const uint16_t len);
void eve_block_transfer(const uint8_t * const p_data, const uint32_t len);
void eve_private_string_write(const char * const p_text);
void eve_private_string_write_burst(const char * const p_text);


#endif /* EVE_LIB_PRIVATE_H */
