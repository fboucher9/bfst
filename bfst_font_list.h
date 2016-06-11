/* See LICENSE for license details. */

/*

Module: bfst_font_list.h

Description:

    List of font names used by font change hot key.

*/

/* Reverse include guard */
#if defined(INC_BFST_FONT_LIST_H)
#error include bfst_font_list.h once
#endif /* #if defined(INC_BFST_FONT_LIST_H) */

#define INC_BFST_FONT_LIST_H

/* Maximum number of fonts in list */
#define BFST_FONT_LIST_MAX 10u

/* Predefine context */
struct bfst_body_ctxt;

/*

Structure: bfst_font_list

Description:

    Array of font names.

Comments:

    -   An instance of this structure is found in the bfst_body container.

    -   Each window may select a different font so information about the
        currently selected font must not be placed here.

*/
struct bfst_font_list
{
    char const * a_font_list[BFST_FONT_LIST_MAX];

    unsigned int i_font_count;

}; /* struct bfst_font_list */

/* Interface: */

/* Initialize module */
void bfst_font_list_init(
    struct bfst_body_ctxt const * const p_body_ctxt);

/* Cleanup module */
void bfst_font_list_cleanup(
    struct bfst_body_ctxt const * const p_body_ctxt);

/* Number of fonts in list */
unsigned int
bfst_font_list_count(
    struct bfst_body_ctxt const * const p_body_ctxt);

/* Name of one of the fonts in the list */
char const *
bfst_font_list_get(
    struct bfst_body_ctxt const * const p_body_ctxt,
    unsigned int const i_font_index);

/* end-of-file: bfst_font_list.h */
