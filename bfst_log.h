/* See LICENSE for license details. */

/*

Module: bfst_log.h

*/

struct bfst_tty_ctxt;

struct bfst_glyph;

struct bfst_log_line
{
    /* Unique identifier for this line */
    unsigned long long int i_unique;

    /* Pointer to array of characters */
    struct bfst_glyph * a_char;

    /* Number of characters in array */
    unsigned int i_len;

}; /* struct bfst_log_line */

struct bfst_log
{
    /* Pointer to array of lines */
    struct bfst_log_line * * a_line;

    /* Index of first valid element in list */
    unsigned int i_index;

    /* Number of valid elements in list */
    unsigned int i_count;

    /* Maximum number of elements in list */
    unsigned int i_max_count;

};

void
bfst_log_init(
    struct bfst_tty_ctxt const * const
        p_term_ctxt);

void
bfst_log_cleanup(
    struct bfst_tty_ctxt const * const
        p_term_ctxt);

unsigned int
bfst_log_get_row_count(
    struct bfst_tty_ctxt const * const
        p_term_ctxt);

unsigned long long int
bfst_log_get_line_unique(
    struct bfst_tty_ctxt const * const
        p_term_ctxt,
    unsigned int const
        i_row_index);

unsigned short int
bfst_log_get_line_len(
    struct bfst_tty_ctxt const * const
        p_term_ctxt,
    unsigned int const
        i_row_index);

void
bfst_log_get_char(
    struct bfst_tty_ctxt const * const
        p_term_ctxt,
    unsigned int const
        i_row_index,
    unsigned short int const
        i_col_index,
    struct bfst_glyph * const
        p_char_info);

void
bfst_log_scroll(
    struct bfst_tty_ctxt const * const
        p_term_ctxt,
    int const
        y);

