/* See LICENSE for license details. */

/*

Module: bfst_child.h

Description:

    Functions for handling of child process

*/

/* Reverse header include guard */
#if defined(INC_BFST_CHILD_H)
#error include bfst_child.h once
#endif

#define INC_BFST_CHILD_H

/* Predefine context handle */
struct bfst_tty_ctxt;

/*

Structure: bfst_child

Description:

    Storage for all members of bfst_child module.

Comments:

    -   Place an instance of this structure into bfst_tty container.

    -   Initialization may be done by calling memset on the structure with
        zeroes.

*/
struct bfst_child
{
    /* File descriptor of child pty */
    int i_pty_fd;

    /* File descriptor to read from child pty */
    int i_read_fd;

    /* File descriptor to write to child pty */
    int i_write_fd;

    /* Process id of child process */
    pid_t pid;

    /* Flag indicating that child process has been terminated */
    char b_dead;

}; /* struct bfst_child */

/* Interface: */

/* Read from pty of child process */
void bfst_child_read(
    struct bfst_tty_ctxt const * const p_term_ctxt);

/* Write to pty of child process */
void bfst_child_write(
    struct bfst_tty_ctxt const * const p_term_ctxt,
    const char * s,
    unsigned long int n);

/* Create a pty and a child process */
void bfst_child_new(
    struct bfst_tty_ctxt const * const p_term_ctxt);

/* Write to pty of child process with echo to tty */
void bfst_child_send(
    struct bfst_tty_ctxt const * const p_term_ctxt,
    char const * s,
    unsigned long int n);

/* Handling of window resize event */
void bfst_child_resize(
    struct bfst_tty_ctxt const * const p_term_ctxt,
    unsigned int const col,
    unsigned int const row,
    unsigned int const width,
    unsigned int const height);

/* end-of-file: bfst_child.h */
