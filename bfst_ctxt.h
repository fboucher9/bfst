/* See LICENSE for license details. */

/*

Module: bfst_ctxt.h

Description:

    Definition of context structures.  The context structures are used to access
    other modules at a given scope.  There are three scopes:

    1.  body

        Modules that are global or common to all views.

    2.  view

        Modules that are per-view or common to all tty.

    3.  tty

        Modules that are per-tty.

    Each context structure is composed of pointers to other structures.  This is used
    to reduce header file dependencies.

*/

/* Reverse include guard */
#if defined(INC_BFST_CTXT_H)
#error include bfst_ctxt.h once
#endif /* #if defined(INC_BFST_CTXT_H) */

#define INC_BFST_CTXT_H

/* Predefine all modules */
struct bfst_options;

struct bfst_display;

struct bfst_font;

struct bfst_font_list;

struct bfst_color;

struct bfst_view_list;

struct bfst_ptr;

struct bfst_tty_list;

struct bfst_view;

struct bfst_window;

struct bfst_draw;

struct bfst_ptr_show;

struct bfst_unique;

struct bfst_tty;

struct bfst_focus;

struct bfst_tab;

/*

Structure: bfst_body_ctxt

Description:

    Define a context structure for 'body' scope.

*/
struct bfst_body_ctxt
{
    /* Pointer to body container */
    struct bfst_body * p_body;

    /* Pointer to options module */
    struct bfst_options * p_opts;

    /* Pointer to display module */
    struct bfst_display * p_display;

    /* Pointer to view_list module */
    struct bfst_view_list * p_view_list;

    /* Pointer to ptr shape module */
    struct bfst_ptr * p_ptr;

    /* Pointer to unique module */
    struct bfst_unique * p_unique;

    /* Pointer to focus module */
    struct bfst_focus * p_focus;

    /* Pointer to font_list module */
    struct bfst_font_list * p_font_list;

}; /* struct bfst_body_ctxt */

/*

Structure: bfst_view_ctxt

Description:

    Define a context structure for 'view' scope.

*/
struct bfst_view_ctxt
{
    /* Pointer to view container */
    struct bfst_view * p_view;

    /* Pointer to font module */
    struct bfst_font * p_font;

    /* Pointer to color module */
    struct bfst_color * p_color;

    /* Pointer to window module */
    struct bfst_window * p_window;

    /* Pointer to draw module */
    struct bfst_draw * p_draw;

    /* Pointer to ptr_show module */
    struct bfst_ptr_show * p_ptr_show;

    /* Pointer to tty_list module */
    struct bfst_tty_list * p_tty_list;

    /* Pointer to tab module */
    struct bfst_tab * p_tab;

    /* Pointer to parent body context */
    struct bfst_body_ctxt * p_body_ctxt;

}; /* struct bfst_view_ctxt */

/*

Structure: bfst_tty_ctxt

Description:

    Define a context structure for 'tty' scope.

*/
struct bfst_tty_ctxt
{
    /* Pointer to container */
    struct bfst_tty * p_term;

    /* Pointer to log module */
    struct bfst_log * p_log;

    /* Pointer to selection module */
    struct bfst_sel * p_sel;

    /* Pointer to child/pty module */
    struct bfst_child * p_child;

    /* Pointer to parent view context */
    struct bfst_view_ctxt * p_view_ctxt;

    /* Pointer to parent body context */
    struct bfst_body_ctxt * p_body_ctxt;

}; /* struct bfst_tty_ctxt */

/* end-of-file: bfst_ctxt.h */
