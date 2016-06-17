/* See LICENSE for license details. */

/*

Module: bfst_sel.c

*/

#include "bfst_os.h"

#include "bfst_body.h"

#define ISDELIM(u) (bfst_utf8_strchr(worddelimiters, u) != NULL)

enum selection_mode {
        SEL_IDLE = 0,
        SEL_EMPTY = 1,
        SEL_READY = 2
};

enum selection_type {
        SEL_REGULAR = 1,
        SEL_RECTANGULAR = 2
};

enum selection_snap {
        SNAP_WORD = 1,
        SNAP_LINE = 2
};

/*
 * word delimiter string
 *
 * More advanced example: " `'\"()[]{}"
 */
static char worddelimiters[] = " ";

/* selection timeouts (in milliseconds) */
static unsigned int doubleclicktimeout = 400;
static unsigned int tripleclicktimeout = 800;

static Atom o_sel_target = None;

static char * p_sel_primary = NULL;

static struct timespec tclick1;

static struct timespec tclick2;

void
bfst_sel_init(
    struct bfst_tty * const
        p_term)
{
    struct bfst_view_ctxt * const p_view_ctxt = p_term->o_term_ctxt.p_view_ctxt;

    struct bfst_body_ctxt * const p_body_ctxt = p_view_ctxt->p_body_ctxt;

    struct bfst_display * const p_display = p_body_ctxt->p_display;

    memset(&tclick1, 0, sizeof(tclick1));

    memset(&tclick2, 0, sizeof(tclick2));

    p_term->sel.mode = SEL_IDLE;

    p_term->sel.ob.x = -1;

    if (None == o_sel_target)
    {
        o_sel_target = XInternAtom(p_display->dpy, "UTF8_STRING", 0);

        if(o_sel_target == None)
        {
            o_sel_target = XA_STRING;
        }
    }
}

static
int
tlinelen(
    struct bfst_tty * p_term,
    int y)
{
    int i;

    if (y >= 0)
    {
        struct bfst_line * p_line;

        if (y < p_term->row)
        {
            p_line = p_term->p_cur->a_line + y;
        }
        else if (y - p_term->row < p_term->row)
        {
            p_line = p_term->p_other->a_line + y - p_term->row;
        }
        else
        {
            p_line = NULL;
        }

        if (p_line)
        {
            i = p_term->col;

            if (p_line->a_glyph[i - 1].mode & ATTR_WRAP)
            {
                return i;
            }

            while(i > 0 && p_line->a_glyph[i - 1].u == ' ')
            {
                --i;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        i = bfst_log_get_line_len(&(p_term->o_term_ctxt), p_term->log.i_count + y);
    }

    return i;
}

static
void
bfst_sel_get_char(
    struct bfst_tty * const
        p_term,
    int const
        i_line_index,
    int const
        i_column_index,
    struct bfst_glyph * const
        p_glyph)
{
    if (
        0
        <= i_line_index)
    {
        struct bfst_line * p_line;

        if (i_line_index < p_term->row)
        {
            p_line = p_term->p_cur->a_line + i_line_index;
        }
        else if (i_line_index - p_term->row < p_term->row)
        {
            p_line = p_term->p_other->a_line + i_line_index - p_term->row;
        }
        else
        {
            p_line = NULL;
        }

        if (p_line)
        {
            *(p_glyph) =
                p_line->a_glyph[i_column_index];
        }
    }
    else
    {
        bfst_log_get_char(
            &(
                p_term->o_term_ctxt),
            p_term->log.i_count + i_line_index,
            i_column_index,
            p_glyph);
    }
}

static
void
selsnap(struct bfst_tty* p_term, int *x, int *y, int direction)
{
    int newx, newy, xt, yt;
    char delim, prevdelim;
    struct bfst_glyph gp, prevgp;

    switch(p_term->sel.snap)
    {
        case SNAP_WORD:
            /*
             * Snap around if the word wraps around at the end or
             * beginning of a line.
             */
            bfst_sel_get_char(p_term, *y, *x, &prevgp);
            prevdelim = ISDELIM(prevgp.u);
            for(;;)
            {
                newx = *x + direction;
                newy = *y;
                if(!BFST_TOOLS_BETWEEN(newx, 0, p_term->col - 1)) {
                    newy += direction;
                    newx = (newx + p_term->col) % p_term->col;
                    if (!BFST_TOOLS_BETWEEN(newy, 0, p_term->row - 1))
                        break;

                    if(direction > 0)
                        yt = *y, xt = *x;
                    else
                        yt = newy, xt = newx;
                    if (yt >= 0)
                    {
                        if (yt < p_term->row)
                        {
                            if(!(p_term->p_cur->a_line[yt].a_glyph[xt].mode & ATTR_WRAP))
                                break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }

                if (newx >= tlinelen(p_term, newy))
                    break;

                bfst_sel_get_char(p_term, newy, newx, &gp);
                delim = ISDELIM(gp.u);
                if(!(gp.mode & ATTR_WDUMMY) && (delim != prevdelim
                        || (delim && gp.u != prevgp.u)))
                    break;

                *x = newx;
                *y = newy;
                prevgp = gp;
                prevdelim = delim;
            }
            break;
        case SNAP_LINE:
            /*
             * Snap around if the the previous line or the current one
             * has set ATTR_WRAP at its end. Then the whole next or
             * previous line will be selected.
             */
            *x = (direction < 0) ? 0 : p_term->col - 1;
            if(direction < 0)
            {
                for(; *y > 0; *y += direction)
                {
                    if (*y-1 >= 0)
                    {
                        if (*y-1 < ((int)p_term->row))
                        {
                            if(!(p_term->p_cur->a_line[*y-1].a_glyph[p_term->col-1].mode
                                    & ATTR_WRAP))
                            {
                                break;
                            }
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
            else if(direction > 0)
            {
                for(; *y < ((int)p_term->row)-1; *y += direction)
                {
                    if (*y >= 0)
                    {
                        if (*y < ((int)p_term->row))
                        {
                            if(!(p_term->p_cur->a_line[*y].a_glyph[p_term->col-1].mode
                                    & ATTR_WRAP))
                            {
                                break;
                            }
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
            break;
    }
}

char
bfst_sel_test(
    struct bfst_tty const * p_term,
    int x,
    int y)
{
    if (p_term->sel.ob.x == -1)
    {
        return 0;
    }

    if (p_term->sel.mode == SEL_EMPTY)
    {
        return 0;
    }

    if (x < 0 || x >= p_term->col)
    {
        return 0;
    }

    if(p_term->sel.type == SEL_RECTANGULAR)
    {
        return BFST_TOOLS_BETWEEN(y, p_term->sel.nb.y, p_term->sel.ne.y)
            && BFST_TOOLS_BETWEEN(x, p_term->sel.nb.x, p_term->sel.ne.x);
    }

    return BFST_TOOLS_BETWEEN(y, p_term->sel.nb.y, p_term->sel.ne.y)
        && (y != p_term->sel.nb.y || x >= p_term->sel.nb.x)
        && (y != p_term->sel.ne.y || x <= p_term->sel.ne.x);
}

static
void
bfst_sel_get(
    struct bfst_tty * const
        p_term)
{
    char * ptr;
    struct bfst_glyph gp;
    struct bfst_glyph last;
    int gpx;
    int y;
    int bufsize;
    int lastx;
    int linelen;

    if (p_term->sel.ob.x == -1)
    {
        return;
    }

    if (p_sel_primary)
    {
        free(p_sel_primary);
        p_sel_primary = NULL;
    }

    bufsize = (p_term->col+1) * (p_term->sel.ne.y-p_term->sel.nb.y+1) * BFST_UTF8_SIZE;
    ptr = p_sel_primary = bfst_malloc(bufsize);

    /* append every set & selected glyph to the selection */
    for (y = p_term->sel.nb.y; y <= p_term->sel.ne.y; y++)
    {
        linelen = tlinelen(p_term, y);

        if (p_term->sel.type == SEL_RECTANGULAR)
        {
            gpx = p_term->sel.nb.x;
            lastx = p_term->sel.ne.x;
        }
        else
        {
            gpx = p_term->sel.nb.y == y ? p_term->sel.nb.x : 0;
            lastx = (p_term->sel.ne.y == y) ? p_term->sel.ne.x : ((int)p_term->col)-1;
        }

        lastx = BFST_TOOLS_MIN(lastx, linelen-1);

        while(lastx >= gpx)
        {
            bfst_sel_get_char(p_term, y, lastx, &last);
            if (last.u == ' ')
            {
                lastx --;
            }
            else
            {
                break;
            }
        }

        for( ; gpx <= lastx; ++gpx)
        {
            bfst_sel_get_char(p_term, y, gpx, &gp);
            if(gp.mode & ATTR_WDUMMY)
                continue;

            ptr += bfst_utf8_encode(gp.u, ptr);
        }

        /*
         * Copy and pasting of line endings is inconsistent
         * in the inconsistent terminal and GUI world.
         * The best solution seems like to produce '\n' when
         * something is copied from st and convert '\n' to
         * '\r', when something to be pasted is received by
         * st.
         * FIXME: Fix the computer world.
         */
        if((y < p_term->sel.ne.y || lastx >= linelen) && !(last.mode & ATTR_WRAP))
            *ptr++ = '\n';
    }
    *ptr = 0;
}

void
bfst_sel_clear(
    struct bfst_tty * const
        p_term)
{
    if(p_term->sel.ob.x == -1)
        return;
    p_term->sel.mode = SEL_IDLE;
    p_term->sel.ob.x = -1;
}

static
void
selnormalize(struct bfst_tty* p_term)
{
        int i;

        if(p_term->sel.type == SEL_REGULAR && p_term->sel.ob.y != p_term->sel.oe.y) {
                p_term->sel.nb.x = p_term->sel.ob.y < p_term->sel.oe.y ? p_term->sel.ob.x : p_term->sel.oe.x;
                p_term->sel.ne.x = p_term->sel.ob.y < p_term->sel.oe.y ? p_term->sel.oe.x : p_term->sel.ob.x;
        } else {
                p_term->sel.nb.x = BFST_TOOLS_MIN(p_term->sel.ob.x, p_term->sel.oe.x);
                p_term->sel.ne.x = BFST_TOOLS_MAX(p_term->sel.ob.x, p_term->sel.oe.x);
        }
        p_term->sel.nb.y = BFST_TOOLS_MIN(p_term->sel.ob.y, p_term->sel.oe.y);
        p_term->sel.ne.y = BFST_TOOLS_MAX(p_term->sel.ob.y, p_term->sel.oe.y);

        selsnap(p_term, &p_term->sel.nb.x, &p_term->sel.nb.y, -1);
        selsnap(p_term, &p_term->sel.ne.x, &p_term->sel.ne.y, +1);

        /* expand selection over line breaks */
        if (p_term->sel.type == SEL_RECTANGULAR)
                return;
        i = tlinelen(p_term, p_term->sel.nb.y);
        if (i < p_term->sel.nb.x)
                p_term->sel.nb.x = i;
        if (tlinelen(p_term, p_term->sel.ne.y) <= p_term->sel.ne.x)
                p_term->sel.ne.x = p_term->col - 1;
}

void
selscroll(
    struct bfst_tty* p_term,
    int orig,
    int n)
{
    (void)(orig);

    if(p_term->sel.ob.x == -1)
        return;

#if 0
    if (BFST_TOOLS_BETWEEN(p_term->sel.ob.y, orig, p_term->bot) ||
        BFST_TOOLS_BETWEEN(p_term->sel.oe.y, orig, p_term->bot))
#endif
    {
        p_term->sel.ob.y += n;
        p_term->sel.oe.y += n;
#if 0
        if ((p_term->sel.ob.y += n) > p_term->bot || (p_term->sel.oe.y += n) < p_term->top)
        {
            bfst_sel_clear(p_term);
            return;
        }
#endif

        if (p_term->sel.type == SEL_RECTANGULAR)
        {
#if 0
            if(p_term->sel.ob.y < p_term->top)
                p_term->sel.ob.y = p_term->top;
#endif
            if(p_term->sel.oe.y > p_term->bot)
                p_term->sel.oe.y = p_term->bot;
        }
        else
        {
#if 0
            if(p_term->sel.ob.y < p_term->top)
            {
                p_term->sel.ob.y = p_term->top;
                p_term->sel.ob.x = 0;
            }
#endif

            if(p_term->sel.oe.y > p_term->bot)
            {
                p_term->sel.oe.y = p_term->bot;
                p_term->sel.oe.x = p_term->col;
            }
        }

        selnormalize(p_term);
    }
}

void
bfst_sel_paste(struct bfst_tty* p_term)
{
    struct bfst_view_ctxt * const p_view_ctxt = p_term->o_term_ctxt.p_view_ctxt;

    struct bfst_body_ctxt * const p_body_ctxt = p_term->o_term_ctxt.p_body_ctxt;

    struct bfst_window * const p_window = p_view_ctxt->p_window;

    struct bfst_display * const p_display = p_body_ctxt->p_display;

    XConvertSelection(
        p_display->dpy,
        XA_PRIMARY,
        o_sel_target,
        XA_PRIMARY,
        p_window->h_win_res,
        CurrentTime);

    XSync(
        p_display->dpy,
        False);
}

void
bfst_sel_click1(
    struct bfst_tty *
        p_term,
    int x,
    int y)
{
    struct timespec now;

    clock_gettime(CLOCK_MONOTONIC, &now);

    bfst_sel_clear(p_term);

    p_term->sel.mode = SEL_EMPTY;
    p_term->sel.type = SEL_REGULAR;
    p_term->sel.oe.x = p_term->sel.ob.x = x;
    p_term->sel.oe.y = p_term->sel.ob.y = y;

    /*
     * If the user clicks below predefined timeouts specific
     * snapping behaviour is exposed.
     */
    if(TIMEDIFF(now, tclick2) <= tripleclicktimeout) {
        p_term->sel.snap = SNAP_LINE;
    } else if(TIMEDIFF(now, tclick1) <= doubleclicktimeout) {
        p_term->sel.snap = SNAP_WORD;
    } else {
        p_term->sel.snap = 0;
    }

    selnormalize(p_term);

    if(p_term->sel.snap != 0)
        p_term->sel.mode = SEL_READY;

    tclick2 = tclick1;
    tclick1 = now;
}

static
void
getbuttoninfo(
    struct bfst_tty * p_term,
    XEvent * e)
{
    unsigned int state = e->xbutton.state;

    p_term->sel.alt = IS_SET(p_term, MODE_ALTSCREEN);

    p_term->sel.oe.x = x2col(p_term, e->xbutton.x);
    p_term->sel.oe.y = y2row(p_term, e->xbutton.y) + p_term->o_term_ctxt.p_view_ctxt->p_draw->top;
    selnormalize(p_term);

    if (state & ControlMask)
    {
        p_term->sel.type = SEL_RECTANGULAR;
    }
    else
    {
        p_term->sel.type = SEL_REGULAR;
    }
}

void
bfst_sel_release1(
    struct bfst_tty * p_term,
    XEvent* e)
{
    struct bfst_view_ctxt * const p_view_ctxt = p_term->o_term_ctxt.p_view_ctxt;

    struct bfst_window * const p_window = p_view_ctxt->p_window;

    struct bfst_body_ctxt * const p_body_ctxt = p_term->o_term_ctxt.p_body_ctxt;

    struct bfst_display * const p_display = p_body_ctxt->p_display;

    struct bfst_view_list * const p_view_list = p_body_ctxt->p_view_list;

    if(p_term->sel.mode == SEL_READY)
    {
        getbuttoninfo(p_term, e);

        bfst_sel_get(p_term);

        XSetSelectionOwner(p_display->dpy, XA_PRIMARY, p_window->h_win_res, e->xbutton.time);

        if (XGetSelectionOwner(p_display->dpy, XA_PRIMARY) != p_window->h_win_res)
        {
            bfst_sel_clear(p_term);
        }

        {
            unsigned int j;

            for (j = 0; j < p_view_list->i_view_count; j ++)
            {
                struct bfst_view_ctxt * p_view_iterator;

                p_view_iterator = p_view_list->a_view_list[j];

                if (p_view_iterator)
                {
                    int i;

                    for (i = 0; i < p_view_iterator->p_tty_list->i_tty_count; i++)
                    {
                        struct bfst_tty * p_term_iterator;

                        p_term_iterator = p_view_iterator->p_tty_list->a_tty_list[i];

                        if (p_term_iterator)
                        {
                            if (p_term_iterator != p_term)
                            {
                                bfst_sel_clear(p_term_iterator);
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        bfst_sel_clear(p_term);
    }
    p_term->sel.mode = SEL_IDLE;
}

void
bfst_sel_motion(
    struct bfst_tty * p_term,
    XEvent* e)
{
    if (!p_term->sel.mode)
    {
        return;
    }

    p_term->sel.mode = SEL_READY;
    getbuttoninfo(p_term, e);
}

char
bfst_sel_is_line_selected(
    struct bfst_tty * const
        p_term,
    int const
        y)
{
    return (p_term->sel.ob.x != -1 && BFST_TOOLS_BETWEEN(y, p_term->sel.nb.y, p_term->sel.ne.y));
}

void
bfst_sel_notify(
    struct bfst_view_ctxt const * const
        p_view_ctxt,
    XEvent * e)
{
    unsigned long int nitems, ofs, rem;
    int format;
    unsigned char *data, *last, *repl;
    Atom type;
    XSelectionEvent *xsev;

    struct bfst_window * const p_window = p_view_ctxt->p_window;

    struct bfst_body_ctxt * const p_body_ctxt = p_view_ctxt->p_body_ctxt;

    struct bfst_display * const p_display = p_body_ctxt->p_display;

    ofs = 0;
    xsev = &e->xselection;
    if (xsev->property == None)
        return;
    do {
        if(XGetWindowProperty(p_display->dpy, p_window->h_win_res, xsev->property, ofs,
                BUFSIZ/4, False, AnyPropertyType,
                &type, &format, &nitems, &rem,
                &data)) {
#if defined(BFST_CFG_DEBUG)
            bfst_msg("Clipboard allocation failed\n");
#endif /* #if defined(BFST_CFG_DEBUG) */
            return;
        }

        /*
         * As seen in bfst_sel_get:
         * Line endings are inconsistent in the terminal and GUI world
         * copy and pasting. When receiving some selection data,
         * replace all '\n' with '\r'.
         * FIXME: Fix the computer world.
         */
        repl = data;
        last = data + nitems * format / 8;
        while((repl = memchr(repl, '\n', last - repl))) {
            *repl++ = '\r';
        }

        struct bfst_tty * const p_term = bfst_tab_get(p_view_ctxt);

        if (p_term)
        {
            if (IS_SET(p_term, MODE_BRCKTPASTE))
            {
                bfst_child_write(&(p_term->o_term_ctxt), "\033[200~", 6);
            }

            bfst_child_send(&(p_term->o_term_ctxt), (char *)data, nitems * format / 8);

            if(IS_SET(p_term, MODE_BRCKTPASTE))
            {
                bfst_child_write(&(p_term->o_term_ctxt), "\033[201~", 6);
            }
        }

        XFree(data);

        /* number of 32-bit chunks returned */
        ofs += nitems * format / 32;

    } while(rem > 0);
}

void
bfst_sel_request(
    struct bfst_body_ctxt const * const p_body_ctxt,
    XEvent * e)
{
    XSelectionRequestEvent *xsre;
    XSelectionEvent xev;
    Atom xa_targets, string;
    char *seltext;

    struct bfst_display * const p_display = p_body_ctxt->p_display;

    xsre = (XSelectionRequestEvent *) e;
    xev.type = SelectionNotify;
    xev.requestor = xsre->requestor;
    xev.selection = xsre->selection;
    xev.target = xsre->target;
    xev.time = xsre->time;
    if (xsre->property == None)
        xsre->property = xsre->target;

    /* reject */
    xev.property = None;

    xa_targets = XInternAtom(p_display->dpy, "TARGETS", 0);

    if (xsre->target == xa_targets)
    {
        /* respond with the supported type */
        string = o_sel_target;
        XChangeProperty(xsre->display, xsre->requestor, xsre->property,
            XA_ATOM, 32, PropModeReplace,
            (unsigned char *) &string, 1);
        xev.property = xsre->property;
    }
    else if (
        xsre->target == o_sel_target ||
        xsre->target == XA_STRING)
    {
        /*
         * xith XA_STRING non ascii characters may be incorrect in the
         * requestor. It is not our problem, use utf8.
         */
        if(xsre->selection == XA_PRIMARY)
        {
            seltext = p_sel_primary;
        }
        else
        {
#if defined(BFST_CFG_DEBUG)
            bfst_msg(
                "Unhandled clipboard selection 0x%lx\n",
                xsre->selection);
#endif /* #if defined(BFST_CFG_DEBUG) */
            return;
        }

        if(seltext != NULL) {
            XChangeProperty(xsre->display, xsre->requestor,
                xsre->property, xsre->target,
                8, PropModeReplace,
                (unsigned char *)seltext, strlen(seltext));
            xev.property = xsre->property;
        }
    }

    /* all done, send a notification to the listener */
    if(!XSendEvent(xsre->display, xsre->requestor, True, 0, (XEvent *) &xev))
    {
#if defined(BFST_CFG_DEBUG)
        bfst_msg("Error sending SelectionNotify event\n");
#endif /* #if defined(BFST_CFG_DEBUG) */
    }

    /* Make sure the event is sent right away */
    XFlush(xsre->display);
    XSync(xsre->display, False);
}

unsigned short int
bfst_sel_calc_flags(
    struct bfst_tty const * const
        p_term,
    int const
        x,
    int const
        y)
{
    unsigned short int flags;

    flags = 0;

    if (bfst_sel_test(p_term, x, y))
    {
        /* test above and left */
        if (!bfst_sel_test(p_term, x-1, y-1))
        {
            flags |= GF_TOP_LEFT;
        }
        if (!bfst_sel_test(p_term, x, y-1))
        {
            flags |= GF_TOP | GF_TOP_LEFT | GF_TOP_RIGHT;
        }
        if (!bfst_sel_test(p_term, x+1, y-1))
        {
            flags |= GF_TOP_RIGHT;
        }
        if (!bfst_sel_test(p_term, x-1, y))
        {
            flags |= GF_LEFT | GF_TOP_LEFT | GF_BOTTOM_LEFT;
        }
        if (!bfst_sel_test(p_term, x+1, y))
        {
            flags |= GF_RIGHT | GF_TOP_RIGHT | GF_BOTTOM_RIGHT;
        }
        if (!bfst_sel_test(p_term, x-1, y+1))
        {
            flags |= GF_BOTTOM_LEFT;
        }
        if (!bfst_sel_test(p_term, x, y+1))
        {
            flags |= GF_BOTTOM | GF_BOTTOM_LEFT | GF_BOTTOM_RIGHT;
        }
        if (!bfst_sel_test(p_term, x+1, y+1))
        {
            flags |= GF_BOTTOM_RIGHT;
        }
    }

    return flags;
}
