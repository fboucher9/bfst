/* See LICENSE for license details. */

/*

Module: bfst_os.h

Description:

    Include of OS-specific header files.

*/

#if defined(INC_BFST_OS_H)
#error include bfst_os.h once
#endif /* #if defined(INC_BFST_OS_H) */

#define INC_BFST_OS_H

/* va_list */
#include <stdarg.h>

/* snprintf() */
#include <stdio.h>

/* unsetenv() */
#include <stdlib.h>

/* strerror() */
#include <string.h>

#include <stdint.h>

#include <limits.h>

#include <time.h>

#include <ctype.h>

#include <wchar.h>

/* errno */
#include <errno.h>

/* getuid() */
#include <unistd.h>

#include <locale.h>

/* signal() */
#include <signal.h>

#include <sys/wait.h>

/* getpwuid() */
#include <pwd.h>

#include <sys/select.h>

/* open() */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* openpty() */
#if   defined(__linux) || 1
 #include <pty.h>
#elif defined(__OpenBSD__) || defined(__NetBSD__) || defined(__APPLE__)
 #include <util.h>
#elif defined(__FreeBSD__) || defined(__DragonFly__)
 #include <libutil.h>
#endif

#include <X11/Xlib.h>

#include <X11/Xutil.h>

#include <X11/cursorfont.h>

#include <X11/Xatom.h>

/* end-of-file: bfst_os.h */
