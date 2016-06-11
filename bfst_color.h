/* See LICENSE for license details. */

/*

Module: bfst_color

Description:

    Management of X11 color resources.

*/

#if defined(INC_BFST_COLOR_H)
#error include bfst_color.h once
#endif /* #if defined(INC_BFST_COLOR_H) */

#define INC_BFST_COLOR_H

/* Predefine context handle */
struct bfst_view_ctxt;

/* Maximum number of entries in color table */
#define BFST_COLOR_MAX 256

/*

Structure: bfst_color

Description:

    Color manager state.

*/
struct bfst_color
{
    /* Table of 256 colors */
    XColor a_color[BFST_COLOR_MAX];

}; /* struct bfst_color */

XColor const * bfst_color_get(
    struct bfst_view_ctxt const * const p_view_ctxt,
    int const i_color_index);

int bfst_color_set(
    struct bfst_view_ctxt const * const p_view_ctxt,
    int const i_color_index,
    const char * const p_color_name);

void bfst_color_load_all(
    struct bfst_view_ctxt const * const p_view_ctxt);

/* end-of-file: bfst_color.h */
