/****************************************************************************
 *
 *                OpenWatcom for OS/2 Misc Libray Routine
 *
 *  ========================================================================
 *
 *          Michael K Greene <greenemk@cox.net> September 2004
 *
 *            For use with OS/2 OpenWatom (www.openwatcom.org).
 *
 *  ========================================================================
 *
 * Description:  Handle some GNU items here so very little to change
 *               in other header files.
 *
 *               Fucntions:
 *               1. progname( )    return the executable name
 *               2. getos2path( )  pass env variable and filename return full
 *                                 string: getos2path("ETC", syslog.conf)
 *                                 return X:\MPTN\ETC\syslog.conf
 *               3. progname( )    return executable later in program using
 *                                 Watcom __argv:  call returns xxxxxx.exe
 *               4. getprogname( ) same as progname( )  OpenBSD
 *               5. strlcpy( )     safe strncpy( ) from OpenBSD
 *               6. strlcat( )     safe strncpy( ) from OpenBSD
 *
 ***************************************************************************/

#ifndef _COMPAT_H
#define _COMPAT_H

// Watcom has string.h not strings.h
#define HAVE_STRING_H

#define  __attribute__(x)        // not using GNU C, elide __attribute__
#define __THROW                  // __THROW not used so define it as nothing
#define __const const            // add #define __const cons to handle __const
#define __gnuc_va_list va_list   // so syslog.h vsyslog( ) can be called without error
#define __progname progname( )   // uses WATCOM __argv, returns executable name

#include <stdlib.h>
#include <sys/cdefs.h>  // handle the __BEGIN_DECLS and __END_DECLS
#define _POSIX_MAX_PATH  _MAX_PATH

__BEGIN_DECLS

extern char   *progname( void );
extern char   *getprogname( void );
extern char   *getos2path(char *sysvar, char *filename);
extern size_t strlcat(char *dst, const char *src, size_t size);
extern size_t strlcpy(char *dst, const char *src, size_t size);

__END_DECLS

#endif /* compat.h */

