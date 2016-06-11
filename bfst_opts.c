/* See LICENSE for license details. */

/*

Module: bfst_opts.c

Description:

    Management of command-line options.

*/

#include <stdio.h>

#include <string.h>

#include <X11/Xlib.h>

#include "bfst_opts.h"

/*

Function: bfst_usage()

Description:

    Display usage information.

*/
static void bfst_usage(void)
{
    fprintf(stderr,
        "usage:\n"
        "  bfst \\\n"
        "    [-g geometry] \\\n"
        "    [-w windowid] \\\n"
        "    [-f fontname] \\\n"
        "    [[-e] command]\n");

} /* bfst_usage() */

/*

Function: bfst_parse_geometry()

Description:

    Parse the geometry command-line options.

*/
static void bfst_parse_geometry(
    struct bfst_options * const p_options,
    char * const p_geometry_descriptor)
{
    p_options->opt_gm = XParseGeometry(
            p_geometry_descriptor,
            &(p_options->opt_left),
            &(p_options->opt_top),
            &(p_options->opt_cols),
            &(p_options->opt_rows));

} /* bfst_parse_geometry() */

/*

Function: bfst_options_init()

Description:

    Initialize the bfst_opts structure with default values.

*/
static void bfst_options_init(
    struct bfst_options * const p_options)
{
    memset(p_options, 0x00u, sizeof(*p_options));

    p_options->opt_cols = 80;

    p_options->opt_rows = 24;

} /* bfst_options_init() */

/*

Function: bfst_parse_options()

Description:

    Parse array of command-line options and store results into bfst_options
    structure.

*/
char bfst_parse_options(
    struct bfst_options * const p_options,
    int argc,
    char * * argv)
{
    char b_result;

    /* Shift program name */
    argc --;
    argv ++;

    bfst_options_init(p_options);

    b_result = 1;

    /* For all remaining arguments */
    while ((argc > 0) && ('-' == argv[0][0]))
    {
        /* Detect the first letter of option */
        char const c_option_char = argv[0][1];

        /* Skip the option prefix */
        argc --;
        argv ++;

        if ('d' == c_option_char)
        {
            p_options->opt_debug = 1;
        }
        else if (argc > 0)
        {
            if ('e' == c_option_char)
            {
                p_options->opt_cmd = argv;
                argv += argc;
                argc = 0;
            }
            else if ('g' == c_option_char)
            {
                bfst_parse_geometry(
                    p_options,
                    argv[0]);

                argc --;
                argv ++;
            }
            else if ('w' == c_option_char)
            {
                p_options->opt_embed = argv[0];
                argc --;
                argv ++;
            }
            else if ('f' == c_option_char)
            {
                p_options->opt_font = argv[0];
                argc --;
                argv ++;
            }
            else
            {
                b_result = 0;
            }
        }
        else
        {
            b_result = 0;
        }
    }

    if (b_result)
    {
        if (argc > 0)
        {
            p_options->opt_cmd = argv;
        }
    }
    else
    {
        bfst_usage();
    }

    return b_result;

} /* bfst_parse_options() */

/* end-of-file: bfst_opts.c */
