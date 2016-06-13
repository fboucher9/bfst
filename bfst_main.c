/* See LICENSE for license details. */

/*

Module: bfst_main.c

Description:

    Main entry-point of bfst application.

*/

#include <stdio.h>

#include <string.h>

#include <X11/Xlib.h>

#include "bfst_body.h"

#include "bfst_main.h"

/*

Function: bfst_main_detach()

Description:

    Detach from parent console.

Comments:

    -   This is required when launching bfst directly from window manager.

*/
static void bfst_main_detach(void)
{
    freopen("/dev/null", "wb", stdout);

    freopen("/dev/null", "wb", stderr);

    freopen("/dev/null", "rb", stdin);

} /* bfst_main_detach() */

/*

Function: bfst_main()

Description:

    Process command-line options and run the bfst_body module.

*/
int
bfst_main(
    int const argc,
    char const * const * const argv)
{
    struct bfst_options o_options;

    if (bfst_parse_options(&o_options, argc, argv))
    {
        if (!o_options.opt_debug)
        {
            bfst_main_detach();
        }

        struct bfst_body o_body;

        if (bfst_body_init(&o_body, &o_options))
        {
            bfst_body_run(&o_body);

            /* cleanup ... */
            bfst_body_cleanup(&o_body);
        }
    }

    return 0;

} /* bfst_main() */

/* end-of-file: bfst_main.c */
