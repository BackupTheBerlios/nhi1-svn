/**
 *  \file       theLink/libmsgque/msgque.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef MQ_MSGQUE_H
#define MQ_MSGQUE_H

#if defined(_MSC_VER)

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                   Visual C++ Express-Edition                    ### */
/* ###                                                                 ### */
/* ####################################################################### */

# define __attribute__(dummy)
# define pid_t int
# define mode_t int
# define __func__ __FUNCTION__
# define va_copy(a,b) a = b
# define mq_strtoll _strtoi64
# define mq_strdup(v) _strdup(v)
# define mq_getpid _getpid
# define mq_unlink _unlink
# define mq_snprintf _snprintf
# define MQ_CLRCALL __clrcall
# define MQ_STDCALL __stdcall
# define MQ_CDECL __cdecl
# define mq_inline __inline

#else      

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                             GCC                                 ### */
/* ###                                                                 ### */
/* ####################################################################### */

# define mq_strtoll strtoll
# define mq_strdup(v) strdup(v)
# define mq_getpid getpid
# define mq_unlink unlink
# define mq_snprintf snprintf
# define MQ_CLRCALL
# define MQ_STDCALL
# define MQ_CDECL
# define mq_inline inline

#endif


#if defined(_MANAGED)
# define MQ_DECL MQ_CLRCALL
#elif defined(MQ_COMPILE_AS_CC)
# define MQ_DECL MQ_STDCALL
#else
# define MQ_DECL MQ_CDECL
#endif


/* ####################################################################### */
/* ###                                                                 ### */
/* ###                      M S G Q U E - A P I                        ### */
/* ###                                                                 ### */
/* ####################################################################### */

/** \defgroup type_API PUBLIC: type definition
 *  \ingroup lib
 *  \{
 *  \brief a collection of common used definitions
 */

/* BEGIN_C_DECLS should be used at the beginning of your declarations,
  so that C++ compilers don't mangle their names.  Use END_C_DECLS at
  the end of C declarations. */

#undef BEGIN_C_DECLS
#undef END_C_DECLS
#if defined(MQ_COMPILE_AS_CC)
# define BEGIN_C_DECLS namespace libmsgque {
# define END_C_DECLS }
#elif defined(__cplusplus)
# define BEGIN_C_DECLS extern "C" {
# define END_C_DECLS }
#else
# define BEGIN_C_DECLS /* empty */
# define END_C_DECLS /* empty */
#endif

/* Somewhere in the middle of the GCC 2.96 development cycle, we implemented
   a mechanism by which the user can annotate likely branch directions and
   expect the blocks to be reordered appropriately.  Define __builtin_expect
   to nothing for earlier compilers.  */

#if defined(_MSC_VER) || (__GNUC__ == 2 && __GNUC_MINOR__ < 96)
#   define __builtin_expect(x, expected_value) (x)
#endif

/// \brief gcc jump optimization
#define likely(x)       __builtin_expect((x),1)

/// \brief gcc jump optimization
#define unlikely(x)     __builtin_expect((x),0)

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <limits.h>
#ifndef _MSC_VER
#  include <unistd.h>
#endif
#include <string.h>

#if defined(_MSC_VER)
#include <WinSock2.h>
/// \brief helper to use the timeval
#define mq_timeval timeval

/// \brief \helper to use timezone
struct mq_timezone {
  int  tz_minuteswest;      ///< minutes W of Greenwich
  int  tz_dsttime;          ///< type of dst correction
};
#else
#define mq_timeval timeval
#define mq_timezone timezone
struct mq_timeval;
struct mq_timezone;
#endif

//start c++ save definition
BEGIN_C_DECLS

//15 min timeout
#ifndef MQ_TIMEOUT
/// \brief maximum timeout in sec (900 sec)
#define MQ_TIMEOUT	900
/// \brief long timeout in sec (180 sec)
#define MQ_TIMEOUT5	(MQ_TIMEOUT/5)
/// \brief normal timeout in sec (90 sec)
#define MQ_TIMEOUT10	(MQ_TIMEOUT/10  < 1 ? 1 : MQ_TIMEOUT/10)
/// \brief short timeout in sec (20 sec)
#define MQ_TIMEOUT45	(MQ_TIMEOUT/45  < 1 ? 1 : MQ_TIMEOUT/45)
/// \brief shorter timeout in sec (10 sec)
/// This TIMEOUT is used for socket connection with 'connect'
#define MQ_TIMEOUT90	(MQ_TIMEOUT/90  < 1 ? 1 : MQ_TIMEOUT/90)
/// \brief very short timeout in sec (5 sec)
#define MQ_TIMEOUT180	(MQ_TIMEOUT/180 < 1 ? 1 : MQ_TIMEOUT/180)
/// \brief request the user defined timeout value from the argument of the \c --timeout option
#define MQ_TIMEOUT_USER -2
#endif

/// \ingroup error_api
/// \brief panic on error
///
/// This item is used as special meaning for the \c struct \c MqErrorS 
/// argument of error-functions
#define MQ_ERROR_PANIC ((struct MqS*)NULL)

/// \ingroup error_api
/// \brief ignore error and do not generate any error-text (don't fill the error object)
///
/// This item is used as special meaning for the \c struct \c MqErrorS 
/// argument of error-functions
#define MQ_ERROR_IGNORE ((struct MqS*)0x1)

/// \ingroup error_api
/// \brief print error to stderr
///
/// This item is used as special meaning for the \c struct \c MqErrorS 
/// argument of error-functions
#define MQ_ERROR_PRINT ((struct MqS*)0x2)

/// \ingroup error_api
/// \brief check if the error pointer is a \e real pointer or just a flag
#define MQ_ERROR_IS_POINTER(e) (e>MQ_ERROR_PRINT)

/*****************************************************************************/
/*                                                                           */
/*                          windows conform extern                           */
/*                                                                           */
/*****************************************************************************/

// if the source of libmsgque is direct used in external library
// (without dynamic linking) and this library should *not* export
// the symbol's of libmsgque then the flag MQ_IGNORE_EXTERN have
// to be set
#if defined(PIC) && !defined(MQ_IGNORE_EXTERN)
    // does we build a DLL ?
#   ifdef DLL_EXPORT
        // does we build the libmsgque library ?
#	ifdef MQ_BUILD_LIBMSGQUE_DLL
#	    define MQ_EXTERN __declspec(dllexport)
#	else
#	    define MQ_EXTERN __declspec(dllimport)
#	endif
#   else
      // no DLL
      /// \brief architecture specific extern specifier
#     define MQ_EXTERN __attribute__ ((visibility("default")))
#   endif
#else
/// define the external binding
#   define MQ_EXTERN
#endif

/*****************************************************************************/
/*                                                                           */
/*                            types/definition                               */
/*                                                                           */
/*****************************************************************************/

/// \brief connecting of expressions, step 1
#define MQ_CPPXSTR(s) MQ_CPPSTR(s)
/// \brief connecting of expressions, step 2
#define MQ_CPPSTR(s) #s

/// \brief MQ_ALFA the command-line separator between server and client
#define MQ_ALFA '@'
#define MQ_ALFA_STR "@"

struct MqTransS;
struct MqErrorS;
struct MqBufferLS;
struct MqBufferS;
union  MqBufferU;
struct MqS;
struct MqConfigS;
struct MqEventS;
struct MqFactoryS;

/*****************************************************************************/
/*                                                                           */
/*                            types/generic                                  */
/*                                                                           */
/*****************************************************************************/

/// 1 byte \b byte data-type
typedef signed char MQ_BYT;
/// 1 byte \b boolean data-type
typedef signed char MQ_BOL;
/// 2 byte \b short data-type
typedef short MQ_SRT;
/// 4 byte \b integer data-type
typedef int MQ_INT;
/// 4 byte \b float data-type
typedef float MQ_FLT;
/// 8 byte \b wide integer data-type
#if defined(_MSC_VER)
typedef __int64 MQ_WID;
#else
typedef long long int MQ_WID;
#endif
/// 8 byte \b double data-type
typedef double MQ_DBL;
/// 8 byte \b atom data-type
typedef union MqBufferAtomU MQ_ATO;

/// array size data-type
/// \attention -> need signed integer in pReadWord
///            -> need int because of pSwapI in pReadHDR
typedef int MQ_SIZE;

/// \brief OS wide MQ_TIME_T
typedef MQ_WID MQ_TIME_T;

/// handle data-type
typedef int MQ_HDL;

/// pointer basic data-type
typedef void MQ_PTRB;
/// string basic data-type
typedef char MQ_STRB;
/// binary basic data-type
typedef unsigned char MQ_BINB;
/// list basic data-type
typedef unsigned char MQ_LSTB;

/// generic pointer data-type
typedef MQ_PTRB *MQ_PTR;
/// string pointer data-type
typedef MQ_STRB *MQ_STR;
/// const string pointer data-type
typedef MQ_STRB const * MQ_CST;
/// binary pointer data-type
typedef MQ_BINB *MQ_BIN;
/// list pointer data-type
typedef MQ_LSTB *MQ_LST;
/// Buffer pointer data-type
typedef struct MqBufferS *MQ_BUF;
/// \brief data type for a socket handle
typedef MQ_INT  MQ_SOCK;

/*****************************************************************************/
/*                                                                           */
/*                            enum definitions                               */
/*                                                                           */
/*****************************************************************************/

/// \ingroup msgque_api
/// \brief 'select' type for reading or writing
enum MqIoSelectE {
  MQ_SELECT_RECV = (1 << 0),    ///< 'select' for reading
  MQ_SELECT_SEND = (1 << 1),	///< 'select' for writing
};

/// \ingroup msgque_api
/// \brief boolean NO
#define MQ_NO  0

/// \ingroup msgque_api
/// \brief boolean YES
#define MQ_YES 1

/// \ingroup event_api
/// \brief wait for an event?
enum MqWaitOnEventE {
  MQ_WAIT_NO      = 0,	///< just do the check
  MQ_WAIT_ONCE    = 1,	///< wait for one new event
  MQ_WAIT_FOREVER = 2,	///< wait forever
};

/// \ingroup buffer_api
/// \brief the type is native and has a size of 1 byte
#define MQ_TYPE_IS_1_BYTE   (1<<0)

/// \ingroup buffer_api
/// \brief the type is native and has a size of 2 bytes
#define MQ_TYPE_IS_2_BYTE   (1<<1)

/// \ingroup buffer_api
/// \brief the type is native and has a size of 4 bytes
#define MQ_TYPE_IS_4_BYTE   (1<<2)

/// \ingroup buffer_api
/// \brief the type is native and has a size of 8 bytes
#define MQ_TYPE_IS_8_BYTE   (1<<3)

/// \ingroup buffer_api
/// \brief the type is native
#define MQ_TYPE_IS_NATIVE   (	MQ_TYPE_IS_1_BYTE | MQ_TYPE_IS_2_BYTE |	    \
				MQ_TYPE_IS_4_BYTE | MQ_TYPE_IS_8_BYTE	)

/// \ingroup buffer_api
/// \brief a enum for all data types supported by #MqBufferS
///
/// The type is a one byte character with the following syntax:
/// - bit 1 up to 4 has the size of the native type
/// - bit 5 up to 8 has the type, up to 16 types are allowed
///
/// \attention In the package the space for the type is only \e one char. If additional
/// space is needed the protocol have to be adjusted
enum MqTypeE {
  MQ_BYTT = (1<<4 | MQ_TYPE_IS_1_BYTE),  ///< Y: 1 byte 'byte' type
  MQ_BOLT = (2<<4 | MQ_TYPE_IS_1_BYTE),  ///< O: 1 byte 'boolean' type
  MQ_SRTT = (3<<4 | MQ_TYPE_IS_2_BYTE),  ///< S: 2 byte 'short' type
  MQ_INTT = (4<<4 | MQ_TYPE_IS_4_BYTE),  ///< I: 4 byte 'int' type
  MQ_FLTT = (5<<4 | MQ_TYPE_IS_4_BYTE),  ///< F: 4 byte 'float' type
  MQ_WIDT = (6<<4 | MQ_TYPE_IS_8_BYTE),  ///< W: 8 byte 'long long int' type
  MQ_DBLT = (7<<4 | MQ_TYPE_IS_8_BYTE),  ///< D: 8 byte 'double' type
  MQ_BINT = (8<<4                    ),  ///< B: binary array type
  MQ_STRT = (9<<4                    ),  ///< C: character array type (e.g. with a \\0 at the end)
  MQ_LSTT = (10<<4                   ),  ///< L: list object type
  MQ_RETT = (11<<4                   )   ///< R: return object type
};

/// \ingroup buffer_api
/// \brief allocation style used for the data-segment in #MqBufferS.
enum MqAllocE {
  MQ_ALLOC_STATIC     = 0,	///< opposite from MQ_ALLOC_DYNAMIC)
  MQ_ALLOC_DYNAMIC    = 1,	///< dynamic allocation (e.g. MqSysMalloc, ...)
};

/// \ingroup error_api
/// \brief collection for the different error-codes
enum MqErrorE {
  MQ_OK		= 0,		///< everything is OK, no error available (persistent)
  MQ_CONTINUE	= 1,            ///< continue with upper code
  MQ_ERROR	= 2,            ///< exit upper code with an error (persistent)
  MQ_EXIT	= 3		///< exit parent context
};

/** \} type_api */

/*****************************************************************************/
/*                                                                           */
/*                           C O N F I G - API                               */
/*                                                                           */
/*****************************************************************************/

/** \defgroup config_api PUBLIC: object configuration
 *  \ingroup lib_api
 *  \{
 *  \brief Configuration data of the #MqS object
 */

/// \brief prototype for the \c fork syscall
typedef pid_t (MQ_DECL *MqForkF) (void);

/// \brief prototype for the \c vfork syscall
typedef pid_t (MQ_DECL *MqVForkF) (void);

/// \brief the prototype for a ContextCreate function
///
/// This function is the MqS::fCreate and MqS::fThreadCreate parameter and is used to :
/// -# create a \e child context
/// -# create a \e parent context as the thread entry point
/// .
typedef enum MqErrorE ( MQ_DECL
  *MqCreateF
) (
  struct MqS  *,		    ///< [in] context configuration (NULL is not allowed)
  struct MqBufferLS **		    ///< [in,out] this is the \e args buffer-list pointer
) __attribute__((nonnull (1)));

/// \brief the prototype for a ContextDelete function
///
/// This function is the MqS::fDelete and MqS::fThreadDelete parameter and is used to:
/// -# delete a \e child context
/// -# delete a \e parent context as the thread exit point
/// .
typedef void ( MQ_DECL
  *MqDeleteF
) (
  struct MqS * const context
);

/// \brief prototype for an object method function
typedef enum MqErrorE ( MQ_DECL
  *MqTokenF
) (
  struct MqS * const context,
  MQ_PTR const data
);

/// \brief prototype for a free additional token data function
typedef void ( MQ_DECL
  *MqTokenDataFreeF
) (
  struct MqS const * const context,
  MQ_PTR *dataP
);

/// \brief prototype for a copy additional token data function
typedef enum MqErrorE ( MQ_DECL
  *MqTokenDataCopyF
) (
  struct MqS * const context,
  MQ_PTR *dataP
);

/// \brief prototype for a Event-Checking function
/// \ingroup event_api
typedef void ( MQ_DECL
  *MqEventF
) (
  struct MqS * const
);

/// \brief application specific help function type
typedef void ( MQ_DECL
  *MqHelpF
) (
  MQ_CST 		///< the basename of the tool
) __attribute__ ((noreturn));

/// \brief the \e factory is called to create an object for ...
enum MqFactoryE {
  MQ_FACTORY_NEW_INIT	= 0,
  MQ_FACTORY_NEW_CHILD	= 1,
  MQ_FACTORY_NEW_SLAVE	= 2,
  MQ_FACTORY_NEW_FORK	= 3,
  MQ_FACTORY_NEW_THREAD	= 4,
  MQ_FACTORY_NEW_FILTER	= 5
};

/// \brief prototype for a Object-Creation factory function
typedef enum MqErrorE ( MQ_DECL
  *MqFactoryCreateF
) (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  MQ_PTR  data,
  struct MqS  ** contextP
);

/// \brief prototype for a Object-Delete factory function
typedef void ( MQ_DECL
  *MqFactoryDeleteF
) (
  struct MqS  * context,
  MQ_BOL doFactoryCleanup,
  MQ_PTR data
);

/// \brief Return a \e main factory function found by \e name
/// \param[in] name the string name of the function to select
///
/// This function have to be defined in the target tool (example: \c atool.c)
/// and have to return the \e main function of type #MqFactoryS,
/// The result is used as Entry-Point for \e fork and \e thread startup.
typedef struct MqFactoryS ( MQ_DECL
    *MqFactorySelectorF
) (
    MQ_CST name
);

/// \brief used for factory function pointer management
struct MqFactoryS {
  /// \brief function pointer to \e create an object
  struct MqFactoryCreateS {
    MqFactoryCreateF	fCall;	    ///< create a new object
    MQ_PTR		data;	    ///< additional data pointer for the fCall
    MqTokenDataFreeF	fFree;	    ///< free additional data pointer
    MqTokenDataCopyF	fCopy;	    ///< copy additional data pointer
  } Create;
  /// \brief function pointer to \e delete an object
  struct MqFactoryDeleteS {
    MqFactoryDeleteF	fCall;	    ///< delete the object created with #MqFactoryS::MqFactoryCreateS
    MQ_PTR		data;	    ///< additional data pointer for the fCreate
    MqTokenDataFreeF	fFree;	    ///< free additional data pointer
    MqTokenDataCopyF	fCopy;	    ///< copy additional data pointer
  } Delete;
};

#define MqFactoryS_NULL { {NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL} }

/// \brief prototype for exit a process or thread
typedef void ( MQ_DECL
  *MqExitF
) (
  int num
);

/// \brief used to setup (initialize) a new thread/fork/process created by \libmsgque
///        using the \c SysServer? style commands
typedef void (MQ_DECL *MqSetupF) ( struct MqS * const );

/// \brief Information about how the \e context was created
enum MqStatusIsE {
  MQ_STATUS_IS_INITIAL = 0,	    ///< context is the \e first context
  MQ_STATUS_IS_DUP     = 1<<0,	    ///< context is created as a duplicate of an other context
  MQ_STATUS_IS_THREAD  = 1<<1,	    ///< context is created as a thread
  MQ_STATUS_IS_FORK    = 1<<2,	    ///< context is created as a fork
  MQ_STATUS_IS_SPAWN   = 1<<3,	    ///< context is created as a spawn
};

/// \brief User preferences on HOWTO start a new entity
enum MqStartE {
  MQ_START_DEFAULT     = 0,	    ///< use application-context default entity creation
  MQ_START_FORK        = 1,	    ///< create entity as \e fork
  MQ_START_THREAD      = 2,	    ///< create entity as \e thread
  MQ_START_SPAWN       = 3,	    ///< create entity as \e spawn process
};

/// \brief used for callback function pointer management
struct MqCallbackS {
  MqTokenF		fFunc;	    ///< callback method
  MQ_PTR		data;	    ///< additional data for the callback function
  MqTokenDataFreeF	fFree;	    ///< free additional data pointer
  MqTokenDataCopyF	fCopy;	    ///< copy additional data pointer, used in a #MqSetupDup
};

/// \brief used to Create/Delete of object
struct MqLinkSetupS {
  MqCreateF		fCreate;    ///< create a new object
  MqDeleteF		fDelete;    ///< delete a object
};

/// \brief what kind of socket interface to use?
enum MqIoComE { 
#if defined(MQ_IS_POSIX)
    MQ_IO_UDS, 
#endif
    MQ_IO_TCP, 
    MQ_IO_PIPE 
};

struct MqIoUdsConfigS {
  MQ_BUF file;
};

struct MqIoTcpConfigS {
  struct MqBufferS * host;	    ///< host name
  struct MqBufferS * port;	    ///< port name
  struct MqBufferS * myhost;	    ///< CLIENT: my host name
  struct MqBufferS * myport;	    ///< CLIENT: my port name
};

struct MqIoPipeConfigS {
  MQ_SOCK socks[2] ;		    ///< the result from socketpair
};

struct MqIoConfigS {

  /// \brief The timeout is used for all kind of low-level socket operations like \c send, \c recv and \c connect
  /// <TABLE>
  /// <TR>  <TH>type</TH>          <TH>default</TH>       <TH>option</TH>    <TH>application</TH>   <TH>context</TH>  </TR>
  /// <TR>  <TD>INTEGER (sec)</TD> <TD>MQ_TIMEOUT180</TD> <TD>--timeout</TD> <TD>server/client</TD> <TD>parent</TD>   </TR>
  /// </TABLE>
  /// \attention This \c timeout is independent of the per transaction timeout of #MqSendEND_AND_WAIT.
  MQ_TIME_T timeout;

  /// \brief OS specific value for read socket operation buffer
  /// <TABLE>
  /// <TR>  <TH>type</TH>           <TH>default</TH> <TH>option</TH>       <TH>application</TH>   <TH>context</TH>  </TR>
  /// <TR>  <TD>INTEGER (byte)</TD> <TD>4096</TD>    <TD>--buffersize</TD> <TD>server/client</TD> <TD>parent</TD>   </TR>
  /// </TABLE>
  MQ_INT buffersize;

  /// \brief what kind of socket interface to use?
  /// <TABLE>
  /// <TR>  <TH>type</TH>           <TH>default</TH> <TH>option</TH>             <TH>application</TH>   <TH>context</TH>  </TR>
  /// <TR>  <TD>INTEGER (enum)</TD> <TD>IO_PIPE</TD> <TD>--tcp/--uds/--pipe</TD> <TD>server</TD>	  <TD>parent</TD>   </TR>
  /// </TABLE>
  enum MqIoComE com;

  struct MqIoUdsConfigS	  uds;
  struct MqIoTcpConfigS	  tcp;
  struct MqIoPipeConfigS  pipe;
};

/// \brief end-user configuration data, also available as command-line options
struct MqConfigS {

  /// \brief Suppress logging output
  /// <TABLE>
  /// <TR>  <TH>type</TH> <TH>default</TH> <TH>option</TH>    <TH>application</TH>   <TH>context</TH>        </TR>
  /// <TR>  <TD>BOOL</TD> <TD>NO</TD>      <TD>--silent</TD>  <TD>server/client</TD> <TD>parent/child</TD>   </TR>
  /// </TABLE>
  MQ_BOL isSilent;

  /// \brief Use \e string mode, send as many data as possible as string
  /// <TABLE>
  /// <TR>  <TH>type</TH> <TH>default</TH> <TH>option</TH>    <TH>application</TH>   <TH>context</TH>        </TR>
  /// <TR>  <TD>BOOL</TD> <TD>NO</TD>      <TD>--string</TD>  <TD>client</TD>        <TD>parent</TD>         </TR>
  /// </TABLE>
  ///
  /// \libmsgque distinguish between a \e string based (option: \c --string) and a
  /// \e binary based (this is the default) data packages. The items is a \libmsgque data
  /// package are of type #MqBufferU including OS specific native data of type #MQ_ATO.
  /// The native data can be transmitted as \e string (slow) or as \e binary (fast).
  /// \e string is used for debugging purpose or if the binary data model is different
  /// between client and server. The difference in endianness (http://en.wikipedia.org/wiki/Endian)
  /// is handled by \libmsgque and does not require using the \c --string option.
  /// The options is only available for the \e client-parent setup.
  MQ_BOL isString;

  /// \brief User preferences on HOWTO start a new entity
  /// <TABLE>
  /// <TR>  <TH>type</TH> <TH>default</TH>	     <TH>option</TH>   <TH>application</TH>   <TH>context</TH> </TR>
  /// <TR>  <TD>ENUM</TD> <TD>#MQ_START_DEFAULT</TD> <TD>NO</TD>       <TD>client/server</TD> <TD>parent</TD>  </TR>
  /// <TR>  <TD>ENUM</TD> <TD>#MQ_START_FORK</TD>    <TD>--fork</TD>   <TD>client/server</TD> <TD>parent</TD>  </TR>
  /// <TR>  <TD>ENUM</TD> <TD>#MQ_START_THREAD</TD>  <TD>--thread</TD> <TD>client/server</TD> <TD>parent</TD>  </TR>
  /// <TR>  <TD>ENUM</TD> <TD>#MQ_START_SPAWN</TD>   <TD>--spawn</TD>  <TD>client/server</TD> <TD>parent</TD>  </TR>
  /// </TABLE>
  ///
  /// The #MQ_START_DEFAULT depend on the application-context:
  /// - <I>server setup</I>\n Start a \e UDP or \e TCP server 
  ///   capable to serve \b one incoming \e client connection.
  /// - <I>client setup</I>\n Start a \e PIPE server using the following order:
  ///   - \e fork   (if the \c fork system-call is available)
  ///   - \e thread (if \libmsgque was compiled with \c --enable-threads)
  ///   - \e spawn  (this always works)
  ///   .
  /// .
  /// \attention In \e server mode the \c --fork , \c --thread and the \c --spawn options require the 
  ///   #MqSetupS::Parent - #MqLinkSetupS::fCreate and #MqSetupS::Parent - #MqLinkSetupS::fDelete value to 
  ///   provide an entry-point after the new entity was created. In addition the option 
  ///   #MqSetupS::fProcessExit or #MqSetupS::fThreadExit is used to exit the entity.
  /// \attention The \c --fork option require the \b fork system-call and is \b not compatible with
  ///   threads.
  enum MqStartE startAs;

/** \brief The human-readable name of context
 *  <TABLE>
 *  <TR>  <TH>type</TH>   <TH>default</TH> <TH>option</TH>  <TH>application</TH>   <TH>context</TH>   </TR>
 *  <TR>  <TD>STRING</TD> <TD>unknown</TD> <TD>--name</TD>  <TD>server/client</TD> <TD>parent</TD>    </TR>
 *  </TABLE>
 * 
 *  The name is used as prefix for application specific log and error messages:
\verbatim
C> (NAME) [2009-01-12:16-22-27] [4-0-sIoCheckArg]: option:  io->com = PIPE
\endverbatim
 *  and is created using the following steps:
 *  - initial set to \e unknown
 *  - the argument of the \c --name option in \e argv
 *  - the #MqConfigS::name entry from the related configuration object
 *  - the basename of the first option in \e argv
 *  .
 *  \attention 
 *     - the memory of the \e name data-entry is managed by \libmsgque and freed during
 *       deleting of the #MqS object. A Value of \c NULL is allowed. 
 *     - the \e name data-entry will be replaced by the argument of the \c --srvname command-line
 *       option of the client.
 *     - to initialize this value with a static string use: \code
MqConfigSetName(context, "myString");
\endcode
 *     .
 */
  MQ_STR name;

/** \brief The human-readable name of the server-context
 *  <TABLE>
 *  <TR>  <TH>type</TH>   <TH>default</TH> <TH>option</TH>  <TH>application</TH>   <TH>context</TH>   </TR>
 *  <TR>  <TD>STRING</TD> <TD>unknown</TD> <TD>--srvname</TD>  <TD>client</TD> <TD>parent/child</TD>    </TR>
 *  </TABLE>
 * 
 *  The srvname is used as prefix for the server-application specific log and error messages:
\verbatim
C> (NAME) [2009-01-12:16-22-27] [4-0-sIoCheckArg]: option:  io->com = PIPE
\endverbatim
 *  and is created using the following steps:
 *  - initial set to \e unknown
 *  - the argument of the \c --srvname option in \e argv
 *  - the #MqConfigS::srvname entry from the related configuration object
 *  .
 *  \attention 
 *     - the memory of the \e srvname data-entry is managed by \libmsgque and freed during
 *       deleting of the #MqS object. A Value of \c NULL is allowed.
 *     - to initialize this value with a static string use: \code
MqConfigSetSrvName(context, "myString");
\endcode
 *     .
 */
  MQ_STR srvname;

  /// \brief Send additional debugging output to stderr
  /// <TABLE>
  /// <TR>  <TH>type</TH>          <TH>default</TH> <TH>option</TH>  <TH>application</TH>   <TH>context</TH>        </TR>
  /// <TR>  <TD>INTEGER (0-9)</TD> <TD>0</TD>       <TD>--debug</TD> <TD>server/client</TD> <TD>parent/child</TD>   </TR>
  /// </TABLE>
  /// \attention Using pipe (e.g. --pipe) the \e server get the \e debug flag from the \e client.
  MQ_INT debug;

  /// \brief Create a child-context
  /// <TABLE>
  /// <TR>  <TH>type</TH>    <TH>default</TH> <TH>option</TH> <TH>application</TH>  <TH>context</TH>  </TR>
  /// <TR>  <TD>POINTER</TD> <TD>NULL</TD>    <TD>NO</TD>       <TD>client</TD>       <TD>parent</TD>   </TR>
  /// </TABLE>
  ///
  /// Read more in: \main_cspc
  struct MqS * parent;

  /// \brief SLAVE: a  pointer to the master object or \c NULL
  struct MqS * master;

  /// \brief SLAVE: the ID in the master SLAVES array or \c O
  MQ_SIZE master_id;

  /// \brief do not allow the usage of \e threads
  MQ_BOL ignoreThread;

  /// \brief do not allow the usage of \e spawn
  MQ_BOL ignoreSpawn;

  /// \brief do not allow the usage of \e fork
  MQ_BOL ignoreFork;

  /// \brief Global configuration data belonging to "io"
  struct MqIoConfigS io;
};

/// \brief application-programmer configuration data
struct MqSetupS {

  /// \brief application identifer
  ///
  /// The application \e identifer is used to modify the client or filter behaviour depending on the server \e identifer.
  /// The server set the \e identifer using #MqConfigSetIdent and the client ask for the identifer of the
  /// server using \e #MqConfigGetIdent usually used in the client configuration setup code. The \e identifer
  /// is \b not changeable by the user, like the \e name configuration option, because this is a "build-in" 
  /// feature set by the \e programmer.
  MQ_STR ident;
 
  /// \brief setup/cleanup a \e CHILD object
  /// \attention always call this functions using #MqLinkCreate and #MqLinkDelete
  struct MqLinkSetupS Child;

  /// \brief setup/cleanup a \e PARENT object
  /// \attention always call this functions using #MqLinkCreate and #MqLinkDelete
  struct MqLinkSetupS Parent;

  /// \brief pointer to the background error service
  ///
  /// A background error is an error without an link to an workflow. The error happen if an #MqSendEND
  /// call fails or if an othe asyncrone task fails. if the callback is \b not specified the error
  /// is printed to stderr but no error if left in the context. if the callback is defiend the
  /// context is set to the error and the callback is called to process this error. On the callback
  /// the error can be cleared up with #MqErrorReset.
  struct MqCallbackS BgError;

  /// \brief Create a \e server context
  /// <TABLE>
  /// <TR>  <TH>type</TH> <TH>default</TH> <TH>option</TH> <TH>application</TH>   <TH>context</TH>        </TR>
  /// <TR>  <TD>BOOL</TD> <TD>NO</TD>      <TD>NO</TD>     <TD>server</TD>        <TD>parent</TD>         </TR>
  /// </TABLE>
  ///
  /// If the \e server option is available the #MqLinkCreate will create an \libmsgque object usable
  /// as a server. For detail information read: \main_cspc
  MQ_BOL isServer;

  /// \brief pointer to the Server-Setup function
  ///
  /// This Server-Setup function is used to configure a new server-link and act like a
  /// constructor. This function is called on the end of #MqLinkCreate. A server-context-link is created 
  /// for every new incomming connection request and is used to provide context specific services .
  struct MqCallbackS ServerSetup;

  /// \brief pointer to the Server-Cleanup function
  ///
  /// This Server-Cleanup function is used to cleanup a server-context-link and act like a
  /// destructor. The function is called on the begin of #MqLinkDelete.
  struct MqCallbackS ServerCleanup;

  // misc

  /// \brief Create a link to the calling tool event-handling queue
  /// <TABLE>
  /// <TR>  <TH>type</TH>    <TH>default</TH> <TH>option</TH> <TH>application</TH>   <TH>context</TH>  </TR>
  /// <TR>  <TD>POINTER</TD> <TD>NULL</TD>    <TD>NO</TD>     <TD>server/client</TD> <TD>parent</TD>   </TR>
  /// </TABLE>
  ///
  /// Event-Handling is used to process instructions in the \e background to give the tool-user
  /// the feeling of a non-blocking application. For Example Tcl using the event-handling to
  /// update the Tk user-interface while the application is waiting for data. The event-handling
  /// is usually a loop to check event-sources from time-to-time and act on incoming events.
  MqEventF fEvent;

  /// \brief function pointers used to create and delete an object or a class instance
  /// <TABLE>
  /// <TR>  <TH>type</TH> <TH>default</TH> <TH>option</TH> <TH>application</TH>   <TH>context</TH>  </TR>
  /// <TR>  <TD>DATA</TD> <TD>NULL</TD>    <TD>NO</TD>     <TD>server/client</TD> <TD>parent</TD>   </TR>
  /// </TABLE>
  ///
  /// The \e factory pattern is used to:
  ///  - create a new object (C, TCL) or a new class instance (C++, C#, JAVA, PYTHON, PERL, VB.NET)
  ///  - create a \e server-child object
  ///  - create a \e server-parent object using the \e --thread startup qualification
  ///  - create a \e slave-worker-parent object
  ///  - create a \e slave-child object
  ///  - create a \e filter-parent object using the \e --fork or \e --thread startup qualification
  ///  .
  struct MqFactoryS Factory;

  /// \brief setup and initialize a thread befor a new thread is created by \libmsgque
  MqSetupF fThreadInit;

  /// \brief setup and initialize a fork-process befor a new fork-process is created by \libmsgque
  MqSetupF fForkInit;

  /// \brief exit/cleanup a process
  MqExitF fProcessExit;

  /// \brief exit/cleanup a thread
  MqExitF fThreadExit;

  /// \brief Create a link to the calling tool \e help function
  /// <TABLE>
  /// <TR>  <TH>type</TH>    <TH>default</TH> <TH>option</TH> <TH>application</TH>   <TH>context</TH>  </TR>
  /// <TR>  <TD>POINTER</TD> <TD>NULL</TD>    <TD>NO</TD>     <TD>server/client</TD> <TD>parent</TD>   </TR>
  /// </TABLE>
  ///
  /// As a service to the programmer the \libmsgque startup code in #MqLinkCreate checks for the
  /// both options \c -h and \c --help to provide a tool-specific help-page and exit.
  MqHelpF fHelp;
};

/// \ingroup msgque_api
/// \brief the main \libmsgque object.
///
/// \copydoc msgque_api
///
struct MqLinkS {

  // private variables
  struct MqSendS  * send;	    ///< object for sending a Msgque packet
  struct MqReadS  * read;	    ///< object for reading a Msgque packet
  struct MqIoS    * io;		    ///< object for management of the 'socket' infrastructure

  // private variables
  MQ_BOL endian;		    ///< a endian switch have to be done? (boolean: MQ_YES or MQ_NO)

  // context-management variables
  MQ_SIZE   ctxId;		    ///< the ctxId of this MqS object
  struct MqS *ctxIdP;		    ///< the inital (first) context (home of the ctxIdA)

  // private variables
  struct MqTokenS * srvT;	    ///< identifer for the 'service' token handle

  MQ_BOL onExit;		    ///< is allready an exit ongoing?
  struct MqS * exitctx;		    ///< msgque object got and "_SHD" request (only used at the parent)
  MQ_BOL onCreate;		    ///< is already an "create" ongoing?
  MQ_BOL MqLinkDelete_LOCK;	    ///< is already a "delete" ongoing?
  MQ_BOL deleteProtection;	    ///< object in use -> delete is not allowed
  MQ_BOL onShutdown;		    ///< is allready a "shutdown" ongoing?
  MQ_BOL doFactoryCleanup;	    ///< was the context create by a 'Factory'
  MQ_BOL flagServerSetup;	    ///< setup.ServerSetup.fFunc was called ?

  struct MqCacheS * readCache;	    ///< cache for MqReadS

  // the next 3 items are !!only!! used in the parent
  MQ_SIZE   ctxIdR;		    ///< the largest currently used ctxId number
  MQ_SIZE   ctxIdZ;		    ///< the size of the ctxIdA array
  struct MqS ** ctxIdA;	    ///< array of struct MqLinkS * pointer's

  // the next 3 items are used to map the transactionID (int) to the transaction pointer
  struct MqTransS * trans;	    ///< link to the trans object
  MQ_HDL _trans;		    ///< storage for the Transaktion object from the package header

  // the following lines manage the link between the parent and the child,
  // to be able to delete all childs if the parent is deleted
  struct pChildS * childs;	    ///< linked list of childs
  struct pChildS * self;	    ///< my own child storage

  // master/slave relationship
  struct MqLinkSlaveS * slave;	    ///< link to the SLAVE object
  MQ_BOL  isWORKER;		    ///< is alfa[0] is "WORKER"
};

/// \ingroup msgque_api
/// \brief error-object data type
struct MqErrorS {
  struct MqBufferS * text;      ///< the error message
  enum MqErrorE code;		///< the error code
  MQ_INT num;	                ///< the error number also used as exit code
  MQ_BOL append;		///< allow to append? MQ_YES or MQ_NO
};

/// \ingroup msgque_api
/// \brief signature used in #MqS::signature
#define MQ_MqS_SIGNATURE 0x212CF91

#if !defined(MQ_PRIVATE_CONFIG_CONST)
# define MQ_PRIVATE_CONFIG_CONST const
#endif

/// \ingroup msgque_api
/// \brief Prototype for a context object
struct MqS {
  int signature;		    ///< used to verify object type for typeless languages
  MQ_PRIVATE_CONFIG_CONST
    struct MqConfigS config;	    ///< the configuration data is used for "end-user" configuration
  struct MqSetupS setup;	    ///< the setup data is used to link the object with the user application
  struct MqErrorS error;	    ///< error object data
  struct MqLinkS link;		    ///< link object data
  struct MqBufferS * temp;	    ///< misc temporary \e MqBufferS object
  enum MqStatusIsE statusIs;	    ///< how the context was created?
  MQ_BOL MqContextDelete_LOCK;	    ///< protect MqContextDelete
  MQ_BOL MqContextFree_LOCK;	    ///< protect MqContextFree
  MQ_PTR threadData;		    ///< application specific thread data
  MQ_PTR self;			    ///< link to the managed object
  MQ_SIZE contextsize;		    ///< alloc-size of the user-defined context struct
};

#ifndef MQ_PRIVATE

# ifndef MQ_LINK_WITH_LIBRARY_OBJECT_FILES

/// \ingroup msgque_api
/// \brief the prefix arguments of the starting application
MQ_EXTERN struct MqBufferLS * MqInitBuf;

/// \ingroup msgque_api
/// \brief name of a procedure to return \e main like entry-points 
MQ_EXTERN MqFactorySelectorF MqFactorySelector;

# endif /* MQ_LINK_WITH_OBJECT_FILES */

#endif /* !MQ_PRIVATE */

/*****************************************************************************/
/*                                                                           */
/*                           create / delete                                 */
/*                                                                           */
/*****************************************************************************/

/// \brief initialize the #MqS object related data but do \e not create the object self
MQ_EXTERN void MQ_DECL MqContextInit (
  struct MqS       * const context,
  MQ_SIZE		   size,
  struct MqS const * const tmpl
);

/// \brief free the #MqS object related data but do \e not free the object self
MQ_EXTERN void MQ_DECL MqContextFree (
  struct MqS       * const context
);

/// \brief create the #MqS object
MQ_EXTERN struct MqS * MQ_DECL MqContextCreate (
  MQ_SIZE size,
  struct MqS const * const tmpl
);

/// \brief delete the entire #MqS object
MQ_EXTERN void MQ_DECL MqContextDelete (
  struct MqS ** contextP
) __attribute__((nonnull));

/// \brief clean the #MqS::config data
MQ_EXTERN void MQ_DECL MqConfigReset (
  struct MqS * const context
);

/// \brief copy the #MqS::config data
MQ_EXTERN void MQ_DECL MqConfigDup (
  struct MqS * const to,
  struct MqS const * const from
);

/// \brief copy the #MqS::setup data
MQ_EXTERN enum MqErrorE MQ_DECL MqSetupDup (
  struct MqS * const to,
  struct MqS const * const from
);

/*****************************************************************************/
/*                                                                           */
/*                                   init                                    */
/*                                                                           */
/*****************************************************************************/

/// \brief set the #MqConfigS::name value and cleanup old value
MQ_EXTERN void
MQ_DECL MqConfigSetName (
  struct MqS * const context,
  MQ_CST  data
);

/// \brief set the #MqConfigS::ident value and cleanup old value
MQ_EXTERN void
MQ_DECL MqConfigSetIdent (
  struct MqS * const context,
  MQ_CST  data
);

/// \brief set the #MqConfigS::srvname value and cleanup old value
MQ_EXTERN void
MQ_DECL MqConfigSetSrvName (
  struct MqS * const context,
  MQ_CST  data
);

/// \brief set the #MqIoConfigS::buffersize value
MQ_EXTERN void
MQ_DECL MqConfigSetBuffersize (
  struct MqS * const context,
  MQ_INT  data
);

/// \brief set the #MqConfigS::debug value
MQ_EXTERN void
MQ_DECL MqConfigSetDebug (
  struct MqS * const context,
  MQ_INT  data
);

/// \brief set the #MqIoConfigS::timeout value
MQ_EXTERN void
MQ_DECL MqConfigSetTimeout (
  struct MqS * const context,
  MQ_TIME_T  data
);

/// \brief set the #MqConfigS::isSilent value
MQ_EXTERN void
MQ_DECL MqConfigSetIsSilent (
  struct MqS * const context,
  MQ_BOL  data
);

/// \brief set the #MqConfigS::isServer value
MQ_EXTERN void
MQ_DECL MqConfigSetIsServer (
  struct MqS * const context,
  MQ_BOL  data
);

/// \brief set the #MqConfigS::isString value
MQ_EXTERN void
MQ_DECL MqConfigSetIsString (
  struct MqS * const context,
  MQ_BOL  data
);

/// \brief set the #MqConfigS::parent value
MQ_EXTERN void
MQ_DECL MqConfigSetParent (
  struct MqS * const context,
  struct MqS * const parent
);

/// \brief set the #MqConfigS::master and #MqConfigS::master_id value
MQ_EXTERN void
MQ_DECL MqConfigSetMaster (
  struct MqS * const context,
  struct MqS * const master,
  int master_id
);

/// \brief setup the \e factory pattern
/// \context
/// \param[in] fCreate set the #MqSetupS::Factory - #MqFactoryS::MqFactoryCreateS::fCall value
/// \param[in] CreateData set the #MqSetupS::Factory - #MqFactoryS::MqFactoryCreateS::data value
/// \param[in] fCreateFree free the \e CreateData
/// \param[in] fCreateCopy copy the \e CreateData
/// \param[in] fDelete set the #MqSetupS::Factory - #MqFactoryS::MqFactoryDeleteS::fCall value
/// \param[in] DeleteData set the #MqSetupS::Factory - #MqFactoryS::MqFactoryDeleteS::data value
/// \param[in] fDeleteFree delete the \e DeleteData
/// \param[in] fDeleteCopy copy the \e DeleteData
MQ_EXTERN void 
MQ_DECL MqConfigSetFactory (
  struct MqS * const context,

  MqFactoryCreateF  fCreate,
  MQ_PTR	    CreateData,
  MqTokenDataFreeF  fCreateFree,
  MqTokenDataCopyF  fCreateCopy,

  MqFactoryDeleteF  fDelete,
  MQ_PTR	    DeleteData,
  MqTokenDataFreeF  fDeleteFree,
  MqTokenDataCopyF  fDeleteCopy
);

/// \brief setup the \e factory pattern
/// \context
///
/// The \e default factory is just a wrapper for #MqContextCreate with additional error management code.
/// A simple application \e without an application specific \e factory use this configuration.
MQ_EXTERN void 
MQ_DECL MqConfigSetDefaultFactory (
  struct MqS * const context
);

/// \brief set the #MqConfigS::ignoreFork value
MQ_EXTERN void
MQ_DECL MqConfigSetIgnoreFork (
  struct MqS * const context,
  MQ_BOL data
);

/// \brief set the #MqConfigS::ignoreThread value
MQ_EXTERN void
MQ_DECL MqConfigSetIgnoreThread (
  struct MqS * const context,
  MQ_BOL data
);

/// \brief set the #MqConfigS::startAs value
MQ_EXTERN void
MQ_DECL MqConfigSetStartAs (
  struct MqS * const context,
  enum MqStartE data
);

/// \brief set various attributes from #MqConfigS struct
MQ_EXTERN void
MQ_DECL MqConfigSetSetup (
  struct MqS * const context,
  MqCreateF fChildCreate,
  MqDeleteF fChildDelete,
  MqCreateF fParentCreate,
  MqDeleteF fParentDelete,
  MqExitF   fProcessExit,
  MqExitF   fThreadExit
);

/// \brief set the #MqSetupS::ServerSetup
MQ_EXTERN void
MQ_DECL MqConfigSetServerSetup (
  struct MqS * const context,
  MqTokenF fFunc,
  MQ_PTR data,
  MqTokenDataFreeF fFree,
  MqTokenDataCopyF fCopy
);

/// \brief set the #MqSetupS::ServerCleanup
MQ_EXTERN void
MQ_DECL MqConfigSetServerCleanup (
  struct MqS * const context,
  MqTokenF fTok,
  MQ_PTR data,
  MqTokenDataFreeF fFree,
  MqTokenDataCopyF fCopy
);

/// \brief set the #MqSetupS::BgError
MQ_EXTERN void
MQ_DECL MqConfigSetBgError (
  struct MqS * const context,
  MqTokenF fTok,
  MQ_PTR data,
  MqTokenDataFreeF fFree,
  MqTokenDataCopyF fCopy
);

/// \brief set the #MqIoUdsConfigS::file
MQ_EXTERN enum MqErrorE
MQ_DECL MqConfigSetIoUds (
  struct MqS * const context,
  MQ_CST file
);

/// \brief set the #MqIoTcpConfigS data
/// \context
/// \param[in] host 
///   client: the remote host name\n
///   server: the local interface name
/// \param[in] port
///   client: the remote port name\n
///   server: the local port name
/// \param[in] myhost client: the local host name
/// \param[in] myport client: the local port name
/// \retMqErrorE
/// \attention use a string value for a \e port to support named ports as well
MQ_EXTERN enum MqErrorE
MQ_DECL MqConfigSetIoTcp (
  struct MqS * const context,
  MQ_CST host,
  MQ_CST port,
  MQ_CST myhost,
  MQ_CST myport
);

/// \brief set the \e pipe configuration data
/// \context
/// \param[in] socket the name of the known socket
/// \retMqErrorE
///
/// This is configuration option is only usefull for a \e (x)inetd setup
/// to use the stdin (socket=0) as send/recv communication socket
MQ_EXTERN enum MqErrorE
MQ_DECL MqConfigSetIoPipe (
  struct MqS * const context,
  MQ_SOCK socket
);

/// \brief start the server in daemon mode
/// \context
/// \param[in] pidfile write the PID of the daemon into this file
/// \retMqErrorE
///
/// A \e daemon is a background process without any link to the
/// starting process and the stdin/stdout/stderr closed.
MQ_EXTERN enum MqErrorE
MQ_DECL MqConfigSetDaemon (
  struct MqS * const context,
  MQ_CST pidfile
);

/*****************************************************************************/
/*                                                                           */
/*                                   get                                     */
/*                                                                           */
/*****************************************************************************/

/** \brief does the \e context object is a \e server ?
 *  \context
 *  \return the <TT>(context->setup.isServer == MQ_YES)</TT> value
 */
MQ_EXTERN int MQ_DECL MqConfigGetIsServer (
  struct MqS const * const context
) __attribute__((nonnull));

/** \brief does the \e context object is a \e parent ?
 *  \context
 *  \return the <TT>(context->config.parent == NULL)</TT> value
 */
MQ_EXTERN int MQ_DECL MqConfigGetIsParent (
  struct MqS const * const context
) __attribute__((nonnull));

/** \ingroup config_api
 *  \brief does the \e context object is a \e slave ?
 *  \context
 *  \return the <TT>(context->config.master != NULL)</TT> value
 */
MQ_EXTERN int MQ_DECL MqConfigGetIsSlave (
  struct MqS const * const context
) __attribute__((nonnull));

/** \brief does the \e context object is a \e string ?
 *  \context
 *  \return the <TT>(context->config.isString == MQ_YES)</TT> value
 */
MQ_EXTERN int MQ_DECL MqConfigGetIsString (
  struct MqS const * const context
) __attribute__((nonnull));

/** \brief does the \e context object is \e silent ?
 *  \context
 *  \return the <TT>(context->config.isSilent == MQ_YES)</TT> value
 */
MQ_EXTERN int MQ_DECL MqConfigGetIsSilent (
  struct MqS const * const context
) __attribute__((nonnull));

/** \brief does the \e context object is \e connected ?
 *  \context
 *  \return the <TT>(context->link != NULL)</TT> value
 */
MQ_EXTERN int MQ_DECL MqConfigGetIsConnected (
  struct MqS const * const context
);

/* \brief does the \e context object was created by \libmsgque
 *  \context
 *  \return the <TT>(context->config.doFactoryCleanup == MQ_YES)</TT> value
MQ_EXTERN int MQ_DECL MqConfigGetDoFactoryCleanup (
  struct MqS const * const context
);
 */

/** \brief does the \e context object is \e Duplicate and \e Thread ?
 *  \context
 *  \return the <TT>((config->statusIs & MQ_STATUS_IS_DUP) && (config->statusIs & MQ_STATUS_IS_THREAD))</TT> value
 */
MQ_EXTERN int MQ_DECL MqConfigGetIsDupAndThread (
  struct MqS const * const context
) __attribute__((nonnull));

/** \brief get the \e name of the \e context object
 *  \context
 *  \return the \c context.config.name value
 *  \attention the \e string is owned by \libmsgque -> do not free !!
 */
MQ_EXTERN MQ_CST  MQ_DECL MqConfigGetName (
  struct MqS const * const context
) __attribute__((nonnull));

/** \brief get the \e ident of the \e context object
 *  \context
 *  \return the \c context.config.ident value
 *  \attention the \e string is owned by \libmsgque -> do not free !!
 */
MQ_EXTERN MQ_CST MQ_DECL MqConfigGetIdent (
  struct MqS const * const context
) __attribute__((nonnull));

/** \brief check the \e ident of the \e context object
 *  \context
 *  \param[in] ident the ident to check for
 *  \return #MQ_YES or #MQ_NO
 *
 *  The check is done with an \c _IDN request send to the link target.
 *  This function is only useful if the link is up and running.
 */
MQ_EXTERN MQ_BOL  MQ_DECL MqConfigCheckIdent (
  struct MqS * const context,
  MQ_CST ident
) __attribute__((nonnull));

/** \brief get the \e srvname of the \e context object
 *  \context
 *  \return the \c context.config.srvname value
 *  \attention the \e string is owned by \libmsgque -> do not free !!
 */
MQ_EXTERN MQ_CST  MQ_DECL MqConfigGetSrvName (
  struct MqS const * const context
) __attribute__((nonnull));

/** \brief get the minimum of the \e read/send \e buffersize value of the \e context object
 *  \context
 *  \return the \c context.config.buffersize(R/S) value
 */
MQ_EXTERN MQ_INT MQ_DECL MqConfigGetBuffersize (
  struct MqS const * const context
) __attribute__((nonnull));

/** \brief get the \e debug value of the \e context object
 *  \context
 *  \return the \c context.config.debug value
 */
MQ_EXTERN MQ_INT MQ_DECL MqConfigGetDebug (
  struct MqS const * const context
) __attribute__((nonnull));

/** \brief get the \e timeout value of the \e context object
 *  \context
 *  \return the \c context.config.timeout value
 */
MQ_EXTERN MQ_TIME_T MQ_DECL MqConfigGetTimeout (
  struct MqS const * const context
) __attribute__((nonnull));

/** \brief get the \e parent value of the \e config object
 *  \context
 *  \return the #MqConfigS::parent value
 */
MQ_EXTERN struct MqS * MQ_DECL MqConfigGetParent (
  struct MqS const * const context
) __attribute__((nonnull));

/** \brief get the \e master value of the \e config object
 *  \context
 *  \return the #MqConfigS::master value
 */
MQ_EXTERN struct MqS * MQ_DECL MqConfigGetMaster (
  struct MqS const * const context
) __attribute__((nonnull));

/// \brief get the \e filter object from a bidirectional filter pipeline
///  between a \e master context and a \e slave context with \e id.
/// \context
/// \param[out] filterP the filter object to return
/// \retMqErrorE
///
/// the following order is used to get the filter object:
/// -# return the \e #MqConfigS::master if non NULL
/// -# return the \e #MqSlaveGet with \e id if non NULL
/// -# return a "filter not available" error
/// .
MQ_EXTERN enum MqErrorE MQ_DECL MqConfigGetFilter (
  struct MqS  * const context,
  MQ_SIZE const id,
  struct MqS ** const filterP
) __attribute__((nonnull));

/// \brief return the context-identification
/// \context
/// \return the context-identification number
///
/// The context-identification is a number and is used in the protocol to
/// link a data-package to a context-pointer. This is necessary because
/// the communication interface (\c msgque->io) of a parent-context is
/// used by the child-context too.
MQ_EXTERN MQ_SIZE MQ_DECL MqConfigGetCtxId (
  struct MqS const * const context
);

/// \brief return the current transaction token
/// \context
/// \return the current transaction token as 4 character string
MQ_EXTERN MQ_CST 
MQ_DECL MqConfigGetToken (
  struct MqS const * const context
);

/// \brief check if the ongoing service-call belongs to a transaction
/// \context
/// \return 1=yes, 0=no
MQ_EXTERN int
MQ_DECL MqConfigGetIsTransaction (
  struct MqS const * const context
);

/// \brief return the #MqIoUdsConfigS::file value
MQ_EXTERN MQ_CST
MQ_DECL MqConfigGetIoUdsFile (
  struct MqS * const context
);

/// \brief get the #MqIoTcpConfigS::host configuration data
MQ_EXTERN MQ_CST
MQ_DECL MqConfigGetIoTcpHost (
  struct MqS * const context
);

/// \brief get the #MqIoTcpConfigS::port configuration data
MQ_EXTERN MQ_CST
MQ_DECL MqConfigGetIoTcpPort (
  struct MqS * const context
);

/// \brief get the #MqIoTcpConfigS::myhost configuration data
MQ_EXTERN MQ_CST
MQ_DECL MqConfigGetIoTcpMyHost (
  struct MqS * const context
);

/// \brief get the #MqIoTcpConfigS::myport configuration data
MQ_EXTERN MQ_CST
MQ_DECL MqConfigGetIoTcpMyPort (
  struct MqS * const context
);

/// \brief get the \e pipe configuration data
/// \context
/// \return the pipe socket
MQ_EXTERN MQ_SOCK
MQ_DECL MqConfigGetIoPipeSocket (
  struct MqS * const context
);

/// \brief return the #MqConfigS::startAs value
MQ_EXTERN enum MqStartE
MQ_DECL MqConfigGetStartAs (
  struct MqS * const context
);

/*****************************************************************************/
/*                                                                           */
/*                               Managed Context                             */
/*                                                                           */
/*****************************************************************************/

/** \brief set the #MqS::self value
 *  \context
 *  \param[in] self a pointer to the managed context usually \e self or \e this
 */
MQ_EXTERN void
MQ_DECL MqConfigSetSelf (
  struct MqS * const context,
  void * self
);

/** \brief get the #MqS::self value
 *  \context
 */
MQ_EXTERN void*
MQ_DECL MqConfigGetSelf (
  struct MqS const * const context
);

/**
 * \}
 */

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                    M S G Q U E - A P I                          ### */
/* ###                                                                 ### */
/* ####################################################################### */

/** \defgroup msgque_api PUBLIC: struct MqLinkS
 *  \ingroup lib
 *  \{
 *  \brief the <EM>MqS API</EM> is used as \libmsgque object.
 *
 *  the \e MqS object is created just after the \e MqContextS object usually
 *  in the function \c ContextCreate. every external \e MqContextS object has
 *  only one \e MqS object and every \e MqS object has only one \e MqContextS
 *  object. the \e MqS object will be created with the \c MqLinkCreate and will be deleted 
 *  with the \c MqLinkDelete function. during creation of a \e MqS object additional
 *  objects will be created too:
 */

/*****************************************************************************/
/*                                                                           */
/*                            msgque/definition                              */
/*                                                                           */
/*****************************************************************************/

/** \brief setup and return the \e init object
 *  \return a pointer to the initialization buffer
 *  \attention 
 *  - every usage of this function will free the data of the previous \e init object
 *  - the data have to fit to the underlying programming language
 *  .
 *
 *  The data is used as prefix to setup the argument vector for \e spawn.
 *  The following example demonstrate the usage of #MqInitCreate:
\code
struct MqBufferLS * initB = MqInitCreate();
MqBufferLAppendC(initB, "myExec");
MqBufferLAppendC(initB, "myExecArgument_1");
...
\endcode
 */
MQ_EXTERN struct MqBufferLS* MQ_DECL MqInitCreate (void);

// \brief set the \c fork syscall
MQ_EXTERN void MQ_DECL MqInitSysAPI (MqForkF forkF, MqVForkF vforkF);

/** \brief create a parent \libmsgque object link
 *  \context
 *  \param[in] argsP  command-line arguments will be freed and set to \c NULL on error
 *  \retMqErrorE
 */
MQ_EXTERN enum MqErrorE MQ_DECL MqLinkCreate (
  struct MqS  * const context,
  struct MqBufferLS ** argsP
);

/** \brief create a child \libmsgque object link
 *  \context
 *  \param[in] parent the parent object link
 *  \param[in] argsP  command-line arguments will be freed and set to \c NULL on error
 *  \retMqErrorE
 */
MQ_EXTERN enum MqErrorE MQ_DECL MqLinkCreateChild (
  struct MqS * const context,
  struct MqS * const parent,
  struct MqBufferLS ** argsP
);

/// \brief wrapper for #MqLinkCreate with \e argv error check
/// \copydoc MqLinkCreate
MQ_EXTERN enum MqErrorE MQ_DECL MqDefaultLinkCreate (
  struct MqS  * const context,
  struct MqBufferLS ** argsP
);

/** \brief delete a \libmsgque object
 *  \context
 */

MQ_EXTERN void MQ_DECL MqLinkDelete (
  struct MqS * const context
);

/** \brief exit the current process or thread
 *  \context
 *  \attention this function will never return
 *
 *  -# Delete the context object using: #MqLinkDelete
 *  -# If available call the context specific exit handler \c --fParentExit
 *  -# final call \c SysExit to exit the process
 *  -# It is an Panic error if #MqExit is called twice for the same object
 *  .
 */

MQ_EXTERN void MQ_DECL MqExit (
  struct MqS * context
) __attribute__ ((noreturn));

/** \brief write \libmsgque specific user-help to stderr
 *  \param tool the name of the tool (e.g. argv[0]) or NULL.
 *  \return the help messages as string
 * 
 *  If \e tool != NULL, the function will display a header like:
\code
  tool [ARGUMENT]... syntax:
    aclient [OPTION]... @ tool [OPTION]... @...
\endcode
 *  on the help page.
 */
MQ_EXTERN MQ_STR MQ_DECL MqHelp ( 
  MQ_CST  tool 
);

/** check for left over arguments from parameter \e argv of #MqLinkCreate
 *  \context
 *  \param[in,out] argvP a pointer to the argv, the memory will be freed and the value will be set to \c NULL
 *  \retMqErrorE
 *
 *  This code is not part of #MqLinkCreate because sometimes it is necessary
 *  to check for left over arguments \b after #MqLinkCreate was called and
 *  after the calling function was able to process function specific arguments.
 *  By Default #MqLinkCreate will delete \b all arguments belonging to \libmsgque
 *  from the \e argv object. If the calling function does \b not expect
 *  additional arguments or arguments were added with spelling errors,
 *  a call to #MqCheckForLeftOverArguments will detect this.
 */
MQ_EXTERN enum MqErrorE MQ_DECL MqCheckForLeftOverArguments (
  struct MqS * const context,
  struct MqBufferLS ** argvP
);

/*****************************************************************************/
/*                                                                           */
/*                               logging                                     */
/*                                                                           */
/*****************************************************************************/

#ifdef _DEBUG
/// \brief convenience function to log \e MqS configuration data
/// \context
/// \param prefix used to identify the data logged to the stderr
MQ_EXTERN void MQ_DECL MqLogData (
  struct MqS const * const context,
  MQ_CST const prefix
);

/// \brief convenience function to log \e MqS child dependencies
/// \context
/// \param prefix used to identify the data logged to the stderr
MQ_EXTERN void MQ_DECL MqLogChild (
  struct MqS const * const context,
  char const * const prefix
);
#endif // _DEBUG

/*****************************************************************************/
/*                                                                           */
/*                                misc                                       */
/*                                                                           */
/*****************************************************************************/

/// \brief create a service handle
/// \context
/// \token
/// \param proc an external (non libmsgque) function to provide the service
/// \param data an external (non libmsgque) pointer to a data structure linked to the #MqTokenF function
/// \param datafreeF function to free \e data pointer
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqServiceCreate (
  struct MqS * const context, 
  MQ_CST const token,
  MqTokenF const proc,
  MQ_PTR data,
  MqTokenDataFreeF datafreeF
);

/// \brief use a proxy to proxy the incomming trafic to the outgoing trafic
/// \context
/// \token
/// \param[in] id the slave identifer, only used if \e context is a \e master
/// \retMqErrorE
///
/// If \e context is a \e master context than the data is send to \e slave with \e id.
/// If \e context is a \e slave context than the data is send to the \e master of the \e slave.
MQ_EXTERN enum MqErrorE MQ_DECL MqServiceProxy (
  struct MqS * const context, 
  MQ_CST const token,
  MQ_SIZE const id
);

/// \brief delete service handle
/// \context
/// \token
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqServiceDelete (
  struct MqS const * const context, 
  MQ_CST const token
);

/** \} */

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                     B U F F E R - A P I                         ### */
/* ###                                                                 ### */
/* ####################################################################### */

/** \defgroup buffer_api PUBLIC: struct MqBufferS
 *  \ingroup msgque_api
 *  \{
 *  \brief the <EM>Msgque Buffer API</EM> is used to manage dynamic, generic, mixed typed data.
 *
 *  The MqBufferS struct is used to store and manage #MqTypeE typed data in a
 *  MqBufferU storage. If \libmsgque is working on any kind of data it is working on
 *  MqBufferS objects or on a list of MqBufferS objects called MqBufferLS.
 */

/*****************************************************************************/
/*                                                                           */
/*                              buffer_init                                  */
/*                                                                           */
/*****************************************************************************/

/// \brief union used to set or modify native data from an MqBufferS object
union MqBufferAtomU {
  MQ_BYT    Y;			///< 1 byte byte data
  MQ_BOL    O;			///< 1 byte boolean data
  MQ_SRT    S;			///< 2 byte short data
  MQ_INT    I;			///< 4 byte integer data
  MQ_FLT    F;			///< 4 byte float data
  MQ_WID    W;			///< 8 byte integer data
  MQ_DBL    D;			///< 8 byte double data
  MQ_BINB   B[8];		///< 8 byte ARRAY used for copy
  MQ_BINB   B8[8];		///< 8 byte ARRAY used for copy
  MQ_BINB   B4[4];		///< 4 byte ARRAY used for copy
  MQ_BINB   B2[2];		///< 2 byte ARRAY used for copy
  MQ_BINB   B1[1];		///< 1 byte ARRAY used for copy
};

/** \brief a union for all data items supported by \e MqBufferS.
 * 
 * A \libmsgque data package is a collection of items of type \e MqBufferU
 * with variable length data of type: #MQ_BIN, #MQ_STR and MQ_LST or a
 * a fixed length data of type #MQ_ATO.
 * The \ref MqBufferS object is using a \ref MqBufferU union 
 * and a \ref MqTypeE enum to define an item.
 **/
union MqBufferU {
  MQ_ATO *A;			///< atom for native data-types
  MQ_BIN  B;			///< arbitrary binary data
  MQ_STR  C;			///< C string like data (e.g. with a \\0 at the end)
  MQ_LST  L;			///< list object type data
  MQ_LST  R;			///< return object type data
};

/// \brief initial size of the #MqBufferS:bls object
#define MQ_BLS_SIZE 50

// \brief signature used in #MqBufferS::signature
#define MQ_MqBufferS_SIGNATURE 0x6021139

/** \brief defines an object to store generic (e.g mixed typed) data.
 *
 * an \e MqBufferS object is used for 3 different purposes:
 * - store generic typed single data value
 * - build a Msgque packet by appending items to an existing object
 * - read a single item from an existing Msgque packet 
 * .
 * 'reading from' and 'appending to' an existing Msgque packet is done by incrementing
 * the \a cur.B pointer to a new position.
 * a \ref MqBufferS object is using a \ref MqBufferU union 
 * and a \ref MqTypeE enum to define an item.
 * The object is public to give the developer the opportunity to direct access
 * saved data.
 **/

struct MqBufferS {
  int signature;		///< signature to provide type-safty for non-typed languages
  struct MqS *context;		///< error object of the related msgque
  MQ_BIN data;                  ///< always point to the beginning of the data-segment
  MQ_SIZE size;                 ///< the size of the data-segment
  MQ_SIZE cursize;              ///< currently used size (always: \a cursize < \a size)
  MQ_SIZE numItems;             ///< the number of items in the data-segment (only if it is a package)
  union MqBufferU cur;		///< point to current position, used for 'appending to' or 'reading from' 
				///< a package (always: data <= cur.B <= data+cursize)
  enum MqAllocE alloc;		///< allocation style, MQ_DYNAMIC or MQ_STATIC
  enum MqTypeE type;            ///< type of the item stored into the data-segment

  /// \brief B)uffer L)ocal S)torage used in #MqBufferGetC to get a string from a MQ_ATO data
  ///
  /// +1 to allow strings with MQ_BLS_SIZE=strlen(str) fit into the buffer with an additional
  /// '\\0' at the end
  MQ_BINB bls[MQ_BLS_SIZE+1];
};

/// \brief extract a MQ_BYT from a \e MqBufferS object
/// \bufU
MQ_EXTERN MQ_BYT MQ_DECL MqBufU2BYT ( union MqBufferU bufU );
/// \brief extract a MQ_BOL from a \e MqBufferS object
/// \bufU
MQ_EXTERN MQ_BOL MQ_DECL MqBufU2BOL ( union MqBufferU bufU );
/// \brief extract a MQ_SRT from a \e MqBufferS object
/// \bufU
MQ_EXTERN MQ_SRT MQ_DECL MqBufU2SRT ( union MqBufferU bufU );
/// \brief extract a MQ_INT from a \e MqBufferS object
/// \bufU
MQ_EXTERN MQ_INT MQ_DECL MqBufU2INT ( union MqBufferU bufU );
/// \brief extract a MQ_FLT from a \e MqBufferS object
/// \bufU
MQ_EXTERN MQ_FLT MQ_DECL MqBufU2FLT ( union MqBufferU bufU );
/// \brief extract a MQ_WID from a \e MqBufferS object
/// \bufU
MQ_EXTERN MQ_WID MQ_DECL MqBufU2WID ( union MqBufferU bufU );
/// \brief extract a MQ_DBL from a \e MqBufferS object
/// \bufU
MQ_EXTERN MQ_DBL MQ_DECL MqBufU2DBL ( union MqBufferU bufU );
/// \brief extract a MQ_ATO from a \e MqBufferS object
/// \bufU
MQ_EXTERN MQ_ATO MQ_DECL MqBufU2ATO ( union MqBufferU bufU );

/// \brief create a new \e MqBufferS with \a size
/// \context
/// \param size initial size of the buffer, if \e size = 0 the initial size is set to an default value
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferCreate (
  struct MqS * const context,
  MQ_SIZE size
);

/// \brief delete a new \e MqBufferS 
/// \param bufP a pointer to an struct MqBufferS * object
/// \attention \attDelete
MQ_EXTERN void MQ_DECL MqBufferDelete (
  struct MqBufferS ** const bufP
);

/// \brief reset a \e MqBufferS to the length zero
/// \buf
MQ_EXTERN void MQ_DECL MqBufferReset (
  struct MqBufferS * const buf
);

/// \brief copy the \e MqBufferS from \a srce to \a dest
/// \param dest target of the copy
/// \param srce source of the copy
/// \retval the \e dest object
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferCopy (
  register struct MqBufferS * const dest,
  register struct MqBufferS const * const srce
);

/// \brief create an new object as duplication of an existing object
/// \param srce source of the duplication
/// \retval the new object
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferDup (
  struct MqBufferS const * const srce
);

/*****************************************************************************/
/*                                                                           */
/*                           buffer_create_type                              */
/*                                                                           */
/*****************************************************************************/

/// \brief create and initialize a new #MqBufferS object using native typed value
/// \context
/// \param[in] val the value to initialize the buffer to
/// \retMqBufferS
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferCreateY (
  struct MqS * const context,
  MQ_BYT const val
);

/// \copydoc MqBufferCreateY
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferCreateO (
  struct MqS * const context,
  MQ_BOL const val
);

/// \copydoc MqBufferCreateY
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferCreateS (
  struct MqS * const context,
  MQ_SRT const val
);

/// \copydoc MqBufferCreateY
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferCreateI (
  struct MqS * const context,
  MQ_INT const val
);

/// \copydoc MqBufferCreateY
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferCreateF (
  struct MqS * const context,
  MQ_FLT const val
);

/// \copydoc MqBufferCreateY
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferCreateW (
  struct MqS * const context,
  MQ_WID const val
);

/// \copydoc MqBufferCreateY
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferCreateD (
  struct MqS * const context,
  MQ_DBL const val
);

/// \brief create and initialize an new #MqBufferS object using an #MQ_STR object
/// \context
/// \param val the MQ_STR object
/// \retMqBufferS
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferCreateC (
  struct MqS * const context,
  MQ_CST const val
);

/// \brief create and initialize an new #MqBufferS object using an #MQ_BIN object
/// \context
/// \param val the MQ_BIN object
/// \param len the size of the MQ_BIN object
/// \retMqBufferS
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferCreateB (
  struct MqS * const context,
  MQ_BINB const * const val,
  MQ_SIZE const len
);

/// \brief create and initialize an new #MqBufferS object using an #MQ_BUF object
/// \context
/// \param val the MQ_BUF object
/// \retMqBufferS
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferCreateU (
  struct MqS * const context,
  struct MqBufferS const * const val
);

/*****************************************************************************/
/*                                                                           */
/*                              buffer_get                                   */
/*                                                                           */
/*****************************************************************************/

/// \brief read native typed value from an #MQ_BUF object
/// \buf
/// \param[out] valP a pointer of the value to read
/// \retMqErrorE
MQ_EXTERN enum MqErrorE
MQ_DECL MqBufferGetY (
  struct MqBufferS * const buf,
  MQ_BYT * const valP
);

/// \copydoc MqBufferGetY
MQ_EXTERN enum MqErrorE
MQ_DECL MqBufferGetO (
  struct MqBufferS * const buf,
  MQ_BOL * const valP
);

/// \copydoc MqBufferGetY
MQ_EXTERN enum MqErrorE
MQ_DECL MqBufferGetS (
  struct MqBufferS * const buf,
  MQ_SRT * const valP
);

/// \copydoc MqBufferGetY
MQ_EXTERN enum MqErrorE
MQ_DECL MqBufferGetI (
  struct MqBufferS * const buf,
  MQ_INT * const valP
);

/// \copydoc MqBufferGetY
MQ_EXTERN enum MqErrorE
MQ_DECL MqBufferGetF (
  struct MqBufferS * const buf,
  MQ_FLT * const valP
);

/// \copydoc MqBufferGetY
MQ_EXTERN enum MqErrorE
MQ_DECL MqBufferGetW (
  struct MqBufferS * const buf,
  MQ_WID * const valP
);

/// \copydoc MqBufferGetY
MQ_EXTERN enum MqErrorE
MQ_DECL MqBufferGetD (
  struct MqBufferS * const buf,
  MQ_DBL * const valP
);

/// \brief function to read an #MQ_BIN from an #MQ_BUF object
/// \buf
/// \retval out the pointer to an MQ_BIN array object
/// \retval size the size of the array the array pointer
/// \retMqErrorE
/// \attention the return-pointer (\e out) is owned by the #MQ_BUF object -> never free this pointer
MQ_EXTERN enum MqErrorE
MQ_DECL MqBufferGetB (
  struct MqBufferS * const buf,
  MQ_BIN * const out,
  MQ_SIZE * const size
);

/// \brief function to read an #MQ_STR from an #MQ_BUF object
/// \buf
/// \retval out the pointer to an MQ_STR object
/// \retMqErrorE
/// \attention the return-pointer (\e out) is owned by the #MQ_BUF object -> never free this pointer
MQ_EXTERN enum MqErrorE
MQ_DECL MqBufferGetC (
  struct MqBufferS * const buf,
  MQ_CST * const out
);

/// \brief return the type from a #MQ_BUF object
/// \buf
/// \return the type from a #MqTypeE object as single character value
MQ_EXTERN char MQ_DECL MqBufferGetType (
  struct MqBufferS * const buf
);

/// \brief return the \c MqErrorS object from a #MQ_BUF object
/// \buf
/// \return the \c MqErrorS object
MQ_EXTERN struct MqS * MQ_DECL MqBufferGetContext (
  struct MqBufferS * const buf
);

/*****************************************************************************/
/*                                                                           */
/*                              buffer_set                                   */
/*                                                                           */
/*****************************************************************************/

/// \brief set the #MqBufferS object with a native typed value
/// \buf
/// \param[in] val the value to set the buffer to
/// \retMqBufferS
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferSetY (
  struct MqBufferS * const buf,
  MQ_BYT const val
);

/// \copydoc MqBufferSetY
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferSetO (
  struct MqBufferS * const buf,
  MQ_BOL const val
);

/// \copydoc MqBufferSetY
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferSetS (
  struct MqBufferS * const buf,
  MQ_SRT const val
);

/// \copydoc MqBufferSetY
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferSetI (
  struct MqBufferS * const buf,
  MQ_INT const val
);

/// \copydoc MqBufferSetY
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferSetF (
  struct MqBufferS * const buf,
  MQ_FLT const val
);

/// \copydoc MqBufferSetY
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferSetW (
  struct MqBufferS * const buf,
  MQ_WID const val
);

/// \copydoc MqBufferSetY
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferSetD (
  struct MqBufferS * const buf,
  MQ_DBL const val
);

/// \brief set the #MqBufferS object with an #MQ_STR object
/// \buf
/// \param in the string to set the buffer
/// \retMqBufferS
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferSetC (
  struct MqBufferS * const buf,
  MQ_CST const in
);

/// \brief set the #MqBufferS object with an #MQ_BIN object
/// \buf
/// \param in the binary to set the buffer
/// \param len the length of the binary data
/// \retMqBufferS
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferSetB (
  struct MqBufferS * const buf,
  MQ_BINB const * const in,
  MQ_SIZE const len
);

/// \brief set the #MqBufferS object with an #MQ_BUF object
/// \buf
/// \param in the MQ_BUF to set the buffer
/// \retMqBufferS
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferSetU (
  struct MqBufferS * const buf,
  struct MqBufferS const * const in
);

/// \brief set the MqBufferS to a vararg \e string with \e format
/// \buf
/// \format
/// \retMqBufferS
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferSetV (
  struct MqBufferS * const buf,
  MQ_CST const fmt,
  ...
) __attribute__ ((format (printf, 2, 3)));

/// \brief change the type of an MqBufferS object to \e type
/// \buf
/// \type
/// \retMqErrorE
MQ_EXTERN enum MqErrorE
MQ_DECL MqBufferCastTo (
  struct MqBufferS * const buf,
  enum MqTypeE const type
);

/*****************************************************************************/
/*                                                                           */
/*                              buffer_append                                */
/*                                                                           */
/*****************************************************************************/

/// \brief append a single \a string to a \e MqBufferS
/// \buf
/// \param string the text to append to \e buf
/// \retval the size of the string appended to the MqBufferS object
MQ_EXTERN MQ_SIZE MQ_DECL MqBufferAppendC (
  struct MqBufferS * const buf,
  MQ_CST const string
);

/// \brief append a single string with \a format and \a var_list arguments to a MqBufferS
/// \buf
/// \format
/// \var_list
/// \retval the size of the string appended to the MqBufferS object
MQ_EXTERN MQ_SIZE MQ_DECL MqBufferAppendVL (
  struct MqBufferS * const buf,
  MQ_CST const fmt,
  va_list var_list
);

/// \brief append a single string with \a format and \a ... arguments to a MqBufferS
/// \buf
/// \format
/// \retval the size of the string appended to the MqBufferS object
MQ_EXTERN MQ_SIZE MQ_DECL MqBufferAppendV (
  struct MqBufferS * const buf,
  MQ_CST const fmt,
  ...
) __attribute__ ((format (printf, 2, 3)));

/*****************************************************************************/
/*                                                                           */
/*                              buffer_push                                  */
/*                                                                           */
/*****************************************************************************/

/// \brief add \a str to the \e MqBufferS
/// \buf
/// \param string the text to append to \e buf
/// \retval the size of the string appended to the MqBufferS object
MQ_EXTERN MQ_SIZE MQ_DECL MqBufferPush (
  register struct MqBufferS * const buf,
  MQ_CST const string
);

/// \brief delete \a str from the \e MqBufferS
/// \buf
/// \param string the text to remove from \e buf
/// \retval the size of the string removed from the MqBufferS object
MQ_EXTERN MQ_SIZE MQ_DECL MqBufferPop (
  register struct MqBufferS * const buf,
  MQ_CST const string
);

/*****************************************************************************/
/*                                                                           */
/*                              buffer_log                                   */
/*                                                                           */
/*****************************************************************************/

/// \brief log the whole \e MqBufferS object to the stderr device
/// \context
/// \buf
/// \param prefix item to identify the output
MQ_EXTERN void MQ_DECL MqBufferLog (
  struct MqS const * const context,
  struct MqBufferS const * const buf,
  MQ_CST const prefix
);

/// \brief log the short \e MqBufferS object data to the stderr device
/// \context
/// \buf
/// \param prefix item to identify the output
MQ_EXTERN void MQ_DECL MqBufferLogS (
  struct MqS const * const context,
  struct MqBufferS const * const buf,
  MQ_CST const prefix
);

/** \} */

/*****************************************************************************/
/*                                                                           */
/*                    transactions (require MqBufferAtomU)                   */
/*                                                                           */
/*****************************************************************************/

/**
 *  \ingroup msgque_api
 *  \{
 */

/// \brief check the current token
/// \param[in] context the current context
/// \param[in] token the token to compare with
MQ_EXTERN MQ_BOL
MQ_DECL MqCurrentTokenIs (
  struct MqS const * const context,
  char const * const token
);

/** \} */

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                 B U F F E R - L I S T - A P I                   ### */
/* ###                                                                 ### */
/* ####################################################################### */


/** \defgroup buffer_list_api PUBLIC: struct MqBufferLS
 *  \ingroup msgque_api
 *  \{
 *  \brief the <EM>Msgque Buffer List API</EM> is used to manage a list of \e MqBufferS objects.
 *
 *  the memory is allocated and extended dynamically. 
 */

/*****************************************************************************/
/*                                                                           */
/*                          buffer_list_init                                 */
/*                                                                           */
/*****************************************************************************/

/// \brief the object to manage an array of \e MqBufferS items.
///
/// every \e MqBufferLS is able to store a infinite number of \e MqBufferS objects
/// using dynamic allocation. the \e MqBufferS objects are stored in a flat
/// array of \e MqBufferS object pointers. to access an object only the \a data 
/// or the \a cur member is used (e.g. \b \c data[index]). in difference to the
/// \a data member the \a cur member has a floating position to implement an
/// increment/decrement like behaviour
struct MqBufferLS {
  MQ_SIZE size;                 ///< the total allocated size
  MQ_SIZE cursize;              ///< the currently used size (e.g. 0 <= cursize <= size) 
  struct MqBufferS ** data;	///< pointer to the allocated memory (fixed position)
  struct MqBufferS ** cur;	///< pointer to the current position (e.g. data <= cur <= data+cursize, floating position)
} ;

/// \brief create and initialize a MqBufferLS object
/// \param size create the object with a predefined list size
/// \return the new created and initialized MqBufferLS object
MQ_EXTERN struct MqBufferLS * MQ_DECL MqBufferLCreate (
  MQ_SIZE const size
);

/**
 *  \brief create and initialize a MqBufferLS object with argc/argv data
 *  \param argc the argc from the initial \b main function
 *  \param argv the arguments from the initial \b main function
 *  \retval args the new created MqBufferLS object including all arguments from argv \b before the first #MQ_ALFA
 *  \retval alfa the new created MqBufferLS object including all arguments from argv \b after the first #MQ_ALFA
 *
 *  This function is usually called by the client (the initiator) of a msgque request. The \e alfa arguments
 *  will be used to start the server during the creation of the msgque object in #MqLinkCreate.
 *  \code
int
main ( int argc, char **argv)
{
  // the command-line-arguments before (largv) and after (lalfa) the first MQ_ALFA
  struct MqBufferLS * largv, lalfa;
  MqBufferLCreateArgv(NULL, argc, argv, &largv, &lalfa);
....
 * \endcode
 */
MQ_EXTERN struct MqBufferLS * MQ_DECL MqBufferLCreateArgs (
  int const argc,
  MQ_CST argv[]
);

/// \brief create and return a \c MqBufferLS object using a \e va_list argument
MQ_EXTERN struct MqBufferLS * MQ_DECL MqBufferLCreateArgsVA (
  struct MqS * const context,
  va_list ap
);

/// \brief create and return a \c MqBufferLS object using \e varargs arguments end with \c NULL
MQ_EXTERN struct MqBufferLS * MQ_DECL MqBufferLCreateArgsV (
  struct MqS * const context,
  ...
);

/// \brief create and return a a \c MqBufferLS object using \e main startup arguments
MQ_EXTERN struct MqBufferLS * MQ_DECL MqBufferLCreateArgsVC (
  struct MqS * const context,
  int const   argc,
  MQ_CST      argv[]
);

/// \brief create an #MqBufferLS object as copy from an existing \e MqBufferLS object
/// \param in the \e MqBufferLS object to copy from
/// \return out the \e MqBufferLS object to create
MQ_EXTERN struct MqBufferLS * MQ_DECL MqBufferLDup (
  struct MqBufferLS const * const in
);

/// \brief delete a dynamically created \e MqBufferLS object
/// \param bufP the pointer to an struct MqBufferLS * object
/// \attDelete
MQ_EXTERN void MQ_DECL MqBufferLDelete (
  struct MqBufferLS ** bufP
);

/*****************************************************************************/
/*                                                                           */
/*                          buffer_list_append                               */
/*                                                                           */
/*****************************************************************************/

/// \brief append a MqBufferS item into an MqBufferLS object on \e position
/// \bufL
/// \param[in] in the MqBufferS object to append
/// \param[in] position insert \e in at \e position, shift all following arguments one up
/// \attention Set \e position to \b 0 to append to the beginning or set position to \b -1 to append to the end
MQ_EXTERN void MQ_DECL MqBufferLAppend (
  register struct MqBufferLS * const bufL,
  struct MqBufferS * const in,
  MQ_SIZE const position
);

/// \brief copy a MqBufferLS list into an MqBufferLS object on \e position
/// \bufL
/// \param[in] in the MqBufferLS object to append
/// \param[in] position insert \e in at \e position, shift all following arguments one up
/// \attention Set \e position to \b 0 to append to the beginning or set position to \b -1 to append to the end
MQ_EXTERN void MQ_DECL MqBufferLAppendL (
  register struct MqBufferLS * const bufL,
  struct MqBufferLS * const in,
  MQ_SIZE position
);

/// \brief append a native typed value to the \b end of a MqBufferLS object
/// \bufL
/// \param[in] val the value to append to
MQ_EXTERN void MQ_DECL
MqBufferLAppendY (
  struct MqBufferLS * const bufL,
  MQ_BYT const val
);

/// \copydoc MqBufferLAppendY
MQ_EXTERN void MQ_DECL
MqBufferLAppendO (
  struct MqBufferLS * const bufL,
  MQ_BOL const val
);

/// \copydoc MqBufferLAppendY
MQ_EXTERN void MQ_DECL
MqBufferLAppendS (
  struct MqBufferLS * const bufL,
  MQ_SRT const val
);

/// \copydoc MqBufferLAppendY
MQ_EXTERN void MQ_DECL
MqBufferLAppendI (
  struct MqBufferLS * const bufL,
  MQ_INT const val
);

/// \copydoc MqBufferLAppendY
MQ_EXTERN void MQ_DECL
MqBufferLAppendF (
  struct MqBufferLS * const bufL,
  MQ_FLT const val
);

/// \copydoc MqBufferLAppendY
MQ_EXTERN void MQ_DECL
MqBufferLAppendW (
  struct MqBufferLS * const bufL,
  MQ_WID const val
);

/// \copydoc MqBufferLAppendY
MQ_EXTERN void MQ_DECL
MqBufferLAppendD (
  struct MqBufferLS * const bufL,
  MQ_DBL const val
);

/// \brief append an #MQ_STR object to the \b end of an MqBufferLS object
/// \bufL
/// \param val the string for input
MQ_EXTERN void MQ_DECL
MqBufferLAppendC (
  struct MqBufferLS * const bufL,
  MQ_CST const val
);

/// \brief append an #MQ_BUF object to the \b end of an MqBufferLS object
/// \bufL
/// \param val the buffer for input
/// \attention after the insert the buffer is owed by the \e buf object -> do \b not free \e val
MQ_EXTERN void MQ_DECL
MqBufferLAppendU (
  struct MqBufferLS * const bufL,
  MQ_BUF const val
);

/*****************************************************************************/
/*                                                                           */
/*                              bufferL_check                                */
/*                                                                           */
/*****************************************************************************/

/// \brief search for boolean \e option in MqBufferLS list and fill \e var with #MQ_BOL value 
/// for "found" = #MQ_YES and "not found" = #MQ_NO
/// \context
/// \bufL0
/// \optionL
/// \retval var if \e option string is found set \e var with \e def
/// \retMqErrorE
/// \attention if \e option is found the entry is deleted from the MqBufferLS object
MQ_EXTERN enum MqErrorE MQ_DECL MqBufferLCheckOptionO (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_CST const opt,
  MQ_BOL * const var
);

/// \brief search for an \e option with native typed argument in an #MqBufferLS object
/// \context
/// \bufL0
/// \optionL
/// \param[out] valP the native option value to search for
/// \retMqErrorE
/// \attL
MQ_EXTERN enum MqErrorE MQ_DECL MqBufferLCheckOptionY (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_CST const opt,
  MQ_BYT * const valP
);

/// \copydoc MqBufferLCheckOptionY
MQ_EXTERN enum MqErrorE MQ_DECL MqBufferLCheckOptionS (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_CST const opt,
  MQ_SRT * const valP
);

/// \copydoc MqBufferLCheckOptionY
MQ_EXTERN enum MqErrorE MQ_DECL MqBufferLCheckOptionI (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_CST const opt,
  MQ_INT * const valP
);

/// \copydoc MqBufferLCheckOptionI
MQ_EXTERN enum MqErrorE MQ_DECL MqBufferLCheckOptionF (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_CST const opt,
  MQ_FLT * const valP
);

/// \copydoc MqBufferLCheckOptionI
MQ_EXTERN enum MqErrorE MQ_DECL MqBufferLCheckOptionW (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_CST const opt,
  MQ_WID * const valP
);

/// \copydoc MqBufferLCheckOptionI
MQ_EXTERN enum MqErrorE MQ_DECL MqBufferLCheckOptionD (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_CST const opt,
  MQ_DBL * const valP
);

/// \brief search for an \e option with an #MQ_STR argument in an #MqBufferLS object
/// \context
/// \bufL0
/// \optionL
/// \param[out] var the MQ_STR argument for return
/// \retMqErrorE
/// \attL \n
/// The return string \e var has dynamic allocated data and have to be freed later.\n
/// if the original value of <TT>*var != NULL</TT> than the memory is freed before set with the new value.
MQ_EXTERN enum MqErrorE MQ_DECL MqBufferLCheckOptionC (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_CST const opt,
  MQ_STR * const var
);

/// \brief search for an \e option with an #MQ_BUF argument in an #MqBufferLS object
/// \context
/// \bufL0
/// \optionL
/// \param[out] var the MQ_BUF argument for return
/// \retMqErrorE
/// \attL
MQ_EXTERN enum MqErrorE MQ_DECL MqBufferLCheckOptionU (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_CST const opt,
  MQ_BUF * const var
);

/*****************************************************************************/
/*                                                                           */
/*                              bufferL_misc                                 */
/*                                                                           */
/*****************************************************************************/

MQ_EXTERN void MQ_DECL MqBufferLMove (
  struct MqBufferLS * const to,
  struct MqBufferLS ** fromP
);

/// \brief delete the \e index'th list item from the MqBufferLS object
/// \context
/// \bufL
/// \param index the index'th object from the MqBufferLS object starting with 0
/// \param numitems delete number of items
/// \param doDelete if \e delete != 0 delete the \e MqBufferS object, associated with the index, too
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqBufferLDeleteItem (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  const MQ_SIZE index,
  const MQ_SIZE numitems,
  const MQ_BOL doDelete
);

/// \brief search #MQ_STR item from a MqBufferLS object starting at \e startindex
/// \bufL
/// \param str the string to search for
/// \param len the length of \e str
/// \param startindex start searching in \e buf from index \e startindex
/// \return The index of the \e str found or -1 if not found. The return value can be used as startindex of following calls to \e MqBufferLSearchC
///
/// a typical usage for this code is parsing an MqBufferLS object for multiple occurrences of a string
/// \code
/// while ((startindex = MqBufferLSearchC (buf, str, startindex)) != -1) {
///   ...
/// }
/// \endcode
/// \attention The size of \e str have to be at least 4 bytes
MQ_EXTERN MQ_SIZE MQ_DECL MqBufferLSearchC (
  struct MqBufferLS const * const bufL,
  MQ_CST const str,
  MQ_SIZE const len,
  MQ_SIZE const startindex
);

/// \brief output a MqBufferLS object for debugging purpose to stderr
/// \context
/// \bufL
/// \param func prefix to identify the debugging output
MQ_EXTERN void MQ_DECL MqBufferLLog (
  struct MqS const * const context,
  struct MqBufferLS const * const bufL,
  MQ_CST const func
);

/// \brief \e short output a MqBufferLS object for debugging purpose to stderr
/// \context
/// \bufL
/// \param func prefix to identify the debugging output
/// \param varname prefix to identify the variable name
MQ_EXTERN void MQ_DECL MqBufferLLogS (
  struct MqS const * const context,
  struct MqBufferLS const * const bufL,
  MQ_CST const func,
  MQ_CST const varname
);

/// \brief get an #MqBufferS object from the buffer list \e buf
/// \context
/// \bufL
/// \param index get the \e index'th element of the \e buf list
/// \retval var the MqBufferS object to return
/// \attention the \e index'th element of the \e buf list \b will be deleted from the buffer list
/// \retMqErrorE
/// \attention if \e bufL is \b not \c NULL the object from \e bufL will be deleted first
MQ_EXTERN enum MqErrorE MQ_DECL MqBufferLGetU (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_SIZE const index,
  struct MqBufferS ** const var
);

/** \} */

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                     E R R O R - A P I                           ### */
/* ###                                                                 ### */
/* ####################################################################### */

/** \defgroup error_api PUBLIC: struct MqErrorS
 *  \ingroup msgque_api
 *  \{
 *  \brief the <EM>MqErrorS API</EM> is used to manage error messages.
 *
 *  the management is done with a \e MqErrorS object created by \c MgCreate.
 *  every \e MqS object has only one \e MqErrorS object and every \e MqErrorS object 
 *  has only one \e MqS object.
 *  the \e MqErrorS object is used to collect all data needed to handle an error.
 */

/// \brief do a \b panic with a vararg argument list
/// \context
/// \prefix
/// \param errnum the error number
/// \format
/// \var_list
/// \attention this function will never return
MQ_EXTERN void MQ_DECL MqPanicVL (
  struct MqS * const context,
  MQ_CST const prefix,
  MQ_INT const errnum,
  MQ_CST const fmt,
  va_list var_list
);

/// \brief do a \b panic with vararg arguments
/// \context
/// \prefix
/// \param errnum the error number
/// \format
/// \attention this function will never return
MQ_EXTERN void MQ_DECL MqPanicV (
  struct MqS * const context,
  MQ_CST const prefix,
  MQ_INT const errnum,
  MQ_CST const fmt,
  ...
) __attribute__ ((format (printf, 4, 5)));

/// \brief do a \b panic with \e string as argument
/// \context
/// \prefix
/// \param errnum the error number
/// \param string the string to be displayed
/// \attention this function will never return
#define MqPanicC(context,prefix,errnum,string) MqPanicV(context,prefix,errnum,"%s",string);

/// \brief do a \b panic with a \b system error message
/// \context
/// \attention this function will never return
#define MqPanicSYS(context) MqPanicV(context,__func__,-1,\
	"internal ERROR in function '%s', please contact your local support", __func__);

/// \brief reset a \e MqErrorS object, change error code to #MQ_OK 
/// \context
MQ_EXTERN void MQ_DECL MqErrorReset (
  struct MqS * const context
);

/*****************************************************************************/
/*                                                                           */
/*                         create an error message                           */
/*                                                                           */
/*****************************************************************************/

/// \brief set an error-code in \e MqErrorS with vararg list argument
/// \context
/// \prefix
/// \param errorcode identify the code
/// \param errnum the error number
/// \format
/// \var_list
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqErrorSGenVL (
  struct MqS * const context,
  MQ_CST const prefix,
  enum MqErrorE const errorcode,
  MQ_INT const errnum,
  MQ_CST const fmt,
  va_list var_list
);

/// \brief set a error-code in \e MqErrorS with vararg string argument
/// \context
/// \prefix
/// \param errorcode identify the code
/// \param errnum the error number
/// \format
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqErrorSGenV (
  struct MqS * const context,
  MQ_CST const prefix,
  enum MqErrorE const errorcode,
  MQ_INT const errnum,
  MQ_CST const fmt,
  ...
) __attribute__ ((format (printf, 5, 6)));

/// \brief customized edition of #MqErrorSGenV
/// \context
/// \prefix
/// \param errnum the error number
/// \param[in] message string message to set
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqErrorC (
  struct MqS * const context,
  MQ_CST const prefix,
  MQ_INT const errnum,
  MQ_CST const message
);

/// \brief customized edition of #MqErrorSGenV
#define MqErrorV(error,prefix,errnum,format,...) \
    MqErrorSGenV(error,prefix,MQ_ERROR,errnum,format, __VA_ARGS__)

/*****************************************************************************/
/*                                                                           */
/*                append text to an existing error message                   */
/*                                                                           */
/*****************************************************************************/

/// \brief append a vararg string to a \e MqErrorS
/// \context
/// \format
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqErrorSAppendV (
  struct MqS const * const context,
  MQ_CST const fmt,
  ...
) __attribute__ ((format (printf, 2, 3)));

/// \brief append a string to a \e MqErrorS
#define MqErrorSAppendC(error,str)        MqErrorSAppendV(error,"%s",str);

/// \brief append a function and filename to the error-buffer
/// \context
/// \param func the name of the function to display (e.g. __func__)
/// \param file the name of the file to display (e.g. __FILE__)
/// \retMqErrorE
MQ_EXTERN enum MqErrorE
MQ_DECL MqErrorStackP (
  struct MqS * const context,
  MQ_CST const func,
  MQ_CST const file
);

/// \brief convenience wrapper for #MqErrorStackP
# define MqErrorStack(error) \
    MqErrorStackP(error, __func__, __FILE__)

/*****************************************************************************/
/*                                                                           */
/*                              error_append                                 */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*                              error_set/get                                */
/*                                                                           */
/*****************************************************************************/

/// \brief return the value of \e MqErrorS code member
/// \context
MQ_EXTERN enum MqErrorE MQ_DECL MqErrorGetCode (
  struct MqS const * const context
);

/// \brief set the value of \e MqErrorS code member
/// \context
/// \param code the \e code to set for \e error
MQ_EXTERN enum MqErrorE MQ_DECL MqErrorSetCode (
  struct MqS * const context,
  enum MqErrorE code
);

/// \brief return the value of \e MqErrorS text member
/// \context
MQ_EXTERN MQ_CST MQ_DECL MqErrorGetText (
  struct MqS const * const context
);

/// \brief return the value of \e MqErrorS num member
MQ_EXTERN void MQ_DECL MqErrorPrint (
  struct MqS * const context
);

/// \brief brint an error to stderr and clear the error after
/// \context
MQ_EXTERN MQ_INT MQ_DECL MqErrorGetNum (
  struct MqS const * const context
);

/// \brief set the value of the \e MqErrorS object
/// \context
/// \param[in] num the error number to set
/// \param[in] code the error code to set
/// \param[in] message the error message to set
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqErrorSet (
  struct MqS * const context,
  int num,
  enum MqErrorE code,
  MQ_CST const message
);

/*****************************************************************************/
/*                                                                           */
/*                              error_is/check                               */
/*                                                                           */
/*****************************************************************************/

/// \brief check on error
/// \return #MQ_OK and #MQ_CONTINUE are \e good statues and #MQ_ERROR and #MQ_EXIT are \e bad statues
#define MqErrorCheckI(PROC) (unlikely((PROC) >= MQ_ERROR))
/// \brief version of #MqErrorCheckI
#define MqErrorCheck(PROC) if (MqErrorCheckI(PROC)) goto error

/// \brief process error message
#define MqErrorSwitch(PROC) switch (PROC) {\
case MQ_OK: break; case MQ_ERROR: goto error; case MQ_EXIT: return MQ_EXIT; case MQ_CONTINUE: return MQ_CONTINUE;\
}
/// \brief process error message
#define MqErrorReturn(PROC) switch (PROC) {\
case MQ_OK: return MQ_OK; case MQ_CONTINUE: return MQ_CONTINUE; case MQ_EXIT: case MQ_ERROR: return MqErrorStack (MQ_CONTEXT_S);\
}; return MQ_OK;
/// \brief check on error and goto label \e error1
#define MqErrorCheck1(PROC) if (MqErrorCheckI(PROC)) goto error1
/// \brief check on error and goto label \a JUMP
#define MqErrorCheck2(PROC,JUMP) if (MqErrorCheckI(PROC)) goto JUMP

/*****************************************************************************/
/*                                                                           */
/*                              error_misc                                   */
/*                                                                           */
/*****************************************************************************/

/// \brief log the \e MqErrorS for debugging
/// \context
/// \prefix
MQ_EXTERN void MQ_DECL MqErrorLog (
  struct MqS const * const context,
  MQ_CST const prefix
);

/// \brief copy a \e MqErrorS from \a in to \a out
/// \param out the #MqS::error object created by #MqLinkCreate
/// \param in  the #MqS::error object created by #MqLinkCreate
MQ_EXTERN enum MqErrorE MQ_DECL MqErrorCopy (
  struct MqS * const out,
  struct MqS * const in
);

/** \} error_api */

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                        R E A D - A P I                          ### */
/* ###                                                                 ### */
/* ####################################################################### */

/** \ingroup msgque_api
 *  \defgroup read_api PUBLIC: Read-Buffer object
 *  \{
 *  \brief Read and split a \libmsgque package into several items.
 *
 *  The reading is done by an \e Read-Buffer object using an \e MqIoS object for 
 *  doing the socket io. Every \e MqS object has only \b one \e Read-Buffer object 
 *  and every \e Read-Buffer object has only \b one \e MqS object. The interface is
 *  used to extract the different #MqTypeE data-types from the package doing
 *  the casting from one type to an other type if necessary.
 */

/*****************************************************************************/
/*                                                                           */
/*                                read_reference                             */
/*                                                                           */
/*****************************************************************************/

/// \brief creating a \e LST reference for reading the list items
/// \context
/// \param[in] buf the buffer object to read the LST object data from or \c NULL to
///   use new data from the \e Read-Buffer object. The object \e buf is the return value
///   #MqReadU
/// \retMqErrorE
///
/// Example:
/// \code
/// MQ_BUF buf = MqReadU(read);
/// ...
/// MqRead_LST_START(read,buf);
/// MQ_INT myInt = MqReadI(read);
/// ...
/// MqRead_LST_END(read);
/// \endcode
MQ_EXTERN enum MqErrorE MQ_DECL MqReadL_START (
  struct MqS * const context,
  MQ_BUF buf
) __attribute__((nonnull(1)));

/// \brief deleting a \e LST reference after reading the list items was finished
/// \context
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqReadL_END (
  struct MqS * const context
) __attribute__((nonnull));

/*****************************************************************************/
/*                                                                           */
/*                              read functions                               */
/*                                                                           */
/*****************************************************************************/

/// \brief read a native typed value from the \e Read-Buffer object
/// \context
/// \param[out] valP a pointer of the value to read
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqReadY (
  struct MqS * const context,
  MQ_BYT * const valP
);

/// \copydoc MqReadY
MQ_EXTERN enum MqErrorE MQ_DECL MqReadO (
  struct MqS * const context,
  MQ_BOL * const valP
);

/// \copydoc MqReadY
MQ_EXTERN enum MqErrorE MQ_DECL MqReadS (
  struct MqS * const context,
  MQ_SRT * const valP
);

/// \copydoc MqReadY
MQ_EXTERN enum MqErrorE MQ_DECL MqReadI (
  struct MqS * const context,
  MQ_INT * const valP
);

/// \copydoc MqReadY
MQ_EXTERN enum MqErrorE MQ_DECL MqReadF (
  struct MqS * const context,
  MQ_FLT * const valP
);

/// \copydoc MqReadY
MQ_EXTERN enum MqErrorE MQ_DECL MqReadW (
  struct MqS * const context,
  MQ_WID * const valP
);

/// \copydoc MqReadY
MQ_EXTERN enum MqErrorE MQ_DECL MqReadD (
  struct MqS * const context,
  MQ_DBL * const valP
);

/// \brief generic function to read an #MQ_STR object from the \e Read-Buffer object
/// \context
/// \retval out the string to return
/// \retMqErrorE
/// \attention the string returned has \e static allocation and have to be
/// duplicated for long term storage.
MQ_EXTERN enum MqErrorE MQ_DECL MqReadC (
  struct MqS * const context,
  MQ_CST * const out
);

/// \brief generic function to read an #MQ_BIN object from the \e Read-Buffer object
/// \context
/// \retval out the binary data to return
/// \retval len the length of the binary data
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqReadB (
  struct MqS * const context,
  MQ_BIN * const out,
  MQ_SIZE * const len
);

/// \brief generic function to read a \b native package object from the \e Read-Buffer object
/// \context
/// \retval out the binary data to return
/// \retval len the length of the binary data
/// \retMqErrorE
/// \attention the data retured belongs to \libmsgque.
///
/// The native package data can be saved and send later back with #MqSendN
MQ_EXTERN enum MqErrorE MQ_DECL MqReadN (
  struct MqS * const context,
  MQ_BIN * const out,
  MQ_SIZE * const len
);

/// \brief generic function to read the entire \e body from the \e Read-Buffer object
/// \context
/// \param[out] out body of the data package
/// \param[out] len size of the body
/// \retMqErrorE
/// \attention the data retured belongs to \libmsgque.
///
/// In opposit to #MqReadN all package items are returned.
/// The native package data can be saved and send later back with #MqSendBDY
MQ_EXTERN enum MqErrorE MQ_DECL MqReadBDY (
  struct MqS * const context,
  MQ_BIN * const out,
  MQ_SIZE * const len
);

/// \brief generic function to read a #MQ_BUF object from the \e Read-Buffer object
/// \context
/// \retval out the buffer to return
/// \retMqErrorE
/// \attention the object returned \e out is owned by \e Read-Buffer object and is \b only valid
/// up to the next call to any \e MqRead? function. If a long-term object is required
/// use the #MqBufferDup function.
/// \code
/// MQ_BUF buf = MqBufferDup(MqReadU(read);
/// \endcode
MQ_EXTERN enum MqErrorE MQ_DECL MqReadU (
  struct MqS * const context,
  MQ_BUF * const out
);

/// \brief read an MqBufferLS object from all items of the \e Read-Buffer object
/// \context
/// \param[in,out] out data will be appended to \e *out, if \e *out != \c NULL the
///                    \e *out value will be used otherwise \e *out will be set to a new
///		       #MqBufferLS object
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqReadL (
  struct MqS * const context,
  struct MqBufferLS ** const out
);

MQ_EXTERN enum MqErrorE MQ_DECL MqReadProxy (
  struct MqS * const readctx,
  struct MqS * const sendctx
);

/*****************************************************************************/
/*                                                                           */
/*                                read_high                                  */
/*                                                                           */
/*****************************************************************************/

/// \brief undo the \b last MqReadX operation
/// \context
/// \retMqErrorE
/// \attention Can \b undo only the \b last MqReadX operation, multiple \b undo
/// will \b corrupt the memory.
MQ_EXTERN enum MqErrorE MQ_DECL MqReadUndo (
  struct MqS * const context
);

/// \brief get the number of items left in the \e Read-Buffer object
/// \context
/// \return the number of items
MQ_EXTERN MQ_SIZE MQ_DECL MqReadGetNumItems (
  struct MqS const * const context
);

/// \brief does an additional item exits in the \e Read-Buffer object
/// \context
/// \return the number of items
MQ_EXTERN MQ_BOL MQ_DECL MqReadItemExists (
  struct MqS const * const context
);

/** \} read_api */

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                        S E N D - A P I                          ### */
/* ###                                                                 ### */
/* ####################################################################### */

/** \ingroup msgque_api
 *  \defgroup send_api PUBLIC: Send-Buffer object
 *  \{
 *  \brief Collect and send data using a LibMsgque package
 *
 *  the management is done by a \e Send-Buffer object using an \e Io object for 
 *  doing the socket io. Every \e msgque object has only \b one \e Send-Buffer object 
 *  and every \e Send-Buffer object has only \b one link to a \e MqS object.
 *
 *  example 1: in the client, calling the \e service and wait for an answer
 *  \code
void MyServiceCall(struct MqS * const context) { // the libmsgque object
    ...
    MqSendSTART(context);                          // init the Send-Buffer object
    MqSendI(context,int_value);                    // 1. argument: a MQ_INT value
    MqSendV(context,"num:%x",num);                 // 2. argument: a vararg string value
    MqSendB(context,mypicture,size);               // 3. argument: a binary picture of size length
    // call service "SRV1" and wait maximum 60 seconds for the results
    MqErrorCheck(MqSendEND_AND_WAIT(context,"SRV1",60);
    // ... get the results
    MqErrorCheck(MqReadI(context, ???);
    ...
}
 *  \endcode
 *
 *  example 2: in the server, answer the \e service call
 *  \code
static MqErrorE Ot_WAR1(struct MqS * const context, MQ_PTR data) {
  // ... do some int work
    MqSendSTART(context);                       // init the Send-Buffer object
    MqErrorCheck(MqReadI(context,&myInt);       // read a MQ_INT value
    MqErrorCheck(MqReadC(context,&myStr);       // read a MQ_STR value
    MqErrorCheck(MqReadU(context,&myPic);       // read a MqBufferS object to store the picture data
  // ... do some processing
  error:                                        // something is wrong, error back
    return MqSendRETURN(context);		// send the package as an answer of a previous service-call
}
 *  \endcode
 **/

/*****************************************************************************/
/*                                                                           */
/*                              send functions                               */
/*                                                                           */
/*****************************************************************************/

/// \brief append a native typed value to the \e Send-Buffer object
/// \context
/// \param[in] val the value to send
/// \retMqErrorE
/// \attention In \b binary mode the data will be send as binary, in \b string mode as string.
MQ_EXTERN enum MqErrorE MQ_DECL MqSendY (
  struct MqS * const context,
  const MQ_BYT val
);

/// \copydoc MqSendY
MQ_EXTERN enum MqErrorE MQ_DECL MqSendO (
  struct MqS * const context,
  const MQ_BOL val
);

/// \copydoc MqSendY
MQ_EXTERN enum MqErrorE MQ_DECL MqSendS (
  struct MqS * const context,
  const MQ_SRT val
);

/// \copydoc MqSendY
MQ_EXTERN enum MqErrorE MQ_DECL MqSendI (
  struct MqS * const context,
  const MQ_INT val
);

/// \copydoc MqSendY
MQ_EXTERN enum MqErrorE MQ_DECL MqSendF (
  struct MqS * const context,
  const MQ_FLT val
);

/// \copydoc MqSendY
MQ_EXTERN enum MqErrorE MQ_DECL MqSendW (
  struct MqS * const context,
  const MQ_WID val
);

/// \copydoc MqSendY
MQ_EXTERN enum MqErrorE MQ_DECL MqSendD (
  struct MqS * const context,
  const MQ_DBL val
);

/// \brief append a #MQ_STR object to the \e Send-Buffer object
/// \context
/// \param in the string data to send
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqSendC (
  struct MqS * const context,
  MQ_CST const in
);

/// \brief append a #MQ_BIN object to the \e Send-Buffer object
/// \context
/// \param in the binary data to send
/// \param len the size of the binary data to send
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqSendB (
  struct MqS * const context,
  MQ_BINB const * const in,
  MQ_SIZE const len
);

/// \brief append a native package object to the \e Send-Buffer object
/// \context
/// \param in the binary data to send, result from the previous #MqReadN call
/// \param len the size of the binary data to send
MQ_EXTERN enum MqErrorE MQ_DECL MqSendN (
  struct MqS * const context,
  MQ_BINB const * const in,
  MQ_SIZE const len
);

/// \brief append an entire package body object to the \e Send-Buffer object
/// \context
/// \param in the body package data to send, result from the previous #MqReadBDY call
/// \param len the size of the binary data to send
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqSendBDY (
  struct MqS * const context,
  MQ_BINB const * const in,
  MQ_SIZE const len
);

/// \brief append a #MQ_BUF object to the \e Send-Buffer object
/// \context
/// \param in the pointer to an #MqBufferS object to send
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqSendU (
  struct MqS * const context,
  struct MqBufferS * const in
);

/// \brief append a \e MqBufferLS object to the \e Send-Buffer object
/// \context
/// \param in the pointer to an #MqBufferLS object to send
/// \retMqErrorE
/// \attention all items of \e in are appended as single arguments to the call using #MqSendU
MQ_EXTERN enum MqErrorE MQ_DECL MqSendL (
  struct MqS * const context,
  struct MqBufferLS const * const in
);

/// \brief append a vararg string to the \e Send-Buffer object
/// \context
/// \format
MQ_EXTERN enum MqErrorE MQ_DECL MqSendV (
  struct MqS * const context,
  MQ_CST const fmt,
  ...
) __attribute__ ((format (printf, 2, 3)));

/// \brief append a vararg string as va_list to the \e Send-Buffer object
/// \context
/// \format
/// \var_list
MQ_EXTERN enum MqErrorE MQ_DECL MqSendVL (
  struct MqS * const context,
  MQ_CST const fmt,
  va_list var_list
);

/*****************************************************************************/
/*                                                                           */
/*                              send_compose                                 */
/*                                                                           */
/*****************************************************************************/

/// \brief initialize the \e Send-Buffer object and start to create a Msgque package
/// \context
MQ_EXTERN enum MqErrorE MQ_DECL MqSendSTART (
  struct MqS * const context
);

/** \brief finish and send a \e service Msgque packet without waiting on \e return
 * 
 * This function is used to call a \e remote service or answer a \e service-call.
 * \context
 * \token
 * \retMqErrorE
 */
MQ_EXTERN enum MqErrorE MQ_DECL MqSendEND (
  struct MqS * const context,
  MQ_CST const token
);

/** \brief send a \e service Msgque packet and waiting on \e return
 *
 * This function is used to call a \e remote service and wait for an answer.
 * \context
 * \token
 * \param_timeout
 * \retMqErrorE (without #MQ_CONTINUE)
 */
MQ_EXTERN enum MqErrorE MQ_DECL MqSendEND_AND_WAIT (
  struct MqS * const context,
  MQ_CST const token,
  MQ_TIME_T const timeout
);

/** \brief send a \e service Msgque packet with a \e callback function defined
 *
 * This function is used to call a \e remote service and \e don't wait for an answer.
 * The answer will be processed asynchronous in #MqProcessEvent.
 * \context
 * \token
 * \param[in] proc A service procedure called with the results of the transaction
 * \param[in] data A user defined additional data
 * \param[in] datafreeF A function to free \e data after use
 * \retMqErrorE (without #MQ_CONTINUE)
 * \attention This function will \e never block.
 */
MQ_EXTERN enum MqErrorE MQ_DECL MqSendEND_AND_CALLBACK (
  struct MqS * const context,
  MQ_CST const token,
  MqTokenF const proc,
  MQ_PTR data,
  MqTokenDataFreeF datafreeF
);

/** \brief send a \e return Msgque packet with data from the current \e MqErrorS object
 * 
 *  Utility function to simplify the \e send task for a \e service handle.
 *
 *  \context
 *  \retMqErrorE
 */
MQ_EXTERN enum MqErrorE MQ_DECL MqSendRETURN (
  struct MqS * const context
);

/** \brief send a msgque error from a server to a client
 * 
 *  Utility function to simplify the \e send task for a \e service handle.
 *
 *  \context
 *  \retMqErrorE
 */
MQ_EXTERN enum MqErrorE MQ_DECL MqSendERROR (
  register struct MqS * const context
);

/*****************************************************************************/
/*                                                                           */
/*                              send_list                                    */
/*                                                                           */
/*****************************************************************************/

/** \brief start to append an embedded \e list item to the \e Send-Buffer object
 *  \context
 *
 *  example: create a \e list item
 *  \code
MqSendL_START(send);     // start a LST item
MqSendI(send,myInt);        // first LST sub-item
MqSendC(send,"myString");   // second LST sub-item
// ... do additional MqSend?
MqSendL_END(send);       // finish a LST item
 *  \endcode
 */
MQ_EXTERN enum MqErrorE MQ_DECL MqSendL_START (
  struct MqS * const context
);

/** \brief end to append an embedded \e list item to the \e Send-Buffer object
 *  \context
 */
MQ_EXTERN enum MqErrorE MQ_DECL MqSendL_END (
  struct MqS * const context
);

/** \} send_api */

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                     E V E N T - A P I                           ### */
/* ###                                                                 ### */
/* ####################################################################### */

/** \defgroup event_api PUBLIC: event interface functions
 *  \ingroup lib
 *  \{
 *  \brief handle different aspects of the \e MqS event-queue
 *
 *  the event-queue have to be linked into an existing event-processing infrastructure. 
 *  the linking is done by defining prototypes for two independent functions:
 *  - \b MqEventF is used to call an external event processing function (e.g. \c Tcl_DoOneEvent)
 *  - \b EventCreateF is used to add events, created by the file-handles owned by the \e MqS objects, 
 *       into the external event-queue (e.g. \c Tcl_QueueEvent)
 *  .
 */

/// \brief prototype for a Event-Check function
///
/// This function is called by the \libmsgque event-loop as idle-task in #MqProcessEvent
typedef enum MqErrorE (
  *EventCreateF
) (
  struct MqS * const
);

/** \brief Waiting for an incoming packet on all open msgque objects.
 * 
 *  This function is used to wait for (e.g. #MQ_WAIT_ONCE) or check (e.g. #MQ_WAIT_NO) for an 
 *  incoming event. If an event occurs the header of the Msgque packet is parsed and
 *  the according service-handle is called. if #MQ_WAIT_FOREVER is set the function
 *  will never return, accept for an error or on exit. This function is usually used
 *  on a server to enter the event-loop and wait for incoming service requests.
 * 
 *  \context
 *  \param_timeout_with_default
 *  \param wait should the code wait forever (e.g. #MQ_WAIT_FOREVER), just wait for the next event (e.g. #MQ_WAIT_ONCE) or just return after one single test (#MQ_WAIT_NO)"
 *  \retMqErrorE
 *
 *  \attention The \e timeout argument is only used if #MQ_WAIT_ONCE is set.
 *
 *  A typical server looks like:
\code
...
MqErrorCheck(MqLinkCreate(ctx,argv,alfa);
...
MqErrorCheck(MqProcessEvent(ctx, MQ_TIMEOUT, MQ_WAIT_FOREVER);
...
MqLinkDelete(ctx);
...
\endcode

 **/

MQ_EXTERN enum MqErrorE MQ_DECL MqProcessEvent (
  struct MqS * const context,
  MQ_TIME_T timeout,
  enum MqWaitOnEventE const wait
);

/** \} */

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                       S L A V E - A P I                         ### */
/* ###                                                                 ### */
/* ####################################################################### */

/** \defgroup slave_api PUBLIC: struct MqLinkSlaveS
 *  \ingroup msgque_api
 *  \{
 *  \brief Access to the master / slave feature

The master/slave link is used to create a mesh of nodes and
to link different \e PARENT context objects together.
<B>The master control the slave.</B>

The \e link is used to perform the following tasks:
 - report error messages from the \e slave to the \e master
 - to create a \e slave-child-context if a \e master-child-context is created
 - to delete a \e slave-context if a \e master-context is deleted
 .

In difference to the \e client/server link the \e master/slave link connect
two independent msgque-context in the same process or thread (e.g. node).
This leads to the restriction that only the \e master-msgque-context can be
a \e server because only \e on server per node is possible.

\verbatim
    node-0   |           node-1          |   node-2
===================================================================

| ** client/server link ** | ** client/server link ** |

             | *** master/slave link *** |

                           |- client1-0 -|- server2-0 ...
                           |
             |- server1-0 -|             |- server2-1 ...
             |             |- client1-1 -|- server2-2 ...
  client0-0 -|                           |- server2-3
             |
             |- server1-1 -|- client1-2 -|- server2-4 ...

\endverbatim

 **/

/// \brief the maximum number of slave objects per master
/// \attention this number can be changed but \libmsgque have to be recompiled after
#define MQ_SLAVE_MAX 1024

/// \brief create a \e master/slave link using the same object as \e context
///
/// \e context have to be a \e SERVER-PARENT context without \e CHILD_CONTEXT
/// available
///
/// \context
/// \param[in] id an integer used as unique identifier for the master/slave link 
/// \param[in] argsP command-line arguments passed to the \e worker-client or the \e worker-server. all arguments prior the
///                 first \b @ token are added to the \e worker-client and the other arguments to the \e worker-server.
///                 the memory is freed after use.
/// \retMqErrorE
MQ_EXTERN enum MqErrorE
MQ_DECL MqSlaveWorker (
  struct MqS * const  context,
  MQ_SIZE const	      id,
  struct MqBufferLS ** argsP
);

/// \brief create a \e master/slave link between the current context object and a slave 
/// context object
///
/// The link has the following requirements:
///  - the \e context object have to be a \e PARENT without a \e CHILD
///  - the \e slave object have to be a \e CLIENT-PARENT without a \e CHILD
///  - the \e id have to be unique
///  .
///
/// \context
/// \id
/// \param[in] slave a \e slave context object created by #MqLinkCreate
/// \retMqErrorE
/// \attention after \e MqSlaveCreate the entire \e slave object blongs to \e context 
/// including the inital context. This mean no other external reference should be 
/// used for object cleanup.
MQ_EXTERN enum MqErrorE
MQ_DECL MqSlaveCreate (
  struct MqS * const  context,
  MQ_SIZE const       id,
  struct MqS * const  slave
);

/// \brief delete a \e master/slave
///
/// By default the \e slave context will be deleted if the \e master context is deleted.
/// In addition the \e parent-slave-context can be deleted explicit.
/// It is an \e error to delete a \e child-slave-context.
/// \context
/// \id
/// \retMqErrorE
/// \attention it is still possible to delete a \e child-slave-context using #MqLinkDelete but
/// this will break the internal master/slave order
MQ_EXTERN enum MqErrorE
MQ_DECL MqSlaveDelete (
  struct MqS * const  context,
  MQ_SIZE const id
);

/// \brief get the slave context
///
/// \context
/// \id
/// \return the \e slave context or \c NULL if \e id is not valid
MQ_EXTERN struct MqS *
MQ_DECL MqSlaveGet (
  struct MqS const * const  context,
  MQ_SIZE const id
);

/** \} slave api */

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                     S Y S T E M - A P I                         ### */
/* ###                                                                 ### */
/* ####################################################################### */

/** \defgroup system_api PUBLIC: system interface functions
 *  \ingroup lib
 *  \{
 *  \brief Access to native system functions including \libmsgque error handling
 **/

/// \brief 'calloc' system call with error handling feature
/// \context
/// \param nmemb the number of members in the memory block
/// \param size the size of the new memory block
/// \return a pointer to the new memory block initialized with '0'
MQ_EXTERN MQ_PTR MQ_DECL MqSysCalloc (
  struct MqS * const context,
  MQ_SIZE nmemb,
  MQ_SIZE size
);

/// \brief 'malloc' system call with error handling feature
/// \context
/// \param size the size of the new memory block
/// \return a pointer to the new memory block
MQ_EXTERN MQ_PTR MQ_DECL MqSysMalloc (
  struct MqS * const context,
  MQ_SIZE size
);

/// \brief 'realloc' system call with error handling feature
/// \context
/// \param buf the \b old memory block to extend
/// \param size the size of the new memory block
/// \return a pointer to the new memory block
MQ_EXTERN MQ_PTR MQ_DECL MqSysRealloc (
  struct MqS * const context,
  MQ_PTR buf,
  MQ_SIZE size
);

/// \brief 'free' system call wrapper of libmsgque
MQ_EXTERN void MQ_DECL MqSysFreeP (
  MQ_PTR ptr
);

/// \brief 'free' system call macro
/// \retval tgt the memory block to delete and set to NULL
#define MqSysFree(tgt) \
    do { \
	if ( likely((tgt) != (NULL)) ) { \
	    MqSysFreeP((MQ_PTR)tgt); \
	    (tgt) = (NULL); \
	} \
    } while (0)

/// \brief 'usleep' system call with error handling feature
/// \context
/// \param usec the micro (10^-6) seconds to sleep
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqSysUSleep (
  struct MqS * const context,
  unsigned int const usec
);

/// \brief 'sleep' system call with error handling feature
/// \context
/// \param sec the seconds to sleep
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqSysSleep (
  struct MqS * const context,
  unsigned int const sec
);

/// \brief 'basename' system call with error handling feature
/// \param[in] in name of the string to extract the basename from (value of \e in will not be changed)
/// \param[in] includeExtension add extension like '.exe' to the filename (#MQ_YES or #MQ_NO)
/// \return the basename of \e in (it is save to modify the basename for additional needs)
/// \attention the memory of the basename string returned is owned by the caller and have to be freed
MQ_EXTERN MQ_STR MQ_DECL MqSysBasename (
  char const * const in, 
  MQ_BOL includeExtension
);

/// \brief 'gettimeofday' system call with error handling feature
/// \context
/// \retval tv the timeval object
/// \retval tz the timezone object
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL SysGetTimeOfDay (
  struct MqS * const context,
  struct mq_timeval * tv,
  struct mq_timezone * tz
);

  
/// \brief the function \e strdup which allows the argument \c NULL
static mq_inline MQ_STR mq_strdup_save (
  MQ_CST v
) {
  return v != NULL ? mq_strdup(v) : NULL;
}

/** \} system api */

/** \defgroup log_api PUBLIC: logging interface functions
 *  \ingroup lib
 *  \{
 *  \brief logging information on stderr with a common format.
 */

/*****************************************************************************/
/*                                                                           */
/*                               log_basic                                   */
/*                                                                           */
/*****************************************************************************/

/// \brief log vararg string without formatting to \e channel
/// \param channel the file-handle to report the message on
/// \format
MQ_EXTERN void MQ_DECL MqLog (
  FILE * channel,
  MQ_CST const fmt,
  ...
) __attribute__ ((format (printf, 2, 3)));

/// \brief log vararg string using formatting
/// \context
/// \proc
/// \param level debug level: 0 <= level <= 9
/// \format
/// \attention if the \b -silent option is set nothing will be reported
MQ_EXTERN void MQ_DECL MqDLogX (
  struct MqS const * const context,
  MQ_CST const proc,
  MQ_INT level,
  MQ_CST const fmt,
  ...
) __attribute__ ((format (printf, 4, 5)));

#if defined(MQ_DISABLE_LOG)
#   define MqSetDebugLevel(context)
#   define MqDLogC(context,level,str)
#   define MqDLogV(context,level,fmt,MQ_VAR_ARGS)
#else

#define MqSetDebugLevel(context) debugLevel = context->config.debug

/// \brief log a plain string 
/// \context
/// \param level debug level 0 <= level <= 9
/// \param str string to log
#   define MqDLogC(context, level,str) \
      if (unlikely(context != NULL && context->config.debug >= level)) MqDLogX(context,__func__,level,str)

#   define MqDLogCL(context, level,str) \
      if (unlikely(debugLevel >= level)) MqDLogX(context,__func__,level,str)

/// \brief log a vararg string using formatting and checking for logging level
/// \context
/// \param level debug level 0 <= level <= 9
/// \format
/// \param ... vararg string argument
/// \attention the prefix string is the calling function name
#   define MqDLogV(context, level,fmt,...) \
      if (unlikely(MQ_ERROR_IS_POINTER(context) && context->config.debug >= level)) MqDLogX(context,__func__,level,fmt,__VA_ARGS__)

#   define MqDLogVL(context, level,fmt,...) \
      if (unlikely(debugLevel >= level)) MqDLogX(context,__func__,level,fmt,__VA_ARGS__)

#endif

/// \brief log a vararg error list using formatting
/// \context
/// \proc
/// \format
/// \var_list
MQ_EXTERN void MQ_DECL MqDLogEVL (
  struct MqS const * const context,
  MQ_CST const proc,
  MQ_CST const fmt,
  va_list var_list
);

/// \brief log a vararg error string using formatting
/// \context
/// \proc
/// \format
MQ_EXTERN void MQ_DECL MqDLogEV (
  struct MqS const * const context,
  MQ_CST const proc,
  MQ_CST const fmt,
  ...
) __attribute__ ((format (printf, 3, 4)));

/*****************************************************************************/
/*                                                                           */
/*                               log_advanced                                */
/*                                                                           */
/*****************************************************************************/

/// \brief convert a #MqErrorE into a human readable string
/// \param code a value from #MqErrorE
/// \return the string
MQ_EXTERN MQ_CST MQ_DECL MqLogErrorCode (
  enum MqErrorE code
);

/// \brief get the \e type member from an MqTypeE object in human readable form
/// \type
/// \return the 3 character name of the #MqTypeE without the \c "MQ_" prefix and the \c "T" postfix
MQ_EXTERN MQ_CST MQ_DECL MqLogTypeName (
  const enum MqTypeE type
);

/// \brief convert a MqContextS server member into a human readable string
/// \context
/// \return the string \e SERVER or \e CLIENT
MQ_EXTERN MQ_CST MQ_DECL MqLogServerOrClient (
  struct MqS const * const context
);

/// \brief convert a MqContextS parent member into a human readable string
/// \context
/// \return the string \e PARENT or \e CHILD
MQ_EXTERN MQ_CST MQ_DECL MqLogParentOrChild (
  struct MqS const * const context
);

/// \brief convert a raw #MQ_STR into a human readable string.
/// \param buf buffer of size \e size+1 able to store the temporary string representation of \e str
/// \param str the input string
/// \param size the size of \e str
/// \return the string
///
/// replace non ASCII characters with '?' or 'o' (NULL)
MQ_EXTERN MQ_STR MQ_DECL MqLogC (
  MQ_STR buf,
  MQ_STR str,
  MQ_SIZE size
);

/// \}

//end c++ save definition
END_C_DECLS

#endif /* MQ_MSGQUE_H */







