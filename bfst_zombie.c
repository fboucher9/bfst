/* See LICENSE for license details. */

/*

Module: bfst_zombie.c

Description:

    Handling of zombies.

*/

#include <stdio.h>

#include <errno.h>

#include <string.h>

#include <stdlib.h>

#include <unistd.h>

#include <signal.h>

#include <sys/types.h>

#include <sys/wait.h>

#include <X11/Xlib.h>

#include "bfst_body.h"

static struct bfst_body_ctxt const * p_bfst_zombie_body_ctxt = 0;

static
struct bfst_child *
bfst_zombie_find_tty(
    struct bfst_body_ctxt const * const p_body_ctxt,
    pid_t o_process_id)
{
    struct bfst_child * p_child = NULL;

    unsigned int j;

    struct bfst_view_list const * const p_view_list = p_body_ctxt->p_view_list;

    for (j = 0; j < p_view_list->i_view_count; j++)
    {
        struct bfst_view_ctxt const * const p_view_ctxt = p_view_list->a_view_list[j];

        struct bfst_tty_list const * const p_tty_list = p_view_ctxt->p_tty_list;

        int i;

        for (i = 0 ; i < p_tty_list->i_tty_count; i ++)
        {
            struct bfst_tty * const p_term = p_tty_list->a_tty_list[i];

            if (p_term)
            {
                if (p_term->child.pid == o_process_id)
                {
                    p_child = &(p_term->child);

                    return p_child;
                }
            }
        }
    }

    return p_child;
}

static
void
bfst_sigchld(
    int i_unused)
{
    struct bfst_child * p_child;
    pid_t o_process_id;
    int stat, ret;

    (void)(i_unused);

    if ((o_process_id = waitpid(-1, &stat, WNOHANG)) < 0)
    {
        bfst_die();
    }

    p_child = bfst_zombie_find_tty(p_bfst_zombie_body_ctxt, o_process_id);

    if (p_child)
    {
        ret = WIFEXITED(stat) ? WEXITSTATUS(stat) : EXIT_FAILURE;

        if (ret != EXIT_SUCCESS)
        {
#if defined(BFST_CFG_DEBUG)
            bfst_msg("child finished with error '%d'\n", stat);
#endif /* #if defined(BFST_CFG_DEBUG) */
        }

        p_child->b_dead = 1;
    }

    signal(SIGCHLD, bfst_sigchld);
}

void
bfst_zombie_init(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    p_bfst_zombie_body_ctxt = p_body_ctxt;

    signal(SIGCHLD, bfst_sigchld);
}

void
bfst_zombie_cleanup(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    (void)(p_body_ctxt);
}
