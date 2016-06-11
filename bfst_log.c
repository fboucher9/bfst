/* See LICENSE for license details. */

/*

Module: bfst_log.c

*/

#include <stddef.h>

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <X11/Xlib.h>

#include "bfst_body.h"

#define BFST_LOG_MAX_COUNT 4000u

#define BFST_LOG_DEFAULT_FG_COLOR 7

#define BFST_LOG_DEFAULT_BG_COLOR 0

void bfst_log_init(
    struct bfst_tty_ctxt const * const p_term_ctxt)
{
    struct bfst_log * const p_log = p_term_ctxt->p_log;

    {
        unsigned long int i_array_len;

        i_array_len = BFST_LOG_MAX_COUNT * sizeof(struct bfst_log_line *);

        p_log->a_line = (struct bfst_log_line * *)( bfst_malloc( i_array_len));

    }

    {
        unsigned int i_array_iterator;

        for (i_array_iterator = 0; i_array_iterator < BFST_LOG_MAX_COUNT; i_array_iterator ++)
        {
            p_log->a_line[i_array_iterator] = NULL;
        }
    }

    p_log->i_index = 0;

    p_log->i_count = 0;

    p_log->i_max_count = BFST_LOG_MAX_COUNT;
}

void bfst_log_cleanup(
    struct bfst_tty_ctxt const * const p_term_ctxt)
{
    struct bfst_log * const p_log = p_term_ctxt->p_log;

    unsigned int i_array_iterator;

    for (i_array_iterator = 0; i_array_iterator < p_log->i_max_count; i_array_iterator ++)
    {
        if (p_log->a_line[i_array_iterator])
        {
            bfst_free(p_log->a_line[i_array_iterator]);

            p_log->a_line[i_array_iterator] = NULL;
        }
    }

    bfst_free(p_log->a_line);
}

unsigned int bfst_log_get_row_count(
    struct bfst_tty_ctxt const * const p_term_ctxt)
{
    struct bfst_log * const p_log = p_term_ctxt->p_log;

    return p_log->i_count;
}

unsigned int bfst_log_get_array_index(
    struct bfst_tty_ctxt const * const p_term_ctxt,
    unsigned int const i_row_index)
{
    struct bfst_log * const p_log = p_term_ctxt->p_log;

    unsigned int i_array_index;

    i_array_index = (p_log->i_index + i_row_index);

    if (i_array_index >= p_log->i_max_count)
    {
        i_array_index -= p_log->i_max_count;
    }

    return i_array_index;
}

unsigned long long int bfst_log_get_line_unique(
    struct bfst_tty_ctxt const * const p_term_ctxt,
    unsigned int const i_row_index)
{
    struct bfst_log * const p_log = p_term_ctxt->p_log;

    if (i_row_index < p_log->i_count)
    {
        int const i_array_index = bfst_log_get_array_index(p_term_ctxt, i_row_index);

        if (p_log->a_line[i_array_index])
        {
            return p_log->a_line[i_array_index]->i_unique;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

unsigned short int bfst_log_get_line_len(
    struct bfst_tty_ctxt const * const p_term_ctxt,
    unsigned int const i_row_index)
{
    struct bfst_log * const p_log = p_term_ctxt->p_log;

    if (i_row_index < p_log->i_count)
    {
        int i_array_index = bfst_log_get_array_index(p_term_ctxt, i_row_index);

        if (p_log->a_line[i_array_index])
        {
            return p_log->a_line[i_array_index]->i_len;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

void
bfst_log_get_char(
    struct bfst_tty_ctxt const * const
        p_term_ctxt,
    unsigned int const
        i_row_index,
    unsigned short int const
        i_col_index,
    struct bfst_glyph * const
        p_char_info)
{
    struct bfst_log * const p_log = p_term_ctxt->p_log;

    struct bfst_glyph o_char_info;

    struct bfst_glyph o_space;

    o_space.u = ' ';

    o_space.fg = BFST_LOG_DEFAULT_FG_COLOR;

    o_space.bg = BFST_LOG_DEFAULT_BG_COLOR;

    o_space.mode = ATTR_NULL;

    o_space.flags = 0;

    if (i_row_index < p_log->i_count)
    {
        int i_array_index;

        i_array_index =
            bfst_log_get_array_index(
                p_term_ctxt,
                i_row_index);

        if (p_log->a_line[i_array_index])
        {
            if (i_col_index < p_log->a_line[i_array_index]->i_len)
            {
                o_char_info = p_log->a_line[i_array_index]->a_char[i_col_index];
            }
            else
            {
                o_char_info = o_space;
            }
        }
        else
        {
            o_char_info = o_space;
        }
    }
    else
    {
        o_char_info = o_space;
    }

    *p_char_info = o_char_info;
}

void
bfst_log_scroll(
    struct bfst_tty_ctxt const * const
        p_term_ctxt,
    int const
        y)
{
    struct bfst_log * const p_log = p_term_ctxt->p_log;

    struct bfst_tty const * const p_term = p_term_ctxt->p_term;

    /* Determine length of the line */
    int i_line_len = p_term->col;

    while(i_line_len > 0 && p_term->p_cur->a_line[y].a_glyph[i_line_len - 1].u == ' ')
    {
        --i_line_len;
    }

    /* Is there room left in log ? */
    unsigned int i_log_index;

    i_log_index = p_log->i_index + p_log->i_count;

    if (i_log_index >= p_log->i_max_count)
    {
        i_log_index -= p_log->i_max_count;
    }

    /* Bump existing line */
    if (p_log->a_line[i_log_index])
    {
        bfst_free(p_log->a_line[i_log_index]);

        p_log->a_line[i_log_index] = NULL;
    }

    /* Write into line */
    p_log->a_line[i_log_index] = (struct bfst_log_line *)(bfst_malloc(sizeof(struct bfst_log_line) + sizeof(struct bfst_glyph) * i_line_len));

    struct bfst_log_line * const p_log_line = p_log->a_line[i_log_index];

    if (p_log_line)
    {
        p_log_line->i_unique = bfst_unique_pick(p_term_ctxt->p_body_ctxt);

        p_log_line->i_len = i_line_len;

        p_log_line->a_char = (struct bfst_glyph *)(p_log_line + 1);

        /* Fill in buffer */
        {
            int i_line_iterator;

            for (i_line_iterator = 0; i_line_iterator < i_line_len; i_line_iterator ++)
            {
                struct bfst_glyph u;

                u = p_term->p_cur->a_line[y].a_glyph[i_line_iterator];

                p_log_line->a_char[i_line_iterator] = u;
            }
        }

        if (p_log->i_count < p_log->i_max_count)
        {
            /* Allocate a new line in the log */
            p_log->i_count ++;
        }
        else
        {
            /* Rotate all lines in the log */
            p_log->i_index ++;
            if (p_log->i_index >= p_log->i_max_count)
            {
                p_log->i_index = 0;
            }
        }
    }

}

