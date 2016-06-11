/* See LICENSE for license details. */

/*

Module: bfst_ptr_show.h

Description:

    Show or hide the mouse pointer shape.

*/

/* Assert for multiple inclusion */
#if defined(INC_BFST_PTR_SHOW_H)
#error include bfst_ptr_show.h once
#endif

/* Reverse include guard */
#define INC_BFST_PTR_SHOW_H

/* Predefine view context handle */
struct bfst_view_ctxt;

/*

Structure: bfst_ptr_show

Description:

    State of bfst_ptr module local to a view.

*/
struct bfst_ptr_show
{
    /* Is the pointer shape visible in view */
    char ptr_state;

}; /* struct bfst_cursor_local */

/* Interface: */

/* Initialize module */
void bfst_ptr_show_init(
    struct bfst_view_ctxt const * const p_view_ctxt);

/* Cleanup module */
void bfst_ptr_show_cleanup(
    struct bfst_view_ctxt const * const p_view_ctxt);

/* Change visibility of mouse cursor shape */
void bfst_ptr_show_change(
    struct bfst_view_ctxt const * const p_view_ctxt,
    char const b_show);

/* end-of-file: bfst_ptr_show.h */
