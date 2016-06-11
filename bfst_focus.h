/* See LICENSE for license details. */

/*

Module: bfst_focus

Description:

    User input focus of a view.

*/

struct bfst_body_ctxt;

struct bfst_view_ctxt;

/*

Structure: bfst_focus

Description:

    State for bfst_focus module.

Remarks:

    -   Place an instance of this structure into bfst_body container.

*/
struct bfst_focus
{
    struct bfst_view_ctxt * p_view_focus;

}; /* struct bfst_focus */

/*

Interface: bfst_focus

Description:

    Public methods of bfst_focus module.

*/

struct bfst_view_ctxt *
bfst_focus_get(
    struct bfst_body_ctxt const * const
        p_body_ctxt);

void
bfst_focus_set(
    struct bfst_body_ctxt const * const
        p_body_ctxt,
    struct bfst_view_ctxt * const
        p_view_ctxt);

/* end-of-file: bfst_focus.h */
