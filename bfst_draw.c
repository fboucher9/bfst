/* See LICENSE for license details. */

/*

Tasks:

    -   Concept of offset between visible line and tty line.

    -   Concept of different types of lines

*/

#include "bfst_os.h"

#include "bfst_body.h"

/*
 * thickness of underline and bar cursors
 */
static unsigned int const cursorthickness = 1;

static
void
bfst_rect(
    struct bfst_view_ctxt const * const
        p_view_ctxt,
    XColor const * color,
    int x,
    int y,
    int w,
    int h)
{
    struct bfst_body_ctxt const * const p_body_ctxt = p_view_ctxt->p_body_ctxt;

    struct bfst_display const * const p_display = p_body_ctxt->p_display;

    struct bfst_draw const * const p_draw = p_view_ctxt->p_draw;

    XSetForeground(p_display->dpy, p_display->gc, color->pixel);

    XFillRectangle(p_display->dpy, p_draw->draw_buf, p_display->gc, x, y, w, h);
}

static
void
bfst_draw_selection_segments(
    struct bfst_view_ctxt const * const
        p_view_ctxt,
    int const
        winx,
    int const
        winy,
    unsigned short int
        flags)
{
    struct bfst_body_ctxt const * const p_body_ctxt = p_view_ctxt->p_body_ctxt;

    struct bfst_display const * const p_display = p_body_ctxt->p_display;

    struct bfst_draw const * const p_draw = p_view_ctxt->p_draw;

    struct bfst_font const * const p_font = p_view_ctxt->p_font;

    XColor const * p_outline_color = bfst_color_get(p_view_ctxt, BFST_COLOR_SEL);

    XSetForeground(p_display->dpy, p_display->gc, p_outline_color->pixel);

    if ((flags & (GF_TOP_LEFT | GF_TOP | GF_TOP_RIGHT)) == (GF_TOP_LEFT | GF_TOP | GF_TOP_RIGHT))
    {
        XFillRectangle(p_display->dpy, p_draw->draw_buf, p_display->gc, winx, winy, p_font->i_width, 1);

        flags &= ~(GF_TOP_LEFT | GF_TOP | GF_TOP_RIGHT);
    }
    if ((flags & (GF_BOTTOM_LEFT | GF_BOTTOM | GF_BOTTOM_RIGHT)) == (GF_BOTTOM_LEFT | GF_BOTTOM | GF_BOTTOM_RIGHT))
    {
        XFillRectangle(p_display->dpy, p_draw->draw_buf, p_display->gc, winx, winy + p_font->i_height - 1, p_font->i_width, 1);

        flags &= ~(GF_BOTTOM_LEFT | GF_BOTTOM | GF_BOTTOM_RIGHT);
    }
    if ((flags & (GF_TOP_LEFT | GF_LEFT | GF_BOTTOM_LEFT)) == (GF_TOP_LEFT | GF_LEFT | GF_BOTTOM_LEFT))
    {
        XFillRectangle(p_display->dpy, p_draw->draw_buf, p_display->gc, winx, winy, 1, p_font->i_height);

        flags &= ~(GF_TOP_LEFT | GF_LEFT | GF_BOTTOM_LEFT);
    }
    if ((flags & (GF_TOP_RIGHT | GF_RIGHT | GF_BOTTOM_RIGHT)) == (GF_TOP_RIGHT | GF_RIGHT | GF_BOTTOM_RIGHT))
    {
        XFillRectangle(p_display->dpy, p_draw->draw_buf, p_display->gc, winx + p_font->i_width - 1, winy, 1, p_font->i_height);

        flags &= ~(GF_TOP_RIGHT | GF_RIGHT | GF_BOTTOM_RIGHT);
    }
    if (flags & GF_TOP_LEFT)
    {
        XFillRectangle(p_display->dpy, p_draw->draw_buf, p_display->gc, winx, winy, 1, 1);
    }
    if (flags & GF_TOP)
    {
        XFillRectangle(p_display->dpy, p_draw->draw_buf, p_display->gc, winx + 1, winy, p_font->i_width - 2, 1);
    }
    if (flags & GF_TOP_RIGHT)
    {
        XFillRectangle(p_display->dpy, p_draw->draw_buf, p_display->gc, winx + p_font->i_width - 1, winy, 1, 1);
    }
    if (flags & GF_LEFT)
    {
        XFillRectangle(p_display->dpy, p_draw->draw_buf, p_display->gc, winx, winy + 1, 1, p_font->i_height - 2);
    }
    if (flags & GF_RIGHT)
    {
        XFillRectangle(p_display->dpy, p_draw->draw_buf, p_display->gc, winx + p_font->i_width - 1, winy + 1, 1, p_font->i_height - 2);
    }
    if (flags & GF_BOTTOM_LEFT)
    {
        XFillRectangle(p_display->dpy, p_draw->draw_buf, p_display->gc, winx, winy + p_font->i_height - 1, 1, 1);
    }
    if (flags & GF_BOTTOM)
    {
        XFillRectangle(p_display->dpy, p_draw->draw_buf, p_display->gc, winx + 1, winy + p_font->i_height - 1, p_font->i_width - 2, 1);
    }
    if (flags & GF_BOTTOM_RIGHT)
    {
        XFillRectangle(p_display->dpy, p_draw->draw_buf, p_display->gc, winx + p_font->i_width - 1, winy + p_font->i_height - 1, 1, 1);
    }
}

static
void
bfst_clear(
    struct bfst_view_ctxt const * const
        p_view_ctxt,
    int x1,
    int y1,
    int x2,
    int y2)
{
    bfst_rect(
        p_view_ctxt,
        bfst_color_get(p_view_ctxt, BFST_COLOR_BG),
        x1,
        y1,
        x2-x1,
        y2-y1);
}

static
void
bfst_draw_glyph(
    struct bfst_view_ctxt const * const
        p_view_ctxt,
    struct bfst_tty * const
        p_term,
    struct bfst_glyph g,
    int x,
    int y)
{
    XChar2b char_buf[1];
    XColor revfg;
    /* XColor revbg; */
    XColor const * fg;
    XColor const * bg;
    XColor const * temp;
    int winx;
    int winy;

    struct bfst_body_ctxt const * const p_body_ctxt = p_view_ctxt->p_body_ctxt;

    struct bfst_display const * const p_display = p_body_ctxt->p_display;

    struct bfst_font const * const p_font = p_view_ctxt->p_font;

    struct bfst_draw * const p_draw = p_view_ctxt->p_draw;

    struct bfst_window const * const p_window = p_view_ctxt->p_window;

    char_buf[0].byte2 = (g.u & 0xffu);

    char_buf[0].byte1 = ((g.u >> 8) & 0xffu);

    winx = p_window->i_border + x * p_font->i_width;

    winy = p_window->i_border + y * p_font->i_height;

    fg = bfst_color_get(p_view_ctxt, g.fg);

    bg = bfst_color_get(p_view_ctxt, g.bg);

    /* Change basic system colors [0-7] to bright system colors [8-15] */
    if (((g.mode & ATTR_BOLD_FAINT) == ATTR_BOLD) && (g.fg <= 7))
    {
        fg = bfst_color_get(p_view_ctxt, g.fg + 8);
    }

    if (g.mode & ATTR_REVERSE)
    {
        temp = fg;
        fg = bg;
        bg = temp;
    }

    if((g.mode & ATTR_BOLD_FAINT) == ATTR_FAINT)
    {
        revfg.red = fg->red / 2;
        revfg.green = fg->green / 2;
        revfg.blue = fg->blue / 2;
        XAllocColor(p_display->dpy, p_display->cmap, &revfg);
        fg = &revfg;
    }

    if(g.mode & ATTR_BLINK && p_term->mode & MODE_BLINK)
    {
        fg = bg;
    }

    if(g.mode & ATTR_INVISIBLE)
    {
        fg = bg;
    }

    XSetBackground(
        p_display->dpy,
        p_display->gc,
        bg->pixel);

    XSetForeground(
        p_display->dpy,
        p_display->gc,
        fg->pixel);

    XDrawImageString16(
        p_display->dpy,
        p_draw->draw_buf,
        p_display->gc,
        winx,
        winy + p_font->p_font_res->ascent,
        char_buf,
        1);

    /* Draw the underline */
    if (g.mode & ATTR_UNDERLINE)
    {
        bfst_rect(p_view_ctxt, fg, winx, winy + p_font->i_height - 1, p_font->i_width, 1);
    }
    else if (g.mode & ATTR_ITALIC)
    {
        bfst_rect(p_view_ctxt, fg, winx, winy + p_font->i_height - 1, /*p_font->i_width/2 -*/ 1, 1);
        bfst_rect(p_view_ctxt, fg, winx + p_font->i_width/2, winy + p_font->i_height - 1, /*p_font->i_width/2 -*/ 1, 1);
    }
    if (g.mode & ATTR_STRUCK)
    {
        bfst_rect(p_view_ctxt, fg, winx, winy + p_font->i_height/2 + 1, p_font->i_width, 1);
    }

    /* Draw the outline */
    if (g.flags)
    {
        bfst_draw_selection_segments(p_view_ctxt, winx, winy, g.flags);
    }

}

static
void
bfst_draw_cursor(
    struct bfst_view_ctxt const * const
        p_view_ctxt,
    struct bfst_tty * const
        p_term,
    struct bfst_glyph
        g,
    int const
        x,
    int const
        y)
{
    XColor const * p_color_cs;

    struct bfst_font const * const p_font = p_view_ctxt->p_font;

    struct bfst_window const * const p_window = p_view_ctxt->p_window;

    bfst_draw_glyph(p_view_ctxt, p_term, g, x, y);

    if (IS_SET(p_term, MODE_HIDE))
    {
        return;
    }

    p_color_cs = bfst_color_get(p_view_ctxt, BFST_COLOR_CS);

    /* draw the new one */
    if (p_window->state & WIN_FOCUSED)
    {
        if (p_term->mode & MODE_BLINK)
        {
            if (/*(0 == p_term->c.shape)
                ||*/ (1 == p_term->c.shape)
                || (3 == p_term->c.shape)
                || (5 == p_term->c.shape))
            {
                return;
            }
        }

        switch (p_term->c.shape)
        {
            case 0: /* Blinking Block */
            case 1: /* Blinking Block (Default) */
            case 2: /* Steady Block */
                g.fg = BFST_COLOR_BG;
                g.bg = BFST_COLOR_CS;
                g.mode = ATTR_NULL;

                /* g.mode |= p_term->line[p_term->c.y][curx].mode & ATTR_WIDE; */
                bfst_draw_glyph(p_view_ctxt, p_term, g, x, y);
                break;
            case 3: /* Blinking Underline */
            case 4: /* Steady Underline */
                bfst_rect(p_view_ctxt, p_color_cs,
                    p_window->i_border + x * p_font->i_width,
                    p_window->i_border + (y + 1) * p_font->i_height - cursorthickness,
                    p_font->i_width, cursorthickness);
                break;
            case 5: /* Blinking bar */
            case 6: /* Steady bar */
                bfst_rect(p_view_ctxt, p_color_cs,
                    p_window->i_border + x * p_font->i_width,
                    p_window->i_border + y * p_font->i_height,
                    cursorthickness, p_font->i_height);
                break;
        }
    }
    else
    {
        bfst_rect(p_view_ctxt, p_color_cs,
            p_window->i_border + x * p_font->i_width,
            p_window->i_border + y * p_font->i_height,
            p_font->i_width - 1, 1);

        bfst_rect(p_view_ctxt, p_color_cs,
            p_window->i_border + x * p_font->i_width,
            p_window->i_border + y * p_font->i_height,
            1, p_font->i_height - 1);

        bfst_rect(p_view_ctxt, p_color_cs,
            p_window->i_border + (x + 1) * p_font->i_width - 1,
            p_window->i_border + y * p_font->i_height,
            1, p_font->i_height - 1);

        bfst_rect(p_view_ctxt, p_color_cs,
            p_window->i_border + x * p_font->i_width,
            p_window->i_border + (y + 1) * p_font->i_height - 1,
            p_font->i_width, 1);
    }
}

static
int
bfst_draw_region(
    struct bfst_view_ctxt const * const
        p_view_ctxt,
    struct bfst_tty * const
        p_term,
    int x1,
    int y1,
    int x2,
    int y2)
{
    int
        i_refresh_char_count;

    int
        i_refresh_line_count;

    struct bfst_window const * const p_window = p_view_ctxt->p_window;

    struct bfst_font const * const p_font = p_view_ctxt->p_font;

    struct bfst_draw * const p_draw = p_view_ctxt->p_draw;

    struct bfst_display const * const p_display = p_view_ctxt->p_body_ctxt->p_display;

    i_refresh_char_count =
        0;

    i_refresh_line_count =
        0;

    if (p_window->state & WIN_VISIBLE)
    {
        int x, y;
        struct bfst_glyph new;
        char ena_sel = p_term->sel.ob.x != -1 && p_term->sel.alt == IS_SET(p_term, MODE_ALTSCREEN);

        /* Select the font */
        XSetFont(p_display->dpy, p_display->gc, p_font->p_font_res->fid);

        /* Detect new cursor state */
        char b_cursor_changed;

        unsigned char i_cursor_shape;

        i_cursor_shape = (p_window->state & WIN_FOCUSED) ? p_term->c.shape : 15u;

        if (p_term->mode & MODE_BLINK)
        {
            if (/*(0 == i_cursor_shape) || */
                (1 == i_cursor_shape) ||
                (3 == i_cursor_shape) ||
                (5 == i_cursor_shape))
            {
                i_cursor_shape = 14u;
            }
        }

        if (p_term->mode & MODE_HIDE)
        {
            i_cursor_shape = 14u;
        }

        {
            b_cursor_changed = 0;

            if ((p_draw->i_cursor_y != p_term->c.y)
                || (p_draw->i_cursor_x != p_term->c.x)
                || (p_draw->i_cursor_shape != i_cursor_shape))
            {
                b_cursor_changed = 1;
            }

            /* is the cursor blinking? */
        }

        char b_sel_changed;

        int i_sel_state = (p_term->sel.ob.x != -1);

        {
            b_sel_changed = 0;

            if (i_sel_state != p_draw->i_sel_state)
            {
                b_sel_changed = 1;
            }
            else
            {
                if (i_sel_state)
                {
                    if ((p_draw->i_sel_nbx != p_term->sel.nb.x)
                        || (p_draw->i_sel_nby != p_term->sel.nb.y)
                        || (p_draw->i_sel_nex != p_term->sel.ne.x)
                        || (p_draw->i_sel_ney != p_term->sel.ne.y))
                    {
                        b_sel_changed = 1;
                    }
                }
            }
        }

        for (y = y1; y < y2; y++)
        {
            int y_tty;

            char ena_sel_line;

            unsigned long long int i_unique;

            y_tty = (y + p_draw->top);

            if (y_tty >= 0)
            {
                if (y_tty < p_term->row)
                {
                    if (!p_draw->alt)
                    {
                        i_unique = p_term->p_cur->a_line[y_tty].i_unique;
                    }
                    else
                    {
                        i_unique = p_term->p_other->a_line[y_tty].i_unique;
                    }
                }
                else
                {
                    i_unique = 0;
                }
            }
            else
            {
                i_unique = bfst_log_get_line_unique(&p_term->o_term_ctxt, y_tty + p_term->log.i_count);
            }

            ena_sel_line = (ena_sel && bfst_sel_is_line_selected(p_term, y_tty));

            /* does this line contain any blink characters? */

            if ((i_unique != p_draw->a_cache_line[y].i_unique)
                || ((y_tty == p_term->c.y) && b_cursor_changed)
                || ((y_tty == p_draw->i_cursor_y) && b_cursor_changed)
                || (ena_sel_line && b_sel_changed)
                || (b_sel_changed && p_draw->i_sel_state && (y_tty >= p_draw->i_sel_nby) && (y_tty <= p_draw->i_sel_ney))
                || p_draw->a_cache_line[y].i_blinks)
            {
                int i_blinks;

                i_blinks = 0;

                for (x = x1; x < x2; x++)
                {
                    if (y_tty >= 0)
                    {
                        if (y_tty < p_term->row)
                        {
                            if (!p_draw->alt)
                            {
                                new = p_term->p_cur->a_line[y_tty].a_glyph[x];
                            }
                            else
                            {
                                new = p_term->p_other->a_line[y_tty].a_glyph[x];
                            }
                        }
                        else
                        {
                            new.u = ' ';
                            new.fg = BFST_COLOR_FG;
                            new.bg = BFST_COLOR_BG;
                            new.mode = ATTR_NULL;
                            new.flags = 0;
                        }
                    }
                    else
                    {
                        /* -1 == count */
                        bfst_log_get_char(&p_term->o_term_ctxt, y_tty + p_term->log.i_count, x, &new);
                    }

                    if (new.mode == ATTR_WDUMMY)
                        continue;

                    if (ena_sel_line)
                    {
                        new.flags = bfst_sel_calc_flags(p_term, x, y_tty);
                    }

                    /* Always refresh blinking characters */
                    if (new.mode & ATTR_BLINK)
                    {
                        if (p_term->mode & MODE_BLINK)
                        {
                            new.fg = new.bg;
                        }
                        i_blinks ++;
                    }

                    if ((p_term->c.x == x) && (p_term->c.y == y_tty))
                    {
                        bfst_draw_cursor(p_view_ctxt, p_term, new, x, y);

                        memset(&(p_draw->cache[y * p_window->i_tty_cols + x]), 0x00, sizeof(new));
                    }
                    else
                    {
                        if (0 != memcmp(&new, &(p_draw->cache[y * p_window->i_tty_cols + x]), sizeof(new)))
                        {
                            bfst_draw_glyph(p_view_ctxt, p_term, new, x, y);

                            i_refresh_char_count ++;

                            p_draw->cache[y * p_window->i_tty_cols + x] = new;
                        }
                    }
                }

                p_draw->a_cache_line[y].i_unique = i_unique;

                p_draw->a_cache_line[y].i_blinks = i_blinks;

                i_refresh_line_count ++;
            }
        }

        p_draw->i_cursor_x = p_term->c.x;

        p_draw->i_cursor_y = p_term->c.y;

        p_draw->i_cursor_shape = i_cursor_shape;

        p_draw->i_sel_nbx = p_term->sel.nb.x;

        p_draw->i_sel_nby = p_term->sel.nb.y;

        p_draw->i_sel_nex = p_term->sel.ne.x;

        p_draw->i_sel_ney = p_term->sel.ne.y;

        p_draw->i_sel_state = i_sel_state;

        /* i_line_offset ++; */
    }

#if defined(BFST_CFG_DEBUG)
    {
        static char g_print_refresh_report = 1;
        if (g_print_refresh_report)
        {
            if (i_refresh_line_count || i_refresh_char_count)
            {
                bfst_msg("refresh | %3d lines (%2d %%) | %5d chars (%2d %%)\n",
                    i_refresh_line_count,
                    i_refresh_line_count * 100 / p_window->i_tty_rows,
                    i_refresh_char_count,
                    i_refresh_char_count * 100 / (p_window->i_tty_rows * p_window->i_tty_cols));
            }
        }
    }
#endif /* #if defined(BFST_CFG_DEBUG) */

    return i_refresh_char_count;

}

int bfst_draw_all(
    struct bfst_view_ctxt const * const p_view_ctxt)
{
    int i_refresh_char_count;

    struct bfst_body_ctxt const * const p_body_ctxt = p_view_ctxt->p_body_ctxt;

    struct bfst_display const * const p_display = p_body_ctxt->p_display;

    struct bfst_window const * const p_window = p_view_ctxt->p_window;

    struct bfst_tty * const p_term = bfst_tab_get(p_view_ctxt);

    struct bfst_draw * const p_draw = p_view_ctxt->p_draw;

    i_refresh_char_count = bfst_draw_region(
            p_view_ctxt, p_term, 0, 0,
            p_term->col, p_term->row);

    XCopyArea(
        p_display->dpy, p_draw->draw_buf, p_window->h_win_res, p_display->gc,
        0, 0,
        p_window->i_width_pixels,
        p_window->i_height_pixels,
        0, 0);

    return i_refresh_char_count;

}

void
bfst_draw_invalidate(
    struct bfst_view_ctxt const * const
        p_view_ctxt)
{
    struct bfst_body_ctxt const * const p_body_ctxt = p_view_ctxt->p_body_ctxt;

    struct bfst_display const * const p_display = p_body_ctxt->p_display;

    struct bfst_draw * const p_draw = p_view_ctxt->p_draw;

    struct bfst_window const * const p_window = p_view_ctxt->p_window;

    unsigned int y;

    unsigned int x;

    for (y = 0; y < p_window->i_tty_rows; y++)
    {
        p_draw->a_cache_line[y].i_unique = 0;

        for (x = 0; x < p_window->i_tty_cols; x++)
        {
            p_draw->a_cache_line[y].a_char[x].u = 0;
        }
    }

    /* Refresh the default background color of window */
    {
        XColor const * const p_bg_color = bfst_color_get(p_view_ctxt, BFST_COLOR_BG);

        XSetWindowBackground(
            p_display->dpy,
            p_window->h_win_res,
            p_bg_color->pixel);
    }

    /* Clear the whole cache area */
    bfst_clear(p_view_ctxt, 0, 0, p_window->i_width_pixels, p_window->i_height_pixels);

}

void
bfst_draw_expose(
    struct bfst_view_ctxt const * const
        p_view_ctxt,
    XEvent const *
        p_event)
{
    struct bfst_body_ctxt const * const p_body_ctxt = p_view_ctxt->p_body_ctxt;

    struct bfst_display const * const p_display = p_body_ctxt->p_display;

    struct bfst_window const * const p_window = p_view_ctxt->p_window;

    struct bfst_draw * const p_draw = p_view_ctxt->p_draw;

    XCopyArea(
        p_display->dpy,
        p_draw->draw_buf,
        p_window->h_win_res,
        p_display->gc,
        p_event->xexpose.x,
        p_event->xexpose.y,
        p_event->xexpose.width,
        p_event->xexpose.height,
        p_event->xexpose.x,
        p_event->xexpose.y);
}

void
bfst_draw_resize(
    struct bfst_view_ctxt const * const
        p_view_ctxt)
{
    struct bfst_body_ctxt const * const p_body_ctxt = p_view_ctxt->p_body_ctxt;

    struct bfst_display const * const p_display = p_body_ctxt->p_display;

    struct bfst_window const * const p_window = p_view_ctxt->p_window;

    struct bfst_draw * const p_draw = p_view_ctxt->p_draw;

    if (p_draw->draw_buf)
    {
        XFreePixmap(
            p_display->dpy,
            p_draw->draw_buf);
    }

    p_draw->draw_buf =
        XCreatePixmap(
            p_display->dpy,
            p_window->h_win_res,
            p_window->i_width_pixels,
            p_window->i_height_pixels,
            DefaultDepth(p_display->dpy, p_display->scr));

    /* Clear the whole cache area */
    bfst_clear(p_view_ctxt, 0, 0, p_window->i_width_pixels, p_window->i_height_pixels);

    {
        unsigned int i_new_cache_len;

        i_new_cache_len = p_window->i_tty_rows * p_window->i_tty_cols * sizeof(struct bfst_glyph);

        p_draw->cache = bfst_realloc(p_draw->cache, i_new_cache_len);

        if (p_draw->cache)
        {
            unsigned int i;

            struct bfst_glyph o_space;

            {
                o_space.u = ' ';

                o_space.fg = BFST_COLOR_FG;

                o_space.bg = BFST_COLOR_BG;

                o_space.mode = 0;

                o_space.flags = 0;
            }

            for (i = 0; i < i_new_cache_len / sizeof(struct bfst_glyph); i ++)
            {
                p_draw->cache[i] = o_space;
            }
        }
    }

    {
        p_draw->a_cache_line = (struct bfst_draw_line *)(bfst_realloc(p_draw->a_cache_line, sizeof(struct bfst_draw_line) * p_window->i_tty_rows));

        if (p_draw->a_cache_line)
        {
            unsigned int i_line_index;

            for (i_line_index = 0; i_line_index < p_window->i_tty_rows; i_line_index ++)
            {
                p_draw->a_cache_line[i_line_index].a_char = &(p_draw->cache[i_line_index * p_window->i_tty_cols]);

                p_draw->a_cache_line[i_line_index].i_unique = 0;

                p_draw->a_cache_line[i_line_index].i_char_count = 0;
            }
        }
    }
}

void
bfst_draw_cleanup(
    struct bfst_view_ctxt const * const
        p_view_ctxt)
{
    struct bfst_body_ctxt const * const p_body_ctxt = p_view_ctxt->p_body_ctxt;

    struct bfst_display const * const p_display = p_body_ctxt->p_display;

    struct bfst_draw * const p_draw = p_view_ctxt->p_draw;

    if (p_draw->a_cache_line)
    {
        free(p_draw->a_cache_line);

        p_draw->a_cache_line = NULL;
    }

    if (p_draw->cache)
    {
        free(p_draw->cache);

        p_draw->cache = NULL;
    }

    if (p_draw->draw_buf != None)
    {
        XFreePixmap(p_display->dpy, p_draw->draw_buf);

        p_draw->draw_buf = None;
    }

} /* bfst_draw_cleanup() */

/* end-of-file: bfst_draw.c */
