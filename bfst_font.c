/* See LICENSE for license details. */

/*

Module: bfst_font

Description:

    Management of X11 font.

*/

#include <X11/Xlib.h>

#include "bfst_font.h"

#include "bfst_ctxt.h"

#include "bfst_display.h"

#include "bfst_opts.h"

#include "bfst_tools.h"

/* Examples of font names */

#define BFST_FONT_NAME_TERMINAL "-dec-terminal-medium-r-normal--14-140-75-75-c-80-iso8859-1"

#define BFST_FONT_NAME_SONY "-sony-fixed-medium-r-normal--16-120-100-100-c-80-iso8859-1"

#define BFST_FONT_NAME_COURIER "-adobe-courier-medium-r-normal--18-180-75-75-m-110-iso10646-1"

/* Use the ucs-fonts package if installed */
#if !defined(BFST_FONT_NAME_DEFAULT_UTF8)
#define BFST_FONT_NAME_DEFAULT_UTF8 "ufixed"
#endif /* #if !defined(bfst_FONT_NAME_Xx) */

#if !defined(BFST_FONT_NAME_DEFAULT_LATIN1)
#define BFST_FONT_NAME_DEFAULT_LATIN1 "fixed"
#endif /* #if !defined(bfst_FONT_NAME_Xx) */

/*

Function: bfst_font_try_name()

Description:

    Initialize the font resource using a specific font name.

*/
static char bfst_font_try_name(
    struct bfst_view_ctxt const * const p_view_ctxt,
    char const * const p_font_name)
{
    struct bfst_font * const p_font = p_view_ctxt->p_font;

    struct bfst_display const * const p_display = p_view_ctxt->p_body_ctxt->p_display;

    p_font->p_font_res = XLoadQueryFont(p_display->dpy, p_font_name);

    return (char)(p_font->p_font_res != NULL);

} /* bfst_font_try_name() */

/*

Function: bfst_font_set_res()

Description:

*/
static void bfst_font_set_res(
    struct bfst_view_ctxt const * const p_view_ctxt,
    char const * const p_font_name)
{
    /* First try to load a custom font */
    if (!p_font_name || !bfst_font_try_name(p_view_ctxt, p_font_name))
    {
        /* Then try to load a unicode font */
        if (!bfst_font_try_name(p_view_ctxt, BFST_FONT_NAME_DEFAULT_UTF8))
        {
            /* Else try to load a latin1 font */
            if (!bfst_font_try_name(p_view_ctxt, BFST_FONT_NAME_DEFAULT_LATIN1))
            {
                /* Else fail! */
                bfst_die();
            }
        }
    }

} /* bfst_font_set_res() */

/*

Function: bfst_font_init_res()

Description:

    Initialize the font resource by any means necessary.

*/
static void bfst_font_init_res(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    struct bfst_options const * const p_opts = p_view_ctxt->p_body_ctxt->p_opts;

    bfst_font_set_res(p_view_ctxt, p_opts->opt_font);

} /* bfst_font_init_res() */

/*

Function: bfst_font_cleanup_res()

Description:

    Free all resources allocated by bfst_font_init_res() function.

*/
static void bfst_font_cleanup_res(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    struct bfst_font * const p_font = p_view_ctxt->p_font;

    if (p_font->p_font_res)
    {
        struct bfst_display const * const p_display = p_view_ctxt->p_body_ctxt->p_display;

        XFreeFont(p_display->dpy, p_font->p_font_res);

        p_font->p_font_res = NULL;
    }
} /* bfst_font_cleanup_res() */

/*

Function: bfst_font_init_info()

Description:

    Initialize the font information fields.

*/
static void bfst_font_init_info(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    struct bfst_font * const p_font = p_view_ctxt->p_font;

    /* Calculate fixed width of font */
    p_font->i_width = p_font->p_font_res->min_bounds.width;

    /* Calculate fixed height of font */
    p_font->i_height = p_font->p_font_res->ascent + p_font->p_font_res->descent;

} /* bfst_font_init_info() */

/*

Function: bfst_font_set()

Description:

    Change the X11 font resource using the new name.

*/
void bfst_font_set(
    struct bfst_view_ctxt const * const p_view_ctxt,
    char const * const p_font_name)
{
    bfst_font_cleanup_res(p_view_ctxt);

    bfst_font_set_res(p_view_ctxt, p_font_name);

    bfst_font_init_info(p_view_ctxt);

} /* bfst_font_set() */

/*

Function: bfst_font_init()

Description:

    Initialize the X11 font resource and populate members with font information.

*/
void bfst_font_init(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    bfst_font_init_res(p_view_ctxt);

    bfst_font_init_info(p_view_ctxt);

} /* bfst_font_init() */

/*

Function: bfst_font_cleanup()

Description:

    Undo everything done by bfst_font_init()

*/
void bfst_font_cleanup(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    bfst_font_cleanup_res(p_view_ctxt);
} /* bfst_font_cleanup() */

/* end-of-file: bfst_font.c */
