/* See LICENSE for license details. */

/*

Module: bfst_window.h

*/

#if defined(INC_BFST_WINDOW_H)
#error include bfst_window.h once
#endif

#define INC_BFST_WINDOW_H

/* Predefine context handle */
struct bfst_view_ctxt;

/*

Structure: bfst_window

Description:

    State for bfst_window module.

*/
struct bfst_window
{
    /* Window resource handle */
    Window h_win_res;

    /* Input method handle */
    XIM h_xim_res;

    /* Input method context */
    XIC h_xic_res;

    /* Terminal width in pixels */
    unsigned int i_tty_width_pixels;

    /* Terminal height in pixels */
    unsigned int i_tty_height_pixels;

    /* Terminal width in characters */
    unsigned int i_tty_cols;

    /* Terminal height in characters */
    unsigned int i_tty_rows;

    /* Window width in pixels */
    unsigned int i_width_pixels;

    /* Window height in pixels */
    unsigned int i_height_pixels;

    /* Pending resize width */
    unsigned int i_resize_width_pixels;

    /* Pending resize height */
    unsigned int i_resize_height_pixels;

    /* width of inside border in pixels */
    unsigned int i_border;

    /* State of window: focus, redraw, visible */
    char state;

}; /* struct bfst_window */

/* Interface: */

/*

Function: bfst_window_init()

Description:

    Initialize bfst_window module.

*/
void bfst_window_init(
    struct bfst_view_ctxt const * const p_ctxt);

/*

Function: bfst_window_cleanup()

Description:

    Undo bfst_window_init().

*/
void bfst_window_cleanup(
    struct bfst_view_ctxt const * const p_view_ctxt);

/*

Function: bfst_window_resize()

Description:

    Handle a window resize event.

*/
void bfst_window_resize(
    struct bfst_view_ctxt * const p_view_ctxt,
    int const i_width_chars,
    int const i_height_lines);

/* end-of-file: bfst_window.h */
