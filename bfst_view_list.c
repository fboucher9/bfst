/* See LICENSE for license details. */

/*

Module: bfst_view_list.c

*/

#include <X11/Xlib.h>

#include "bfst_body.h"

#define BFST_VIEW_LIST_GROW_COUNT 32u

void
bfst_view_list_init(
    struct bfst_body_ctxt const * const
        p_body_ctxt)
{
    (void)(p_body_ctxt);
}

void
bfst_view_list_cleanup(
    struct bfst_body_ctxt const * const
        p_body_ctxt)
{
    (void)(p_body_ctxt);
}

void
bfst_view_list_add(
    struct bfst_body_ctxt const * const
        p_body_ctxt)
{
    struct bfst_view * p_view;

    p_view = bfst_view_new(p_body_ctxt);

    if (p_view)
    {
        struct bfst_view_list * const p_view_list = p_body_ctxt->p_view_list;

        if (p_view_list->i_view_count >= p_view_list->i_view_max_count)
        {
            p_view_list->i_view_max_count += BFST_VIEW_LIST_GROW_COUNT;

            unsigned long int i_array_length;

            i_array_length = p_view_list->i_view_max_count * sizeof(struct bfst_view *);

            p_view_list->a_view_list = (struct bfst_view_ctxt * *)(bfst_realloc(p_view_list->a_view_list, i_array_length));
        }

        p_view_list->a_view_list[p_view_list->i_view_count] = &p_view->o_view_ctxt;

        p_view_list->i_view_count ++;
    }
}

void
bfst_view_list_remove(
    struct bfst_view_ctxt * const
        p_view_ctxt)
{
    unsigned int j;

    char b_found_view;

    struct bfst_body_ctxt const * const p_body_ctxt = p_view_ctxt->p_body_ctxt;

    struct bfst_view_list * const p_view_list = p_body_ctxt->p_view_list;

    b_found_view = 0;

    j = 0;
    while (!b_found_view && (j < p_view_list->i_view_count))
    {
        if (p_view_list->a_view_list[j] == p_view_ctxt)
        {
            /* kill this window */
            bfst_view_delete(p_view_ctxt);

            {
                unsigned int k;

                for (k=j; k + 1 < p_view_list->i_view_count; k++)
                {
                    p_view_list->a_view_list[k] = p_view_list->a_view_list[k + 1];
                }
            }

            p_view_list->i_view_count --;

            b_found_view = 1;
        }
        else
        {
            j ++;
        }
    }

} /* bfst_view_list_remove() */

struct bfst_view_ctxt *
bfst_view_list_find_win(
    struct bfst_body_ctxt const * const
        p_body_ctxt,
    Window const
        i_window_id)
{
    unsigned int j;

    struct bfst_view_list const * const p_view_list = p_body_ctxt->p_view_list;

    for (j = 0; j < p_view_list->i_view_count; j++)
    {
        struct bfst_view_ctxt * const p_view_ctxt = p_view_list->a_view_list[j];

        if (p_view_ctxt)
        {
            if (i_window_id == p_view_ctxt->p_window->h_win_res)
            {
                return p_view_ctxt;
            }
        }
    }

    return 0;
}

/*

Function: bfst_view_list_scan()

Description:

    Scan the list of views and notify caller for each view in list.

*/
void
bfst_view_list_scan(
    struct bfst_body_ctxt const * const
        p_body_ctxt,
    enum bfst_view_list_scan_result (*p_callback)(
        struct bfst_view_ctxt const * const
            p_view_ctxt,
        void * const
            h_context),
    void * const
        h_context)
{
    struct bfst_view_list const * const p_view_list = p_body_ctxt->p_view_list;

    enum bfst_view_list_scan_result e_scan_result = bfst_view_list_scan_result_continue;

    unsigned int j = 0;

    while ((bfst_view_list_scan_result_stop != e_scan_result) && (j < p_view_list->i_view_count))
    {
        struct bfst_view_ctxt * const p_view_ctxt = p_view_list->a_view_list[j];

        if (p_view_ctxt)
        {
            e_scan_result = (*p_callback)(p_view_ctxt, h_context);

            if (bfst_view_list_scan_result_remove == e_scan_result)
            {
                bfst_view_list_remove(p_view_ctxt);
            }
            else if (bfst_view_list_scan_result_continue == e_scan_result)
            {
                j ++;
            }
        }
        else
        {
            j ++;
        }

    }
} /* bfst_view_list_scan() */

/* end-of-file: bfst_view_list.c */
