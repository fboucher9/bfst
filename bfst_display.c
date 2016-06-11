/* See LICENSE for license details. */

/*

Module: bfst_display.c

Description:

    Management of X11 resources common to all views.

*/

#include <stdlib.h>

#include <string.h>

#include <locale.h>

#include <X11/Xlib.h>

#include "bfst_body.h"

/*

Function: bfst_display_init_gc()

Description:

    Init the X11 graphics context resource.

*/
static void bfst_display_init_gc(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_display * const p_display = p_body_ctxt->p_display;

    XGCValues o_gc_values;

    memset(&(o_gc_values), 0, sizeof(o_gc_values));

    o_gc_values.graphics_exposures = False;

    p_display->gc = XCreateGC(p_display->dpy, p_display->parent, GCGraphicsExposures, &(o_gc_values));

} /* bfst_display_init_gc() */

/*

Function: bfst_display_cleanup_gc()

Description:

    Free all resources allocated by bfst_display_init_gc() function.

*/
static void bfst_display_cleanup_gc(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_display * const p_display = p_body_ctxt->p_display;

    XFreeGC(p_display->dpy, p_display->gc);

} /* bfst_display_cleanup_gc() */

/*

Function: bfst_display_init_parent()

Description:

    Init the parent Window resource.

*/
static void bfst_display_init_parent(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_display * const p_display = p_body_ctxt->p_display;

    struct bfst_options const * const p_opts = p_body_ctxt->p_opts;

    if (p_opts->opt_embed)
    {
        p_display->parent = strtol(p_opts->opt_embed, NULL, 0);

        if (p_display->parent)
        {
        }
        else
        {
            p_display->parent = p_display->root;
        }
    }
    else
    {
        p_display->parent = p_display->root;
    }

} /* bfst_display_init_parent() */

/*

Function: bfst_display_cleanup_parent()

Description:

    Undo initialization of parent Window resource.

*/
static void bfst_display_cleanup_parent(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_display * const p_display = p_body_ctxt->p_display;

    p_display->parent = None;

} /* bfst_display_cleanup_parent() */

/*

Function: bfst_display_init_root()

Description:

    Init root Window resource.

*/
static void bfst_display_init_root(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_display * const p_display = p_body_ctxt->p_display;

    p_display->root = XRootWindow(p_display->dpy, p_display->scr);

} /* bfst_display_init_root() */

/*

Function: bfst_display_cleanup_root()

Description:

    Undo initialization of root window resource.

*/
static void bfst_display_cleanup_root(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_display * const p_display = p_body_ctxt->p_display;

    p_display->root = None;

} /* bfst_display_cleanup_root() */

/*

Function: bfst_display_init_colormap()

Description:

    Init colormap resource.

*/
static void bfst_display_init_colormap(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_display * const p_display = p_body_ctxt->p_display;

    p_display->cmap = XDefaultColormap(p_display->dpy, p_display->scr);

} /* bfst_display_init_colormap() */

/*

Function: bfst_display_cleanup_colormap()

Description:

    Free resources allocated by bfst_display_init_colormap() function.

*/
static void bfst_display_cleanup_colormap(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_display * const p_display = p_body_ctxt->p_display;

    /* XFreeColormap(p_display->dpy, p_display->cmap); */

    p_display->cmap = None;

} /* bfst_display_cleanup_colormap() */

/*

Function: bfst_display_init_visual()

Description:

    Init visual resource.

*/
static void bfst_display_init_visual(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_display * const p_display = p_body_ctxt->p_display;

    p_display->vis = XDefaultVisual(p_display->dpy, p_display->scr);

} /* bfst_display_init_visual() */

/*

Function: bfst_display_cleanup_visual()

Description:

    Free resources allocated by bfst_display_init_visual() function.

*/
static void bfst_display_cleanup_visual(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_display * const p_display = p_body_ctxt->p_display;

    p_display->vis = NULL;

} /* bfst_display_cleanup_visual() */

/*

Function: bfst_display_init_screen()

Description:

    Initialize X11 screen index.

*/
static void bfst_display_init_screen(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_display * const p_display = p_body_ctxt->p_display;

    p_display->scr = XDefaultScreen(p_display->dpy);

} /* bfst_display_init_screen() */

/*

Function: bfst_display_cleanup_screen()

Description:

    Undo initialization of X11 screen index.

*/
static void bfst_display_cleanup_screen(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_display * const p_display = p_body_ctxt->p_display;

    p_display->scr = 0;

} /* bfst_display_cleanup_screen() */

/*

Function: bfst_display_init_connection()

Description:

    Init X11 connection using default display name.

Comments:

    -   Todo: add support for a display name on command-line

*/
static void bfst_display_init_connection(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_display * const p_display = p_body_ctxt->p_display;

    p_display->dpy = XOpenDisplay(NULL);

    if (!p_display->dpy)
    {
        bfst_die();
    }

} /* bfst_display_init_connection() */

/*

Function: bfst_display_cleanup_connection()

Description:

    Free resources allocated by bfst_display_init_connection() function.

*/
static void bfst_display_cleanup_connection(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_display * const p_display = p_body_ctxt->p_display;

    if (p_display->dpy)
    {
        XCloseDisplay(p_display->dpy);

        p_display->dpy = NULL;
    }

} /* bfst_display_cleanup_connection() */

/*

Function: bfst_display_init_locale()

Description:

    Init locale settings for X11 and posix

*/
static void bfst_display_init_locale(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    (void)(p_body_ctxt);

    setlocale(LC_CTYPE, "");

    XSetLocaleModifiers("");

} /* bfst_display_init_locale() */

/*

Function: bfst_display_cleanup_locale()

Description:

    Free all resources allocated by bfst_display_init_locale()

*/
static void bfst_display_cleanup_locale(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    (void)(p_body_ctxt);

} /* bfst_display_cleanup_locale() */

/*

Function: bfst_display_init()

Description:

    Initialize all common X11 resources.

*/
void bfst_display_init(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    bfst_display_init_locale(p_body_ctxt);

    bfst_display_init_connection(p_body_ctxt);

    bfst_display_init_screen(p_body_ctxt);

    bfst_display_init_visual(p_body_ctxt);

    bfst_display_init_colormap(p_body_ctxt);

    bfst_display_init_root(p_body_ctxt);

    bfst_display_init_parent(p_body_ctxt);

    bfst_display_init_gc(p_body_ctxt);

} /* bfst_display_init() */

/*

Function: bfst_display_cleanup()

Description:

    Free all resources allocated by bfst_display_init() function.

*/
void bfst_display_cleanup(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    bfst_display_cleanup_gc(p_body_ctxt);

    bfst_display_cleanup_parent(p_body_ctxt);

    bfst_display_cleanup_root(p_body_ctxt);

    bfst_display_cleanup_colormap(p_body_ctxt);

    bfst_display_cleanup_visual(p_body_ctxt);

    bfst_display_cleanup_screen(p_body_ctxt);

    bfst_display_cleanup_connection(p_body_ctxt);

    bfst_display_cleanup_locale(p_body_ctxt);

} /* bfst_display_cleanup() */

/* end-of-file: bfst_display.c */
