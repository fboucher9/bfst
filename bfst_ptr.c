/* See LICENSE for license details. */

/*

Module: bfst_ptr.c

Description:

    Management of mouse cursor shape.

*/

#include "bfst_os.h"

#include "bfst_body.h"

#if !defined(BFST_CURSOR_FONT)
/* #define BFST_CURSOR_FONT XC_xterm */
#define BFST_CURSOR_FONT XC_crosshair
#endif /* #if !defined(BFST_CURSOR_FONT) */

/* Note: it is also possible to customize the color of the pointer */

/*

Function: bfst_ptr_init_gray_color()

Description:

    Fill XColor structure for specified gray color.

*/
static void bfst_ptr_init_gray_color(
    XColor * const p_x11_color,
    unsigned short int const i_gray_shade)
{
    p_x11_color->red = i_gray_shade;

    p_x11_color->green = i_gray_shade;

    p_x11_color->blue = i_gray_shade;

} /* bfst_ptr_init_gray_color() */

/*

Function: bfst_ptr_init_white_color()

Description:

    Fill XColor structure for white.

*/
static void bfst_ptr_init_white_color(
    XColor * const p_x11_color)
{
    bfst_ptr_init_gray_color(p_x11_color, 0xffff);

} /* bfst_ptr_init_white_color() */

/*

Function: bfst_ptr_init_black_color()

Description:

    Fill XColor structure for black.

*/
static void bfst_ptr_init_black_color(
    XColor * const p_x11_color)
{
    bfst_ptr_init_gray_color(p_x11_color, 0x0000);

} /* bfst_ptr_init_black_color() */

/*

Function: bfst_ptr_init_color()

Description:

    Setup foreground and background colors of visible pointer shape.

Comments:

    -   The colors are designed to work with a dark background.

*/
static void bfst_ptr_init_color(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_ptr * const p_ptr = p_body_ctxt->p_ptr;

    struct bfst_display const * const p_display = p_body_ctxt->p_display;

    XColor o_color_white;

    XColor o_color_black;

    bfst_ptr_init_white_color(&o_color_white);

    bfst_ptr_init_black_color(&o_color_black);

    XRecolorCursor(p_display->dpy, p_ptr->o_visible,
        &(o_color_white), &(o_color_black));

} /* bfst_ptr_init_color() */

/*

Function: bfst_ptr_init_visible()

Description:

    Create a cursor shape used when mouse is moved over drawing area.

*/
static void bfst_ptr_init_visible(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_ptr * const p_ptr = p_body_ctxt->p_ptr;

    struct bfst_display const * const p_display = p_body_ctxt->p_display;

    p_ptr->o_visible = XCreateFontCursor(p_display->dpy, BFST_CURSOR_FONT);

} /* bfst_ptr_init_visible() */

/*

Function: bfst_ptr_create_hidden_font()

Description:

    Create a font required by pointer shape creation.  Any font may work,
    try to use a font that is available on all systems.

*/
static XFontStruct * bfst_ptr_create_hidden_font(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    XFontStruct * p_hidden_font;

    struct bfst_display const * const p_display = p_body_ctxt->p_display;

    p_hidden_font = XLoadQueryFont(p_display->dpy, "nil2");

    if (!p_hidden_font)
    {
        p_hidden_font = XLoadQueryFont(p_display->dpy, "fixed");
    }

    return p_hidden_font;

} /* bfst_ptr_create_hidden_font() */

/*

Function: bfst_ptr_destroy_hidden_font()

Description:

    Destroy font resource created by bfst_ptr_create_hidden_font().

*/
static void bfst_ptr_destroy_hidden_font(
    struct bfst_body_ctxt const * const p_body_ctxt,
    XFontStruct * const p_hidden_font)
{
    struct bfst_display const * const p_display = p_body_ctxt->p_display;

    XFreeFont(p_display->dpy, p_hidden_font);

} /* bfst_ptr_destroy_hidden_font() */

/*

Function: bfst_ptr_init_hidden()

Description:

    Create a pointer shape that is hidden.

Comments:

    -   This technique is copied from xterm source code.

*/
static void bfst_ptr_init_hidden(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_ptr * const p_ptr = p_body_ctxt->p_ptr;

    struct bfst_display const * const p_display = p_body_ctxt->p_display;

    XFontStruct * const p_hidden_font = bfst_ptr_create_hidden_font(p_body_ctxt);

    if (p_hidden_font)
    {
        XColor dummy;

        /* a space character seems to work as a cursor (dots are not needed) */
        p_ptr->o_hidden = XCreateGlyphCursor(
                p_display->dpy,
                p_hidden_font->fid,
                p_hidden_font->fid,
                'X',
                ' ',
                &dummy,
                &dummy);

        bfst_ptr_destroy_hidden_font(p_body_ctxt, p_hidden_font);
    }
    else
    {
        p_ptr->o_hidden = None;
    }

} /* bfst_ptr_init_hidden() */

/*

Function: bfst_ptr_init()

Description:

    Create pointer shapes.

*/
void bfst_ptr_init(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    bfst_ptr_init_visible(p_body_ctxt);

    bfst_ptr_init_hidden(p_body_ctxt);

    bfst_ptr_init_color(p_body_ctxt);

} /* bfst_ptr_init() */

/*

Function: bfst_ptr_cleanup_visible()

Description:

    Free all resources allocated for visible pointer shape.

*/
static void bfst_ptr_cleanup_visible(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_ptr * const p_ptr = p_body_ctxt->p_ptr;

    struct bfst_display const * const p_display = p_body_ctxt->p_display;

    if (None != p_ptr->o_visible)
    {
        XFreeCursor(p_display->dpy, p_ptr->o_visible);

        p_ptr->o_visible = None;
    }

} /* bfst_ptr_cleanup_visible() */

/*

Function: bfst_ptr_cleanup_hidden()

Description:

    Free all resource allocated for hidden pointer shape.

*/
static void bfst_ptr_cleanup_hidden(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_ptr * const p_ptr = p_body_ctxt->p_ptr;

    struct bfst_display const * const p_display = p_body_ctxt->p_display;

    if (None != p_ptr->o_hidden)
    {
        XFreeCursor(p_display->dpy, p_ptr->o_hidden);

        p_ptr->o_hidden = None;
    }

} /* bfst_ptr_cleanup_hidden() */

/*

Function: bfst_ptr_cleanup()

Description:

    Free all resources allocated by bfst_ptr module.

*/
void bfst_ptr_cleanup(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    bfst_ptr_cleanup_visible(p_body_ctxt);

    bfst_ptr_cleanup_hidden(p_body_ctxt);

} /* bfst_ptr_cleanup() */

/*

Function: bfst_ptr_get

Description:

    Get ptr shape handle

*/
Cursor bfst_ptr_get(
    struct bfst_body_ctxt const * const p_body_ctxt,
    char const b_show)
{
    struct bfst_ptr const * const p_ptr = p_body_ctxt->p_ptr;

    Cursor h_cur_res;

    if (b_show)
    {
        h_cur_res = p_ptr->o_visible;
    }
    else
    {
        h_cur_res = p_ptr->o_hidden;
    }

    return h_cur_res;

} /* bfst_ptr_get() */

/* end-of-file: bfst_ptr.c */
