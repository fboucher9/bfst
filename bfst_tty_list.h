/* See LICENSE for license details. */

/*

Module: bfst_tty_list.h

Description:

    List of bfst_tty objects.

*/

/* Reverse include guard */
#if defined(INC_BFST_TTY_LIST_H)
#error include bfst_tty_list.h once
#endif /* #if defined(INC_BFST_TTY_LIST_H) */

#define INC_BFST_TTY_LIST_H

/* Predefine context handle */
struct bfst_view_ctxt;

/* Predefine tty object */
struct bfst_tty;

/*

Structure: bfst_tty_list

Description:

    List of bfst_tty objects.

*/

struct bfst_tty_list
{
    /* Array of pointers to bfst_tty objects */
    struct bfst_tty * * a_tty_list;

    /* Maximum number of objects in array */
    int i_tty_max_count;

    /* Number of object in array */
    int i_tty_count;

}; /* struct bfst_tty_list */

/* Interface */

void bfst_tty_list_init(
    struct bfst_view_ctxt const * const p_view_ctxt);

void bfst_tty_list_cleanup(
    struct bfst_view_ctxt const * const p_view_ctxt);

void bfst_tty_list_add(
    struct bfst_view_ctxt const * const p_view_ctxt,
    struct bfst_tty * const p_term_existing);

void bfst_tty_list_check_for_dead(
    struct bfst_view_ctxt const * const p_view_ctxt);

void bfst_tty_list_sel_clear(
    struct bfst_view_ctxt const * const p_view_ctxt);

void bfst_tty_list_resize(
    struct bfst_view_ctxt const * const p_view_ctxt);

/* end-of-file: bfst_tty_list.h */
