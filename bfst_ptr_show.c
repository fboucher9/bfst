/* See LICENSE for license details. */

/*

Module: bfst_ptr_show.h

Description:

    Show or hide the mouse pointer shape.

*/

#include <X11/Xlib.h>

#include "bfst_body.h"

/*

Function: bfst_ptr_show_set

Description:

    Apply the ptr show state regardless of previous state.

*/
static void bfst_ptr_show_set(
    struct bfst_view_ctxt const * const p_view_ctxt,
    char const b_show)
{
    struct bfst_ptr_show * const p_ptr_show = p_view_ctxt->p_ptr_show;

    struct bfst_window const * const p_window = p_view_ctxt->p_window;

    struct bfst_body_ctxt const * const p_body_ctxt = p_view_ctxt->p_body_ctxt;

    struct bfst_display const * const p_display = p_body_ctxt->p_display;

    Cursor const h_cur_res = bfst_ptr_get(p_body_ctxt, b_show);

    XDefineCursor(
        p_display->dpy,
        p_window->h_win_res,
        h_cur_res);

    p_ptr_show->ptr_state = b_show;

} /* bfst_ptr_show_set() */

/*

Function: bfst_ptr_show_init

Description:

    Initialize module.

*/
void bfst_ptr_show_init(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    /* When opening a new view, the ptr is initially visible. */
    bfst_ptr_show_set(p_view_ctxt, 1);

} /* bfst_ptr_show_init() */

/*

Function: bfst_ptr_show_cleanup

Description:

    Cleanup module.

*/
void bfst_ptr_show_cleanup(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    /* unused parameter */
    (void)(p_view_ctxt);

} /* bfst_ptr_show_cleanup() */

/*

Function: bfst_ptr_show_change()

Description:

    Change of the ptr visibility state.

*/
void bfst_ptr_show_change(
    struct bfst_view_ctxt const * const p_view_ctxt,
    char const b_show)
{
    struct bfst_ptr_show const * const p_ptr_show = p_view_ctxt->p_ptr_show;

    if (b_show != p_ptr_show->ptr_state)
    {
        bfst_ptr_show_set(p_view_ctxt, b_show);
    }

} /* bfst_ptr_show_change() */

/* end-of-file: bfst_ptr_show.c */
