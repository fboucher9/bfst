/* See LICENSE for license details. */

/*

Module: bfst_color

Description:

    Management of X11 color resources.

*/

#include <X11/Xlib.h>

#include "bfst_color.h"

#include "bfst_ctxt.h"

#include "bfst_display.h"

/* #define BFST_COLOR_THEME_XTERM */
#define BFST_COLOR_THEME_CUSTOM
/* #define BFST_COLOR_THEME_SOLARIZED */
/* #define BFST_COLOR_THEME_GNOME */

/* Terminal colors (16 first used in escape sequence) */
/* 8 normal colors */
/* 8 bright colors */
static
const char * const g_color_theme[] =
{
#if defined(BFST_COLOR_THEME_XTERM)
    "black",        /*   0/  0/  0  #000000 */
    "red3",         /* 205/  0/  0  #cd0000 */
    "green3",       /*   0/205/  0  #00cd00 */
    "yellow3",      /* 205/205/  0  #cdcd00 */
    "blue2",        /*   0/  0/238  #0000ee */
    "magenta3",     /* 205/  0/205  #cd00cd */
    "cyan3",        /*   0/205/205  #00cdcd */
    "gray90",       /* 229/229/229  #e5e5e5 */
    "gray50",       /* 127/127/127  #7f7f7f */
    "red",          /* 255/  0/  0  #ff0000 */
    "green",        /*   0/255/  0  #00ff00 */
    "yellow",       /* 255/255/  0  #ffff00 */
    "#5c5cff",      /*  92/ 92/255  #5c5cff */
    "magenta",      /* 255/  0/255  #ff00ff */
    "cyan",         /*   0/255/255  #00ffff */
    "white",        /* 255/255/255  #ffffff */
#endif /* #if defined(BFST_COLOR_THEME_XTERM) */

#if defined(BFST_COLOR_THEME_CUSTOM)
    "black",        /*   0/  0/  0  #000000 */
    "red3",         /* 205/  0/  0  #cd0000 */
    "green3",       /*   0/205/  0  #00cd00 */
    "yellow3",      /* 205/205/  0  #cdcd00 */
    "#6060e0",      /*  96/ 96/224  #6060e0 */
    "magenta3",     /* 205/  0/205  #cd00cd */
    "cyan3",        /*   0/205/205  #00cdcd */
    "lightgray",    /* 211/211/211  #d3d3d3 */
    "gray50",       /* 127/127/127  #7f7f7f */
    "red",          /* 255/  0/  0  #ff0000 */
    "green",        /*   0/255/  0  #00ff00 */
    "yellow",       /* 255/255/  0  #ffff00 */
    "#8080ff",      /* 128/128/255  #8080ff */
    "magenta",      /* 255/  0/255  #ff00ff */
    "cyan",         /*   0/255/255  #00ffff */
    "white",        /* 255/255/255  #ffffff */
#endif /* #if defined(BFST_COLOR_THEME_CUSTOM) */

#if defined(BFST_COLOR_THEME_SOLARIZED)

#define S_base03        "#002b36"
#define S_base02        "#073642"
#define S_base01        "#586e75"
#define S_base00        "#657b83"
#define S_base0         "#839496"
#define S_base1         "#93a1a1"
#define S_base2         "#eee8d5"
#define S_base3         "#fdf6e3"
#define S_yellow        "#b58900"
#define S_orange        "#cb4b16"
#define S_red           "#dc322f"
#define S_magenta       "#d33682"
#define S_violet        "#6c71c4"
#define S_blue          "#268bd2"
#define S_cyan          "#2aa198"
#define S_green         "#859900"

    S_base03, S_red, S_green, S_yellow,
    S_blue, S_magenta, S_cyan, S_base2,
    S_base02, S_orange, S_base01, S_base00,
    S_base0, S_violet, S_base1, S_base3
#endif

#if defined(BFST_COLOR_THEME_GNOME)
    "#090909", "#E61A41", "#17E385", "#FFDE6A",
    "#4F85FF", "#A074C4", "#4FA5C7", "#D3D7CF",
    "#404040", "#CF183A", "#0DC772", "#F4CD45",
    "#265DD9", "#884EBA", "#2F6277", "#EEEEEC"
#endif

#if 0
    "#222222", "#9E5641", "#6C7E55", "#CAAF2B",
    "#7C9AA6", "#956D9D", "#4c8ea1", "#808080",
    "#454545", "#CC896D", "#7DB37D", "#BFB556",
    "#8FADBF", "#C18FCB", "#6bc1d0", "#cdcdcd"
#endif

#if 0
    "#222222", "#B22222", "#556B2F", "#B8860B",
    "#4682B4", "#9932CC", "#87CEEB", "#C0C0C0",
    "#454545", "#FA8072", "#9ACD32", "#DAA520",
    "#6495ED", "#DA70D6", "#B0E0E6", "#FFFFFF"
#endif

};

/*

Function: bfst_color_convert_index()

Description:

    Convert a 0-5 index to a 16-bit r/g/b component.

*/
static unsigned short int bfst_color_convert_index(int x)
{
    return x == 0 ? 0 : 0x3737 + 0x2828 * x;
}

/*

Function: bfst_color_fill_attr()

Description:

    Fill X11 color structure with default value.

*/
static void bfst_color_fill_attr(
    XColor * const p_color_node,
    int const i_color_index)
{
    /* 256 color */
    if (i_color_index < 6*6*6+16)
    {
        /* same colors as xterm */
        int const i_red_index = (((i_color_index - 16) / 36) % 6);

        int const i_green_index = (((i_color_index - 16) / 6) % 6);

        int const i_blue_index = (((i_color_index - 16) / 1) % 6);

        p_color_node->red = bfst_color_convert_index(i_red_index);

        p_color_node->green = bfst_color_convert_index(i_green_index);

        p_color_node->blue = bfst_color_convert_index(i_blue_index);
    }
    else
    {
        /* greyscale */
        p_color_node->red = 0x0808 + 0x0a0a * (i_color_index - (6*6*6+16));
        p_color_node->green = p_color_node->blue = p_color_node->red;
    }
}

/*

Function: bfst_color_set()

Description:

    Setup one of the colors in the color table.  If no color name is provided
    then the default color value is used.

Return value:
    0       Success
    1       Index is out of range

*/
int bfst_color_set(
    struct bfst_view_ctxt const * const p_view_ctxt,
    int const i_color_index,
    const char * const p_color_name)
{
    if ( ( i_color_index >= 0) && ( i_color_index < BFST_COLOR_MAX))
    {
        struct bfst_display const * const p_display = p_view_ctxt->p_body_ctxt->p_display;

        struct bfst_color * const p_color = p_view_ctxt->p_color;

        XColor * p_color_node;

        p_color_node = p_color->a_color + i_color_index;

        if (p_color_name || (i_color_index < 16))
        {
            XAllocNamedColor(
                p_display->dpy,
                p_display->cmap,
                p_color_name ? p_color_name : g_color_theme[i_color_index],
                p_color_node,
                p_color_node);
        }
        else
        {
            bfst_color_fill_attr(p_color_node, i_color_index);

            XAllocColor(
                p_display->dpy,
                p_display->cmap,
                p_color_node);
        }

        return 0;
    }
    else
    {
        return 1;
    }
} /* bfst_color_set() */

/*

Function: bfst_color_load_all()

Description:

    Reset all colors to default values.  This will undo any modifications
    made to color table via bfst_color_set() function.

*/
void bfst_color_load_all(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    int i_color_index;

    for (i_color_index = 0; i_color_index < BFST_COLOR_MAX; i_color_index++)
    {
        bfst_color_set(p_view_ctxt, i_color_index, NULL);
    }

} /* bfst_color_load_all() */

/*

Function: bfst_color_get()

Description:

    Get pointer to one of the colors in the color table.

Comments:

    -   If index is invalid, then first entry is returned.

*/
XColor const *
bfst_color_get(
    struct bfst_view_ctxt const * const p_view_ctxt,
    int const i_color_index)
{
    XColor const * p_color_node;

    struct bfst_color const * const p_color = p_view_ctxt->p_color;

    if ((i_color_index >= 0) && (i_color_index < BFST_COLOR_MAX))
    {
        p_color_node = p_color->a_color + i_color_index;
    }
    else
    {
        p_color_node = p_color->a_color + 0u;
    }

    return p_color_node;

} /* bfst_color_get() */

/* end-of-file: bfst_color.c */
