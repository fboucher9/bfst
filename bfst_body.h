/* See LICENSE for license details. */

/*

Module: bfst_body.h

Description:

    Management of global modules and dispatching of events to views and ttys.

*/

#include "bfst_ctxt.h"

#include "bfst_opts.h"

#include "bfst_display.h"

#include "bfst_font.h"

#include "bfst_font_list.h"

#include "bfst_color.h"

#include "bfst_ptr.h"

#include "bfst_ptr_show.h"

#include "bfst_draw.h"

#include "bfst_window.h"

#include "bfst_unique.h"

#include "bfst_tty_list.h"

#include "bfst_tab.h"

#include "bfst_view.h"

#include "bfst_view_list.h"

#include "bfst_focus.h"

#include "bfst_log.h"

#include "bfst_sel.h"

#include "bfst_child.h"

#include "bfst_utf8.h"

#include "bfst_tty.h"

#include "bfst_zombie.h"

#include "bfst_tools.h"

#include "bfst_key.h"

/*

Structure: bfst_body

Description:

    Container of modules for 'body' scope.  This is a structure of structures.

Comments:

    -   You may add members here that are at global scope or common to all views.

*/
struct bfst_body
{
    /* Context */
    struct bfst_body_ctxt o_body_ctxt;

    /* Options */
    struct bfst_options o_opts;

    /* Per display or per screen information ... */
    struct bfst_display o_display;

    /* Per window information... */
    struct bfst_view_list o_view_list;

    /* Pointer shapes */
    struct bfst_ptr o_ptr;

    /* Unique identifier for lines */
    struct bfst_unique o_unique;

    /* Pointer to view with user input focus */
    struct bfst_focus o_focus;

    /* List of font names */
    struct bfst_font_list o_font_list;

}; /* struct bfst_body */

/* Interface: */

/* Initialize module */
char
bfst_body_init(
    struct bfst_body * const p_body,
    struct bfst_options const * const p_options);

/* Cleanup module */
void
bfst_body_cleanup(
    struct bfst_body * const p_body);

/* Main event handling loop */
void
bfst_body_run(
    struct bfst_body * const p_body);

/* Select a custom font */
void
bfst_body_set_font(
    struct bfst_view_ctxt const * const p_view_ctxt,
    char const * const p_font_name);

/* Coordinate conversion */
int x2col(
    struct bfst_tty * p_term,
    int x);

/* Coordinate conversion */
int y2row(
    struct bfst_tty * p_term,
    int y);

/* end-of-file: bfst_body.h */
