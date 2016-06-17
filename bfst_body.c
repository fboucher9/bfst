/* See LICENSE for license details. */

/*

Module: bfst_body.c

*/

#include <stdio.h>

#include <unistd.h>

#include <locale.h>

#include <stdint.h>

#include <stdlib.h>

#include <string.h>

#include <time.h>

#include <errno.h>

#include <sys/select.h>

#include <X11/Xlib.h>

#include <X11/Xutil.h>

#include <limits.h>

#include "bfst_body.h"

/* frames per second st should at maximum draw to the screen */
static int const xfps = 60;
static int const actionfps = 20;

/*
 * blinking timeout (set to 0 to disable blinking) for the terminal blinking
 * attribute.
 */
static long const blinktimeout = 800;

/* Override mouse-select while mask is active (when MODE_MOUSE is set).
 * Note that if you want to use ShiftMask with selmasks, set this to an other
 * modifier, set to 0 to not use it. */
static unsigned int const forceselmod = ShiftMask;

/*
 * State bits to ignore when matching key or button events.  By default,
 * numlock (Mod2Mask) and keyboard layout (XK_SWITCH_MOD) are ignored.
 */
static unsigned int const ignoremod = Mod2Mask|XK_SWITCH_MOD;

struct bfst_shortcut
{
    unsigned int mod;

    KeySym keysym;

    void (* func)(
        struct bfst_view_ctxt const * const
            p_view_ctxt);
};

typedef struct {
        unsigned int b;
        unsigned int mask;
        char *s;
} Mousekey;

/* Internal mouse shortcuts. */
/* Beware that overloading Button1 will disable the selection. */
static Mousekey mshortcuts[] = {
        /* button               mask            string */
        { Button4,              XK_ANY_MOD,     "\031" },
        { Button5,              XK_ANY_MOD,     "\005" },
};

void
numlock(struct bfst_view_ctxt const * const p_view_ctxt)
{
    (void)(p_view_ctxt);

    bfst_key_toggle_numlock_flag();
}

void
selpaste(struct bfst_view_ctxt const * const p_view_ctxt)
{
    struct bfst_tty * const p_term = bfst_tab_get(p_view_ctxt);

    if (p_term)
    {
        bfst_sel_paste(p_term);
    }
}

void
nextterm(
    struct bfst_view_ctxt const * const
        p_view_ctxt)
{
    bfst_tab_next(p_view_ctxt);

}

static void openterm(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    bfst_tty_list_add(p_view_ctxt, NULL);
}

/*

Function: detachterm

Description:

    Detach the current tty from the current view and create a new view using
    the detached tty as its first tty.

*/
static void detachterm(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    struct bfst_tty_list * const p_tty_list = p_view_ctxt->p_tty_list;

    /* Make sure there are at least two terminals in this window */
    if (p_tty_list->i_tty_count > 1)
    {
        struct bfst_tty * const p_term = bfst_tab_get(p_view_ctxt);

        if (p_term)
        {
            /* remove this tty from the current view */
            {
                struct bfst_tab const * const p_tab = p_view_ctxt->p_tab;

                int const i = p_tab->i_tty_index;

                p_tty_list->a_tty_list[i] = NULL;

                int k;

                for (k=i ; k < p_tty_list->i_tty_count - 1; k++)
                {
                    p_tty_list->a_tty_list[k] = p_tty_list->a_tty_list[k + 1];
                }

                p_tty_list->i_tty_count --;

                bfst_tab_refresh(p_view_ctxt);
            }

            /* insert this tty into a new view */
            {
                struct bfst_body_ctxt const * const p_body_ctxt = p_view_ctxt->p_body_ctxt;

                bfst_view_list_add(p_body_ctxt, p_term);
            }
        }
    }

}

static
void
openview(
    struct bfst_view_ctxt const * const
        p_view_ctxt)
{
    struct bfst_body_ctxt const * const p_body_ctxt = p_view_ctxt->p_body_ctxt;

    bfst_view_list_add(p_body_ctxt, NULL);
}

static
void
viewchange(
    struct bfst_view_ctxt const * const p_view_ctxt,
    int const i_half_page)
{
    struct bfst_draw * const p_draw = p_view_ctxt->p_draw;

    struct bfst_tty * const p_term = bfst_tab_get(p_view_ctxt);

    p_draw->top += i_half_page;

    if (p_term)
    {
        if (p_draw->top < 0)
        {
            if ((unsigned int)(-p_draw->top) > (p_term->log.i_count))
            {
                p_draw->top = -(p_term->log.i_count);
            }
        }

        if (p_draw->top > 0)
        {
            p_draw->top = 0;
        }
    }
    else
    {
        p_draw->top = 0;
    }
}

static
void
viewlogup(
    struct bfst_view_ctxt const * const
        p_view_ctxt)
{
    struct bfst_window * const p_window = p_view_ctxt->p_window;

    viewchange(p_view_ctxt, -p_window->i_tty_rows);
}

static void viewlogdn(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    struct bfst_window * const p_window = p_view_ctxt->p_window;

    viewchange(p_view_ctxt, p_window->i_tty_rows);
}

static void viewlogup1(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    viewchange(p_view_ctxt, -1);
}

static void viewlogdn1(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    viewchange(p_view_ctxt, 1);
}

static
void
viewalt(
    struct bfst_view_ctxt const * const
        p_view_ctxt)
{
    struct bfst_draw * const p_draw = p_view_ctxt->p_draw;

    if (p_draw->alt == 0)
    {
        p_draw->alt = 1;
    }
    else
    {
        p_draw->alt = 0;
    }
}

/*

Function: bfst_body_set_font()

Description:

    Change the font, keeping the same window size but resizing the terminal.

*/
void bfst_body_set_font(
    struct bfst_view_ctxt const * const p_view_ctxt,
    char const * const p_font_name)
{
    bfst_font_set(p_view_ctxt, p_font_name);

    bfst_view_resize(p_view_ctxt->p_view, 0, 0);

} /* bfst_body_set_font() */


/*

Function: nextfont()

Description:

    Select the next font from the predefined list of fonts.

*/
static void nextfont(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    static unsigned int g_font_index = 0;

    g_font_index ++;

    if (g_font_index >= bfst_font_list_count(p_view_ctxt->p_body_ctxt))
    {
        g_font_index = 0;
    }

    bfst_body_set_font(p_view_ctxt, bfst_font_list_get(p_view_ctxt->p_body_ctxt, g_font_index));

} /* nextfont() */

/* Internal keyboard shortcuts. */
#define MODKEY Mod1Mask

static struct bfst_shortcut shortcuts[] = {
        /* mask                 keysym          function   */
        { ShiftMask,            XK_Insert,      selpaste   },
        { MODKEY,               XK_Num_Lock,    numlock    },
        { MODKEY,               XK_n,           nextterm   },
        { MODKEY,               XK_o,           openterm   },
        { MODKEY,               XK_t,           openview   },
        { ShiftMask,            XK_Page_Up,     viewlogup  },
        { ShiftMask,            XK_Page_Down,   viewlogdn  },
        { ShiftMask,            XK_Up,          viewlogup1 },
        { ShiftMask,            XK_Down,        viewlogdn1 },
        { MODKEY,               XK_semicolon,   viewalt    },
        { MODKEY,               XK_f,           nextfont   },
        { MODKEY,               XK_y,           detachterm },
};

/*

Function: bfst_body_init_ctxt()

Description:

*/
static void bfst_body_init_ctxt(
    struct bfst_body * const p_body)
{
    p_body->o_body_ctxt.p_body = p_body;

    p_body->o_body_ctxt.p_opts = &p_body->o_opts;

    p_body->o_body_ctxt.p_display = &p_body->o_display;

    p_body->o_body_ctxt.p_view_list = &p_body->o_view_list;

    p_body->o_body_ctxt.p_ptr = &p_body->o_ptr;

    p_body->o_body_ctxt.p_unique = &p_body->o_unique;

    p_body->o_body_ctxt.p_focus = &p_body->o_focus;

    p_body->o_body_ctxt.p_font_list = &p_body->o_font_list;

} /* bfst_body_init_ctxt() */

char
bfst_body_init(
    struct bfst_body * const p_body,
    struct bfst_options const * const p_options)
{
    memset(p_body, 0x00, sizeof(*p_body));

    /* context */
    bfst_body_init_ctxt(p_body);

    p_body->o_opts = *p_options;

    if (p_body->o_opts.opt_cols < 1)
    {
        p_body->o_opts.opt_cols = 1;
    }

    if (p_body->o_opts.opt_rows < 1)
    {
        p_body->o_opts.opt_rows = 1;
    }

    /* resources */
    bfst_display_init(&p_body->o_body_ctxt);

    /* font list */
    bfst_font_list_init(&p_body->o_body_ctxt);

    /* white cursor, black outline */
    bfst_ptr_init(&p_body->o_body_ctxt);

    /* install sigchld handler */
    bfst_zombie_init(&p_body->o_body_ctxt);

    /* view list */
    bfst_view_list_init(&p_body->o_body_ctxt);

    return 1;
}

void
bfst_body_cleanup(
    struct bfst_body * const p_body)
{
    (void)(p_body);
}

static
char
match(unsigned int mask, unsigned int state)
{
        return mask == XK_ANY_MOD || mask == (state & ~ignoremod);
}

static int oldbutton = 3; /* button event on startup: 3 = release */

void
mousereport(
    struct bfst_tty_ctxt * p_term_ctxt,
    XEvent *e)
{
    struct bfst_view_ctxt const * const p_view_ctxt = p_term_ctxt->p_view_ctxt;

    struct bfst_draw const * const p_draw = p_view_ctxt->p_draw;

    struct bfst_tty * const p_term = p_term_ctxt->p_term;

    int x = x2col(p_term, e->xbutton.x), y = y2row(p_term, e->xbutton.y) + p_draw->top,
        button = e->xbutton.button, state = e->xbutton.state,
        len;
    char buf[40];
    static int ox, oy;

    if (y < 0)
        return;

    /* from urxvt */
    if(e->xbutton.type == MotionNotify)
    {
        if(x == ox && y == oy)
            return;
        if(!IS_SET(p_term, MODE_MOUSEMOTION) && !IS_SET(p_term, MODE_MOUSEMANY))
            return;
        /* MOUSE_MOTION: no reporting if no button is pressed */
        if(IS_SET(p_term, MODE_MOUSEMOTION) && oldbutton == 3)
            return;

        button = oldbutton + 32;
        ox = x;
        oy = y;
    } else {
        if(!IS_SET(p_term, MODE_MOUSESGR) && e->xbutton.type == ButtonRelease) {
            button = 3;
        } else {
            button -= Button1;
            if(button >= 3)
                button += 64 - 3;
        }
        if(e->xbutton.type == ButtonPress) {
            oldbutton = button;
            ox = x;
            oy = y;
        } else if(e->xbutton.type == ButtonRelease) {
            oldbutton = 3;
            /* MODE_MOUSEX10: no button release reporting */
            if(IS_SET(p_term, MODE_MOUSEX10))
                return;
            if (button == 64 || button == 65)
                return;
        }
    }

    if(!IS_SET(p_term, MODE_MOUSEX10)) {
        button += ((state & ShiftMask  ) ? 4  : 0)
            + ((state & Mod4Mask   ) ? 8  : 0)
            + ((state & ControlMask) ? 16 : 0);
    }

    if(IS_SET(p_term, MODE_MOUSESGR)) {
        len = snprintf(buf, sizeof(buf), "\033[<%d;%d;%d%c",
            button, x+1, y+1,
            e->xbutton.type == ButtonRelease ? 'm' : 'M');
    } else if(x < 223 && y < 223) {
        len = snprintf(buf, sizeof(buf), "\033[M%c%c%c",
            32+button, 32+x+1, 32+y+1);
    } else {
        return;
    }

    bfst_child_write(&p_term->o_term_ctxt, buf, len);
}


static
void
bfst_handle_bpress(
    struct bfst_body_ctxt const * const p_body_ctxt,
    XEvent *e)
{
    Mousekey *mk;

    struct bfst_view_ctxt * p_view_ctxt;

    p_view_ctxt = bfst_view_list_find_win(p_body_ctxt, e->xbutton.window);

    if (p_view_ctxt)
    {
        struct bfst_tty * const p_term = bfst_tab_get(p_view_ctxt);

        if (p_term)
        {
            struct bfst_view * const p_view = p_view_ctxt->p_view;

            if (IS_SET(p_term, MODE_MOUSE) && !(e->xbutton.state & forceselmod))
            {
                mousereport(&p_term->o_term_ctxt, e);
                return;
            }

            for(mk = mshortcuts; mk < mshortcuts + BFST_TOOLS_LEN(mshortcuts); mk++)
            {
                if(e->xbutton.button == mk->b
                    && match(mk->mask, e->xbutton.state))
                {
                    bfst_child_send(&(p_term->o_term_ctxt), mk->s, strlen(mk->s));
                    return;
                }
            }

            if (e->xbutton.button == Button1)
            {
                /* Clear previous selection, logically and visually. */
                bfst_sel_click1(
                    p_term,
                    x2col(
                        p_term,
                        e->xbutton.x),
                    y2row(
                        p_term,
                        e->xbutton.y) + p_view->o_draw.top);
            }
        }
    }
}

static
enum bfst_view_list_scan_result
bfst_handle_selclear_view_cb(
    struct bfst_view_ctxt const * const p_view_ctxt,
    void * const h_context)
{
    (void)(h_context);

    bfst_tty_list_sel_clear(p_view_ctxt);

    return bfst_view_list_scan_result_continue;
}

static
void
bfst_handle_selclear(
    struct bfst_body_ctxt const * const p_body_ctxt,
    XEvent *e)
{
    (void)(e);

    bfst_view_list_scan(
        p_body_ctxt,
        &(bfst_handle_selclear_view_cb),
        NULL);
}

static
void
bfst_handle_selnotify(
    struct bfst_body_ctxt const * const p_body_ctxt,
    XEvent *e)
{
    struct bfst_view_ctxt const * const p_view_ctxt = bfst_focus_get(p_body_ctxt);

    if (p_view_ctxt)
    {
        bfst_sel_notify(p_view_ctxt, e);
    }
}

static
void
bfst_handle_selrequest(
    struct bfst_body_ctxt const * const p_body_ctxt,
    XEvent *e)
{
    bfst_sel_request(p_body_ctxt, e);
}

static
void
bfst_handle_brelease(
    struct bfst_body_ctxt const * const p_body_ctxt,
    XEvent *e)
{
    struct bfst_view_ctxt * const p_view_ctxt = bfst_view_list_find_win(p_body_ctxt, e->xbutton.window);

    if (p_view_ctxt)
    {
        struct bfst_tty * const p_term = bfst_tab_get(p_view_ctxt);

        if (p_term)
        {
            if(IS_SET(p_term, MODE_MOUSE) && !(e->xbutton.state & forceselmod))
            {
                mousereport(&p_term->o_term_ctxt, e);
                return;
            }

            if(e->xbutton.button == Button3)
            {
                bfst_sel_paste(p_term);
            }
            else if(e->xbutton.button == Button1)
            {
                bfst_sel_release1(p_term, e);
            }
        }
    }
}

static
void
bfst_handle_bmotion(
    struct bfst_body_ctxt const * const p_body_ctxt,
    XEvent *e)
{
    struct bfst_view_ctxt * const p_view_ctxt = bfst_view_list_find_win(p_body_ctxt, e->xmotion.window);

    if (p_view_ctxt)
    {
        bfst_ptr_show_change(p_view_ctxt, 1);

        struct bfst_tty * const p_term = bfst_tab_get(p_view_ctxt);

        if (p_term)
        {
            if (IS_SET(p_term, MODE_MOUSE) && !(e->xbutton.state & forceselmod))
            {
                mousereport(&p_term->o_term_ctxt, e);
                return;
            }

            bfst_sel_motion(p_term, e);
        }
    }
}

static
void
bfst_handle_expose(
    struct bfst_body_ctxt const * const p_body_ctxt,
    XEvent *ev)
{
    struct bfst_view_ctxt * const p_view_ctxt = bfst_view_list_find_win(p_body_ctxt, ev->xexpose.window);

    if (p_view_ctxt)
    {
        bfst_draw_expose(p_view_ctxt, ev);
    }
}

static
void
bfst_handle_visibility(
    struct bfst_body_ctxt const * const p_body_ctxt,
    XEvent *ev)
{
    XVisibilityEvent *e = &ev->xvisibility;

    struct bfst_view_ctxt * const p_view_ctxt = bfst_view_list_find_win(p_body_ctxt, e->window);

    if (p_view_ctxt)
    {
        struct bfst_window * const p_window = p_view_ctxt->p_window;

        MODBIT(p_window->state, e->state != VisibilityFullyObscured, WIN_VISIBLE);
    }
}

static
void
bfst_handle_unmap(
    struct bfst_body_ctxt const * const p_body_ctxt,
    XEvent *ev)
{
    struct bfst_view_ctxt * const p_view_ctxt = bfst_view_list_find_win(p_body_ctxt, ev->xunmap.window);

    if (p_view_ctxt)
    {
        struct bfst_window * const p_window = p_view_ctxt->p_window;

        p_window->state &= ~WIN_VISIBLE;
    }
}

static
void
bfst_set_focus(
    struct bfst_view_ctxt * const
        p_view_ctxt,
    char b_focus_in)
{
    struct bfst_body_ctxt const * const p_body_ctxt = p_view_ctxt->p_body_ctxt;

    struct bfst_tty * const p_term = bfst_tab_get(p_view_ctxt);

    struct bfst_window * const p_window = p_view_ctxt->p_window;

    if (
        b_focus_in)
    {
        p_window->state |= WIN_FOCUSED;

        if (p_term)
        {
            if (IS_SET(p_term, MODE_FOCUS))
            {
                bfst_child_write(&(p_term->o_term_ctxt), "\033[I", 3);
            }
        }

        /* Make the mouse pointer visible on alt-tab */
        bfst_ptr_show_change(p_view_ctxt, 1);

        bfst_focus_set(p_body_ctxt, p_view_ctxt);
    }
    else
    {
        bfst_focus_set(p_body_ctxt, NULL);

        p_window->state &= ~WIN_FOCUSED;

        if (p_term)
        {
            if (IS_SET(p_term, MODE_FOCUS))
            {
                bfst_child_write(&(p_term->o_term_ctxt), "\033[O", 3);
            }
        }
    }
}

static
void
bfst_handle_focus(
    struct bfst_body_ctxt const * const p_body_ctxt,
    XEvent *ev)
{
    XFocusChangeEvent *e = &ev->xfocus;

    if(e->mode == NotifyGrab)
    {
        return;
    }

    struct bfst_view_ctxt * const p_view_ctxt = bfst_view_list_find_win(p_body_ctxt, e->window);

    if (p_view_ctxt)
    {
        bfst_set_focus(
            p_view_ctxt,
            ev->type == FocusIn);
    }
}

static
void
bfst_handle_enterleave(
    struct bfst_body_ctxt const * const p_body_ctxt,
    XEvent *ev)
{
    XCrossingEvent *e = &ev->xcrossing;

    struct bfst_view_ctxt * const p_view_ctxt = bfst_view_list_find_win(p_body_ctxt, e->window);

    if (p_view_ctxt)
    {
        bfst_set_focus(
            p_view_ctxt,
            e->type == EnterNotify);
    }
}

static
void
bfst_handle_kpress(
    struct bfst_body_ctxt const * const p_body_ctxt,
    XEvent *ev)
{
    XKeyEvent *e = &ev->xkey;
    KeySym ksym;
    char buf[32];
    char const * customkey;
    int len;
    unsigned long int c;
    XComposeStatus compstatus;
    struct bfst_shortcut *bp;

    struct bfst_view_ctxt * const p_view_ctxt = bfst_view_list_find_win(p_body_ctxt, e->window);

    if (p_view_ctxt)
    {
        struct bfst_view * const p_view = p_view_ctxt->p_view;

        struct bfst_tty * const p_term = bfst_tab_get(p_view_ctxt);

        if(IS_SET(p_term, MODE_KBDLOCK))
            return;

        len = XLookupString(e, buf, sizeof(buf), &ksym, &compstatus);

        /* 1. shortcuts */
        for(bp = shortcuts; bp < shortcuts + BFST_TOOLS_LEN(shortcuts); bp++)
        {
            if(ksym == bp->keysym && match(bp->mod, e->state))
            {
                bp->func(p_view_ctxt);
                return;
            }
        }

        /* 2. custom keys from config.h */
        if((customkey = bfst_key_lookup(p_term, ksym, e->state))) {
            bfst_ptr_show_change(p_view_ctxt, 0);
            p_view->o_draw.top = 0;
            bfst_child_send(&(p_term->o_term_ctxt), customkey, strlen(customkey));
            return;
        }

        /* 3. composed string from input method */
        if(len == 0)
            return;

        if(len == 1 && e->state & Mod1Mask)
        {
            if(IS_SET(p_term, MODE_8BIT))
            {
                if(*buf < 0177)
                {
                    c = *buf | 0x80;
                    len = bfst_utf8_encode(c, buf);
                }
            }
            else
            {
                buf[1] = buf[0];
                buf[0] = '\033';
                len = 2;
            }
        }

        bfst_ptr_show_change(p_view_ctxt, 0);
        p_view->o_draw.top = 0;
        bfst_child_send(&(p_term->o_term_ctxt), buf, len);
    }
}

static
void
bfst_handle_resize(
    struct bfst_body_ctxt const * const p_body_ctxt,
    XEvent *e)
{
    struct bfst_view_ctxt * const p_view_ctxt = bfst_view_list_find_win(p_body_ctxt, e->xconfigure.window);

    if (p_view_ctxt)
    {
        struct bfst_window * const p_window = p_view_ctxt->p_window;

        if (
            (
                e->xconfigure.width
                != (int)(p_window->i_width_pixels))
            || (
                e->xconfigure.height
                != (int)(p_window->i_height_pixels)))
        {
            /* Schedule a resize for later... */

            p_window->i_resize_width_pixels = e->xconfigure.width;

            p_window->i_resize_height_pixels = e->xconfigure.height;
        }
    }
}

static void (*handler[LASTEvent])(struct bfst_body_ctxt const * const p_body_ctxt, XEvent *) =
{
        [KeyPress] = bfst_handle_kpress,
        [ConfigureNotify] = bfst_handle_resize,
        [VisibilityNotify] = bfst_handle_visibility,
        [UnmapNotify] = bfst_handle_unmap,
        [Expose] = bfst_handle_expose,
        [FocusIn] = bfst_handle_focus,
        [FocusOut] = bfst_handle_focus,
        [EnterNotify] = bfst_handle_enterleave,
        [LeaveNotify] = bfst_handle_enterleave,
        [MotionNotify] = bfst_handle_bmotion,
        [ButtonPress] = bfst_handle_bpress,
        [ButtonRelease] = bfst_handle_brelease,
        [SelectionClear] = bfst_handle_selclear,
        [SelectionNotify] = bfst_handle_selnotify,
        [SelectionRequest] = bfst_handle_selrequest,
};

static
enum bfst_view_list_scan_result
bfst_body_check_for_dead(
    struct bfst_view_ctxt const * const p_view_ctxt,
    void * const h_context)
{
    struct bfst_tty_list * const p_tty_list = p_view_ctxt->p_tty_list;

    (void)(h_context);

    bfst_tty_list_check_for_dead(p_view_ctxt);

    if (0 == p_tty_list->i_tty_count)
    {
        return bfst_view_list_scan_result_remove;
    }
    else
    {
        return bfst_view_list_scan_result_continue;
    }
}

static int volatile g_event_count = 0;

#define BFST_SELECT_CHILD 1
#define BFST_SELECT_X11 2

static
unsigned int
bfst_body_select(
    struct bfst_body_ctxt const * const p_body_ctxt,
    struct timespec * p_timeout)
{
    unsigned int ui_select_mask;
    int i;
    unsigned int j;

    ui_select_mask = 0;

    struct bfst_view_list * const p_view_list = p_body_ctxt->p_view_list;

    bfst_view_list_scan(
        p_body_ctxt,
        &bfst_body_check_for_dead,
        NULL);

    if (p_view_list->i_view_count == 0)
    {
        /* kill application */
        exit(EXIT_SUCCESS);
    }

    (void)(p_timeout);

    for (j = 0; j < p_view_list->i_view_count; j++)
    {
        struct bfst_view * p_view;

        p_view = p_view_list->a_view_list[j]->p_view;

        for (i = 0; i < p_view->o_tty_list.i_tty_count; i++)
        {
            int i_read_result;

            i_read_result = bfst_child_read(&(p_view->o_tty_list.a_tty_list[i]->o_term_ctxt));

            if (0 < i_read_result)
            {
                ui_select_mask |= BFST_SELECT_CHILD;
            }
        }
    }

    {
        struct bfst_display const * const p_display = p_body_ctxt->p_display;

        while (XPending(p_display->dpy))
        {
            XEvent ev;

            XNextEvent(p_display->dpy, &ev);

            ui_select_mask |= BFST_SELECT_X11;

            if(handler[ev.type])
            {
                (handler[ev.type])(p_body_ctxt, &ev);
            }
        }
    }


    if (0 == ui_select_mask)
    {
        usleep(16ul * 1000ul);
    }

    return ui_select_mask;
}

void
bfst_body_run(
    struct bfst_body * const p_body)
{
    int xev, dodraw = 0;
    struct timespec drawtimeout, *tv = NULL, now, last, lastblink;
    unsigned int j;

    struct bfst_body_ctxt const * const p_body_ctxt = &p_body->o_body_ctxt;

    struct bfst_display const * const p_display = p_body_ctxt->p_display;

    struct bfst_view_list * const p_view_list = p_body_ctxt->p_view_list;

    bfst_view_list_add(p_body_ctxt, NULL);

    clock_gettime(CLOCK_MONOTONIC, &last);

    lastblink = last;

    for (xev = actionfps;;)
    {
        unsigned int ui_select_mask;

        ui_select_mask = bfst_body_select(p_body_ctxt, tv);

        dodraw = 0;

        if (BFST_SELECT_CHILD & ui_select_mask)
        {
            /* dodraw = 1; */
        }

        if (BFST_SELECT_X11 & ui_select_mask)
        {
            xev = actionfps;

            /* dodraw = 1; */
        }

        clock_gettime(CLOCK_MONOTONIC, &now);

        drawtimeout.tv_sec = 0;

        drawtimeout.tv_nsec =  (1000 * 1E6)/ xfps;

        tv = &drawtimeout;

        long deltatime;

        {
            deltatime = TIMEDIFF(now, lastblink);

            if (deltatime > blinktimeout)
            {
                for (j=0; j<p_view_list->i_view_count; j++)
                {
                    struct bfst_view_ctxt * const p_view_ctxt = p_view_list->a_view_list[j];

                    if (p_view_ctxt)
                    {
                        struct bfst_tty * const p_term = bfst_tab_get(p_view_ctxt);

                        if (p_term)
                        {
                            p_term->mode ^= MODE_BLINK;
                        }
                    }
                }

                lastblink = now;

                dodraw = 1;
            }
        }

        {
            deltatime = TIMEDIFF(now, last);

            if (deltatime > 1000 / (xev ? xfps : actionfps))
            {
                dodraw = 1;
            }
        }

        if (dodraw)
        {
            last = now;

            static unsigned long int i_last_resize_check = 0;

            unsigned long int i_time_now = bfst_time();

            char b_check_resize;

            b_check_resize = 0;

            if ((i_time_now - i_last_resize_check) > 300)
            {
                b_check_resize = 1;

                i_last_resize_check = i_time_now;
            }

            for (j=0; j<p_view_list->i_view_count; j++)
            {
                struct bfst_view * p_view;

                p_view = p_view_list->a_view_list[j]->p_view;

                if (p_view)
                {
                    if (b_check_resize && p_view->o_window.i_resize_width_pixels)
                    {
                        /* check for resize... */
                        bfst_view_resize(
                            p_view,
                            p_view->o_window.i_resize_width_pixels,
                            p_view->o_window.i_resize_height_pixels);

                        p_view->o_window.i_resize_width_pixels =
                            0;

                        p_view->o_window.i_resize_height_pixels =
                            0;
                    }

                    bfst_draw_all(&p_view->o_view_ctxt);
                }
            }

            XFlush(p_display->dpy);

            if (xev && !(BFST_SELECT_X11 & ui_select_mask))
            {
                xev--;
            }

            if (0 == ui_select_mask)
            {
                drawtimeout.tv_sec = 0;
                drawtimeout.tv_nsec = 50000000;
            }
        }
    }
}

int
x2col(
    struct bfst_tty * p_term,
    int x)
{
    struct bfst_view_ctxt const * const p_view_ctxt = p_term->o_term_ctxt.p_view_ctxt;

    struct bfst_font const * const p_font = p_view_ctxt->p_font;

    x -= p_view_ctxt->p_window->i_border;
    x /= p_font->i_width;

    return BFST_TOOLS_LIMIT(x, 0, ((int)p_term->col)-1);
}

int
y2row(
    struct bfst_tty * p_term,
    int y)
{
    struct bfst_view_ctxt const * const p_view_ctxt = p_term->o_term_ctxt.p_view_ctxt;

    struct bfst_font const * const p_font = p_view_ctxt->p_font;

    y -= p_view_ctxt->p_window->i_border;
    y /= p_font->i_height;

    return BFST_TOOLS_LIMIT(y, 0, ((int)p_term->row)-1);
}
