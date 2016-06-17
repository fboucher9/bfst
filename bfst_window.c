/* See LICENSE for license details. */

/*

Module: bfst_window.c

*/

#include "bfst_os.h"

#include "bfst_body.h"

/*

Function: bfst_window_fill_attributes()

Description:

    Fill a XSetWindowAttributes structure with information required to create
    a X11 window resource.

*/
static void bfst_window_fill_attributes(
    struct bfst_view_ctxt const * const p_view_ctxt,
    XSetWindowAttributes * const p_window_attributes)
{
    struct bfst_display * const p_display = p_view_ctxt->p_body_ctxt->p_display;

    /* Set the default background color for the window.  This color is used by
       X11 when clearing the background of the window on a resize event.
       This attribute may be updated later if the color theme is changed. */
    p_window_attributes->background_pixel = bfst_color_get(p_view_ctxt, BFST_COLOR_BG)->pixel;

    /* Set the default border color.  This may be overriden by the window manager. */
    {
        XColor o_border_color;

        {
            o_border_color.red = 0x4444;

            o_border_color.green = 0x4444;

            o_border_color.blue = 0x4444;
        }

        XAllocColor(p_display->dpy, p_display->cmap, &o_border_color);

        p_window_attributes->border_pixel = o_border_color.pixel;
    }

    /* Set the gravity information */
    p_window_attributes->bit_gravity = NorthWestGravity;

    /* Select the events that will be sent to the main event loop. */
    p_window_attributes->event_mask =

        /* Changes of keyboard focus */
        FocusChangeMask

        /* Key is pressed */
        | KeyPressMask

        /* Mouse pointer enters the drawing area */
        | EnterWindowMask

        /* Mouse pointer leaved the drawing area */
        | LeaveWindowMask

        /* Mouse pointer is moved within the drawing area */
        | PointerMotionMask

        /* Part of the drawing area needs to be refreshed */
        | ExposureMask

        /* Changes of visibility state, no need to render when completely hidden */
        | VisibilityChangeMask

        /* Resize */
        | StructureNotifyMask

        /* Mouse pointer is moved while a button is pressed */
        | ButtonMotionMask

        /* Mouse button is pressed */
        | ButtonPressMask

        /* Mouse button is released */
        | ButtonReleaseMask;

    /* Specify the colormap resource */
    p_window_attributes->colormap = p_display->cmap;

} /* bfst_window_fill_attributes() */

/*

Function: bfst_window_init_resource()

Description:

    Create a X11 window resource using information from command line options.

*/
static void bfst_window_init_resource(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    struct bfst_body_ctxt const * const p_body_ctxt = p_view_ctxt->p_body_ctxt;

    struct bfst_display const * const p_display = p_body_ctxt->p_display;

    struct bfst_options const * const p_opts = p_body_ctxt->p_opts;

    struct bfst_window * const p_window = p_view_ctxt->p_window;

    XSetWindowAttributes o_window_attributes;

    bfst_window_fill_attributes(
        p_view_ctxt,
        &(o_window_attributes));

    p_window->h_win_res =
        XCreateWindow(
            p_display->dpy,
            p_display->parent,
            p_opts->opt_left,
            p_opts->opt_top,
            p_window->i_width_pixels,
            p_window->i_height_pixels,
            1,
            XDefaultDepth(p_display->dpy, p_display->scr),
            InputOutput,
            p_display->vis,
            CWBackPixel | CWBorderPixel | CWBitGravity
            | CWEventMask | CWColormap,
            &o_window_attributes);

} /* bfst_window_init_resource() */

static void bfst_window_cleanup_resource(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    struct bfst_display const * const p_display = p_view_ctxt->p_body_ctxt->p_display;

    struct bfst_window * const p_window = p_view_ctxt->p_window;

    XDestroyWindow(p_display->dpy, p_window->h_win_res);

} /* bfst_window_cleanup_resource() */

static void bfst_window_default_size(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    struct bfst_window * const p_window = p_view_ctxt->p_window;

    struct bfst_options const * const p_opts = p_view_ctxt->p_body_ctxt->p_opts;

    struct bfst_font const * const p_font = p_view_ctxt->p_font;

    p_window->i_border = 2;

#if 0
    struct bfst_view_ctxt const * const p_view_focus = bfst_focus_get(p_view_ctxt->p_body_ctxt);

    if (p_view_focus)
    {
        /* Copy information from currently focused window... */
        struct bfst_window const * const p_other_window = p_view_focus->p_window;

        p_window->i_tty_cols = p_other_window->i_tty_cols;

        p_window->i_tty_rows = p_other_window->i_tty_rows;

        p_window->i_width_pixels = p_other_window->i_width_pixels;

        p_window->i_height_pixels = p_other_window->i_height_pixels;
    }
    else
#endif
    {
        p_window->i_tty_cols = p_opts->opt_cols;

        p_window->i_tty_rows = p_opts->opt_rows;

        /* adjust fixed window geometry */
        p_window->i_width_pixels = 2 * p_window->i_border + p_window->i_tty_cols * p_font->i_width;

        p_window->i_height_pixels = 2 * p_window->i_border + p_window->i_tty_rows * p_font->i_height;
    }

    p_window->i_tty_width_pixels = p_window->i_tty_cols * p_font->i_width;

    p_window->i_tty_height_pixels = p_window->i_tty_rows * p_font->i_height;

    p_window->state = WIN_VISIBLE | WIN_FOCUSED;
}

/*

Function: bfst_window_init()

Description:

    Initialize the bfst_window object.

*/
void bfst_window_init(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    bfst_window_default_size(p_view_ctxt);

    bfst_window_init_resource(p_view_ctxt);

    {
        struct bfst_display const * const p_display = p_view_ctxt->p_body_ctxt->p_display;

        struct bfst_window * const p_window = p_view_ctxt->p_window;

        XMapWindow(p_display->dpy, p_window->h_win_res);

        XSync(p_display->dpy, False);
    }
}

/*

Function: bfst_window_cleanup()

*/
void bfst_window_cleanup(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    bfst_window_cleanup_resource(p_view_ctxt);

} /* bfst_window_cleanup() */

void
bfst_window_resize(
    struct bfst_view_ctxt * const
        p_view_ctxt,
    int const
        i_width_chars,
    int const
        i_height_lines)
{
    struct bfst_window * const p_window = p_view_ctxt->p_window;

    struct bfst_font const * const p_font = p_view_ctxt->p_font;

    p_window->i_tty_cols = i_width_chars;

    p_window->i_tty_rows = i_height_lines;

    p_window->i_tty_width_pixels = i_width_chars * p_font->i_width;
    if (p_window->i_tty_width_pixels < 1)
    {
        p_window->i_tty_width_pixels = 1;
    }

    p_window->i_tty_height_pixels = i_height_lines * p_font->i_height;
    if (p_window->i_tty_height_pixels < 1)
    {
        p_window->i_tty_height_pixels = 1;
    }
}
