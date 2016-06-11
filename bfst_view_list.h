/* See LICENSE for license details. */

/*

Module: bfst_view_list

Description:

    List of bfst_view objects.

*/

#if defined(INC_BFST_VIEW_LIST_H)
#error include bfst_view_list.h
#endif /* #if defined(INC_BFST_VIEW_LIST_H) */

#define INC_BFST_VIEW_LIST_H

struct bfst_body_ctxt;

struct bfst_view_ctxt;

/*

Structure: bfst_view_list

Description:

    List of bfst_view objects.

*/
struct bfst_view_list
{
    /* Pointer to array of object pointers */
    struct bfst_view_ctxt * * a_view_list;

    /* Number of objects currently active in array */
    unsigned int i_view_count;

    /* Maximum number of objects allocated for array */
    unsigned int i_view_max_count;

}; /* struct bfst_view_list */

void bfst_view_list_init(
    struct bfst_body_ctxt const * const p_body_ctxt);

void bfst_view_list_cleanup(
    struct bfst_body_ctxt const * const p_body_ctxt);

void bfst_view_list_add(
    struct bfst_body_ctxt const * const p_body_ctxt);

void bfst_view_list_remove(
    struct bfst_view_ctxt * const p_view_ctxt);

struct bfst_view_ctxt * bfst_view_list_find_win(
    struct bfst_body_ctxt const * const p_body_ctxt,
    Window const i_window_id);

enum bfst_view_list_scan_result
{
    bfst_view_list_scan_result_continue = 0,
    bfst_view_list_scan_result_stop,
    bfst_view_list_scan_result_remove
}; /* enum bfst_view_list_scan_result */

void bfst_view_list_scan(
    struct bfst_body_ctxt const * const p_body_ctxt,
    enum bfst_view_list_scan_result (*p_callback)(
        struct bfst_view_ctxt const * const p_view_ctxt,
        void * const h_context),
    void * const h_context);

/* end-of-file: bfst_view_list.h */
