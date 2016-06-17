/* See LICENSE for license details. */

/*

Module: bfst_child.c

*/

/* snprintf() */
#include <stdio.h>

/* getuid() */
#include <unistd.h>

/* signal() */
#include <signal.h>

/* strerror() */
#include <string.h>

/* errno */
#include <errno.h>

/* unsetenv() */
#include <stdlib.h>

/* getpwuid() */
#include <pwd.h>

/* open() */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <X11/Xlib.h>

/* openpty() */
#if   defined(__linux) || 1
 #include <pty.h>
#elif defined(__OpenBSD__) || defined(__NetBSD__) || defined(__APPLE__)
 #include <util.h>
#elif defined(__FreeBSD__) || defined(__DragonFly__)
 #include <libutil.h>
#endif

#include "bfst_body.h"

/* default SHELL value */
static char a_bfst_child_shell[] = "/bin/sh";

/* default TERM value */
static char a_bfst_child_termname[] = "xterm-256color";

static
void
bfst_child_fixenv(
    char const * const
        user,
    char const * const
        home,
    char const * const
        sh,
    unsigned long int const
        win)
{
    unsetenv("COLUMNS");

    unsetenv("LINES");

    unsetenv("TERMCAP");

    setenv("LOGNAME", user, 1);

    setenv("USER", user, 1);

    setenv("SHELL", sh, 1);

    setenv("HOME", home, 1);

    setenv("TERM", a_bfst_child_termname, 1);

    {
        char buf[sizeof(long) * 8 + 1];

        snprintf(buf, sizeof(buf), "%lu", win);

        setenv("WINDOWID", buf, 1);
    }
}

static
void
bfst_child_fixsig(void)
{
    signal(SIGCHLD, SIG_DFL);

    signal(SIGHUP, SIG_DFL);

    signal(SIGINT, SIG_DFL);

    signal(SIGQUIT, SIG_DFL);

    signal(SIGTERM, SIG_DFL);

    signal(SIGALRM, SIG_DFL);
}

static
void
bfst_child_execsh(
    char const * const *
        opt_cmd,
    unsigned long int const
        win)
{
    char const * sh [2];

    const struct passwd *pw;

    errno = 0;

    if ((pw = getpwuid(getuid())) == NULL)
    {
        bfst_die();
    }

    sh[0] = getenv("SHELL");

    sh[1] = NULL;

    if (!sh[0])
    {
        sh[0] = (pw->pw_shell[0]) ? pw->pw_shell : a_bfst_child_shell;
    }

    bfst_child_fixenv(pw->pw_name, pw->pw_dir, sh[0], win);

    bfst_child_fixsig();

    {
        char const * prog;
        char const * const * args;

        prog = (opt_cmd) ? opt_cmd[0] : sh[0];

        args = (opt_cmd) ? opt_cmd : sh;

        execvp((char *)(prog), (char * * )(args));

        _exit(EXIT_FAILURE);
    }
}

void bfst_child_init(
    struct bfst_tty_ctxt const * const p_term_ctxt)
{
    struct bfst_child * const p_child = p_term_ctxt->p_child;

    struct bfst_options const * const p_opts = p_term_ctxt->p_body_ctxt->p_opts;

    struct bfst_window const * const p_window = p_term_ctxt->p_view_ctxt->p_window;

    int i_master_fd;

    int i_slave_fd;

    struct winsize o_winsize = {p_window->i_tty_rows, p_window->i_tty_cols, 0, 0};

    /* seems to work fine on linux, openbsd and freebsd */
    if (openpty(&i_master_fd, &i_slave_fd, NULL, NULL, &o_winsize) < 0)
    {
        bfst_die();
    }

    p_child->pid = fork();

    if (-1 == p_child->pid)
    {
        bfst_die();
    }

    if (0 == p_child->pid)
    {
        close(STDOUT_FILENO);

        setsid(); /* create a new process group */

        dup2(i_slave_fd, STDIN_FILENO);

        dup2(i_slave_fd, STDOUT_FILENO);

        dup2(i_slave_fd, STDERR_FILENO);

        if (ioctl(i_slave_fd, TIOCSCTTY, NULL) < 0)
        {
            bfst_die();
        }

        close(i_slave_fd);

        close(i_master_fd);

        bfst_child_execsh(p_opts->opt_cmd, p_window->h_win_res);
    }

    close(i_slave_fd);

    p_child->i_pty_fd = i_master_fd;

    p_child->i_read_fd = p_child->i_pty_fd;

    p_child->i_write_fd = p_child->i_pty_fd;

    {
        int flags = fcntl(p_child->i_pty_fd, F_GETFL, 0);
        fcntl(p_child->i_pty_fd, F_SETFL, flags | O_NONBLOCK);
    }
}

int
bfst_child_read(
    struct bfst_tty_ctxt const * const
        p_term_ctxt)
{
    static char ac_cmdfd_read_buf[BUFSIZ];
    static int i_cmdfd_read_buflen = 0;
    int i_read_count;
    int i_put_count;

    struct bfst_child * const p_child = p_term_ctxt->p_child;

    struct bfst_tty * const p_term = p_term_ctxt->p_term;

    /* append read bytes to unprocessed bytes */
    i_read_count =
        read(
            p_child->i_read_fd,
            ac_cmdfd_read_buf + i_cmdfd_read_buflen,
            sizeof(ac_cmdfd_read_buf) - i_cmdfd_read_buflen);

    if (i_read_count >= 0)
    {
        i_cmdfd_read_buflen += i_read_count;

        if (i_cmdfd_read_buflen > 0)
        {
            /* process every complete utf8 char */
            i_put_count = tputstr(p_term, ac_cmdfd_read_buf, i_cmdfd_read_buflen);

            if (i_put_count > 0)
            {
                i_cmdfd_read_buflen -= i_put_count;

                /* keep any uncomplete utf8 char for the next call */
                memmove(ac_cmdfd_read_buf, ac_cmdfd_read_buf + i_put_count, i_cmdfd_read_buflen);
            }
        }
    }
    else
    {
#if defined(BFST_CFG_DEBUG)
        bfst_msg("error reading from child pty\n");
#endif /* #if defined(BFST_CFG_DEBUG) */
    }

    return i_read_count;
}

void bfst_child_write(
    struct bfst_tty_ctxt const * const p_term_ctxt,
    const char *s,
    unsigned long int n)
{
    struct bfst_child * const p_child = p_term_ctxt->p_child;

    if (bfst_write(p_child->i_write_fd, s, n) == -1)
    {
        bfst_die();
    }
}

void bfst_child_send(
    struct bfst_tty_ctxt const * const p_term_ctxt,
    char const * s,
    unsigned long int n)
{
    bfst_child_write(p_term_ctxt, s, n);

    techostr(p_term_ctxt->p_term, s, n);
}

void bfst_child_resize(
    struct bfst_tty_ctxt const * const p_term_ctxt,
    unsigned int const col,
    unsigned int const row,
    unsigned int const width,
    unsigned int const height)
{
    struct bfst_child * const p_child = p_term_ctxt->p_child;

    if (-1 != p_child->i_pty_fd)
    {
        struct winsize w;

        w.ws_row = row;

        w.ws_col = col;

        w.ws_xpixel = width;

        w.ws_ypixel = height;

        if (ioctl(p_child->i_pty_fd, TIOCSWINSZ, &w) < 0)
        {
#if defined(BFST_CFG_DEBUG)
            bfst_msg("Couldn't set window size: %s\n", strerror(errno));
#endif /* #if defined(BFST_CFG_DEBUG) */
        }
    }
}

/*

Function: bfst_child_cleanup

Description:

    Release all resources.

Comments:

    -   This is only called when child process has been killed.

*/
void bfst_child_cleanup(
    struct bfst_tty_ctxt const * const p_term_ctxt)
{
    struct bfst_child * const p_child = p_term_ctxt->p_child;

    /* Release the read file descriptor */
    if (p_child->i_read_fd != p_child->i_pty_fd)
    {
        if (-1 != p_child->i_read_fd)
        {
            close(p_child->i_read_fd);

            p_child->i_read_fd = -1;
        }
    }

    /* Release the write file descriptor */
    if (p_child->i_write_fd != p_child->i_pty_fd)
    {
        if (-1 != p_child->i_write_fd)
        {
            close(p_child->i_write_fd);

            p_child->i_write_fd = -1;
        }
    }

    /* Release the master file descriptor */
    if (-1 != p_child->i_pty_fd)
    {
        close(p_child->i_pty_fd);

        p_child->i_pty_fd = -1;
    }

} /* bfst_child_cleanup() */

/* end-of-file: bfst_child.c */
