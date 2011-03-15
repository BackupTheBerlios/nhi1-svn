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

// This macro determines if SQLite creates databases with the auto_vacuum flag set 
// by default to OFF (0), FULL (1), or INCREMENTAL (2). The default value is 0 
// meaning that databases are created with auto-vacuum turned off. In any case the 
// compile-time default may be overridden by the PRAGMA auto_vacuum command. 
#define SQLITE_DEFAULT_AUTOVACUUM 0

// This macro sets the default size of the page-cache for each attached database, 
// in pages. This can be overridden by the PRAGMA cache_size command. The default 
// value is 2000.
#define SQLITE_DEFAULT_CACHE_SIZE 2000

// The default schema-level file format used by SQLite when creating new database 
// files is set by this macro. The file formats are all very similar. The difference 
// between formats 1 and 4 is that format 4 understands descending indices and has a 
// tighter encoding for boolean values.
//
// SQLite (as of version 3.6.0) can read and write any file format between 1 and 4. 
// But older versions of SQLite might not be able to read formats greater than 1. So 
// that older versions of SQLite will be able to read and write database files created 
// by newer versions of SQLite, the default file format is set to 1 for maximum 
// compatibility.
//
// The file format for a new database can be set at runtime using the PRAGMA 
// legacy_file_format command. 
#define SQLITE_DEFAULT_FILE_FORMAT 4

// This option sets the size limit on rollback journal files in persistent journal mode. 
// When this compile-time option is omitted there is no upper bound on the size of the 
// rollback journal file. The journal file size limit can be changed at run-time using 
// the journal_size_limit pragma. 
//#define SQLITE_DEFAULT_JOURNAL_SIZE_LIMIT=

// This macro is used to determine whether or not the features enabled and disabled using 
// the SQLITE_CONFIG_MEMSTATUS argument to sqlite3_config() are available by default. 
// The default value is 1 (SQLITE_CONFIG_MEMSTATUS related features enabled). 
#define SQLITE_DEFAULT_MEMSTATUS 1

// This macro is used to set the default page-size used when a database is created. The 
// value assigned must be a power of 2. The default value is 1024. The compile-time 
// default may be overridden at runtime by the PRAGMA page_size command.
#define SQLITE_DEFAULT_PAGE_SIZE 1024

// This macro sets the default size of the page-cache for temporary files created by 
// SQLite to store intermediate results, in pages. It does not affect the page-cache 
// for the temp database, where tables created using CREATE TEMP TABLE are stored. 
// The default value is 500.
#define SQLITE_DEFAULT_TEMP_CACHE_SIZE 500

// This macro sets the maximum depth of the LALR(1) stack used by the SQL parser within 
// SQLite. The default value is 100. A typical application will use less than about 20 
// levels of the stack. Developers whose applications contain SQL statements that need 
// more than 100 LALR(1) stack entries should seriously consider refactoring their SQL 
// as it is likely to be well beyond the ability of any human to comprehend.
#define YYSTACKDEPTH 100

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

// When this option is defined in the amalgamation, version 3 of the full-text search 
// engine is added to the build automatically. 
#define SQLITE_ENABLE_FTS3

// This option causes SQLite to include support for the R*Tree index extension. 
//#define SQLITE_ENABLE_RTREE

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

// This option omits the entire extension loading mechanism from SQLite, including 
// sqlite3_enable_load_extension() and sqlite3_load_extension() interfaces. 
#define SQLITE_OMIT_LOAD_EXTENSION 1

// For backwards compatibility with older versions of SQLite that lack the sqlite3_initialize() 
// interface, the sqlite3_initialize() interface is called automatically upon entry to certain 
// key interfaces such as sqlite3_open(), sqlite3_vfs_register(), and sqlite3_mprintf(). The 
// overhead of invoking sqlite3_initialize() automatically in this way may be omitted by building 
// SQLite with the SQLITE_OMIT_AUTOINIT C-preprocessor macro. When built using SQLITE_OMIT_AUTOINIT, 
// SQLite will not automatically initialize itself and the application is required to invoke 
// sqlite3_initialize() directly prior to beginning use of the SQLite library.
#define SQLITE_OMIT_AUTOINIT 1

#endif /* CONFIG_H */

