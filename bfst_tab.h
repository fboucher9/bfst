/* See LICENSE for license details. */

/*

Module: bfst_tab.h

Description:

    Management of currently selected tab within a view.

*/

/* Reverse include guard */
#if defined(INC_BFST_TAB_H)
#error include bfst_tab.h once
#endif /* #if defined(INC_BFST_TAB_H) */

#define INC_BFST_TAB_H

/* Predefine context */
struct bfst_view_ctxt;

/* Predefine tty object */
struct bfst_tty;

/*

Structure: bfst_tab

Description:

    State for currently selected tab.

*/
struct bfst_tab
{
    /* Pointer to currently selected terminal */
    struct bfst_tty * p_tty_object;

    /* Index of currently selected terminal */
    int i_tty_index;

}; /* struct bfst_tab */

/* Interface: */

void bfst_tab_init(
    struct bfst_view_ctxt const * const p_view_ctxt);

void bfst_tab_cleanup(
    struct bfst_view_ctxt const * const p_view_ctxt);

void bfst_tab_next(
    struct bfst_view_ctxt const * const p_view_ctxt);

void bfst_tab_last(
    struct bfst_view_ctxt const * const p_view_ctxt);

void bfst_tab_refresh(
    struct bfst_view_ctxt const * const p_view_ctxt);

struct bfst_tty *
bfst_tab_get(
    struct bfst_view_ctxt const * const p_view_ctxt);

/* end-of-file: bfst_tab.h */
