/* See LICENSE for license details. */

/*

Module: bfst_tty.c

*/

#include <ctype.h>

#include <limits.h>

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <stdint.h>

#include <wchar.h>

#include <X11/Xlib.h>

#include "bfst_body.h"

#define ISCONTROLC0(c) (((c) <= 0x1f) || (c) == '\177')

#define ISCONTROLC1(c) (BFST_TOOLS_BETWEEN(c, 0x80, 0x9f))

#define ISCONTROL(c) (ISCONTROLC0(c) || ISCONTROLC1(c))

/*
 * appearance
 */

/* identification sequence returned in DA and DECID */
static char vtiden[] = "\033[?6c";

static unsigned int tabspaces = 8;

/* Globals */

static
void
tmoveto(Term* p_term, int x, int y)
{
    int miny, maxy;

    if(p_term->c.state & CURSOR_ORIGIN)
    {
        miny = p_term->top;
        maxy = p_term->bot;
    }
    else
    {
        miny = 0;
        maxy = p_term->row - 1;
    }
    p_term->c.state &= ~CURSOR_WRAPNEXT;
    p_term->c.x = BFST_TOOLS_LIMIT(x, 0, p_term->col-1);
    p_term->c.y = BFST_TOOLS_LIMIT(y, miny, maxy);
}

static
void
tcursor(Term* p_term, int mode)
{
    int alt = IS_SET(p_term, MODE_ALTSCREEN);

    if(mode == CURSOR_SAVE)
    {
        p_term->c_save[alt] = p_term->c;
    }
    else if(mode == CURSOR_LOAD)
    {
        p_term->c = p_term->c_save[alt];
        tmoveto(p_term, p_term->c_save[alt].x, p_term->c_save[alt].y);
    }
}

static
void
tclearregion(
    struct bfst_tty * const
        p_term,
    int x1,
    int y1,
    int x2,
    int y2)
{
    int x, y, temp;
    char b_changed;
    struct bfst_line * lp;
    struct bfst_glyph * gp;
    struct bfst_glyph o_empty_char;

    if (x1 > x2)
    {
        temp = x1;
        x1 = x2;
        x2 = temp;
    }

    if (y1 > y2)
    {
        temp = y1;
        y1 = y2;
        y2 = temp;
    }

    BFST_TOOLS_LIMIT(x1, 0, p_term->col-1);
    BFST_TOOLS_LIMIT(x2, 0, p_term->col-1);
    BFST_TOOLS_LIMIT(y1, 0, p_term->row-1);
    BFST_TOOLS_LIMIT(y2, 0, p_term->row-1);

    o_empty_char.fg = p_term->c.attr.fg;
    o_empty_char.bg = p_term->c.attr.bg;
    o_empty_char.mode = 0;
    o_empty_char.flags = 0;
    o_empty_char.u = ' ';

    for(y = y1; y <= y2; y++)
    {
        lp = p_term->p_cur->a_line + y;

        b_changed = 0;

        for(x = x1; x <= x2; x++)
        {
            if(bfst_sel_test(p_term, x, y))
            {
                bfst_sel_clear(p_term);
            }

            gp = &lp->a_glyph[x];

            if (memcmp(gp, &o_empty_char, sizeof(o_empty_char)))
            {
                *gp = o_empty_char;

                b_changed = 1;
            }
        }

        if (b_changed)
        {
            lp->i_unique = bfst_unique_pick(p_term->o_term_ctxt.p_body_ctxt);
        }
    }
}

static
void
tinitscreenptr(
    struct bfst_tty * const
        p_term)
{
    p_term->p_cur = (p_term->mode & MODE_ALTSCREEN) ? &(p_term->o_alt) : &(p_term->o_pri);

    p_term->p_other = (p_term->mode & MODE_ALTSCREEN) ? &(p_term->o_pri) : &(p_term->o_alt);
}

static
void
tswapscreen(Term* p_term)
{
    p_term->mode ^= MODE_ALTSCREEN;

    tinitscreenptr(p_term);
}

static
void
treset(Term* p_term) {
        int i;

        p_term->c = (TCursor){{
                .mode = ATTR_NULL,
                .fg = p_term->o_term_ctxt.p_view_ctxt->p_color->i_fg_index,
                .bg = p_term->o_term_ctxt.p_view_ctxt->p_color->i_bg_index,
                .flags = 0
        }, .x = 0, .y = 0, .state = CURSOR_DEFAULT};

        memset(p_term->tabs, 0, p_term->col * sizeof(*p_term->tabs));
        for(i = tabspaces; i < p_term->col; i += tabspaces)
                p_term->tabs[i] = 1;
        p_term->top = 0;
        p_term->bot = p_term->row - 1;
        p_term->mode = MODE_WRAP;
        tinitscreenptr(p_term);
        memset(p_term->trantbl, CS_USA, sizeof(p_term->trantbl));
        p_term->charset = 0;

        for(i = 0; i < 2; i++) {
                tmoveto(p_term, 0, 0);
                tcursor(p_term, CURSOR_SAVE);
                tclearregion(p_term, 0, 0, p_term->col-1, p_term->row-1);
                tswapscreen(p_term);
        }
}

Term *
tnew(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    struct bfst_window const * const p_window = p_view_ctxt->p_window;

    Term * p_term;

    p_term = (Term *)(bfst_malloc(sizeof(Term)));

    memset(p_term, 0x00, sizeof(Term));

    p_term->o_term_ctxt.p_term = p_term;

    p_term->o_term_ctxt.p_view_ctxt = (struct bfst_view_ctxt *)(p_view_ctxt);

    p_term->o_term_ctxt.p_body_ctxt = p_view_ctxt->p_body_ctxt;

    p_term->o_term_ctxt.p_log = &p_term->log;

    p_term->o_term_ctxt.p_sel = &p_term->sel;

    p_term->o_term_ctxt.p_child = &p_term->child;

    p_term->c.shape = 0;
    p_term->c.attr.fg = p_view_ctxt->p_color->i_fg_index;
    p_term->c.attr.bg = p_view_ctxt->p_color->i_bg_index;

    tinitscreenptr(p_term);

    tresize(p_term, p_window->i_tty_cols, p_window->i_tty_rows);

    treset(p_term);

    bfst_log_init(&(p_term->o_term_ctxt));

    return p_term;
}

void
tdelete(
    Term * p_term)
{
    int i;

    struct bfst_line * lp;

    bfst_log_cleanup(&(p_term->o_term_ctxt));

    if (p_term->o_pri.a_line)
    {
        for (i = 0; i < p_term->row; i ++)
        {
            lp = p_term->o_pri.a_line + i;

            if (lp->a_glyph)
            {
                free(lp->a_glyph);
                lp->a_glyph = NULL;
            }
        }

        free(p_term->o_pri.a_line);
    }

    if (p_term->o_alt.a_line)
    {
        for (i = 0; i < p_term->row; i ++)
        {
            lp = p_term->o_alt.a_line + i;

            if (lp->a_glyph)
            {
                free(lp->a_glyph);
                lp->a_glyph = NULL;
            }
        }

        free(p_term->o_alt.a_line);
    }

    free(p_term->tabs);
    free(p_term);
}

static
void
tscrolldown(Term* p_term, int orig, int n)
{
    int i;
    struct bfst_line temp;

    BFST_TOOLS_LIMIT(n, 0, p_term->bot-orig+1);

    for(i = p_term->bot; i >= orig+n; i--)
    {
        temp = p_term->p_cur->a_line[i];

        p_term->p_cur->a_line[i] = p_term->p_cur->a_line[i-n];

        p_term->p_cur->a_line[i-n] = temp;
    }

    tclearregion(p_term, 0, orig, p_term->col-1, orig+n-1);

    selscroll(p_term, orig, n);
}

static
void
tscrollup(Term* p_term, int orig, int n)
{
        int i;
        struct bfst_line temp;

        BFST_TOOLS_LIMIT(n, 0, p_term->bot-orig+1);

        /* felix: store old lines into backlog... */

        if ((orig == p_term->top) && !(p_term->mode & MODE_ALTSCREEN))
        {
            for (i = orig; i < orig+n; i ++)
            {
                bfst_log_scroll(&(p_term->o_term_ctxt), i);
            }
        }

        for(i = orig; i <= p_term->bot-n; i++) {
                temp = p_term->p_cur->a_line[i];
                p_term->p_cur->a_line[i] = p_term->p_cur->a_line[i+n];
                p_term->p_cur->a_line[i+n] = temp;
        }

        tclearregion(p_term, 0, p_term->bot-n+1, p_term->col-1, p_term->bot);

        selscroll(p_term, orig, -n);
}

static
void
tnewline(Term* p_term, int first_col)
{
        int y = p_term->c.y;

        if(y == p_term->bot) {
                tscrollup(p_term, p_term->top, 1);
        } else {
                y++;
        }
        tmoveto(p_term, first_col ? 0 : p_term->c.x, y);
}

static
void
csiparse(Term* p_term) {
        char *p = p_term->csiescseq.buf, *np;
        long int v;

        p_term->csiescseq.narg = 0;
        if(*p == '?') {
                p_term->csiescseq.priv = 1;
                p++;
        }

        p_term->csiescseq.buf[p_term->csiescseq.len] = '\0';
        while(p < p_term->csiescseq.buf+p_term->csiescseq.len) {
                np = NULL;
                v = strtol(p, &np, 10);
                if(np == p)
                        v = 0;
                if(v == LONG_MAX || v == LONG_MIN)
                        v = -1;
                p_term->csiescseq.arg[p_term->csiescseq.narg++] = v;
                p = np;
                if(*p != ';' || p_term->csiescseq.narg == ESC_ARG_SIZ)
                        break;
                p++;
        }
        p_term->csiescseq.mode[0] = *p++;
        p_term->csiescseq.mode[1] = (p < p_term->csiescseq.buf+p_term->csiescseq.len) ? *p : '\0';
}

/* for absolute user moves, when decom is set */
static
void
tmoveato(Term* p_term, int x, int y) {
        tmoveto(p_term, x, y + ((p_term->c.state & CURSOR_ORIGIN) ? p_term->top: 0));
}

static
void
tsetchar(
    struct bfst_tty * const
        p_term,
    unsigned long int u,
    struct bfst_glyph const * const attr,
    int const x,
    int const y)
{
    static char *vt100_0[62] =
    { /* 0x41 - 0x7e */
        "↑", "↓", "→", "←", "█", "▚", "☃", /* A - G */
        0, 0, 0, 0, 0, 0, 0, 0, /* H - O */
        0, 0, 0, 0, 0, 0, 0, 0, /* P - W */
        0, 0, 0, 0, 0, 0, 0, " ", /* X - _ */
        "◆", "▒", "␉", "␌", "␍", "␊", "°", "±", /* ` - g */
        "␤", "␋", "┘", "┐", "┌", "└", "┼", "⎺", /* h - o */
        "⎻", "─", "⎼", "⎽", "├", "┤", "┴", "┬", /* p - w */
        "│", "≤", "≥", "π", "≠", "£", "·", /* x - ~ */
    };

    /*
     * The table is proudly stolen from rxvt.
     */
    if ((p_term->trantbl[p_term->charset] == CS_GRAPHIC0) &&
        BFST_TOOLS_BETWEEN(u, 0x41, 0x7e) &&
        vt100_0[u - 0x41])
    {
        bfst_utf8_decode(vt100_0[u - 0x41], &u, BFST_UTF8_SIZE);
    }

    char b_changed;

    struct bfst_glyph o_modified_char;

    struct bfst_line * lp;

    b_changed = 0;

    lp = p_term->p_cur->a_line + y;

    if (lp->a_glyph[x].mode & ATTR_WIDE)
    {
        if (x+1 < p_term->col)
        {
            o_modified_char = lp->a_glyph[x + 1];

            o_modified_char.u = ' ';

            o_modified_char.mode &= ~ATTR_WDUMMY;

            if (memcmp(&lp->a_glyph[x + 1], &o_modified_char, sizeof(o_modified_char)))
            {
                lp->a_glyph[x+1] = o_modified_char;

                b_changed = 1;
            }
        }
    }
    else if (lp->a_glyph[x].mode & ATTR_WDUMMY)
    {
        if (x > 0)
        {
            o_modified_char = lp->a_glyph[x - 1];

            o_modified_char.u = ' ';

            o_modified_char.mode &= ~ATTR_WIDE;

            if (memcmp(&lp->a_glyph[x - 1], &o_modified_char, sizeof(o_modified_char)))
            {
                lp->a_glyph[x - 1] = o_modified_char;

                b_changed = 1;
            }
        }
    }

    o_modified_char = *attr;

    o_modified_char.u = u;

    if (memcmp(&lp->a_glyph[x], &o_modified_char, sizeof(o_modified_char)))
    {
        lp->a_glyph[x] = o_modified_char;

        b_changed = 1;
    }

    if (b_changed)
    {
        lp->i_unique = bfst_unique_pick(p_term->o_term_ctxt.p_body_ctxt);
    }
}

static
void
tdeletechar(Term* p_term, int n)
{
    int dst, src, size;
    struct bfst_line * lp;
    struct bfst_glyph *gp;

    BFST_TOOLS_LIMIT(n, 0, p_term->col - p_term->c.x);

    dst = p_term->c.x;
    src = p_term->c.x + n;
    size = p_term->col - src;
    lp = p_term->p_cur->a_line + p_term->c.y;
    gp = lp->a_glyph;

    {
        char b_changed;

        b_changed = 0;

        int i;

        for (i = 0; i < size; i++)
        {
            if (memcmp(&gp[dst + i], &gp[src + i], sizeof(struct bfst_glyph)))
            {
                gp[dst + i] = gp[src + i];

                b_changed = 1;
            }
        }

        if (b_changed)
        {
            lp->i_unique = bfst_unique_pick(p_term->o_term_ctxt.p_body_ctxt);
        }
    }

    tclearregion(p_term, p_term->col-n, p_term->c.y, p_term->col-1, p_term->c.y);
}

static
void
tinsertblank(Term* p_term, int n)
{
    int dst, src, size;
    struct bfst_line * lp;
    struct bfst_glyph * gp;

    BFST_TOOLS_LIMIT(n, 0, p_term->col - p_term->c.x);

    dst = p_term->c.x + n;
    src = p_term->c.x;
    size = p_term->col - dst;
    lp = p_term->p_cur->a_line + p_term->c.y;
    gp = lp->a_glyph;

    {
        char b_changed;

        b_changed = 0;

        int i;

        for (i = size - 1 ; i >= 0; i--)
        {
            if (memcmp(gp + dst + i, gp + src + i, sizeof(struct bfst_glyph)))
            {
                gp[dst + i] = gp[src + i];

                b_changed = 1;
            }
        }

        if (b_changed)
        {
            lp->i_unique = bfst_unique_pick(p_term->o_term_ctxt.p_body_ctxt);
        }
    }

    tclearregion(p_term, src, p_term->c.y, dst - 1, p_term->c.y);
}

static
void
tinsertblankline(Term* p_term, int n)
{
    if (BFST_TOOLS_BETWEEN(p_term->c.y, p_term->top, p_term->bot))
    {
        tscrolldown(p_term, p_term->c.y, n);
    }
}

static
void
tdeleteline(Term* p_term, int n)
{
    if (BFST_TOOLS_BETWEEN(p_term->c.y, p_term->top, p_term->bot))
    {
        tscrollup(p_term, p_term->c.y, n);
    }
}

static
unsigned char
tdefcolor(
    struct bfst_tty * const p_term,
    int *attr,
    int *npar,
    int l)
{
    unsigned char idx = 255;
    unsigned int r, g, b;

    (void)(p_term);

    switch (attr[*npar + 1])
    {
        case 2: /* direct color in RGB space */
                if (*npar + 4 >= l)
                {
#if defined(BFST_CFG_DEBUG)
                        bfst_msg(
                                "erresc(38): Incorrect number of parameters (%d)\n",
                                *npar);
#endif /* #if defined(BFST_CFG_DEBUG) */
                        break;
                }
                r = attr[*npar + 2];
                g = attr[*npar + 3];
                b = attr[*npar + 4];
                *npar += 4;

                /* Limit to 0-255 range */
                if (r > 255) r = 255;
                if (g > 255) g = 255;
                if (b > 255) b = 255;

                /* Try to find matching color */
                idx = 16 + (((r * 6)/256)*36) + (((g * 6)/256)*6) + (((b * 6)/256)*1);
                break;
        case 5: /* indexed color */
                if (*npar + 2 >= l)
                {
#if defined(BFST_CFG_DEBUG)
                        bfst_msg(
                                "erresc(38): Incorrect number of parameters (%d)\n",
                                *npar);
#endif /* #if defined(BFST_CFG_DEBUG) */
                        break;
                }
                *npar += 2;
                if(!BFST_TOOLS_BETWEEN(attr[*npar], 0, 255))
                {
#if defined(BFST_CFG_DEBUG)
                        bfst_msg("erresc: bad fgcolor %d\n", attr[*npar]);
#endif /* #if defined(BFST_CFG_DEBUG) */
                }
                else
                {
                        idx = attr[*npar];
                }
                break;
        case 0: /* implemented defined (only foreground) */
        case 1: /* transparent */
        case 3: /* direct color in CMY space */
        case 4: /* direct color in CMYK space */
        default:
#if defined(BFST_CFG_DEBUG)
                bfst_msg(
                        "erresc(38): gfx attr %d unknown\n", attr[*npar]);
#endif /* #if defined(BFST_CFG_DEBUG) */
                break;
    }

    return idx;
}

#if defined(BFST_CFG_DEBUG)
static
void
csidump(Term* p_term)
{
        int i;
        unsigned int c;

        bfst_msg("ESC[");
        for(i = 0; i < p_term->csiescseq.len; i++) {
                c = p_term->csiescseq.buf[i] & 0xff;
                if(isprint(c)) {
                        bfst_msg("%c", c);
                } else if(c == '\n') {
                        bfst_msg("(\\n)");
                } else if(c == '\r') {
                        bfst_msg("(\\r)");
                } else if(c == 0x1b) {
                        bfst_msg("(\\e)");
                } else {
                        bfst_msg("(%02x)", c);
                }
        }
        bfst_msg("%c", '\n');
}
#endif /* #if defined(BFST_CFG_DEBUG) */


static
void
tsetattr(Term* p_term, int *attr, int l) {
        int i;
        int32_t idx;

        for(i = 0; i < l; i++) {
                switch(attr[i]) {
                case 0:
                        p_term->c.attr.mode &= ~(
                                ATTR_BOLD       |
                                ATTR_FAINT      |
                                ATTR_ITALIC     |
                                ATTR_UNDERLINE  |
                                ATTR_BLINK      |
                                ATTR_REVERSE    |
                                ATTR_INVISIBLE  |
                                ATTR_STRUCK     );
                        p_term->c.attr.fg = p_term->o_term_ctxt.p_view_ctxt->p_color->i_fg_index;
                        p_term->c.attr.bg = p_term->o_term_ctxt.p_view_ctxt->p_color->i_bg_index;
                        break;
                case 1:
                        p_term->c.attr.mode |= ATTR_BOLD;
                        break;
                case 2:
                        p_term->c.attr.mode |= ATTR_FAINT;
                        break;
                case 3:
                        p_term->c.attr.mode |= ATTR_ITALIC;
                        break;
                case 4:
                        p_term->c.attr.mode |= ATTR_UNDERLINE;
                        break;
                case 5: /* slow blink */
                        /* FALLTHROUGH */
                case 6: /* rapid blink */
                        p_term->c.attr.mode |= ATTR_BLINK;
                        break;
                case 7:
                        p_term->c.attr.mode |= ATTR_REVERSE;
                        break;
                case 8:
                        p_term->c.attr.mode |= ATTR_INVISIBLE;
                        break;
                case 9:
                        p_term->c.attr.mode |= ATTR_STRUCK;
                        break;
                case 22:
                        p_term->c.attr.mode &= ~(ATTR_BOLD | ATTR_FAINT);
                        break;
                case 23:
                        p_term->c.attr.mode &= ~ATTR_ITALIC;
                        break;
                case 24:
                        p_term->c.attr.mode &= ~ATTR_UNDERLINE;
                        break;
                case 25:
                        p_term->c.attr.mode &= ~ATTR_BLINK;
                        break;
                case 27:
                        p_term->c.attr.mode &= ~ATTR_REVERSE;
                        break;
                case 28:
                        p_term->c.attr.mode &= ~ATTR_INVISIBLE;
                        break;
                case 29:
                        p_term->c.attr.mode &= ~ATTR_STRUCK;
                        break;
                case 38:
                        if ((idx = tdefcolor(p_term, attr, &i, l)) >= 0)
                                p_term->c.attr.fg = idx;
                        break;
                case 39:
                        p_term->c.attr.fg = p_term->o_term_ctxt.p_view_ctxt->p_color->i_fg_index;
                        break;
                case 48:
                        if ((idx = tdefcolor(p_term, attr, &i, l)) >= 0)
                                p_term->c.attr.bg = idx;
                        break;
                case 49:
                        p_term->c.attr.bg = p_term->o_term_ctxt.p_view_ctxt->p_color->i_bg_index;
                        break;
                default:
                        if(BFST_TOOLS_BETWEEN(attr[i], 30, 37)) {
                                p_term->c.attr.fg = attr[i] - 30;
                        } else if(BFST_TOOLS_BETWEEN(attr[i], 40, 47)) {
                                p_term->c.attr.bg = attr[i] - 40;
                        } else if(BFST_TOOLS_BETWEEN(attr[i], 90, 97)) {
                                p_term->c.attr.fg = attr[i] - 90 + 8;
                        } else if(BFST_TOOLS_BETWEEN(attr[i], 100, 107)) {
                                p_term->c.attr.bg = attr[i] - 100 + 8;
                        } else {
#if defined(BFST_CFG_DEBUG)
                                bfst_msg(
                                        "erresc(default): gfx attr %d unknown\n",
                                        attr[i]);
                                csidump(p_term);
#endif /* #if defined(BFST_CFG_DEBUG) */
                        }
                        break;
                }
        }
}

static
void
tsetscroll(Term* p_term, int t, int b) {
        int temp;

        BFST_TOOLS_LIMIT(t, 0, p_term->row-1);
        BFST_TOOLS_LIMIT(b, 0, p_term->row-1);
        if(t > b) {
                temp = t;
                t = b;
                b = temp;
        }
        p_term->top = t;
        p_term->bot = b;
}

static
void
tsetmode(Term* p_term, char priv, char set, int *args, int narg)
{
        int *lim;
        char alt;

        for(lim = args + narg; args < lim; ++args) {
                if(priv) {
                        switch(*args) {
                        case 1: /* DECCKM -- Cursor key */
                                MODBIT(p_term->mode, set, MODE_APPCURSOR);
                                break;
                        case 5: /* DECSCNM -- Reverse video */
                                MODBIT(p_term->mode, set, MODE_REVERSE);
                                break;
                        case 6: /* DECOM -- Origin */
                                MODBIT(p_term->c.state, set, CURSOR_ORIGIN);
                                tmoveato(p_term, 0, 0);
                                break;
                        case 7: /* DECAWM -- Auto wrap */
                                MODBIT(p_term->mode, set, MODE_WRAP);
                                break;
                        case 0:  /* Error (IGNORED) */
                        case 2:  /* DECANM -- ANSI/VT52 (IGNORED) */
                        case 3:  /* DECCOLM -- Column  (IGNORED) */
                        case 4:  /* DECSCLM -- Scroll (IGNORED) */
                        case 8:  /* DECARM -- Auto repeat (IGNORED) */
                        case 18: /* DECPFF -- Printer feed (IGNORED) */
                        case 19: /* DECPEX -- Printer extent (IGNORED) */
                        case 42: /* DECNRCM -- National characters (IGNORED) */
                        case 12: /* att610 -- Start blinking cursor (IGNORED) */
                                break;
                        case 25: /* DECTCEM -- Text Cursor Enable Mode */
                                MODBIT(p_term->mode, !set, MODE_HIDE);
                                break;
                        case 9:    /* X10 mouse compatibility mode */
                                MODBIT(p_term->mode, 0, MODE_MOUSE);
                                MODBIT(p_term->mode, set, MODE_MOUSEX10);
                                break;
                        case 1000: /* 1000: report button press */
                                MODBIT(p_term->mode, 0, MODE_MOUSE);
                                MODBIT(p_term->mode, set, MODE_MOUSEBTN);
                                break;
                        case 1002: /* 1002: report motion on button press */
                                MODBIT(p_term->mode, 0, MODE_MOUSE);
                                MODBIT(p_term->mode, set, MODE_MOUSEMOTION);
                                break;
                        case 1003: /* 1003: enable all mouse motions */
                                MODBIT(p_term->mode, 0, MODE_MOUSE);
                                MODBIT(p_term->mode, set, MODE_MOUSEMANY);
                                break;
                        case 1004: /* 1004: send focus events to tty */
                                MODBIT(p_term->mode, set, MODE_FOCUS);
                                break;
                        case 1006: /* 1006: extended reporting mode */
                                MODBIT(p_term->mode, set, MODE_MOUSESGR);
                                break;
                        case 1034:
                                MODBIT(p_term->mode, set, MODE_8BIT);
                                break;
                        case 1049: /* swap screen & set/restore cursor as xterm */
                                tcursor(p_term, (set) ? CURSOR_SAVE : CURSOR_LOAD);
                                /* FALLTHROUGH */
                        case 47: /* swap screen */
                        case 1047:
                                alt = IS_SET(p_term, MODE_ALTSCREEN);
#if 0
                                if(alt) {
                                        tclearregion(p_term, 0, 0, p_term->col-1,
                                                        p_term->row-1);
                                }
#endif
                                if(set ^ alt) /* set is always 1 or 0 */
                                        tswapscreen(p_term);
                                if(*args != 1049)
                                        break;
                                /* FALLTHROUGH */
                        case 1048:
                                tcursor(p_term, (set) ? CURSOR_SAVE : CURSOR_LOAD);
                                break;
                        case 2004: /* 2004: bracketed paste mode */
                                MODBIT(p_term->mode, set, MODE_BRCKTPASTE);
                                break;
                        /* Not implemented mouse modes. See comments there. */
                        case 1001: /* mouse highlight mode; can hang the
                                      terminal by design when implemented. */
                        case 1005: /* UTF-8 mouse mode; will confuse
                                      applications not supporting UTF-8
                                      and luit. */
                        case 1015: /* urxvt mangled mouse mode; incompatible
                                      and can be mistaken for other control
                                      codes. */
                        default:
#if defined(BFST_CFG_DEBUG)
                                bfst_msg(
                                        "erresc: unknown private set/reset mode %d\n",
                                        *args);
#endif /* #if defined(BFST_CFG_DEBUG) */
                                break;
                        }
                } else {
                        switch(*args) {
                        case 0:  /* Error (IGNORED) */
                                break;
                        case 2:  /* KAM -- keyboard action */
                                MODBIT(p_term->mode, set, MODE_KBDLOCK);
                                break;
                        case 4:  /* IRM -- Insertion-replacement */
                                MODBIT(p_term->mode, set, MODE_INSERT);
                                break;
                        case 12: /* SRM -- Send/Receive */
                                MODBIT(p_term->mode, !set, MODE_ECHO);
                                break;
                        case 20: /* LNM -- Linefeed/new line */
                                MODBIT(p_term->mode, set, MODE_CRLF);
                                break;
                        default:
#if defined(BFST_CFG_DEBUG)
                                bfst_msg(
                                        "erresc: unknown set/reset mode %d\n",
                                        *args);
#endif /* #if defined(BFST_CFG_DEBUG) */
                                break;
                        }
                }
        }
}

static
void
tdumpsel(Term* p_term) {
    (void)(p_term);
}

static
void
tdumpline(Term* p_term, int n) {
    (void)(p_term);
    (void)(n);
}

static
void
tdump(Term* p_term) {
    (void)(p_term);
}

static
void
tputtab(Term* p_term, int n) {
        int x = p_term->c.x;

        if(n > 0) {
                while(x < p_term->col && n--)
                        for(++x; x < p_term->col && !p_term->tabs[x]; ++x)
                                /* nothing */ ;
        } else if(n < 0) {
                while(x > 0 && n++)
                        for(--x; x > 0 && !p_term->tabs[x]; --x)
                                /* nothing */ ;
        }
        p_term->c.x = BFST_TOOLS_LIMIT(x, 0, p_term->col-1);
}

static
void
csihandle(Term* p_term)
{
        char buf[40];
        int len;

        switch(p_term->csiescseq.mode[0]) {
        default:
        unknown:
#if defined(BFST_CFG_DEBUG)
                bfst_msg("erresc: unknown csi ");
                csidump(p_term);
                /* bfst_die(); */
#endif /* #if defined(BFST_CFG_DEBUG) */
                break;
        case '@': /* ICH -- Insert <n> blank char */
                BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[0], 1);
                tinsertblank(p_term, p_term->csiescseq.arg[0]);
                break;
        case 'A': /* CUU -- Cursor <n> Up */
                BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[0], 1);
                tmoveto(p_term, p_term->c.x, p_term->c.y-p_term->csiescseq.arg[0]);
                break;
        case 'B': /* CUD -- Cursor <n> Down */
        case 'e': /* VPR --Cursor <n> Down */
                BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[0], 1);
                tmoveto(p_term, p_term->c.x, p_term->c.y+p_term->csiescseq.arg[0]);
                break;
        case 'i': /* MC -- Media Copy */
                switch(p_term->csiescseq.arg[0]) {
                case 0:
                        tdump(p_term);
                        break;
                case 1:
                        tdumpline(p_term, p_term->c.y);
                        break;
                case 2:
                        tdumpsel(p_term);
                        break;
                case 4:
                        p_term->mode &= ~MODE_PRINT;
                        break;
                case 5:
                        p_term->mode |= MODE_PRINT;
                        break;
                }
                break;
        case 'c': /* DA -- Device Attributes */
                if(p_term->csiescseq.arg[0] == 0)
                        bfst_child_write(&p_term->o_term_ctxt, vtiden, sizeof(vtiden) - 1);
                break;
        case 'C': /* CUF -- Cursor <n> Forward */
        case 'a': /* HPR -- Cursor <n> Forward */
                BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[0], 1);
                tmoveto(p_term, p_term->c.x+p_term->csiescseq.arg[0], p_term->c.y);
                break;
        case 'D': /* CUB -- Cursor <n> Backward */
                BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[0], 1);
                tmoveto(p_term, p_term->c.x-p_term->csiescseq.arg[0], p_term->c.y);
                break;
        case 'E': /* CNL -- Cursor <n> Down and first col */
                BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[0], 1);
                tmoveto(p_term, 0, p_term->c.y+p_term->csiescseq.arg[0]);
                break;
        case 'F': /* CPL -- Cursor <n> Up and first col */
                BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[0], 1);
                tmoveto(p_term, 0, p_term->c.y-p_term->csiescseq.arg[0]);
                break;
        case 'g': /* TBC -- Tabulation clear */
                switch(p_term->csiescseq.arg[0]) {
                case 0: /* clear current tab stop */
                        p_term->tabs[p_term->c.x] = 0;
                        break;
                case 3: /* clear all the tabs */
                        memset(p_term->tabs, 0, p_term->col * sizeof(*p_term->tabs));
                        break;
                default:
                        goto unknown;
                }
                break;
        case 'G': /* CHA -- Move to <col> */
        case '`': /* HPA */
                BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[0], 1);
                tmoveto(p_term, p_term->csiescseq.arg[0]-1, p_term->c.y);
                break;
        case 'H': /* CUP -- Move to <row> <col> */
        case 'f': /* HVP */
                BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[0], 1);
                BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[1], 1);
                tmoveato(p_term, p_term->csiescseq.arg[1]-1, p_term->csiescseq.arg[0]-1);
                break;
        case 'I': /* CHT -- Cursor Forward Tabulation <n> tab stops */
                BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[0], 1);
                tputtab(p_term, p_term->csiescseq.arg[0]);
                break;
        case 'J': /* ED -- Clear screen */
                /* bfst_sel_clear(p_term); */
                switch(p_term->csiescseq.arg[0]) {
                case 0: /* below */
                        tclearregion(p_term, p_term->c.x, p_term->c.y, p_term->col-1, p_term->c.y);
                        if(p_term->c.y < p_term->row-1) {
                                tclearregion(p_term, 0, p_term->c.y+1, p_term->col-1,
                                                p_term->row-1);
                        }
                        break;
                case 1: /* above */
                        if(p_term->c.y > 1)
                                tclearregion(p_term, 0, 0, p_term->col-1, p_term->c.y-1);
                        tclearregion(p_term, 0, p_term->c.y, p_term->c.x, p_term->c.y);
                        break;
                case 2: /* all */
                        tclearregion(p_term, 0, 0, p_term->col-1, p_term->row-1);
                        break;
                default:
                        goto unknown;
                }
                break;
        case 'K': /* EL -- Clear line */
                switch(p_term->csiescseq.arg[0]) {
                case 0: /* right */
                        tclearregion(p_term, p_term->c.x, p_term->c.y, p_term->col-1,
                                        p_term->c.y);
                        break;
                case 1: /* left */
                        tclearregion(p_term, 0, p_term->c.y, p_term->c.x, p_term->c.y);
                        break;
                case 2: /* all */
                        tclearregion(p_term, 0, p_term->c.y, p_term->col-1, p_term->c.y);
                        break;
                }
                break;
        case 'S': /* SU -- Scroll <n> line up */
                BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[0], 1);
                tscrollup(p_term, p_term->top, p_term->csiescseq.arg[0]);
                break;
        case 'T': /* SD -- Scroll <n> line down */
                BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[0], 1);
                tscrolldown(p_term, p_term->top, p_term->csiescseq.arg[0]);
                break;
        case 'L': /* IL -- Insert <n> blank lines */
                BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[0], 1);
                tinsertblankline(p_term, p_term->csiescseq.arg[0]);
                break;
        case 'l': /* RM -- Reset Mode */
                tsetmode(p_term, p_term->csiescseq.priv, 0, p_term->csiescseq.arg, p_term->csiescseq.narg);
                break;
        case 'M': /* DL -- Delete <n> lines */
                BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[0], 1);
                tdeleteline(p_term, p_term->csiescseq.arg[0]);
                break;
        case 'X': /* ECH -- Erase <n> char */
                BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[0], 1);
                tclearregion(p_term, p_term->c.x, p_term->c.y,
                                p_term->c.x + p_term->csiescseq.arg[0] - 1, p_term->c.y);
                break;
        case 'P': /* DCH -- Delete <n> char */
                BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[0], 1);
                tdeletechar(p_term, p_term->csiescseq.arg[0]);
                break;
        case 'Z': /* CBT -- Cursor Backward Tabulation <n> tab stops */
                BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[0], 1);
                tputtab(p_term, -p_term->csiescseq.arg[0]);
                break;
        case 'd': /* VPA -- Move to <row> */
                BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[0], 1);
                tmoveato(p_term, p_term->c.x, p_term->csiescseq.arg[0]-1);
                break;
        case 'h': /* SM -- Set terminal mode */
                tsetmode(p_term, p_term->csiescseq.priv, 1, p_term->csiescseq.arg, p_term->csiescseq.narg);
                break;
        case 'm': /* SGR -- Terminal attribute (color) */
                tsetattr(p_term, p_term->csiescseq.arg, p_term->csiescseq.narg);
                break;
        case 'n': /* DSR – Device Status Report (cursor position) */
                if (p_term->csiescseq.arg[0] == 6) {
                        len = snprintf(buf, sizeof(buf),"\033[%i;%iR",
                                        p_term->c.y+1, p_term->c.x+1);
                        bfst_child_write(&p_term->o_term_ctxt, buf, len);
                }
                break;
        case 'r': /* DECSTBM -- Set Scrolling Region */
                if(p_term->csiescseq.priv) {
                        goto unknown;
                } else {
                        BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[0], 1);
                        BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[1], p_term->row);
                        tsetscroll(p_term, p_term->csiescseq.arg[0]-1, p_term->csiescseq.arg[1]-1);
                        tmoveato(p_term, 0, 0);
                }
                break;
        case 's': /* DECSC -- Save cursor position (ANSI.SYS) */
                tcursor(p_term, CURSOR_SAVE);
                break;
        case 'u': /* DECRC -- Restore cursor position (ANSI.SYS) */
                tcursor(p_term, CURSOR_LOAD);
                break;
        case ' ':
                switch (p_term->csiescseq.mode[1]) {
                        case 'q': /* DECSCUSR -- Set Cursor Style */
                                BFST_TOOLS_DEFAULT(p_term->csiescseq.arg[0], 1);
                                if (!BFST_TOOLS_BETWEEN(p_term->csiescseq.arg[0], 0, 6)) {
                                        goto unknown;
                                }
                                p_term->c.shape = p_term->csiescseq.arg[0];
                                break;
                        default:
                                goto unknown;
                }
                break;
        case '>':
                /* Unknown escape used by xterm? */
                break;
        }
}

static
void
csireset(Term* p_term) {
        memset(&p_term->csiescseq, 0, sizeof(p_term->csiescseq));
}

#if defined(BFST_CFG_DEBUG)
static
void
strdump(Term* p_term) {
        int i;
        unsigned int c;

        bfst_msg("ESC%c", p_term->strescseq.type);
        for(i = 0; i < p_term->strescseq.len; i++) {
                c = p_term->strescseq.buf[i] & 0xff;
                if(c == '\0') {
                        return;
                } else if(isprint(c)) {
                        bfst_msg("%c", c);
                } else if(c == '\n') {
                        bfst_msg("(\\n)");
                } else if(c == '\r') {
                        bfst_msg("(\\r)");
                } else if(c == 0x1b) {
                        bfst_msg("(\\e)");
                } else {
                        bfst_msg("(%02x)", c);
                }
        }
        bfst_msg("ESC\\\n");
}
#endif /* #if defined(BFST_CFG_DEBUG) */

static
void
strparse(Term* p_term) {
        int c;
        char *p = p_term->strescseq.buf;

        p_term->strescseq.narg = 0;
        p_term->strescseq.buf[p_term->strescseq.len] = '\0';

        if(*p == '\0')
                return;

        while(p_term->strescseq.narg < STR_ARG_SIZ) {
                p_term->strescseq.args[p_term->strescseq.narg++] = p;
                while((c = *p) != ';' && c != '\0')
                        ++p;
                if(c == '\0')
                        return;
                *p++ = '\0';
        }
}

static
void
strhandle(Term* p_term) {
        char *p = NULL;
        int j, narg, par;

        p_term->esc &= ~(ESC_STR_END|ESC_STR);
        strparse(p_term);
        par = (narg = p_term->strescseq.narg) ? atoi(p_term->strescseq.args[0]) : 0;

        switch(p_term->strescseq.type) {
        case ']': /* OSC -- Operating System Command */
                switch(par) {
                case 0:
                case 1:
                case 2:
                        /* if(narg > 1)
                                xsettitle(p_term->strescseq.args[1]); */
                        return;
                case 4: /* color set */
                        if (narg > 1)
                        {
                            j = atoi(p_term->strescseq.args[1]);
                            if (narg > 2)
                            {
                                p = p_term->strescseq.args[2];
                            }
                            if (bfst_color_set(p_term->o_term_ctxt.p_view_ctxt, j, p))
                            {
#if defined(BFST_CFG_DEBUG)
                                    bfst_msg("erresc: invalid color %s\n", p);
#endif /* #if defined(BFST_CFG_DEBUG) */
                                    return;
                            }
                            bfst_draw_invalidate(p_term->o_term_ctxt.p_view_ctxt);
                        }
                        return;
                case 104: /* color reset, here p = NULL */
                        if (narg > 1)
                        {
                            j = atoi(p_term->strescseq.args[1]);
                            if (bfst_color_set(p_term->o_term_ctxt.p_view_ctxt, j, NULL))
                            {
#if defined(BFST_CFG_DEBUG)
                                    bfst_msg("erresc: invalid color %s\n", p);
#endif /* #if defined(BFST_CFG_DEBUG) */
                                    return;
                            }
                        }
                        else
                        {
                            for (j = 0; j < 256; j++)
                            {
                                bfst_color_set(p_term->o_term_ctxt.p_view_ctxt, j, NULL);
                            }
                        }
                        bfst_draw_invalidate(p_term->o_term_ctxt.p_view_ctxt);

                        /*
                        * TODO if defaultbg color is changed, borders
                        * are dirty
                        */
                        return;
                case 77:
                        if (narg > 1)
                        {
                            p = p_term->strescseq.args[1];
                        }
                        bfst_body_set_font(p_term->o_term_ctxt.p_view_ctxt, (char const *)(p));
                        return;
                }
                break;
        case 'k': /* old title set compatibility */
                /* xsettitle(p_term->strescseq.args[0]); */
                return;
        case 'P': /* DCS -- Device Control String */
        case '_': /* APC -- Application Program Command */
        case '^': /* PM -- Privacy Message */
                return;
        }

#if defined(BFST_CFG_DEBUG)
        bfst_msg("erresc: unknown str ");
        strdump(p_term);
#endif /* #if defined(BFST_CFG_DEBUG) */
}

static
void
strreset(Term* p_term) {
        memset(&p_term->strescseq, 0, sizeof(p_term->strescseq));
}

static
void
tdeftran(Term* p_term, char ascii) {
        static char cs[] = "0B";
        static int vcs[] = {CS_GRAPHIC0, CS_USA};
        char *p;

        if((p = strchr(cs, ascii)) == NULL)
        {
#if defined(BFST_CFG_DEBUG)
                bfst_msg("esc unhandled charset: ESC ( %c\n", ascii);
#endif /* #if defined(BFST_CFG_DEBUG) */
        } else {
                p_term->trantbl[p_term->icharset] = vcs[p - cs];
        }
}

static
void
tdectest(Term* p_term, char c) {
        int x, y;

        if(c == '8') { /* DEC screen alignment test. */
                for(x = 0; x < p_term->col; ++x) {
                        for(y = 0; y < p_term->row; ++y)
                                tsetchar(p_term, 'E', &p_term->c.attr, x, y);
                }
        }
}

static
void
tstrsequence(Term* p_term, unsigned char c) {
        switch (c) {
        case 0x90:   /* DCS -- Device Control String */
                c = 'P';
                break;
        case 0x9f:   /* APC -- Application Program Command */
                c = '_';
                break;
        case 0x9e:   /* PM -- Privacy Message */
                c = '^';
                break;
        case 0x9d:   /* OSC -- Operating System Command */
                c = ']';
                break;
        }
        strreset(p_term);
        p_term->strescseq.type = c;
        p_term->esc |= ESC_STR;
}

static
void
tcontrolcode(Term* p_term, unsigned char ascii) {
        switch(ascii) {
        case '\t':   /* HT */
                tputtab(p_term, 1);
                return;
        case '\b':   /* BS */
                tmoveto(p_term, p_term->c.x-1, p_term->c.y);
                return;
        case '\r':   /* CR */
                tmoveto(p_term, 0, p_term->c.y);
                return;
        case '\f':   /* LF */
        case '\v':   /* VT */
        case '\n':   /* LF */
                /* go to first col if the mode is set */
                tnewline(p_term, IS_SET(p_term, MODE_CRLF));
                return;
        case '\a':   /* BEL */
                if(p_term->esc & ESC_STR_END) {
                        /* backwards compatibility to xterm */
                        strhandle(p_term);
                } else {
                }
                break;
        case '\033': /* ESC */
                csireset(p_term);
                p_term->esc &= ~(ESC_CSI|ESC_ALTCHARSET|ESC_TEST);
                p_term->esc |= ESC_START;
                return;
        case '\016': /* SO (LS1 -- Locking shift 1) */
        case '\017': /* SI (LS0 -- Locking shift 0) */
                p_term->charset = 1 - (ascii - '\016');
                return;
        case '\032': /* SUB */
                tsetchar(p_term, '?', &p_term->c.attr, p_term->c.x, p_term->c.y);
        case '\030': /* CAN */
                csireset(p_term);
                break;
        case '\005': /* ENQ (IGNORED) */
        case '\000': /* NUL (IGNORED) */
        case '\021': /* XON (IGNORED) */
        case '\023': /* XOFF (IGNORED) */
        case 0177:   /* DEL (IGNORED) */
                return;
        case 0x84:   /* TODO: IND */
                break;
        case 0x85:   /* NEL -- Next line */
                tnewline(p_term, 1); /* always go to first col */
                break;
        case 0x88:   /* HTS -- Horizontal tab stop */
                p_term->tabs[p_term->c.x] = 1;
                break;
        case 0x8d:   /* TODO: RI */
        case 0x8e:   /* TODO: SS2 */
        case 0x8f:   /* TODO: SS3 */
        case 0x98:   /* TODO: SOS */
                break;
        case 0x9a:   /* DECID -- Identify Terminal */
                bfst_child_write(&p_term->o_term_ctxt, vtiden, sizeof(vtiden) - 1);
                break;
        case 0x9b:   /* TODO: CSI */
        case 0x9c:   /* TODO: ST */
                break;
        case 0x90:   /* DCS -- Device Control String */
        case 0x9f:   /* APC -- Application Program Command */
        case 0x9e:   /* PM -- Privacy Message */
        case 0x9d:   /* OSC -- Operating System Command */
                tstrsequence(p_term, ascii);
                return;
        }
        /* only CAN, SUB, \a and C1 chars interrupt a sequence */
        p_term->esc &= ~(ESC_STR_END|ESC_STR);
}

/*
 * returns 1 when the sequence is finished and it hasn't to read
 * more characters for this sequence, otherwise 0
 */
static
int
eschandle(Term* p_term, unsigned char ascii) {
        switch(ascii) {
        case '[':
                p_term->esc |= ESC_CSI;
                return 0;
        case '#':
                p_term->esc |= ESC_TEST;
                return 0;
        case 'P': /* DCS -- Device Control String */
        case '_': /* APC -- Application Program Command */
        case '^': /* PM -- Privacy Message */
        case ']': /* OSC -- Operating System Command */
        case 'k': /* old title set compatibility */
                tstrsequence(p_term, ascii);
                return 0;
        case 'n': /* LS2 -- Locking shift 2 */
        case 'o': /* LS3 -- Locking shift 3 */
                p_term->charset = 2 + (ascii - 'n');
                break;
        case '(': /* GZD4 -- set primary charset G0 */
        case ')': /* G1D4 -- set secondary charset G1 */
        case '*': /* G2D4 -- set tertiary charset G2 */
        case '+': /* G3D4 -- set quaternary charset G3 */
                p_term->icharset = ascii - '(';
                p_term->esc |= ESC_ALTCHARSET;
                return 0;
        case 'D': /* IND -- Linefeed */
                if(p_term->c.y == p_term->bot) {
                        tscrollup(p_term, p_term->top, 1);
                } else {
                        tmoveto(p_term, p_term->c.x, p_term->c.y+1);
                }
                break;
        case 'E': /* NEL -- Next line */
                tnewline(p_term, 1); /* always go to first col */
                break;
        case 'H': /* HTS -- Horizontal tab stop */
                p_term->tabs[p_term->c.x] = 1;
                break;
        case 'M': /* RI -- Reverse index */
                if(p_term->c.y == p_term->top) {
                        tscrolldown(p_term, p_term->top, 1);
                } else {
                        tmoveto(p_term, p_term->c.x, p_term->c.y-1);
                }
                break;
        case 'Z': /* DECID -- Identify Terminal */
                bfst_child_write(&p_term->o_term_ctxt, vtiden, sizeof(vtiden) - 1);
                break;
        case 'c': /* RIS -- Reset to inital state */
                treset(p_term);
                bfst_color_load_all(p_term->o_term_ctxt.p_view_ctxt);
                break;
        case '=': /* DECPAM -- Application keypad */
                p_term->mode |= MODE_APPKEYPAD;
                break;
        case '>': /* DECPNM -- Normal keypad */
                p_term->mode &= ~MODE_APPKEYPAD;
                break;
        case '7': /* DECSC -- Save Cursor */
                tcursor(p_term, CURSOR_SAVE);
                break;
        case '8': /* DECRC -- Restore Cursor */
                tcursor(p_term, CURSOR_LOAD);
                break;
        case '\\': /* ST -- String Terminator */
                if(p_term->esc & ESC_STR_END)
                        strhandle(p_term);
                break;
        default:
#if defined(BFST_CFG_DEBUG)
                bfst_msg("erresc: unknown sequence ESC 0x%02X '%c'\n",
                        (unsigned char) ascii, isprint(ascii)? ascii:'.');
#endif /* #if defined(BFST_CFG_DEBUG) */
                break;
        }
        return 1;
}

void
tputc(Term* p_term, unsigned long int u)
{
    char c[BFST_UTF8_SIZE];
    char control;
    int width, len;
    struct bfst_line * lp;
    struct bfst_glyph * gp;

    len = bfst_utf8_encode(u, c);
    if((width = wcwidth(u)) == -1) {
        memcpy(c, "\357\277\275", 4); /* UTF_INVALID */
        width = 1;
    }

    control = ISCONTROL(u);

    /*
     * STR sequence must be checked before anything else
     * because it uses all following characters until it
     * receives a ESC, a SUB, a ST or any other C1 control
     * character.
     */
    if(p_term->esc & ESC_STR) {
        if(u == '\a' || u == 030 || u == 032 || u == 033 ||
            ISCONTROLC1(u)) {
            p_term->esc &= ~(ESC_START|ESC_STR);
            p_term->esc |= ESC_STR_END;
        } else if(p_term->strescseq.len + len < (int)(sizeof(p_term->strescseq.buf) - 1)) {
            memmove(&p_term->strescseq.buf[p_term->strescseq.len], c, len);
            p_term->strescseq.len += len;
            return;
        } else {
            /*
             * Here is a bug in terminals. If the user never sends
             * some code to stop the str or esc command, then st
             * will stop responding. But this is better than
             * silently failing with unknown characters. At least
             * then users will report back.
             *
             * In the case users ever get fixed, here is the code:
             */
            /*
             * p_term->esc = 0;
             * strhandle();
             */
            return;
        }
    }

    /*
     * Actions of control codes must be performed as soon they arrive
     * because they can be embedded inside a control sequence, and
     * they must not cause conflicts with sequences.
     */
    if(control) {
        tcontrolcode(p_term, u);
        /*
         * control codes are not shown ever
         */
        return;
    } else if(p_term->esc & ESC_START) {
        if(p_term->esc & ESC_CSI) {
            p_term->csiescseq.buf[p_term->csiescseq.len++] = u;
            if(BFST_TOOLS_BETWEEN(u, 0x40, 0x7E)
                || p_term->csiescseq.len >= \
                (int)(sizeof(p_term->csiescseq.buf)-1)) {
                p_term->esc = 0;
                csiparse(p_term);
                csihandle(p_term);
            }
            return;
        } else if(p_term->esc & ESC_ALTCHARSET) {
            tdeftran(p_term, u);
        } else if(p_term->esc & ESC_TEST) {
            tdectest(p_term, u);
        } else {
            if (!eschandle(p_term, u))
                return;
            /* sequence already finished */
        }
        p_term->esc = 0;
        /*
         * All characters which form part of a sequence are not
         * printed
         */
        return;
    }

    if (bfst_sel_is_line_selected(p_term, p_term->c.y))
    {
        bfst_sel_clear(p_term);
    }

    lp = p_term->p_cur->a_line + p_term->c.y;
    gp = lp->a_glyph + p_term->c.x;

    if(IS_SET(p_term, MODE_WRAP) && (p_term->c.state & CURSOR_WRAPNEXT))
    {
        gp->mode |= ATTR_WRAP;

        tnewline(p_term, 1);

        lp = p_term->p_cur->a_line + p_term->c.y;
        gp = lp->a_glyph + p_term->c.x;
    }

    if (IS_SET(p_term, MODE_INSERT) && (p_term->c.x+width < p_term->col))
    {
        char b_changed;
        int dst;
        int src;
        int size;
        int i;

        b_changed = 0;

        dst = width;

        src = 0;

        size = p_term->col - p_term->c.x - width;

        for (i = size - 1 ; i >= 0; i--)
        {
            if (memcmp(gp + dst + i, gp + src + i, sizeof(struct bfst_glyph)))
            {
                gp[dst + i] = gp[src + i];

                b_changed = 1;
            }
        }

        if (b_changed)
        {
            lp->i_unique = bfst_unique_pick(p_term->o_term_ctxt.p_body_ctxt);
        }
    }

    if (p_term->c.x+width > p_term->col)
    {
        tnewline(p_term, 1);

        lp = p_term->p_cur->a_line + p_term->c.y;
        gp = lp->a_glyph + p_term->c.x;
    }

    tsetchar(p_term, u, &p_term->c.attr, p_term->c.x, p_term->c.y);

    if(width == 2)
    {
        struct bfst_glyph o_modified_char;

        char b_changed;

        b_changed = 0;

        o_modified_char = gp[0];

        o_modified_char.mode |= ATTR_WIDE;

        if (memcmp(gp + 0, &o_modified_char, sizeof(o_modified_char)))
        {
            gp[0] = o_modified_char;

            b_changed = 1;
        }

        if (p_term->c.x+1 < p_term->col)
        {
            o_modified_char = gp[1];

            o_modified_char.u = '\0';

            o_modified_char.mode = ATTR_WDUMMY;

            if (memcmp(gp + 1, &o_modified_char, sizeof(o_modified_char)))
            {
                gp[1] = o_modified_char;

                b_changed = 1;
            }
        }

        if (b_changed)
        {
            lp->i_unique = bfst_unique_pick(p_term->o_term_ctxt.p_body_ctxt);
        }
    }

    if(p_term->c.x+width < p_term->col)
    {
        tmoveto(p_term, p_term->c.x+width, p_term->c.y);
    }
    else
    {
        p_term->c.state |= CURSOR_WRAPNEXT;
    }
}

int
tputstr(
    Term* p_term,
    char * buf,
    unsigned long int buflen)
{
    int result;
    int charsize; /* size of utf8 char in bytes */
    unsigned long int unicodep;

    result = 0;
    while((charsize = bfst_utf8_decode(buf + result, &unicodep, buflen - result)))
    {
        tputc(p_term, unicodep);
        result += charsize;
    }

    return result;
}

static
void
techo(
    Term* p_term,
    unsigned long int u) {
        if(ISCONTROL(u)) { /* control code */
                if(u & 0x80) {
                        u &= 0x7f;
                        tputc(p_term, '^');
                        tputc(p_term, '[');
                } else if(u != '\n' && u != '\r' && u != '\t') {
                        u ^= 0x40;
                        tputc(p_term, '^');
                }
        }
        tputc(p_term, u);
}

void
techostr(Term* p_term, char const *s, unsigned long int n)
{
    int len;
    unsigned long int u;

    if (IS_SET(p_term, MODE_ECHO))
    {
        while((len = bfst_utf8_decode(s, &u, n)) > 0)
        {
            techo(p_term, u);
            n -= len;
            s += len;
        }
    }
}


void
tresize(Term* p_term, int col, int row)
{
    int i;
    int minrow = BFST_TOOLS_MIN(row, p_term->row);
    int mincol = BFST_TOOLS_MIN(col, p_term->col);
    char *bp;
    TCursor c;

    if(col < 1 || row < 1)
    {
#if defined(BFST_CFG_DEBUG)
        bfst_msg(
            "tresize: error resizing to %dx%d\n", col, row);
#endif /* #if defined(BFST_CFG_DEBUG) */
        return;
    }

    if ((col == p_term->col) && (row == p_term->row))
        return;

    /*
     * slide screen to keep cursor where we expect it -
     * tscrollup would work here, but we can optimize to
     * memmove because we're freeing the earlier lines
     */
    if ((row <= p_term->c.y) && !(p_term->mode & MODE_ALTSCREEN))
    {
        for (i = 0; i <= p_term->c.y - row; i ++)
        {
            bfst_log_scroll(&(p_term->o_term_ctxt), i);
        }
    }

    for(i = 0; i <= p_term->c.y - row; i++) {
        free(p_term->o_pri.a_line[i].a_glyph);
        free(p_term->o_alt.a_line[i].a_glyph);
    }
    /* ensure that both src and dst are not NULL */
    if (i > 0) {
        memmove(p_term->o_pri.a_line, p_term->o_pri.a_line + i, row * sizeof(struct bfst_line));
        memmove(p_term->o_alt.a_line, p_term->o_alt.a_line + i, row * sizeof(struct bfst_line));
    }
    for(i += row; i < p_term->row; i++) {
        free(p_term->o_pri.a_line[i].a_glyph);
        free(p_term->o_alt.a_line[i].a_glyph);
    }

    /* resize to new width */

    /* resize to new height */
    p_term->o_pri.a_line = bfst_realloc(p_term->o_pri.a_line, row * sizeof(struct bfst_line));
    p_term->o_alt.a_line = bfst_realloc(p_term->o_alt.a_line, row * sizeof(struct bfst_line));
    p_term->tabs = bfst_realloc(p_term->tabs, col * sizeof(*p_term->tabs));

    /* resize each row to new width, zero-pad if needed */
    for(i = 0; i < minrow; i++) {
        p_term->o_pri.a_line[i].a_glyph = bfst_realloc(p_term->o_pri.a_line[i].a_glyph, col * sizeof(struct bfst_glyph));
        p_term->o_alt.a_line[i].a_glyph = bfst_realloc(p_term->o_alt.a_line[i].a_glyph, col * sizeof(struct bfst_glyph));
    }

    /* allocate any new rows */
    for(/* i == minrow */; i < row; i++) {
        p_term->o_pri.a_line[i].a_glyph = bfst_malloc(col * sizeof(struct bfst_glyph));
        p_term->o_alt.a_line[i].a_glyph = bfst_malloc(col * sizeof(struct bfst_glyph));
    }
    if(col > p_term->col) {
        bp = p_term->tabs + p_term->col;

        memset(bp, 0, sizeof(*p_term->tabs) * (col - p_term->col));
        while(--bp > p_term->tabs && !*bp)
            /* nothing */ ;
        for(bp += tabspaces; bp < p_term->tabs + col; bp += tabspaces)
            *bp = 1;
    }
    /* update terminal size */
    p_term->col = col;
    p_term->row = row;
    /* reset scrolling region */
    tsetscroll(p_term, 0, row-1);
    /* make use of the BFST_TOOLS_LIMIT in tmoveto */
    tmoveto(p_term, p_term->c.x, p_term->c.y);
    /* Clearing both screens (it makes dirty all lines) */
    c = p_term->c;
    for(i = 0; i < 2; i++) {
        if(mincol < col && 0 < minrow) {
            tclearregion(p_term, mincol, 0, col - 1, minrow - 1);
        }
        if(0 < col && minrow < row) {
            tclearregion(p_term, 0, minrow, col - 1, row - 1);
        }
        tswapscreen(p_term);
        tcursor(p_term, CURSOR_LOAD);
    }
    p_term->c = c;

}

