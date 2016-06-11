/* See LICENSE for license details. */

/*

Module: bfst_sel.h

*/

struct bfst_tty;

struct bfst_view_ctxt;

struct bfst_sel
{
    int mode;

    int type;

    int snap;

    /*
     * Selection variables:
     * nb – normalized coordinates of the beginning of the selection
     * ne – normalized coordinates of the end of the selection
     * ob – original coordinates of the beginning of the selection
     * oe – original coordinates of the end of the selection
     */
    struct
    {
        int x;
        int y;
    } nb, ne, ob, oe;

    char alt;

};

void bfst_sel_init(
    struct bfst_tty * const
        p_term);

char
bfst_sel_is_line_selected(
    struct bfst_tty * const
        p_term,
    int const
        y);

void
bfst_sel_clear(
    struct bfst_tty * const p_term);

void
selscroll(
    struct bfst_tty* p_term,
    int,
    int);

char
bfst_sel_test(
    struct bfst_tty const * p_term,
    int x,
    int y);

void
bfst_sel_paste(
    struct bfst_tty* p_term);

void
bfst_sel_click1(
    struct bfst_tty* p_term,
    int x,
    int y);

void
bfst_sel_release1(
    struct bfst_tty * const p_term,
    XEvent * const e);

void
bfst_sel_motion(
    struct bfst_tty * const p_term,
    XEvent * const e);

void
bfst_sel_notify(
    struct bfst_view_ctxt const * const p_view_ctxt,
    XEvent * e);

void
bfst_sel_request(
    struct bfst_body_ctxt const * const p_body_ctxt,
    XEvent * e);

unsigned short int
bfst_sel_calc_flags(
    struct bfst_tty const * const p_term,
    int const x,
    int const y);
