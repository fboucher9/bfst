/* See LICENSE for license details. */

/*

Module: bfst_unique.h

Description:

    Unique identifier for each line of each terminal.

Notes:

    -   Value of zero is reserved.

    -   Value must be at least 64-bits in length, so enable support for
        long long type in your compiler and disable warnings about
        use of long long type in C language.

*/

#if defined(INC_BFST_UNIQUE_H)
#error include bfst_unique.h once
#endif /* #if defined(INC_BFST_UNIQUE_H) */

#define INC_BFST_UNIQUE_H

/* Predefine context handle */
struct bfst_body_ctxt;

/*

Structure: bfst_unique

Description:

    State for unique identifier.

Comments:

    -   Place an instance of this structure into bfst_body container.

*/
struct bfst_unique
{
    unsigned long long int i_next_unique;

}; /* struct bfst_unique */

void bfst_unique_init(
    struct bfst_body_ctxt const * const p_body_ctxt);

void bfst_unique_cleanup(
    struct bfst_body_ctxt const * const p_body_ctxt);

unsigned long long int bfst_unique_pick(
    struct bfst_body_ctxt const * const p_body_ctxt);

/* end-of-file: bfst_unique.h */
