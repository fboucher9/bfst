/* See LICENSE for license details. */

/*

Module: bfst_key.h

Description:

    Translation of X11 key codes into terminal escape sequences.

*/

/* Reverse include guard */
#if defined(INC_BFST_KEY_H)
#error include bfst_key.h once
#endif /* #if defined(INC_BFST_KEY_H) */

#define INC_BFST_KEY_H

/* Macros: */
#define XK_ANY_MOD    (~0u)
#define XK_NO_MOD     0
#define XK_SWITCH_MOD (1<<13)

/* Predefine tty handle */
struct bfst_tty;

/* Interface: */

char const *
bfst_key_lookup(
    struct bfst_tty const * const p_term,
    KeySym const k,
    unsigned int const state);

void
bfst_key_toggle_numlock_flag(void);

/* end-of-file: bfst_key.h */
