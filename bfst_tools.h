/* See LICENSE for license details. */

/*

Module: bfst_tools.h

Description:

    Bunch of miscellaneous macros and functions.

*/

/* Reverse include guard */
#if defined(INC_BFST_TOOLS_H)
#error include bfst_tools.h once
#endif /* #if defined(INC_BFST_TOOLS_H) */

#define INC_BFST_TOOLS_H

/* Macros: */

#define BFST_TOOLS_MIN(a, b)  ((a) < (b) ? (a) : (b))

#define BFST_TOOLS_MAX(a, b)  ((a) < (b) ? (b) : (a))

#define BFST_TOOLS_LEN(a)     (sizeof(a) / sizeof(a)[0])

#define BFST_TOOLS_DEFAULT(a, b)     (a) = (a) ? (a) : (b)

#define BFST_TOOLS_BETWEEN(x, a, b)  ((a) <= (x) && (x) <= (b))

#define BFST_TOOLS_LIMIT(x, a, b)    (x) = (x) < (a) ? (a) : (x) > (b) ? (b) : (x)

/* Functions: */

void bfst_die(void);

unsigned long int bfst_time(void);

signed long int bfst_write(
    int const h_file,
    const char * const p_buf,
    unsigned long int const i_len);

void * bfst_malloc(
    unsigned long int const len);

void * bfst_realloc(
    void *p,
    unsigned long int len);

char * bfst_strdup(
    char const * const s);

void bfst_free(
    void * const p);

#if defined(BFST_CFG_DEBUG)

void bfst_msg(
    char const * const p_format,
    ...);

#endif /* #if defined(BFST_CFG_DEBUG) */

/* end-of-file: bfst_tools.h */
