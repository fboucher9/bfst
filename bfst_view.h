/* See LICENSE for license details. */

/*

Module: bfst_view.h

*/

struct bfst_tty;

struct bfst_glyph;

enum window_state
{
    WIN_VISIBLE = 1,
    WIN_FOCUSED = 2
};

/*

Note: caching of view

    Attributes:

        y position of cached view

        alt state of cached view

        new line frame index

        cached line frame index

        cached buffer frame index

        global frame index

    Methods:

        A line has changed if alt state has changed.

        A line has changed if position has changed.

        A line has changed if frame number is different.

        A line has changed if number of rows or colums has changed.

        A line has changed if it contains cursor.

        A line has changed if it contains a selection.

        A character has changed if glyph is different.

*/

#if 0
struct bfst_line
{
    /* Pointer to array of glyph objects */
    struct bfst_glyph * p_chars;

    /* Current number of columns used by line */
    unsigned long int i_columns;

    /* Maximum number of columns allocated for line */
    unsigned long int i_max_columns;

    /* Identification of a line:

        -   Terminal identifier
        -   Line identifier
            - Negative lines for log
            - Position lines for primary/alt
        -   Alt flag
        -   Modification timestamp

    */

    /* Identifier for this line.  Globally unique identifier */
    unsigned long long int i_frame;

    /* Identifier for terminal */
    unsigned long int i_term;

    /* Index of this line in terminal buffer */
    long int i_pos;

    /* Is this line from primary or alternate buffer? */
    char b_alt;

    /* Number of blink characters in line */
    unsigned long int i_blinks;

};

struct bfst_cache
{
    /* Pointer to array of line objects */
    struct bfst_line * p_lines;

    /* Current number of rows used by cache */
    unsigned long int i_rows;

    /* Maximum number of rows allocated in array */
    unsigned long int i_max_rows;

    /* Number of blink characters for all lines */
    unsigned long int i_blinks;

    /* Identifier for terminal */
    unsigned long int i_term;

    /* Index of first line in cache */
    long int i_pos;

    /* Indicate that non-log lines are primary or alt */
    char b_alt;

    char b_cur_shape;

    int i_cur_x;

    int i_cur_y;

    int i_sel_nbx;

    int i_sel_nby;

    int i_sel_nex;

    int i_sel_ney;

    int i_sel_type;

};
#endif

struct bfst_view
{
    /* Context */
    struct bfst_view_ctxt o_view_ctxt;

    /* Font */
    struct bfst_font o_font;

    /* Color */
    struct bfst_color o_color;

    /* Window */
    struct bfst_window o_window;

    /* Draw */
    struct bfst_draw o_draw;

    /* visibility of pointer */
    struct bfst_ptr_show o_ptr_show;

    /* List of children */
    struct bfst_tty_list o_tty_list;

    /* Current tab */
    struct bfst_tab o_tab;

}; /* struct bfst_view */

/* Interface: */

struct bfst_view * bfst_view_new(
    struct bfst_body_ctxt const * const p_body_ctxt);

void bfst_view_delete(
    struct bfst_view_ctxt * const p_view_ctxt);

void bfst_view_resize(
    struct bfst_view * const p_view,
    int const i_screen_width_pix,
    int const i_screen_height_pix);

/* end-of-file: bfst_view.h */
