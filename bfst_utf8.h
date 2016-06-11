/* See LICENSE for license details. */

/*

Module: bfst_utf8.h

Description:

    Utility functions for encoding and decoding of UTF-8 characters.

*/

/* Special code that indicates an invalid UTF-8 character */
#define BFST_UTF8_INVALID   0xFFFD

/* Maximum length of a UTF-8 character */
#define BFST_UTF8_SIZE      4

/* Interface: */

unsigned long int
bfst_utf8_decode(
    char const * const p_utf8_buf,
    unsigned long int * const i_utf8_code,
    unsigned long int const i_utf8_buf_len);

unsigned long int
bfst_utf8_encode(
    unsigned long int const i_utf8_code,
    char * const p_utf8_buf);

char *
bfst_utf8_strchr(
    char * const p_utf8_buf,
    unsigned long int const i_utf8_code);

