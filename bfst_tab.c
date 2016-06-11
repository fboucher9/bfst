/* See LICENSE for license details. */

/*

Module: bfst_tab.c

Description:

    Management of currently selected tab within a view.

*/

#include <X11/Xlib.h>

#include "bfst_body.h"

/*

Function: bfst_tab_init

Description:

    Initialization of module.

*/
void bfst_tab_init(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    (void)(p_view_ctxt);

} /* bfst_tab_init() */

/*

Function: bfst_tab_cleanup

Description:

    Free all resources allocated by module.

*/
void bfst_tab_cleanup(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    (void)(p_view_ctxt);

} /* bfst_tab_cleanup() */

/*

Function: bfst_tab_next

Description:

    Select the next terminal in the list.  If the last is selected, then
    wraparound to the first.

*/
void bfst_tab_next(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    struct bfst_tty_list const * const p_tty_list = p_view_ctxt->p_tty_list;

    struct bfst_tab * const p_tab = p_view_ctxt->p_tab;

    if (p_tty_list->i_tty_count)
    {
        p_tab->i_tty_index ++;

        if (p_tab->i_tty_index >= p_tty_list->i_tty_count)
        {
            p_tab->i_tty_index = 0;
        }

        p_tab->p_tty_object = p_tty_list->a_tty_list[p_tab->i_tty_index];
    }
    else
    {
        p_tab->p_tty_object = NULL;

        p_tab->i_tty_index = 0;
    }

} /* bfst_tab_next() */

/*

Function: bfst_tab_last

Description:

    A new terminal has been created, select it.

*/
void bfst_tab_last(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    struct bfst_tty_list const * const p_tty_list = p_view_ctxt->p_tty_list;

    struct bfst_tab * const p_tab = p_view_ctxt->p_tab;

    if (p_tty_list->i_tty_count)
    {
        p_tab->i_tty_index = p_tty_list->i_tty_count - 1;

        p_tab->p_tty_object = p_tty_list->a_tty_list[p_tab->i_tty_index];
    }
    else
    {
        p_tab->p_tty_object = NULL;

        p_tab->i_tty_index = 0;
    }

} /* bfst_tab_last() */

/*

Function: bfst_tab_refresh

Description:

    The list of terminals has been modified, verify that the
    currently selected one is still valid.  If the current selection
    is invalid, then select another.

*/
void bfst_tab_refresh(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    struct bfst_tty_list const * const p_tty_list = p_view_ctxt->p_tty_list;

    struct bfst_tab * const p_tab = p_view_ctxt->p_tab;

    if (p_tty_list->i_tty_count)
    {
        if (p_tab->i_tty_index >= p_tty_list->i_tty_count)
        {
            p_tab->i_tty_index = p_tty_list->i_tty_count - 1;
        }

        p_tab->p_tty_object = p_tty_list->a_tty_list[p_tab->i_tty_index];
    }
    else
    {
        p_tab->p_tty_object = NULL;

        p_tab->i_tty_index = 0;
    }
} /* bfst_tab_refresh() */

/*

Function: bfst_tab_get

Description:

    Return pointer to currently selected bsft_tty object.

*/
struct bfst_tty *
bfst_tab_get(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    struct bfst_tab * const p_tab = p_view_ctxt->p_tab;

    return p_tab->p_tty_object;

} /* bfst_tab_get() */

/* end-of-file: bfst_tab.c */
