/* See LICENSE for license details. */

/*

Module: bfst_unique.c

Description:

    Unique identifier for each line of each terminal.

*/

#include "bfst_os.h"

#include "bfst_unique.h"

#include "bfst_ctxt.h"

/*

Function: bfst_unique_init()

Description:

    Initialization of bfst_unique module.

*/
void bfst_unique_init(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_unique * const p_unique = p_body_ctxt->p_unique;

    p_unique->i_next_unique = 0;

} /* bfst_unique_init() */

/*

Function: bfst_unique_cleanup()

Description:

    Undo initialization of bfst_unique module.

*/
void bfst_unique_cleanup(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    (void)(p_body_ctxt);
} /* bfst_unique_cleanup() */

/*

Function: bfst_unique_pick()

Description:

    Pick the next unique identifier from the magic bag of unique identifiers.

*/
unsigned long long int bfst_unique_pick(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_unique * const p_unique = p_body_ctxt->p_unique;

    p_unique->i_next_unique ++;

    return p_unique->i_next_unique;

} /* bfst_unique_pick() */

/* bfst_unique.c */
