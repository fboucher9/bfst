/* See LICENSE for license details. */

/*

Module: bfst_os.c

Description:

    OS-specific entry point.

*/

#include "bfst_os.h"

#include "bfst_main.h"

/*

Function: main()

Description:

    Standard C entry point.  Convert parameters and dispatch to portable entry point.

*/
int
main(
    int argc,
    char *argv[])
{
    return bfst_main(argc, (char const * const *)(argv));

} /* main() */

/* end-of-file: bfst_os.c */
