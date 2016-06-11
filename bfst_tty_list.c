/* See LICENSE for license details. */

/*

Module: bfst_tty_list.c

Description:

    List of bfst_tty object.

*/

#include <unistd.h>

#include <X11/Xlib.h>

#include "bfst_body.h"

#define BFST_TTY_LIST_GROW_COUNT 32u

void bfst_tty_list_init(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    struct bfst_tty_list * const p_tty_list = p_view_ctxt->p_tty_list;

    p_tty_list->a_tty_list = NULL;

    p_tty_list->i_tty_max_count = 0;

    p_tty_list->i_tty_count = 0;

} /* bfst_tty_list_init() */

void bfst_tty_list_cleanup(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    (void)(p_view_ctxt);

} /* bfst_tty_list_cleanup() */

static struct bfst_tty * bfst_tty_node_new(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    struct bfst_tty * const p_term = tnew(p_view_ctxt);

    bfst_sel_init(p_term);

    bfst_child_new(&p_term->o_term_ctxt);

    return p_term;

} /* bfst_tty_node_new() */

void bfst_tty_list_add(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    struct bfst_tty_list * const p_tty_list = p_view_ctxt->p_tty_list;

    struct bfst_tty * const p_term = bfst_tty_node_new(p_view_ctxt);

    if (p_term)
    {
        if (p_tty_list->i_tty_count >= p_tty_list->i_tty_max_count)
        {
            p_tty_list->i_tty_max_count += BFST_TTY_LIST_GROW_COUNT;

            unsigned long int const i_array_length = p_tty_list->i_tty_max_count * sizeof(struct bfst_tty *);

            p_tty_list->a_tty_list = (struct bfst_tty * *)(bfst_realloc(p_tty_list->a_tty_list, i_array_length));
        }

        p_tty_list->a_tty_list[p_tty_list->i_tty_count] = p_term;

        p_tty_list->i_tty_count ++;

        bfst_tab_last(p_view_ctxt);
    }

} /* bfst_tty_list_add() */

void bfst_tty_list_check_for_dead(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    int i = 0;

    struct bfst_tty_list * const p_tty_list = p_view_ctxt->p_tty_list;

    while (i < p_tty_list->i_tty_count)
    {
        struct bfst_tty * const p_term = p_tty_list->a_tty_list[i];

        if (p_term && p_term->child.b_dead)
        {
            if (p_term->child.i_read_fd != p_term->child.i_pty_fd)
            {
                close(p_term->child.i_read_fd);
            }

            if (p_term->child.i_write_fd != p_term->child.i_pty_fd)
            {
                close(p_term->child.i_write_fd);
            }

            if (-1 != p_term->child.i_pty_fd)
            {
                close(p_term->child.i_pty_fd);
            }

            tdelete(p_term);

            p_tty_list->a_tty_list[i] = NULL;

            int k;

            for (k=i ; k < p_tty_list->i_tty_count - 1; k++)
            {
                p_tty_list->a_tty_list[k] = p_tty_list->a_tty_list[k + 1];
            }

            p_tty_list->i_tty_count --;

            bfst_tab_refresh(p_view_ctxt);
        }
        else
        {
            i ++;
        }
    }
} /* bfst_tty_list_check_for_dead() */

void bfst_tty_list_sel_clear(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    struct bfst_tty_list const * const p_tty_list = p_view_ctxt->p_tty_list;

    if (p_tty_list)
    {
        int k;

        for (k=0; k<p_tty_list->i_tty_count; k++)
        {
            struct bfst_tty * const p_term = p_tty_list->a_tty_list[k];

            if (p_term)
            {
                bfst_sel_clear(p_term);
            }
        }
    }

} /* bfst_tty_list_sel_clear() */

static void bfst_tty_node_resize(
    struct bfst_tty * const p_term,
    int const i_term_width_char,
    int const i_term_height_char)
{
    tresize( p_term, i_term_width_char, i_term_height_char);

    bfst_child_resize(
        &p_term->o_term_ctxt,
        p_term->col,
        p_term->row,
        p_term->o_term_ctxt.p_view_ctxt->p_window->i_tty_width_pixels,
        p_term->o_term_ctxt.p_view_ctxt->p_window->i_tty_height_pixels);
}

void bfst_tty_list_resize(
    struct bfst_view_ctxt const * const p_view_ctxt,
    int const i_term_width_char,
    int const i_term_height_char)
{
    int i_term_index;

    struct bfst_tty_list * const p_tty_list = p_view_ctxt->p_tty_list;

    for (i_term_index = 0; i_term_index < p_tty_list->i_tty_count; i_term_index ++)
    {
        struct bfst_tty * p_term;

        p_term = p_tty_list->a_tty_list[i_term_index];

        if (p_term)
        {
            bfst_tty_node_resize(p_term, i_term_width_char, i_term_height_char);
        }
    }
}

/* end-of-file: bfst_tty_list.c */
