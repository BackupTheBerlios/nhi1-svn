/**
 *  \file       theLink/sqlite/config.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef CONFIG_H
#   define CONFIG_H

#include "mqconfig.h"

#if defined(_MSC_VER)
# define MQ_CDECL __cdecl
#else
# define MQ_CDECL
#endif


/*****************************************************************************/
/*                                                                           */
/*                                sqlite                                     */
/*                                                                           */
/*    this configuration is also used in ../sqlite/sqlit3.c to setup the     */
/*                     compile-time configuration                            */
/*                                                                           */
/*****************************************************************************/

// The SQLite source code contains literally thousands of assert() statements used to 
// verify internal assumptions and subroutine preconditions and postconditions. These 
// assert() statements are normally turned off (they generate no code) since turning 
// them on makes SQLite run approximately three times slower. But for testing and 
// analysis, it is useful to turn the assert() statements on. The SQLITE_DEBUG compile-
// time option does this. SQLITE_DEBUG also turns on some other debugging features. 
#ifdef _DEBUG
# define SQLITE_DEBUG 1
#endif

// The SQLITE_MEMDEBUG option causes an instrumented debugging memory allocator to be 
// used as the default memory allocator within SQLite. The instrumented memory allocator 
// checks for misuse of dynamically allocated memory. Examples of misuse include using 
// memory after it is freed, writing off the ends of a memory allocation, freeing memory 
// not previously obtained from the memory allocator, or failing to initialize newly 
// allocated memory. 
#ifdef _DEBUG
# define SQLITE_MEMDEBUG 1
#endif

// This option controls whether or not code is included in SQLite to enable it to operate 
// safely in a multithreaded environment. The default is SQLITE_THREADSAFE=1 which is safe 
// for use in a multithreaded environment. When compiled with SQLITE_THREADSAFE=0 all 
// mutexing code is omitted and it is unsafe to use SQLite in a multithreaded program. When 
// compiled with SQLITE_THREADSAFE=2, SQLite can be used in a multithreaded program so long 
// as no two threads attempt to use the same database connection at the same time.
#if defined(MQ_HAS_THREAD) && defined(HAVE_PTHREAD)
# define SQLITE_THREADSAFE 2
#else
# define SQLITE_THREADSAFE 0
#endif

// For backwards compatibility with older versions of SQLite that lack the sqlite3_initialize() 
// interface, the sqlite3_initialize() interface is called automatically upon entry to certain 
// key interfaces such as sqlite3_open(), sqlite3_vfs_register(), and sqlite3_mprintf(). The 
// overhead of invoking sqlite3_initialize() automatically in this way may be omitted by building 
// SQLite with the SQLITE_OMIT_AUTOINIT C-preprocessor macro. When built using SQLITE_OMIT_AUTOINIT, 
// SQLite will not automatically initialize itself and the application is required to invoke 
// sqlite3_initialize() directly prior to beginning use of the SQLite library.
#define SQLITE_OMIT_AUTOINIT 1

#endif /* CONFIG_H */

