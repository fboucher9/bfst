/* See LICENSE for license details. */

/*

Module: bfst_tools.c

Description:

    Bunch of miscellaneous macros and functions.

*/

#include <stdarg.h>

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <time.h>

#include <unistd.h>

#include "bfst_tools.h"

/*

Function: bfst_die

Description:

    Exit application with an error code.

*/
void bfst_die(void)
{
    exit(EXIT_FAILURE);

} /* bfst_die() */

/*

Function: bfst_time

Description:

    Get current time in milli-seconds.

*/
unsigned long int
bfst_time(void)
{
    struct timespec o_time;

    clock_gettime(CLOCK_MONOTONIC, &o_time);

    return (unsigned long int)((o_time.tv_sec * 1000) + (o_time.tv_nsec / 1000000));

} /* bfst_time() */

/*

Function: bfst_write

Description:

    Write array of characters to file.  Loop until all characters have been
    written.

*/
signed long int bfst_write(
    int const h_file,
    char const * const p_buf,
    unsigned long int const i_len)
{
    char const * p_iterator = p_buf;

    unsigned long int i_remain = i_len;

    while (i_remain > 0)
    {
        signed long int const r = write(h_file, p_iterator, i_remain);

        if (r < 0)
        {
            return r;
        }

        i_remain -= r;

        p_iterator += r;
    }

    return i_len;

} /* bfst_write() */

/*

Function: bfst_malloc

Description:

    Allocate memory.  Programs dies if allocation fails.

*/
void * bfst_malloc(
    unsigned long int const len)
{
    void * const p = malloc(len);

    if (!p)
    {
        bfst_die();
    }

    return p;

} /* bfst_malloc() */

/*

Function: bfst_realloc

Description:

    Re-allocate memory.

*/
void *
bfst_realloc(
    void * const p,
    unsigned long int len)
{
    void * const p_result = realloc(p, len);

    if (!p_result)
    {
        bfst_die();
    }

    return p_result;

} /* bfst_realloc() */

/*

Function: bfst_strdup

Description:

    Duplicate a string.  Program dies if allocation fails.

*/
char *
bfst_strdup(
    char const * const s)
{
    char * const p_result = strdup(s);

    if (!p_result)
    {
        bfst_die();
    }

    return p_result;
}

/*

Function: bfst_free

Description:

    Free memory allocated by bfst_malloc, bfst_realloc or bfst_strdup.

*/
void
bfst_free(
    void * const p)
{
    free(p);
} /* bfst_free() */

#if defined(BFST_CFG_DEBUG)

/*

Function: bfst_msg

Description:

    Print a message to stderr or to a log file.

*/
void bfst_msg(
    char const * const p_format,
    ...)
{
    va_list o_args;

    va_start(o_args, p_format);

    vfprintf(stderr, p_format, o_args);

    va_end(o_args);

} /* bfst_msg() */

#endif /* #if defined(BFST_CFG_DEBUG) */

/* end-of-file: bfst_tools.c */
