/* See LICENSE for license details. */

/*

Module: bfst_view.c

Description:

    Management of a single view object.

*/

#include <stdlib.h>

#include <string.h>

#include <X11/Xlib.h>

#include "bfst_body.h"

/*

Function: bfst_view_init_ctxt

Description:

    Initialize the bfst_view_ctxt structure.

*/
static void bfst_view_init_ctxt(
    struct bfst_body_ctxt const * const p_body_ctxt,
    struct bfst_view * const p_view)
{
    p_view->o_view_ctxt.p_view = p_view;

    p_view->o_view_ctxt.p_color = &p_view->o_color;

    p_view->o_view_ctxt.p_font = &p_view->o_font;

    p_view->o_view_ctxt.p_window = &p_view->o_window;

    p_view->o_view_ctxt.p_draw = &p_view->o_draw;

    p_view->o_view_ctxt.p_ptr_show = &p_view->o_ptr_show;

    p_view->o_view_ctxt.p_tty_list = &p_view->o_tty_list;

    p_view->o_view_ctxt.p_tab = &p_view->o_tab;

    p_view->o_view_ctxt.p_body_ctxt = (struct bfst_body_ctxt *)(p_body_ctxt);

} /* bfst_view_init_ctxt() */

/*

Function: bfst_view_init()

*/
static void bfst_view_init(
    struct bfst_body_ctxt const * const p_body_ctxt,
    struct bfst_view * const p_view,
    struct bfst_tty * const p_term_existing)
{
    memset(p_view, 0x00, sizeof(struct bfst_view));

    /* context */
    bfst_view_init_ctxt(p_body_ctxt, p_view);

    /* fonts */
    bfst_font_init(&p_view->o_view_ctxt);

    /* colors */
    bfst_color_load_all(&p_view->o_view_ctxt);

    /* window */
    bfst_window_init(&p_view->o_view_ctxt);

    /* drawing */
    bfst_draw_resize(&p_view->o_view_ctxt);

    /* ptr */
    bfst_ptr_show_init(&p_view->o_view_ctxt);

    /* tty list */
    bfst_tty_list_init(&p_view->o_view_ctxt);

    /* tab */
    bfst_tab_init(&p_view->o_view_ctxt);

    /* tty node */
    bfst_tty_list_add(&p_view->o_view_ctxt, p_term_existing);

    bfst_view_resize(p_view, p_view->o_window.i_width_pixels, p_view->o_window.i_height_pixels);

} /* bfst_view_init() */

struct bfst_view * bfst_view_new(
    struct bfst_body_ctxt const * const p_body_ctxt,
    struct bfst_tty * const p_term_existing)
{
    struct bfst_view * const p_view = (struct bfst_view *)(bfst_malloc(sizeof(struct bfst_view)));

    if (p_view)
    {
        bfst_view_init(p_body_ctxt, p_view, p_term_existing);
    }

    return p_view;

}

static void bfst_view_cleanup(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    bfst_tab_cleanup(p_view_ctxt);

    bfst_draw_cleanup(p_view_ctxt);

    bfst_window_cleanup(p_view_ctxt);

    bfst_ptr_show_cleanup(p_view_ctxt);

    /* bfst_color_cleanup(p_view_ctxt); */

    bfst_font_cleanup(p_view_ctxt);

} /* bfst_view_cleanup() */

void bfst_view_delete(
    struct bfst_view_ctxt * const p_view_ctxt)
{
    struct bfst_view * const p_view = p_view_ctxt->p_view;

    bfst_view_cleanup(p_view_ctxt);

    free(p_view);

} /* bfst_view_delete() */

void bfst_view_resize(
    struct bfst_view * const p_view,
    int const i_screen_width_pix,
    int const i_screen_height_pix)
{
    unsigned int i_term_width_char;
    unsigned int i_term_height_char;

    struct bfst_window * const p_window = &p_view->o_window;

    struct bfst_font const * const p_font = p_view->o_view_ctxt.p_font;

    if (i_screen_width_pix != 0)
    {
        p_window->i_width_pixels = i_screen_width_pix;
    }

    if (i_screen_height_pix != 0)
    {
        p_window->i_height_pixels = i_screen_height_pix;
    }

    if (p_font->i_width && (p_window->i_width_pixels > 2 * p_window->i_border))
    {
        i_term_width_char = (p_window->i_width_pixels - 2 * p_window->i_border) / p_font->i_width;

        if (i_term_width_char < 1)
        {
            i_term_width_char = 1;
        }
    }
    else
    {
        i_term_width_char = 1;
    }

    if (p_font->i_height && (p_window->i_height_pixels > 2 * p_window->i_border))
    {
        i_term_height_char = (p_window->i_height_pixels - 2 * p_window->i_border) / p_font->i_height;
        if (i_term_height_char < 1)
        {
            i_term_height_char = 1;
        }
    }
    else
    {
        i_term_height_char = 1;
    }

    if (
        (
            i_term_width_char != p_window->i_tty_cols)
        || (
            i_term_height_char != p_window->i_tty_rows))
    {
        bfst_window_resize(&p_view->o_view_ctxt, i_term_width_char, i_term_height_char);

        bfst_draw_resize(&p_view->o_view_ctxt);

        bfst_tty_list_resize(&p_view->o_view_ctxt);
    }

} /* bfst_view_resize() */

/* end-of-file: bfst_view.c */
