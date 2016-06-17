/* See LICENSE for license details. */

/*

Module: bfst_font_list.h

Description:

    List of font names used by font change hot key.

*/

#include "bfst_os.h"

#include "bfst_font_list.h"

#include "bfst_ctxt.h"

/*

Function: bfst_font_list_init

Description:

    Initialize module.

*/
void bfst_font_list_init(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_font_list * const p_font_list = p_body_ctxt->p_font_list;

    /* Merge default list and command line options... */

    p_font_list->i_font_count = 10u;

    p_font_list->a_font_list[0u] = "-misc-fixed-medium-r-semicondensed--13-120-75-75-c-60-iso10646-1";

    p_font_list->a_font_list[1u] = "-misc-fixed-medium-r-normal--6-60-75-75-c-40-iso10646-1";

    p_font_list->a_font_list[2u] = "-misc-fixed-medium-r-normal--7-70-75-75-c-50-iso10646-1";

    p_font_list->a_font_list[3u] = "-misc-fixed-medium-r-normal--8-80-75-75-c-50-iso10646-1";

    p_font_list->a_font_list[4u] = "-misc-fixed-medium-r-normal--9-90-75-75-c-60-iso10646-1";

    p_font_list->a_font_list[5u] = "-misc-fixed-medium-r-normal--10-100-75-75-c-60-iso10646-1";

    p_font_list->a_font_list[6u] = "-misc-fixed-medium-r-normal--14-130-75-75-c-70-iso10646-1";

    p_font_list->a_font_list[7u] = "-misc-fixed-medium-r-normal--15-140-75-75-c-90-iso10646-1";

    p_font_list->a_font_list[8u] = "-misc-fixed-medium-r-normal--18-120-100-100-c-90-iso10646-1";

    p_font_list->a_font_list[9u] = "-misc-fixed-medium-r-normal--20-200-75-75-c-100-iso10646-1";

} /* bfst_font_list_init() */

/*

Function: bfst_font_list_cleanup

Description:

    Cleanup module.

*/
void bfst_font_list_cleanup(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    (void)(p_body_ctxt);

} /* bfst_font_list_cleanup() */

/* Number of fonts in list */
unsigned int
bfst_font_list_count(
    struct bfst_body_ctxt const * const p_body_ctxt)
{
    struct bfst_font_list const * const p_font_list = p_body_ctxt->p_font_list;

    return p_font_list->i_font_count;

} /* bfst_font_list_count() */

/* Name of one of the fonts in the list */
char const *
bfst_font_list_get(
    struct bfst_body_ctxt const * const p_body_ctxt,
    unsigned int const i_font_index)
{
    struct bfst_font_list const * const p_font_list = p_body_ctxt->p_font_list;

    return p_font_list->a_font_list[i_font_index];

} /* bfst_font_list_get() */

