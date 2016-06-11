/* See LICENSE for license details. */

/*

Module: bfst_font.h

Description:

    Management of X11 font.

Comments:

    .   Only bitmap fonts are supported.  This is to avoid anti-aliasing
        of rendered font to display.  The anti-aliasing changes the color
        of pixels

    .   Only fixed width fonts are supported.  This simplifies the logic
        calculating the positions of characters in the drawing area.

    .   To support a wider range of UTF-8 characters, consider using the
        ucs-fonts package by Markus Kuhn:

            http://www.cl.cam.ac.uk/~mgk25/ucs-fonts.html

    .   The font may be configured at compile time or from the command
        line using the '-f' option.

*/

#if defined(INC_BFST_FONT_H)
#error include bfst_font.h once
#endif /* #if defined(INC_BFST_FONT_H) */

#define INC_BFST_FONT_H

/* Predefine the context structure */
struct bfst_view_ctxt;

/*

Structure: bfst_font

Description:

    State for font manager.

*/
struct bfst_font
{
    /* Pointer to X11 font resource */
    XFontStruct * p_font_res;

    /* X11 font character height in pixels */
    int i_height;

    /* X11 font character width in pixels */
    int i_width;

}; /* struct bfst_font */

/* Methods... */

void bfst_font_init(
    struct bfst_view_ctxt const * const p_view_ctxt);

void bfst_font_cleanup(
    struct bfst_view_ctxt const * const p_view_ctxt);

void bfst_font_set(
    struct bfst_view_ctxt const * const p_view_ctxt,
    char const * const p_font_name);

/* end-of-file: bfst_font.h */
