/* See LICENSE for license details. */

/*

Module: bfst_zombie.h

Description:

    Handling of zombies.

*/

#if defined(INC_BFST_ZOMBIE_H)
#error include bfst_zombie.h once
#endif

#define INC_BFST_ZOMBIE_H

/* Interface: */

/* Initialization */
void bfst_zombie_init(
    struct bfst_body_ctxt const * const p_body_ctxt);

/* Cleanup */
void bfst_zombie_cleanup(
    struct bfst_body_ctxt const * const p_body_ctxt);

/* end-of-file: bfst_zombie.h */
