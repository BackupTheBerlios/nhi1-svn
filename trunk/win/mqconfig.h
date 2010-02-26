/**
 *  \file       win/mqconfig.h
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Define if pointers to integers require aligned access */
/* #undef HAVE_ALIGNED_ACCESS_REQUIRED */

/* Define to 1 if you have the <dlfcn.h> header file. */
/* #undef HAVE_DLFCN_H */

/* Define to 1 if you have the `execvp' function. */
/* #undef HAVE_EXECVP */

/* Define to 1 if you have the `fork' function. */
/* #undef HAVE_FORK */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `posix_spawn' function. */
/* #undef HAVE_POSIX_SPAWN */

/* Define if you have POSIX threads libraries and header files. */
/* #undef HAVE_PTHREAD */

/* Define to 1 if you have the `signal' function. */
#define HAVE_SIGNAL 1

/* Define to 1 if you have the <signal.h> header file. */
#define HAVE_SIGNAL_H 1

/* Define to 1 if you have the `socketpair' function. */
/* #undef HAVE_SOCKETPAIR */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strcasecmp' function. */
/* #define HAVE_STRCASECMP */

/* Define to 1 if you have the <strings.h> header file. */
/* #define HAVE_STRINGS_H */

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strtod' function. */
#define HAVE_STRTOD 1

/* Define to 1 if you have the `strtof' function. */
/* #define HAVE_STRTOF 1 */

/* Define to 1 if you have the `strtoul' function. */
#define HAVE_STRTOUL 1

/* Define to 1 if you have the `strtoull' function. */
#define HAVE_STRTOULL 1

/* Define to 1 if you have the <sys/filio.h> header file. */
/* #undef HAVE_SYS_FILIO_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/*  to 1 if you have the `vfork' function. */
/* #undef HAVE_VFORK */

/* version of the 'libmsgque' package */
#define LIBMSGQUE_VERSION "4.5"

/* Define to 1 if you have the `_stricmp' function. */
#define HAVE__STRICMP 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* does the user require thread support */
#define MQ_HAS_THREAD /**/

/* Substitute for HUGE_VAL */
#define MQ_HUGE_VAL HUGE_VAL

/* Substitute for HUGE_VALF */
#define MQ_HUGE_VALF HUGE_VALF

/* is this a POSIX like system (e.g. UNIX) */
/* #undef MQ_IS_POSIX */

/* is this a WIN32 like system (e.g. WINDOWS) */
#define MQ_IS_WIN32 /**/

/* Name of package */
#define PACKAGE "NHI1"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "aotto1968@users.berlios.de"

/* Define to the full name of this package. */
#define PACKAGE_NAME "NHI1"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "NHI1 0.7"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "NHI1"

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.7"

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "0.7"

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif
