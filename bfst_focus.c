/* See LICENSE for license details. */

/*

Module: bfst_focus

Description:

*/

#include "bfst_os.h"

#include "bfst_body.h"

struct bfst_view_ctxt *
bfst_focus_get(
    struct bfst_body_ctxt const * const
        p_body_ctxt)
{
    struct bfst_focus const * const p_focus = p_body_ctxt->p_focus;

    return p_focus->p_view_focus;

} /* bfst_focus_get() */

void
bfst_focus_set(
    struct bfst_body_ctxt const * const
        p_body_ctxt,
    struct bfst_view_ctxt * const
        p_view_ctxt)
{
    struct bfst_focus * const p_focus = p_body_ctxt->p_focus;

    p_focus->p_view_focus = p_view_ctxt;

} /* bfst_focus_set() */

/* end-of-file: bfst_focus.c */
