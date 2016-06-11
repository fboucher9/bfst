/* See LICENSE for license details. */

/*

Module: bfst_utf8.c

Description:

    Utility functions for encoding and decoding of UTF-8 characters.

*/

#include <string.h>

#include "bfst_utf8.h"

#include "bfst_tools.h"

/*

    Handling of UTF-8 bytes:

        0x00 - 0x7F     -   Single byte
        0x80 - 0xBF     -   Single byte
        0xC0 - 0xDF     -   Two bytes
        0xE0 - 0xEF     -   Three bytes
        0xF0 - 0xFF     -   Four bytes

*/

static unsigned char const auc_fxterm_utf8_byte[BFST_UTF8_SIZE + 1] = {0x80,    0, 0xC0, 0xE0, 0xF0};

static unsigned char const auc_fxterm_utf8_mask[BFST_UTF8_SIZE + 1] = {0xC0, 0x80, 0xE0, 0xF0, 0xF8};

static unsigned long int const aui_fxterm_utf8_min[BFST_UTF8_SIZE + 1] = {       0,    0,  0x80,  0x800,  0x10000};

static unsigned long int const aui_fxterm_utf8_max[BFST_UTF8_SIZE + 1] = {0x10FFFF, 0x7F, 0x7FF, 0xFFFF, 0x10FFFF};

static
unsigned long int
bfst_utf8_validate(
    unsigned long int * u,
    unsigned long int i)
{
    if (!BFST_TOOLS_BETWEEN(*u, aui_fxterm_utf8_min[i], aui_fxterm_utf8_max[i]) || BFST_TOOLS_BETWEEN(*u, 0xD800, 0xDFFF))
    {
        *u = BFST_UTF8_INVALID;
    }

    for (i = 1; *u > aui_fxterm_utf8_max[i]; ++i)
        ;

    return i;
}

static
unsigned long int
bfst_utf8_decode_byte(
    char c,
    unsigned long int *i)
{
    for(*i = 0; *i < BFST_TOOLS_LEN(auc_fxterm_utf8_mask); ++(*i))
    {
        if(((unsigned char)c & auc_fxterm_utf8_mask[*i]) == auc_fxterm_utf8_byte[*i])
        {
            return (unsigned char)c & ~auc_fxterm_utf8_mask[*i];
        }
    }
    return 0;
}

unsigned long int
bfst_utf8_decode(
    char const *c,
    unsigned long int *u,
    unsigned long int clen)
{
    unsigned long int i, j, len, type;
    unsigned long int udecoded;

    *u = BFST_UTF8_INVALID;
    if(!clen)
        return 0;
    udecoded = bfst_utf8_decode_byte(c[0], &len);
    if(!BFST_TOOLS_BETWEEN(len, 1, BFST_UTF8_SIZE))
        return 1;
    for(i = 1, j = 1; i < clen && j < len; ++i, ++j) {
        udecoded = (udecoded << 6) | bfst_utf8_decode_byte(c[i], &type);
        if(type != 0)
            return j;
    }
    if(j < len)
        return 0;
    *u = udecoded;
    bfst_utf8_validate(u, len);
    return len;
}

static
char
bfst_utf8_encode_byte(
    unsigned long int u,
    unsigned long int i)
{
    return auc_fxterm_utf8_byte[i] | (u & ~auc_fxterm_utf8_mask[i]);
}

unsigned long int
bfst_utf8_encode(
    unsigned long int u,
    char *c)
{
    unsigned long int len, i;

    len = bfst_utf8_validate(&u, 0);
    if(len > BFST_UTF8_SIZE)
        return 0;
    for(i = len - 1; i != 0; --i) {
        c[i] = bfst_utf8_encode_byte(u, 0);
        u >>= 6;
    }
    c[0] = bfst_utf8_encode_byte(u, len);
    return len;
}

char *
bfst_utf8_strchr(
    char * p_input_string,
    unsigned long int ul_ref_rune)
{
    unsigned long int ul_tmp_rune;
    unsigned long int i, j, len;

    len = strlen(p_input_string);
    for(i = 0, j = 0; i < len; i += j) {
        if(!(j = bfst_utf8_decode(&p_input_string[i], &ul_tmp_rune, len - i)))
            break;
        if(ul_tmp_rune == ul_ref_rune)
            return &(p_input_string[i]);
    }
    return NULL;
}

