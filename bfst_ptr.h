/* See LICENSE for license details. */

/*

Module: bfst_ptr.h

Description:

    Management of mouse pointer shape.

*/

#if defined(INC_BFST_PTR_H)
#error include bfst_ptr.h once
#endif /* #if defined(INC_BFST_PTR_H) */

#define INC_BFST_PTR_H

/* Predefine body context handle */
struct bfst_body_ctxt;

/*

Structure: bfst_ptr

Description:

    State of bfst_ptr module at global scope.

*/
struct bfst_ptr
{
    /* cursor resource handle for visible cursor shape */
    Cursor o_visible;

    /* cursor resource handle for hidden cursor shape */
    Cursor o_hidden;

}; /* struct bfst_ptr */

/* Interface: */

/* Initialize bfst_ptr module */
void bfst_ptr_init(
    struct bfst_body_ctxt const * const p_body_ctxt);

/* Cleanup of bfst_ptr module */
void bfst_ptr_cleanup(
    struct bfst_body_ctxt const * const p_body_ctxt);

/* Get ptr shape handle */
Cursor bfst_ptr_get(
    struct bfst_body_ctxt const * const p_body_ctxt,
    char const b_visible);

/* end-of-file: bfst_ptr.h */
