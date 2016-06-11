/* See LICENSE for license details. */

/*

Module: bfst_opts.h

Description:

    Management of command-line options.

*/

#if defined(INC_FXTERM_OPTIONS_H)
#error include bfst_options.h once
#endif

#define INC_FXTERM_OPTIONS_H

/*

Structure: bfst_options

Description:

    Stores a copy of all command line options or default values.

*/
struct bfst_options
{
    /* Command-line of child process to launch within the terminal */
    char * * opt_cmd;

    /* Identification of parent window for embedding */
    char * opt_embed;

    /* Name of font resource */
    char * opt_font;

    /* Left coordinate of geometry */
    int opt_left;

    /* Top coordinate of geometry */
    int opt_top;

    /* Number of columns of geometry */
    unsigned int opt_cols;

    /* Number of rows of geometry */
    unsigned int opt_rows;

    /* Result of XParseGeometry() */
    int opt_gm;

    /* Debug mode */
    char opt_debug;

}; /* struct bfst_options */

/* Methods for bfst_opts module */

char bfst_parse_options(
    struct bfst_options * const p_options,
    int argc,
    char * * argv);

/* end-of-file: bfst_opts.h */
