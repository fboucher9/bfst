/* See LICENSE for license details. */

/*

    Module: bfst_draw.h

    Interface for rendering of fonts towards window.  Input of rendering is
    a terminal buffer containing an array of unicode characters with
    attributes.  Output of rendering is the frame buffer of the graphical
    window.

    Notes:

        -   Concept of cache:
            -   Array of lines
            -   Number of lines
            -   Array of characters
            -   Line length
            -   Blink count
            -   Special count
            -   Unique id
            -   Cursor position
            -   Cursor shape
            -   Selection position
            -   Selection state

*/

/* Reverse include guard */
#if defined(INC_BFST_DRAW_H)
#error include bfst_draw.h once
#endif /* #if defined(INC_BFST_DRAW_H) */

#define INC_BFST_DRAW_H

/* Predefine context handle */
struct bfst_view_ctxt;

/*

Structure: bfst_draw_line

Description:

    Cache representation of a line.

*/
struct bfst_draw_line
{
    /* Identifier for line in cache, used to detect changes in text */
    unsigned long long int i_unique;

    /* Pointer to array of characters */
    struct bfst_glyph * a_char;

    /* Length of the line */
    unsigned short int i_char_count;

    /* This line contains a blink */
    unsigned short int i_blinks;

}; /* struct bfst_draw_line */

/*

Structure: bfst_draw

Description:

    State for drawing of terminal visible area.

*/
struct bfst_draw
{
    /* Double buffer of visible window */
    Drawable draw_buf;

    /* Cache of currently visible characters */
    struct bfst_glyph * cache;

    /* Pointer to array of pointers to line objects */
    struct bfst_draw_line * a_cache_line;

    /* Terminal line index of first visible line */
    int top;

    /* Select display of primary screen or alternate screen */
    int alt;

    /* Position of cursor in cache */
    signed int i_cursor_x;

    signed int i_cursor_y;

    /* Type of cursor in cache */
    unsigned char i_cursor_shape;

    /* Position of selection rectangle in cache */
    signed int i_sel_nbx;

    signed int i_sel_nby;

    /* Position of selection rectangle in cache */
    signed int i_sel_nex;

    signed int i_sel_ney;

    /* Type of selection rectangle in cache */
    unsigned char i_sel_state;

}; /* struct bfst_draw */

/* Interface: */

void bfst_draw_cleanup(
    struct bfst_view_ctxt const * const p_view_ctxt);

void bfst_draw_resize(
    struct bfst_view_ctxt const * const p_view_ctxt);

int bfst_draw_all(
    struct bfst_view_ctxt const * const p_view_ctxt);

void bfst_draw_expose(
    struct bfst_view_ctxt const * const p_view_ctxt,
    XEvent const * p_event);

void bfst_draw_invalidate(
    struct bfst_view_ctxt const * const p_view_ctxt);

/* end-of-file: bfst_draw.h */
