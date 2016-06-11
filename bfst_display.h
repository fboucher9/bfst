/* See LICENSE for license details. */

/*

Module: bfst_display.h

Description:

    Management of X11 resources common to all views.

*/

#if defined(INC_BFST_DISPLAY_H)
#error include bfst_display.h once
#endif /* #if defined(INC_BFST_DISPLAY_H) */

#define INC_BFST_DISPLAY_H

/* Predefine context handle */
struct bfst_body_ctxt;

/*

Structure: bfst_display

Description:

    Storage for X11 resources common to all views.

Comments:

    -   Place an instance of this structure into the bfst_body container.

*/
struct bfst_display
{
    /* X11 display resource */
    Display *dpy;

    /* X11 colormap resource */
    Colormap cmap;

    /* X11 root window id */
    Window root;

    /* X11 parent window id */
    Window parent;

    /* X11 visual resource */
    Visual *vis;

    /* X11 graphics context */
    GC gc;

    /* X11 screen index */
    int scr;

}; /* struct bfst_display */

/* Interface: */

/* Initialization */
void bfst_display_init(
    struct bfst_body_ctxt const * const p_ctxt);

/* Cleanup */
void bfst_display_cleanup(
    struct bfst_body_ctxt const * const p_ctxt);

/* end-of-file: bfst_display.h */
