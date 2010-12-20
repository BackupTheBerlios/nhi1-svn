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

/// \defgroup Mq_C_API Mq_C_API
/// \{
/// \brief public libmsgque API
///
/// The msgque project is an infrastructure to link software together to act like a single software.
/// To link mean distributing work from one software to an other software an wait or not wait for an
/// answer. The linking is done using unix or inet domain sockets and is based on packages send from one software
/// to an other software and back. The msgque project is used to handle all the different aspects for
/// setup and maintain the link and is responsible for:
///   - starting and stopping the server application
///   - starting and stopping the communication interface
///   - sending and receiving package data
///   - reading and writing data from or into a package
///   - setup and maintain the event-handling for an asynchronous transfer
///   - propagate the error messages from the server to the client
///   .

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                      M S G Q U E - A P I                        ### */
/* ###                                                                 ### */
/* ####################################################################### */

/** \defgroup Mq_Type_C_API Mq_Type_C_API
 *  \{
 *  \brief a collection of common used data types and definitions
 */

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
# define mq_strdup _strdup
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

/// \ingroup Mq_System_C_API
# define mq_strtoll strtoll
/// \ingroup Mq_System_C_API
# define mq_strdup strdup
/// \ingroup Mq_System_C_API
# define mq_getpid getpid
/// \ingroup Mq_System_C_API
# define mq_unlink unlink
/// \ingroup Mq_System_C_API
# define mq_snprintf snprintf
/// \ingroup Mq_System_C_API
# define MQ_CLRCALL
/// \ingroup Mq_System_C_API
# define MQ_STDCALL
/// \ingroup Mq_System_C_API
# define MQ_CDECL
/// \ingroup Mq_System_C_API
# define mq_inline inline

#endif


#if defined(_MANAGED)
# define MQ_DECL MQ_CLRCALL
#elif defined(MQ_COMPILE_AS_CC)
# define MQ_DECL MQ_STDCALL
#else
# define MQ_DECL MQ_CDECL
#endif

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
/// \ingroup Mq_System_C_API
#define mq_timeval timeval
/// \ingroup Mq_System_C_API
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
/// \brief request the default timeout value
#define MQ_TIMEOUT_DEFAULT -1
/// \brief request the user defined timeout value from the \RNSC{timeout} configuration value
#define MQ_TIMEOUT_USER -2
/// \brief request the maximum possible (infinite) timeout value
#define MQ_TIMEOUT_MAX -3
#endif

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
#   if defined(DLL_EXPORT)
        // does we build the libmsgque library ?
#	if defined(MQ_BUILD_LIBMSGQUE_DLL)
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
/// \brief MQ_ALFA the command-line separator as string
#define MQ_ALFA_STR "@"

struct MqS;
struct MqTransS;
struct MqErrorS;
struct MqBufferLS;
struct MqBufferS;
union  MqBufferU;
struct MqConfigS;
struct MqEventS;
struct MqFactoryItemS;

/*****************************************************************************/
/*                                                                           */
/*                            types/generic                                  */
/*                                                                           */
/*****************************************************************************/

/// 1 byte \b byte data-type
typedef signed char MQ_BYT;
/// 1 byte \b boolean data-type
typedef unsigned char MQ_BOL;
/// 2 byte \b short data-type
typedef signed short MQ_SRT;
/// 4 byte \b integer data-type
typedef signed int MQ_INT;
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

/// \brief array size data-type
/// \attention -> need signed integer in pReadWord
///            -> need int because of pSwapI in pReadHDR
typedef int MQ_SIZE;

/// \brief OS wide MQ_TIME_T
typedef MQ_WID MQ_TIME_T;

/// handle data-type
typedef int MQ_HDL;

// "google go" can not uses nested typedef's -> use "#define" for the basic typ's

/// pointer basic data-type
#define MQ_PTRB void
/// string basic data-type
#define MQ_STRB char
/// \e byte-array basic data-type
#define MQ_BINB unsigned char
/// list basic data-type
#define MQ_LSTB unsigned char

/// generic pointer data-type
typedef MQ_PTRB *MQ_PTR;
/// string pointer data-type
typedef MQ_STRB *MQ_STR;
/// const string pointer data-type
typedef MQ_STRB const * MQ_CST;
/// \e byte-array pointer data-type
typedef MQ_BINB *MQ_BIN;
/// \e const byte-array pointer data-type
typedef MQ_BINB const* MQ_CBI;
/// list pointer data-type
typedef MQ_LSTB *MQ_LST;
/// Buffer pointer data-type
typedef struct MqBufferS *MQ_BUF;
/// Buffer-List pointer data-type
typedef struct MqBufferLS *MQ_BFL;
/// Context pointer data-type
typedef struct MqS *MQ_CTX;
/// \brief data type for a socket handle
typedef MQ_INT  MQ_SOCK;

/*****************************************************************************/
/*                                                                           */
/*                            enum definitions                               */
/*                                                                           */
/*****************************************************************************/

/// \brief boolean NO
#define MQ_NO  0

/// \brief boolean YES
#define MQ_YES 1

/// \ingroup Mq_Error_C_API
/// \brief collection for the different error-codes
enum MqErrorE {
  MQ_OK		= 0,		///< everything is OK, no error available (persistent)
  MQ_CONTINUE	= 1,            ///< continue with upper code
  MQ_ERROR	= 2,            ///< exit upper code with an error (persistent)
};

/// \ingroup Mq_Link_C_API
/// \brief object responsible to manage a client/server link data
struct MqLinkS {

  // private variables
  struct MqSendS    * send;	    ///< object for sending a Msgque packet
  struct MqReadS    * read;	    ///< object for reading a Msgque packet
  struct MqIoS	    * io;	    ///< object for management of the 'socket' infrastructure
  struct MqBufferLS * alfa;	    ///< the command-line arguments for the server

  // context-management variables
  MQ_SIZE   ctxId;		    ///< the ctxId of this MqS object
  struct MqS *ctxIdP;		    ///< the initial (first) context (home of the ctxIdA)

  // private variables
  struct pTokenS * srvT;	    ///< identifier for the 'service' token handle
  MQ_CST targetIdent;		    ///< 'ident' of the link target
  struct MqCacheS * readCache;	    ///< cache for MqReadS

  /// \brief bit-field to represent the boolean values
  struct {
    MQ_BOL endian	      : 1 ; ///< a endian switch have to be done? (boolean: MQ_YES or MQ_NO)
    MQ_BOL onCreateStart      : 1 ; ///< Start "MqLinkCreate"
    MQ_BOL onCreateEnd	      : 1 ; ///< End "MqLinkCreate"
    MQ_BOL onDelete	      :	1 ; ///< is already a "delete" ongoing?
    MQ_BOL onShutdown	      :	1 ; ///< is already a "shutdown" ongoing?
    MQ_BOL prepareDone	      :	1 ; ///< was a prepare already done ?
    MQ_BOL doFactoryCleanup   :	1 ; ///< was the context create by a 'Factory'
    MQ_BOL flagServerSetup    :	1 ; ///< setup.ServerSetup.fCall was called ?
    MQ_BOL isWORKER	      :	1 ; ///< is alfa[0] set to "WORKER"
    MQ_BOL isConnected	      : 1 ; ///< is the "client-link" is "connected" ?
  } bits;

  // the next 3 items are !!only!! used in the parent
  MQ_SIZE   ctxIdR;		    ///< the largest currently used ctxId number
  MQ_SIZE   ctxIdZ;		    ///< the size of the ctxIdA array
  struct MqS ** ctxIdA;		    ///< array of struct MqLinkS * pointer's

  // the next 3 items are used to map the transactionID (int) to the transaction pointer
  struct MqTransS * trans;	    ///< link to the trans object
  MQ_HDL _trans;		    ///< storage for the Transaction object from the package header

  // the following lines manage the link between the parent and the child,
  // to be able to delete all child's if the parent is deleted
  struct pChildS * childs;	    ///< linked list of child's
  struct pChildS * self;	    ///< my own child storage

  // master/slave relationship
  struct MqLinkSlaveS * slave;	    ///< link to the SLAVE object
};

/// \ingroup Mq_Error_C_API
/// \brief error-object data type
struct MqErrorS {
  enum MqErrorE code;		///< the error code
  struct MqBufferS * text;      ///< the error message
  MQ_INT num;	                ///< the error number also used as exit code
  MQ_BOL append;		///< allow to append? MQ_YES or MQ_NO
  struct MqS * errctx;		///< source of the error, NULL = current context
};

/** \} Mq_Type_C_API */

/*****************************************************************************/
/*                                                                           */
/*                           C O N F I G - API                               */
/*                                                                           */
/*****************************************************************************/

/// \defgroup Mq_Config_C_API Mq_Config_C_API
/// \{
/// \brief configuration of a context
///
/// The configuration is done persistent using \e config-api functions or
/// on link-setup using \e command-line-arguments.

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
typedef void ( MQ_DECL
  *MqTokenDataCopyF
) (
  struct MqS * const context,
  MQ_PTR *dataP
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
  struct MqFactoryItemS* const item,
  struct MqS  ** contextP
);

/// \brief prototype for a Object-Delete factory function
typedef void ( MQ_DECL
  *MqFactoryDeleteF
) (
  struct MqS  * context,
  MQ_BOL doFactoryCleanup,
  struct MqFactoryItemS* const item
);

/// \brief prototype for a Garbage-Collection mark function
typedef void ( MQ_DECL
  *MqMarkF
) (
  MQ_PTR data
);

/// \ingroup Mq_Type_C_API
/// \brief prototype for exit a process or thread
typedef void ( MQ_DECL
  *MqExitF
) (
  int num
);

/// \ingroup Mq_Type_C_API
/// \brief used to setup (initialize) a new thread/fork/process created by \libmsgque
///        using the \c SysServer? style commands
typedef void (MQ_DECL *MqSetupF) ( struct MqS * const );

/// \brief function pointer to \e create an object
struct MqFactoryCreateS {
  MqFactoryCreateF	fCall;	    ///< create a new object
  MQ_PTR		data;	    ///< additional data pointer for the fCall
  MqTokenDataFreeF	fFree;	    ///< free additional data pointer
};

/// \brief function pointer to \e delete an object
struct MqFactoryDeleteS {
  MqFactoryDeleteF	fCall;	    ///< delete the object created with #MqFactoryCreateS
  MQ_PTR		data;	    ///< additional data pointer for the fCreate
  MqTokenDataFreeF	fFree;	    ///< free additional data pointer
};

/// \brief data used to define a factory
struct MqFactoryItemS {
  MQ_CST name;			    ///< public known factory name
  struct MqFactoryCreateS Create;   ///< object creation function
  struct MqFactoryDeleteS Delete;   ///< object deletion function
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
  MqTokenF		fCall;	    ///< callback method
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
    MQ_IO_PIPE = 0,
    MQ_IO_UDS  = 1, 
    MQ_IO_TCP  = 2
};

/// \brief configure a context to use a \e uds-client-server-link
///
/// The \e uds-socket (http://en.wikipedia.org/wiki/Unix_domain_socket) is usually 
/// 50% faster than a local tcp communication but only available on UNIX.
/// \param[in] file name of a \e uds-socket-file (default: \null)
struct MqIoUdsConfigS {
  MQ_BUF file;
};

/// \brief configure a context to use a \e tcp-client-server-link
///
/// \param[in] host
///  client: name of the remote interface (default: localhost)\n
///  server: name of the local interface (default: listen on \e all interfaces)
/// \param[in] port
///  client: name of the remote port\n
///  server: name of the local port
/// \param[in] myhost client: name of the local interface
/// \param[in] myport client: name of the local port
struct MqIoTcpConfigS {
  struct MqBufferS * host;
  struct MqBufferS * port;
  struct MqBufferS * myhost;
  struct MqBufferS * myport;
};

/// \brief configure a context to use a \e pipe-client-server-link
///
/// The \e socket option is special because it is used only for internal purpose to submit the socket from the
/// client to the server started as pipe by the client.
/// The only public usage for this option is to serve as
/// interface for an existing tool like (x)inetd. The (x)inetd tool is a \e UNIX service to listen on a tcp/ip port
/// and start for every connection the proper entry from the file \e /etc/(x)inetd.conf with the file-descriptor
/// \e 0 as send/recv socket handle.
/// \param[in] socket the \e file-descriptor-number (default: not set)
struct MqIoPipeConfigS {
  MQ_SOCK socket[2] ; ///< result from a \e socketpair function-call, [0] for the \e client-socket and [1] for the \e server-socket
};
 
/// \brief configuration data which belong to \e MqIoS
struct MqIoConfigS {

  /// \brief user defined timeout to terminate a blocking function call (default: 90 sec)
  MQ_TIME_T timeout;

  /// \brief set the OS specific value for the \e socket-operation-buffer (default: OS specific)
  MQ_INT buffersize;

  /// \brief what kind of socket interface to use?
  /// <TABLE>
  /// <TR>  <TH>type</TH>           <TH>default</TH> <TH>option</TH>             <TH>application</TH>   <TH>context</TH>  </TR>
  /// <TR>  <TD>INTEGER (enum)</TD> <TD>IO_PIPE</TD> <TD>--tcp/--uds/--pipe</TD> <TD>server</TD>	  <TD>parent</TD>   </TR>
  /// </TABLE>
  enum MqIoComE com;

  /// \brief set \e uds configuration-data
  struct MqIoUdsConfigS	  uds;

  /// \brief set \e tcp configuration-data
  struct MqIoTcpConfigS	  tcp;

  /// \brief set \e pipe configuration-data
  struct MqIoPipeConfigS  pipe;
};

/// \brief end-user configuration data, also available as command-line options
struct MqConfigS {

  /// \brief write (\no) or don't write (\yes) any massages to stdout or stderr (default: \no)
  MQ_BOL isSilent;

  /// \brief use the string-mode (\yes) or the binary-mode (\no) to send \e native-data
  ///
  /// The items in a \e data-package are defined as \RNS{BufferIdentifer} and can 
  /// be transmitted as \e string (slow) or as \e binary (fast) data.
  /// The \e string-mode is used for debugging purpose or if the \e binary-data-model is different
  /// between the client and the server. The difference in endianness (http://en.wikipedia.org/wiki/Endian)
  /// is handled by \libmsgque and does not require the \e string-mode.
  /// The option is only available for the \e client-parent-context. (default: binary-mode)
  MQ_BOL isString;

  /// \brief create a new \e application-context as thread, spawn or fork
  ///
  /// A new \e application-context is created if:
  /// - a \e tcp-uds-server listen on socket and get a \e connection-request from a client.
  ///   This require: \RNSC{IFactory} and \RNSC{IServerSetup} 
  /// - a \e filter-context create a new \e filter-instance
  /// - a \e server-context create a new \e worker-context using \RNSA{SlaveWorker}
  /// - a \e server-context start a new \e client-server-link using \e SELF as 
  ///	    executable-name using \RNSA{LinkCreate}
  /// .
  /// \if MAN
  /// The allowed integer values are:
  /// - \b \MQ_START_DEFAULT = 0, use application-context default entity creation
  /// - \b \MQ_START_FORK = 1, create entity as \e fork
  /// - \b \MQ_START_THREAD = 2, create entity as \e thread
  /// - \b \MQ_START_SPAWN = 3, create entity as \e spawn process
  /// .
  /// \endif
  /// (default: do not create a new application-context)
  enum MqStartE startAs;

/// \brief set the human-readable name of the \e local-context
///
/// Use the \e name as a prefix in the local debug/error/log output:
/// \verbatim C> (name) [2009-01-12:16-22-27] [4-0-sIoCheckArg]: option:  io->com = PIPE \endverbatim
/// The value is define using the following order:
/// -# initial set to \e unknown
/// -# the \RNSC{name} \e configuration-option
/// -# use the basename of the \e first entry from the \e command-line-arguments, usually the \e executable-name
/// -# use the \e --name argument from the \e command-line-arguments
/// .
/// \ifnot MAN
/// \attention 
///   - the memory of the \e name-data-entry is managed by \libmsgque and freed during
///     deleting of the #MqS object. A Value of \c NULL is allowed. 
///   - the \e name-data-entry on the \e server will be replaced by the \RNSC{srvname} 
///     configuration option of the client.
///   - to initialize this value with a static string use: \code MqConfigSetName(context, "myString"); \endcode
///   .
/// \endif
  MQ_STR name;

/// \brief set the human-readable name of the \e server-context
///
/// Use the \e server-name as a client specific prefix in the server debug/error/log output to
/// link \e server-messages to a specific \e client-connection.
/// \verbatim C> (server-name) [2009-01-12:16-22-27] [4-0-sIoCheckArg]: option:  io->com = PIPE \endverbatim
/// If the \e server-name is not defined on the \e client the \e server is using \RNSC{name} to
/// create a usable name.  (default: \null)
/// \ifnot MAN
/// \attention 
///   - the memory of the \e server-name-data-entry is managed by \libmsgque and freed with
///     \RNSA{LinkDelete}. A Value of \c NULL is allowed.
///   - to initialize this value with a static string use: \code MqConfigSetSrvName(context, "myString"); \endcode
///   .
/// \endif
  MQ_STR srvname;

  /// \brief set the \e debug-level of the \e context
  /// \details Valid values are 0 <= \e debug-level <= 9 using 0 for \e no-debug and 9 for \e maximum-debug. (default: 0)
  MQ_INT debug;

  /// \brief Create a child-context
  /// <TABLE>
  /// <TR>  <TH>type</TH>    <TH>default</TH> <TH>option</TH> <TH>application</TH>  <TH>context</TH>  </TR>
  /// <TR>  <TD>POINTER</TD> <TD>NULL</TD>    <TD>NO</TD>       <TD>client</TD>       <TD>parent</TD>   </TR>
  /// </TABLE>
  ///
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

  /// \brief set the application-identifier
  ///
  /// The \e application-identifier is used as \e factory-lookup-identifier to create a new \e server-context
  /// and to modify the client or filter behaviour depending on the \e identifier-value of
  /// the \e remote-context and as factory lookup identifier to create a new \e server-context.
  /// The \e identifier is \b not changeable by the user, like the \RNSC{name} configuration option, because this is a "build-in" 
  /// feature set by the \e programmer.
  /// Without the \e application-identifier only the initial \e startup-context is available to serve
  /// incoming requests. In general every \e server need to provide an \e application-identifier.
  ///
  /// The \e application-identifier is used to create a new:
  ///  - \e application-context (C, TCL) or a new \e class-instance (C++, C#, JAVA, PYTHON, PERL, VB.NET)
  ///  - \e server-child-context or \e slave-child-context using \RNSA{LinkCreateChild}
  ///  - \e server-parent-context using the \RNSC{startAs} option \e --thread
  ///  - \e slave-worker-context using \RNSA{SlaveWorker}
  ///  - \e filter-parent-context using the \RNSC{startAs} option \e --fork or \e --thread
  ///  .
  /// (default: "DEFAULT")
  ///
  /// \attention: if this value changes the factory need to be updated too.
  MQ_CST ident;
  struct MqFactoryItemS* factory;
 
  /// \brief setup/cleanup a \e CHILD object
  /// \attention always call this functions using #MqLinkCreate and #MqLinkDelete
  struct MqLinkSetupS Child;

  /// \brief setup/cleanup a \e PARENT object
  /// \attention always call this functions using #MqLinkCreate and #MqLinkDelete
  struct MqLinkSetupS Parent;

  /// \brief define the \e background-error-interface
  ///
  /// A background error is an error without a link to an \e application-context and happen if an \RNSA{SendEND}
  /// call fails or if an other asynchronous task fails. if the interface is \e not defined the error
  /// is printed to stderr and the application continue to work. if the interface is defined the
  /// context is set to error and the callback is called to process this error. Inside the callback
  /// the error is available using \RNSA{ErrorGetNum} and \RNSA{ErrorGetText} and can be cleared using \RNSA{ErrorReset}.
  struct MqCallbackS BgError;

  /// \brief change the context to act as \e server-context (\yes) or not (\no)
  ///
  /// A \e server-context is responsible to answer \e service-requests. This options is also set as 
  /// side-effect in a \RNSC{IServerSetup} or \RNSC{IServerCleanup} function-call. (default: \no)
  MQ_BOL isServer;

  /// \brief define the \e server-setup-interface
  ///
  /// This interface is used to configure a new \e server-context-link, like a
  /// constructor, and is called at the end of \RNSA{LinkCreate} or \RNSA{LinkCreateChild}. 
  /// This interface is called for every new incoming connection request and is used to define 
  /// context specific services using \RNSA{ServiceCreate} or to initialize \e context-specific
  /// variables. As side-effect this interface set \RNSC{isServer} to \yes.
  struct MqCallbackS ServerSetup;

  /// \brief define the \e server-cleanup-interface
  ///
  /// This interface is used to cleanup an old \e server-context-link, like a
  /// destructor, and is called at the beginning of \RNSA{LinkDelete} to free \e context-specific 
  /// variables. As side-effect this interface set \RNSC{isServer} to \yes.
  struct MqCallbackS ServerCleanup;

  // misc

  /// \brief Create a link to the calling tool event-handling queue
  ///
  /// Event-Handling is used to process instructions in the \e background to give the tool-user
  /// the feeling of a non-blocking application. For example Tcl using the event-handling to
  /// update the Tk user-interface while the application is waiting for data. 
  /// The event handling function is called on idle and
  /// is designed for a very \b short function execution time. Do only \e one
  /// action per function call. This function will be called with a ~10000 usec interval to guarantee
  /// a parallel like execution.
  /// \attention Together with \RNSC{ignoreExit} the Event-Handler is used to start background processing
  /// of tasks. Return the Event-Handler with \RNSA{ErrorSetCONTINUE} to signal that all tasks are finished 
  /// and the process/thread is ready to exit.
  /// If \b all \e child context Event-Handler return with \RNSA{ErrorSetCONTINUE} too
  /// and the client/server links is already shutdown the process/thread will \b exit.
  /// example: \c theLink/example/LANG/Filter4.EXT
  struct MqCallbackS Event;

  /// \brief setup and initialize a thread before a new thread is created by \libmsgque
  MqSetupF fThreadInit;

  /// \brief setup and initialize a fork-process before a new fork-process is created by \libmsgque
  MqSetupF fForkInit;

  /// \brief setup and initialize a spawn-process before a new spawn-process is created by \libmsgque
  MqSetupF fSpawnInit;

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

  /// \brief do not delete the \e server-context on \e client-server-link shutdown
  ///
  /// By default the \e server-context is deleted and the \e server-process or \e server-thread exit 
  /// if the \e client close the connection. If this option is set to \yes
  /// the \e server will continue to work. Without \e client connection only
  /// the internal event function (set with \RNSC{IEvent}) is available to work on tasks.
  /// if \e all (parent and child) event functions return with \RNSA{ErrorSetCONTINUE} (nothing to do) 
  /// the process finally exit. (default: \no)
  MQ_BOL ignoreExit;
};

/*****************************************************************************/
/*                                                                           */
/*                           factory                                         */
/*                                                                           */
/*****************************************************************************/

MQ_EXTERN MQ_PTR MQ_DECL MqFactoryItemGetCreateData (
  struct MqFactoryItemS  const * const item
);

MQ_EXTERN MQ_PTR MQ_DECL MqFactoryItemGetDeleteData (
  struct MqFactoryItemS  const * const item
);

/*
MQ_EXTERN void MQ_DECL MqFactoryDelete (
  void
);
*/

MQ_EXTERN struct MqS * MQ_DECL MqFactoryNew (
  MQ_CST           const name,
  MqFactoryCreateF const fCreate,
  MQ_PTR           const createData,
  MqTokenDataFreeF const createDatafreeF,
  MqFactoryDeleteF const fDelete,
  MQ_PTR           const deleteData,
  MqTokenDataFreeF const deleteDatafreeF
) __attribute__((nonnull(1,2)));

MQ_EXTERN void MQ_DECL MqFactoryAdd (
  MQ_CST           const name,
  MqFactoryCreateF const fCreate,
  MQ_PTR           const createData,
  MqTokenDataFreeF const createDatafreeF,
  MqFactoryDeleteF const fDelete,
  MQ_PTR           const deleteData,
  MqTokenDataFreeF const deleteDatafreeF
) __attribute__((nonnull(1,2)));

MQ_EXTERN void MQ_DECL MqFactoryDefault (
  MQ_CST           const name,
  MqFactoryCreateF const fCreate,
  MQ_PTR           const createData,
  MqTokenDataFreeF const createDatafreeF,
  MqFactoryDeleteF const fDelete,
  MQ_PTR           const deleteData,
  MqTokenDataFreeF const deleteDatafreeF
) __attribute__((nonnull(1,2)));

MQ_EXTERN struct MqS* MQ_DECL MqFactoryCall (
  MQ_CST const name
) __attribute__((nonnull(1)));

MQ_EXTERN enum MqErrorE MQ_DECL MqFactoryInvoke (
  struct MqS * const context,
  enum MqFactoryE create,
  struct MqFactoryItemS* item,
  struct MqS ** contextP
);

MQ_EXTERN struct MqFactoryItemS* MQ_DECL MqFactoryItemGet (
  MQ_CST const name
) __attribute__((nonnull(1)));

/*****************************************************************************/
/*                                                                           */
/*                           create / delete                                 */
/*                                                                           */
/*****************************************************************************/

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
/// \detail \e MqSetupDup is an important function, because
/// every new created object need to fill the #MqSetupS data.
/// The typical software flow is:\n
/// - \e Call-Factory -> #MqContextCreate (default setup) -> \e Object-Specific-Setup -> #MqSetupDup
/// .
/// #MqSetupDup has code to protect setup data filled in \e Object-Specific-Setup 
// from neeing overwritten.
MQ_EXTERN void MQ_DECL MqSetupDup (
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

/// \brief set the #MqSetupS::ident value and cleanup old value
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

/// \brief set the #MqSetupS::isServer value
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

/// \brief set the #MqSetupS::ignoreExit value
MQ_EXTERN void
MQ_DECL MqConfigSetIgnoreExit (
  struct MqS * const context,
  MQ_BOL  data
);

/// \brief setup the \e factory pattern
/// \context
/// \param[in] ident the application identifier
/// \param[in] fCreate set the #MqSetupS::Factory - #MqFactoryCreateS::fCall value
/// \param[in] CreateData set the #MqSetupS::Factory - #MqFactoryCreateS::data value
/// \param[in] fCreateFree free the \e CreateData
/// \param[in] fDelete set the #MqSetupS::Factory - #MqFactoryDeleteS::fCall value
/// \param[in] DeleteData set the #MqSetupS::Factory - #MqFactoryDeleteS::data value
/// \param[in] fDeleteFree delete the \e DeleteData
MQ_EXTERN void 
MQ_DECL MqConfigSetFactory (
  struct MqS * const context,
  MQ_CST	    ident,

  MqFactoryCreateF  fCreate,
  MQ_PTR	    CreateData,
  MqTokenDataFreeF  fCreateFree,

  MqFactoryDeleteF  fDelete,
  MQ_PTR	    DeleteData,
  MqTokenDataFreeF  fDeleteFree
);

MQ_EXTERN void 
MQ_DECL MqConfigSetFactoryItem (
  struct MqS * const context,
  struct MqFactoryItemS * const item
);

/// \brief setup the default \e factory pattern
/// \context
///
/// The \e default factory is just a wrapper for #MqContextCreate with additional error management code.
/// A simple application \e without an application specific \e factory use this configuration.
MQ_EXTERN void 
MQ_DECL MqConfigSetDefaultFactory (
  struct MqS * const context,
  MQ_CST const ident
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

/// \brief set the #MqSetupS::Event
MQ_EXTERN void
MQ_DECL MqConfigSetEvent (
  struct MqS * const context,
  MqTokenF fCall,
  MQ_PTR data,
  MqTokenDataFreeF fFree,
  MqTokenDataCopyF fCopy
);

/// \brief set the #MqSetupS::ServerSetup
MQ_EXTERN void
MQ_DECL MqConfigSetServerSetup (
  struct MqS * const context,
  MqTokenF fCall,
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

/// \brief \copybrief MqIoUdsConfigS
/// \details \copydetails MqIoUdsConfigS
/// \ctx
/// \retException
MQ_EXTERN enum MqErrorE
MQ_DECL MqConfigSetIoUdsFile (
  struct MqS * const ctx,
  MQ_CST file
);

/// \brief \copybrief MqIoTcpConfigS
/// \details \copydetails MqIoTcpConfigS
/// \ctx
/// \retException
MQ_EXTERN enum MqErrorE
MQ_DECL MqConfigSetIoTcp (
  struct MqS * const ctx,
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
/// This is configuration option is only useful for a \e (x)inetd setup
/// to use the stdin (socket=0) as send/recv communication socket
MQ_EXTERN enum MqErrorE
MQ_DECL MqConfigSetIoPipeSocket (
  struct MqS * const context,
  MQ_SOCK socket
);

/// \brief start the \e server-context as daemon
///
/// A \e daemon is a \e server-process without any link to the \e parent-process.
/// A \e daemon-process has closed all default IO (e.g stdout, stdin, stderr) and forked
/// into the background. (default: no daemon)
/// \attention this option require the \b fork system-call and is \b not compatible with threads.
/// \ctx
/// \param[in] pidfile write the PID of the daemon into this file (default: NULL, do not start as daemon)
/// \retException
MQ_EXTERN enum MqErrorE
MQ_DECL MqConfigSetDaemon (
  struct MqS * const ctx,
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

/* \brief does the \e context object was created by \libmsgque
 *  \context
 *  \return the <TT>(context->config.doFactoryCleanup == MQ_YES)</TT> value
MQ_EXTERN int MQ_DECL MqConfigGetDoFactoryCleanup (
  struct MqS const * const context
);
 */

/* \brief does the \e context object is \e Duplicate and \e Thread ?
 *  \context
 *  \return the <TT>((config->statusIs & MQ_STATUS_IS_DUP) && (config->statusIs & MQ_STATUS_IS_THREAD))</TT> value
MQ_EXTERN int MQ_DECL MqConfigGetIsDupAndThread (
  struct MqS const * const context
) __attribute__((nonnull));
 */

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

/// \brief return the #MqS::statusIs value
MQ_EXTERN enum MqStatusIsE
MQ_DECL MqConfigGetStatusIs (
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

/// \brief Initialize the process \e startup-prefix argument
///
/// The \e startup-prefix have to be the name of the executable, found in the 
/// \c PATH environment variable, and additional arguments like the script name or 
/// the required startup options. The \e startup-prefix is used for two different purpose:
///  - To start a new entity using the \RNSC{startAs} "--spawn" command-line option.
///  - To replace the \e command-line-argument <TT>"... @ SELF ..."</TT> with <TT>"... @ startup-prefix ..."</TT> at \RNSA{LinkCreate}.
///  .
/// Every use of this function will free the data of the previous \e startup-prefix.
/// By default the \e startup-prefix is set during package loading or during \RNSA{LinkCreate}
/// and have \b not to be initialized again.
///\ifnot MAN
///\return a pointer to the initialization buffer (Only C-API)
///\endif
///\ifnot MAN
///\code
///struct MqBufferLS * args = MqInitCreate();
///MqBufferLAppendC(args, "myExec");
///MqBufferLAppendC(args, "myExecArgument_1");
///...
///\endcode
///\endif
/// \if MSGQUE
/// \anchor \NS{Init}
/// \endif
MQ_EXTERN struct MqBufferLS* MQ_DECL MqInitCreate (void);

MQ_EXTERN struct MqBufferLS* MQ_DECL MqInitGet (void);

/// \} Mq_Config_C_API

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                    C O N T E X T - A P I                        ### */
/* ###                                                                 ### */
/* ####################################################################### */

/// \defgroup Mq_Context_C_API Mq_Context_C_API
/// \{
/// \brief \e context, the top-level data structure and \e application-handle
///
/// The \e context-data-structure is created with \RNSA{ContextCreate} and 
/// deleted with \RNSA{ContextDelete}. The structure has a \e libmsgque-specific-part and an
/// \e application-specific-part. Both parts are linked together.
/// The \e libmsgque-specific-part has all data required to manage a
/// \e client-server-link. The \e application-specific-part as all data
/// required by the application. The following C-API example demonstrate
/// the layout of the \e context-data-structure.
/// \code
/// struct MyCtxDataS {
///   struct MqS  mqctx;   // libmsgque-specific-data
///   int         mydata;  // application-specific-data
///   ...                  // application-specific-data
/// };
/// \endcode
/// The \e libmsgque-specific-data have to be the \e first data entry in the structure.\n
/// A \e high-level-programming-language like JAVA, C#, C++, Perl, Python, Ruby, Tcl or VB-NET
/// is using a wrapper around this \e data-structure as \e application-handle.
/// The \e application-specific-data is available as \RNS{ContextLocalStorage}
/// \if MSGQUE
/// \anchor \NS{ContextLocalStorage}
/// \endif

/// \brief Information about how the \e context was created
enum MqStatusIsE {
  MQ_STATUS_IS_INITIAL = 0,	    ///< context is the \e first context
  MQ_STATUS_IS_DUP     = 1<<0,	    ///< context is created as a duplicate of an other context
  MQ_STATUS_IS_THREAD  = 1<<1,	    ///< context is created as a thread
  MQ_STATUS_IS_FORK    = 1<<2,	    ///< context is created as a fork
  MQ_STATUS_IS_SPAWN   = 1<<3,	    ///< context is created as a spawn
};

/// \brief signature marker used in #MqS::signature
#define MQ_MqS_SIGNATURE 0x212CF91

#if !defined(MQ_PRIVATE_CONFIG_CONST)
# define MQ_PRIVATE_CONFIG_CONST const
#endif

/// \brief data structure for the \e libmsgque-specific-data
struct MqS {
  MQ_INT signature;		    ///< used to verify the \e pointer-data-type in a type-less programming languages
  MQ_PRIVATE_CONFIG_CONST
    struct MqConfigS config;	    ///< the configuration data is used for "end-user" configuration
  struct MqSetupS setup;	    ///< the setup data is used to link the object with the user application
  struct MqErrorS error;	    ///< error object data
  struct MqLinkS link;		    ///< link object data
  struct MqBufferS * temp;	    ///< misc temporary #MqBufferS object
  enum MqStatusIsE statusIs;	    ///< how the context was created?
  struct {
    MQ_BOL MqContextDelete_LOCK :1; ///< protect MqContextDelete against double-call
    MQ_BOL MqContextFree_LOCK	:1; ///< protect MqContextFree against double-call
    MQ_BOL EventProc_LOCK	:1; ///< protect sCallEventProc against double-call
    MQ_BOL onExit	        :1; ///< is already an "exit" ongoing?
  } bits;			    ///< boolean bit-fields
  MQ_PTR threadData;		    ///< application specific thread data
  MQ_PTR self;			    ///< link to the managed object
  MQ_SIZE contextsize;		    ///< ALLOC-size of the user-defined context struct
  MQ_INT refCount;		    ///< is an object in use?
  struct MqGcS *gc;		    ///< link to the garbage-collection
  MQ_SIZE gcid;			    ///< position in the gc array
  MQ_CST caller;		    ///< identify the parent of the current called function
};

/// \brief initialize the #MqS object related data but do \e not create the object self
MQ_EXTERN void MQ_DECL MqContextInit (
  struct MqS       * const context,
  MQ_SIZE		   size,
  struct MqS const * const tmpl
);

/// \brief free the #MqS object related data but do \b not free the \e toplevel object self
/// \details Shutdown the \e client-server-link and free the related memory.
/// The \e toplevel object is still alive to give an existing 
/// Garbage-Collection the control when to delete the object.
/// The \e context can \b not be reused.
MQ_EXTERN void MQ_DECL MqContextFree (
  struct MqS       * const context
);

/// \brief create a new context and initialize the default configuration data
/// \param[in] size (C-API) the number of bytes in the \e context-data-structure as returned by <TT>sizeof(struct MyCtxDataS)</TT>
///                  (default: \e 0, use only \e libmsgque-specific-data and no \e application-specific-data)
/// \param[in] tmpl (C-API) an other \e context-data-structure used as template to initialize the configuration data. 
///                  This template is used for a \e child to get the configuration data from the \e parent. 
///                  (default: \e NULL, create an initial context)
/// \return the new \e context, no error return because this function \e panic on \e out-of-memory-error
MQ_EXTERN struct MqS * MQ_DECL MqContextCreate (
  MQ_SIZE size,
  struct MqS const * const tmpl
);

/// \brief delete the #MqS object and \b all depending data including the \e toplevel object self
/// \details Shutdown the \e client-server-link, free the memory and set the \e ctx to \null. 
/// The \e context can \b not be reused.
/// \ctx
MQ_EXTERN void MQ_DECL MqContextDelete (
  struct MqS ** ctx
) __attribute__((nonnull));

/// \brief delete the \e context and exit the current process or thread
/// \details To delete an application is a difficult task because the \e link-target
/// have to be informed. This information is send as \e shutdown-event and 
/// finally as \e socket-exit after \e application-exit. This library tries to perform this two 
/// steps even if the default \b exit function is called. 
/// This is no problem because the second step (\e socket-exit) is enough to signal a \e link-down.
/// It is \b not secure to depend only on \e socket-exit for application exit
/// because sometimes the sockets stop working or the \e link-target does not get a \e socket-exit.
/// For example the \e pipe-link on windows. The client can not exit and create a \e socket-exit
/// error on the \e server because the \e server is still running without receiving a \e shutdown-event.
/// A second argument for using the \RNSA{Exit} function is, that it can be used for process \b and 
/// thread exit. A thread, started by \libmsgque, does exit but the process continue to work.
/// To make it short use \RNSA{Exit} to exit your application.
/// \ifnot MAN
/// The following steps are performed:
/// -# delete the \e client-server-link using: #MqLinkDelete
/// -# if available call the \e context-specific-exit-handler: #MqSetupS::fProcessExit
/// -# or call \e application-default-exit-handler
/// -# it is a \e panic-error to call #MqExit twice for the same object
/// .
/// \endif
/// \param[in] prefix identify the caller
/// \ctx
MQ_EXTERN void MQ_DECL MqExitP (
  MQ_CST prefix,
  struct MqS * ctx
) __attribute__ ((noreturn));

/// \brief wrapper to add calling function to #MqExitP
#define MqExit(ctx) MqExitP(__func__, ctx)

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

/// \brief Mark objects as \e used to avoid to free used external memory in an external Garbage-Collection
/// \details An external Garbage-Collction (GC) need information about external objects 
/// referenced by \libmsgque. This objects are the <TT>MQ_PTR data</TT> parameter required
/// by different functions.
/// \context
/// \param markF function provided by the embedded language
MQ_EXTERN void MQ_DECL MqMark (
  struct MqS * const context,
  MqMarkF markF
);

#if defined(_DEBUG)
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
  MQ_CST const prefix
);
#endif // _DEBUG

/** \} Mq_Context_C_API */

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                       L I N K - A P I                           ### */
/* ###                                                                 ### */
/* ####################################################################### */

/// \defgroup Mq_Link_C_API Mq_Link_C_API
/// \{
/// \brief setup and manage a \e client-server-link
///
/// The \e client-server-link connect two \e context, a \e client-parent-context and a \e server-parent-context. 
/// The \e link can be \e local (connect two \e context on the same host) or can be \e remote 
/// (connect two \e context on different hosts). On-Top the \e parent-context multiple \e child-context are allowed.
///\verbatim
///  !on local host!                                  !on remote host!
///
///      server1---------x                     x----------server2
///         |            |                     |             |
///         |     child-context-1       child-context-2      |
///         |            |                     |             |                      server
/// parent-context-1-----x                     x-----parent-context-2
///         |                                                |
///(MqConfigS::server)                  (MqConfigS::server --fork --tcp --port)
///         |                                                |
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
///         |                                                |
///     (--pipe)                                   (--tcp --host --port)
///         |                                                |
/// parent-context-1-----x                     x-----parent-context-2
///         |            |                     |             |                      client
///         |     child-context-1       child-context-2      |
///         |            |                     |             |
///         x------------x--------client-------x-------------x
///
///                           !on local host!  \endverbatim
///
/// <B>Definition of a "client-context"</B>\n
/// - every new \e client-parent-context create a new \e client-server-link and perform the \e connect system-call.
/// - every new \e client-server-link start a new \e server-instance and creating a new \e server-parent-context
/// - every deletion of the \e client-parent-context stop the previous started \e server-instance and delete the server-parent-context
/// - the \e client-context get all the error/warning/info messages from the \e server-context
/// .
/// <B>Definition of a "server-context"</B>\n
/// - every new \e server-parent-context is created by request from the \e client-parent-context:
/// - in remote mode every new \e server-parent-context wait for a \e tcp or \e uds connection request using the \e accept system-call.
///\verbatim
///client --tcp --hots REMOTE_HOST --port REMOTE_PORT ...    or
///client --uds --file MYFILE ...\endverbatim
/// - in local mode new \e server-parent-context is started by the \e client-parent-context as \e pipe:
///\verbatim
///client @ server \endverbatim
/// - a \e server-context have to implement the \RNSC{IServerSetup} and the \RNSC{IFactory} interface.
/// - a \e server-context have to enter the \e event-loop and wait for incoming \e service-request using
///   \RNSA{ProcessEvent} together with the \MQ_WAIT_FOREVER.
/// .
/// \if MSGQUE
/// \anchor \NS{ClientServerLink}
/// \endif
///
/// <B>Definition of a "parent-context"</B>\n
/// 
///  - the parent is the first context created and is created with \RNSA{LinkCreate}
///  - every new \e parent-context on the client create a new communication to a server.
///  - every new \e parent-context on the client connect to a new \e parent-context on the server
///  - every new \e parent-context on the server is using a new \e server-instance
///  - the \e parent-context control the socket communication interface
///  .
/// <B>Definition of a "child-context"</B>\n
/// 
///  - the \e child-context is the second or more context and is created with \RNSA{LinkCreateChild}.
///  - every new \e child-context on the client create a new \e child-context on the server.
///  - every new \e child-context is totally independent from the \e parent-context setup and is able 
///     to serve its own services.
///  - the \e child-context is using the \e parent-context as a tunnel.
///  - the parent of a \e child-context can be a \e parent-context or an other \e child-context. 
///     A tree like structure is created.
///  - the \e child-context is using the communication interface from the parent-context.
///  - if a \e context is deleted (parent or child) the \e depending context (parent or child) is deleted too.
///  - if a \e context is deleted the \e depending context-tree is deleted too.
///  .


/// \brief make \e ctx to a \e parent-context and setup a new \e client-server-link
/// \ctx
/// \param[in] args  \e command-line-arguments to configure the \e client-server-link
///                  including the \b "@" item to add \e server-command-line-arguments
///		     for the \e --pipe setup.
/// \ifnot MAN
///                  (only C-API: the memory of known arguments will be freed and 
///		      \e args will be set to \c NULL on error or if list is empty)
/// \endif
/// \retException
/// \attention if the first argument after the \b "@" item is the string \b "SELF" an independent
/// server of the current server is started. This is not a \RNS{SlaveContext}. The "SELF" argument is
/// replaced by an application default setting (if available) or by arguments set with \RNSA{Init}
MQ_EXTERN enum MqErrorE MQ_DECL MqLinkCreate (
  struct MqS  * const ctx,
  struct MqBufferLS ** args
);

/// \brief re-connect a \e client-server-link after a server crash or a network downtime
///
/// Do nothing if the \e client-server-link is already connected.
/// This function is only useful in an \e event-function (\RNSC{IEvent}) if the \e link-disconnect
/// (\RNSA{ErrorIsEXIT}) is ignored (\RNSA{ErrorReset}). 
/// Read more in the: <TT>/theLink/example/LNG/Filter4.EXT</TT> example.
/// \ctx
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqLinkConnect (
  struct MqS  * const ctx
);

/// \brief make a \e context to a \e child-context on-top of an existing \e parent-client-server-link
/// \details A child is using the same process or thread as the parent but a different 
/// namespace. With a different namespace a child is able to act on different services 
/// on the shared server.
/// \ctx
/// \param[in] parent the \e parent-context defined with \RNSA{LinkCreate}
/// \param[in] args  \e command-line-arguments to configure the \e client-server-link
///                  without the \b "@" item.
/// \ifnot MAN
///                  (only C-API: the memory of known arguments will be freed and 
///		      \e args will be set to \c NULL on error or if list is empty)
/// \endif
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqLinkCreateChild (
  struct MqS * const ctx,
  struct MqS * const parent,
  struct MqBufferLS ** args
);

/// \brief helper: wrapper for #MqLinkCreate or #MqLinkCreateChild with additional \e error-check code
/// \details The function have to be used as argument to #MqSetupS::Child,
/// #MqSetupS::Parent or #MqConfigSetSetup as default \e context-create function.
/// Return an \e error if an unknown \e command-line-argument was found.
/// \ctx
/// \param[in] args  \e command-line-arguments to configure the \e client-server-link
///                  with or without the \b "@" item.
/// \ifnot MAN
///                  (only C-API: the memory of known arguments will be freed and 
///		      \e args will be set to \c NULL on error)
/// \endif
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqLinkDefault (
  struct MqS  * const ctx,
  struct MqBufferLS ** args
);

/// \brief close the \e client-server-link
/// \details On a client the \e context will be set to \e not-connected and
/// the function \RNSA{LinkIsConnected} will return \no. On a server the
/// \e context will be deleted but only if \RNSC{ignoreExit} is \e not
/// set to \yes. If the \e link is already \e not-connected nothing will happen.
/// \ctx
MQ_EXTERN void MQ_DECL MqLinkDelete (
  struct MqS * const ctx
);

/// \brief is the context connected?
/// \details A context is connected if the \RNSA{LinkCreate} command
/// was successful. A context is \e not connected if:
/// - the context has just been created and not connected
/// - the link was deleted with \RNSA{LinkDelete}
/// .
/// \ctx
/// \return a boolean value, \yes or \no
MQ_EXTERN int MQ_DECL MqLinkIsConnected (
  struct MqS const * const ctx
) __attribute__((nonnull));

/// \brief get the \e parent-context from a client/server link
/// \ctx
/// \return the \e parent-context or \null if not a \e child-context
MQ_EXTERN struct MqS * MQ_DECL MqLinkGetParent (
  struct MqS const * const ctx
) __attribute__((nonnull));

/// \copydoc MqLinkGetParent
static mq_inline struct MqS * MqLinkGetParentI (
  struct MqS const * const ctx
)
{
  return ctx->config.parent;
};

/// \brief is the \e context a \e parent-context?
/// \details A \e context is a \e parent-context if it was created with \RNSA{LinkCreate}
/// \ctx
/// \return a boolean value, \yes or \no
MQ_EXTERN int MQ_DECL MqLinkIsParent (
  struct MqS const * const ctx
) __attribute__((nonnull));

/// \copydoc MqLinkIsParent
static mq_inline int MqLinkIsParentI (
  struct MqS const * const ctx
)
{
  return ctx->config.parent == NULL;
};

/// \brief get an identifier which is \e unique per \e parent-or-child-context
///
/// The \e context-identifier is a number and is used in the protocol to link 
/// a \e data-package to a \e context-pointer. This is necessary because the 
/// communication interface is shared between the \e parent-context and the
/// \e child-context. This number is \e unique.
/// \ctx
/// \return the identifier as number
MQ_EXTERN MQ_SIZE MQ_DECL MqLinkGetCtxId (
  struct MqS const * const ctx
) __attribute__((nonnull));

/// \copydoc MqLinkGetCtxId
static mq_inline MQ_SIZE MqLinkGetCtxIdI (
  struct MqS const * const ctx
)
{
  return ctx->link.ctxId;
};

/// \brief get the \e ident of the \e link-target
///
/// This function is only useful if the link is up and running.
/// \ctx
/// \return get the \e ident of the \e link-target or \null if not connected
MQ_EXTERN MQ_CST MQ_DECL MqLinkGetTargetIdent (
  struct MqS * const ctx
) __attribute__((nonnull));

/// \} Mq_Link_C_API

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                    S E R V I C E - A P I                        ### */
/* ###                                                                 ### */
/* ####################################################################### */

/// \defgroup Mq_Service_C_API Mq_Service_C_API
/// \{
/// \brief create and manage services
///
/// To provide a \e service is the main purpose of a server and the main-purpose
/// of a \e client/server connection is to call a service and to process the results.
/// A service have to be defined on a server during \e link-setup (\RNSC{IServerSetup})
/// with the \RNSA{ServiceCreate} function. The cleanup is done implicit on 
/// \e link-delete or explicit with the \RNSA{ServiceDelete} function.
///

/// \brief a 4 byte string to identify different services
/// \brief The \e token-identifier is defined by the programmer using \RNSA{ServiceCreate}
/// to link a \e service-handler with an \e identifier. The \e identifier is part of the
/// \RNSA{SendEND}, \RNSA{SendEND_AND_WAIT} or \RNSA{SendEND_AND_CALLBACK} function-call 
/// to access the services.\n
/// For internal purpose some special \e token are predefined:
///  - <B>_???</B> - all \e token starting with a \b "_" are for \b internal usage only
///  - \b +ALL - used in \RNSA{ServiceCreate} and \RNSA{ServiceDelete} to listen on \b all token not handled by other \e token more precise
///  - \b -ALL - used in \RNSA{ServiceDelete} to delete \b all token
///  - \b +FTR and \b +EOF - used for \e one-directional-filter
/// \if MSGQUE
/// \anchor \NS{ServiceIdentifier}
/// \endif
typedef MQ_CST MQ_TOK;

/// \brief function used as \e service-handle
/// \details A service is using a \e callback to act on an incoming \e service-request. 
/// The \e callback is linked to a \RNS{ServiceIdentifier} with \RNSA{ServiceCreate} or 
/// is used as argument to the \RNSA{SendEND_AND_CALLBACK} function.
/// \if MSGQUE
/// \anchor \NS{ServiceCallback}
/// \endif
typedef MqTokenF MqServiceCallbackF;

/// \brief wait for an event?
enum MqWaitOnEventE {
  MQ_WAIT_NO      = 0,	///< just do the check
  MQ_WAIT_ONCE    = 1,	///< wait for one new event
  MQ_WAIT_FOREVER = 2,	///< wait forever
};

/// \brief get the \e filter-context or the \e master-context
///
/// A \e filter-pipeline has two context, one on the left and one on the right.
/// The \e left-context is linked with the \e master-context and the \e right-context
/// is linked to the \e slave-context.
/// This function extract the \e other-context related to the initial \e ctx argument 
/// using the following order:
/// -# return the \e master-context if not NULL
/// -# return the \e slave-context using \e id if not NULL
/// -# return a "filter not available" error
/// .
/// 
/// \ctx
/// \id
/// \param[out] filter the \e other-context or \null on error
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqServiceGetFilter (
  struct MqS  * const ctx,
  MQ_SIZE const id,
  struct MqS  ** filter
) __attribute__((nonnull(1)));

/// \brief get the \e filter-context or the \e master-context
///
/// A \e filter-pipeline has two context, one on the left and one on the right.
/// The \e left-context is linked with the \e master-context and the \e right-context
/// is linked to the \e slave-context.
/// This function extract the \e other-context related to the initial \e ctx argument 
/// using the following order:
/// -# return the \e master-context if not NULL
/// -# return the \e slave-context using \e id if not NULL
/// -# return a "filter not available" error
/// .
/// 
/// \ctx
/// \id
/// \return the \e filter-context or \null if the \e context hs no \e filter-context
/// \attention if no filter is available an error-message is created in the context
MQ_EXTERN struct MqS  * const MQ_DECL MqServiceGetFilter2 (
  struct MqS  * const ctx,
  MQ_SIZE const id
) __attribute__((nonnull(1)));

/// \brief check if the \e ongoing-service-call belongs to a transaction
///
/// A \e service-call can be \e with-transaction (return \yes if the package was send with 
/// \RNSA{SendEND_AND_WAIT} or \RNSA{SendEND_AND_CALLBACK}) 
/// or can be \e without-transaction (return \no if the package was send with \RNSA{SendEND})
/// \ctx
/// \return a boolean value, \yes or \no
MQ_EXTERN int
MQ_DECL MqServiceIsTransaction (
  struct MqS const * const ctx
) __attribute__((nonnull(1)));

/// \brief get the \RNS{ServiceIdentifier} from an \e ongoing-service-call
///
/// This function is needed on the server to process a \e service-request
/// defined as \e +ALL or as an \e alias to extract the \e current \RNS{ServiceIdentifier}.
/// \ctx
/// \return the \RNS{ServiceIdentifier}
MQ_EXTERN MQ_TOK 
MQ_DECL MqServiceGetToken (
  struct MqS const * const ctx
) __attribute__((nonnull(1)));

/// \brief in an \e ongoing-service-call compare the \RNS{ServiceIdentifier} with \e token on equity
/// \ctx
/// \token
/// \return a boolean value, \yes or \no
MQ_EXTERN MQ_BOL MQ_DECL MqServiceCheckToken (
  struct MqS const * const ctx,
  MQ_TOK const token
) __attribute__((nonnull(1)));

/// \brief create a link between a \RNS{ServiceIdentifier} and a \RNS{ServiceCallback}
///
/// The \e token have to be unique but the \e callback not, this is called an \e alias. 
/// Use \RNS{ServiceGetToken} to get the current token in an \e incoming-service-call.
/// \ctx
/// \token
/// \param[in] callback the function to process the incoming \e service-request
/// \param[in] data a user defined additional \e data-argument for the \e callback function (C-API only)
/// \param[in] datafreeF the function to free the \e data-argument after use (C-API only)
/// \retException
/// \attention the link-setup (\RNSA{LinkCreate} or \RNSA{LinkCreateChild}) have to done \b befor using this function
MQ_EXTERN enum MqErrorE MQ_DECL MqServiceCreate (
  struct MqS * const ctx, 
  MQ_TOK const token,
  MqServiceCallbackF const callback,
  MQ_PTR data,
  MqTokenDataFreeF datafreeF
) __attribute__((nonnull(1)));

/// \brief create a service to link a \e master-context with a \e slave-context.
/// \details This function is used to create a \e proxy-service to forward the 
/// \e body-data from the \e read-data-package of the \e master to the \e send-data-package
/// of the slave. The data is \e not changed. This function support the \e reverse-operation
/// also. If the \e ctx is a \e master-context than the data is send to \e slave identified by \e id.
/// If the \e ctx is a \e slave-context than the data is send to the \e master of the \e slave.
/// Use \e id=0 to send that data to the \e filter-context.
/// \ctx
/// \token
/// \param[in] id the slave identifier, only used if \e ctx is a \e master-context
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqServiceProxy (
  struct MqS * const ctx, 
  MQ_TOK const token,
  MQ_SIZE const id
) __attribute__((nonnull(1)));

/// \brief delete a service.
/// \ctx
/// \token
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqServiceDelete (
  struct MqS * const ctx, 
  MQ_TOK const token
) __attribute__((nonnull(1)));

/** \brief enter the \e event-loop and wait for an incoming \e service-request.
 * 
 *  This function is used to enter the \e event-loop and start listen on open
 *  \e file-handles and to call \RNSC{IEvent} on idle.\n
 *  This function is direct used on a server to enter the \e event-loop and wait 
 *  for incoming service requests or after the \RNSA{SendEND_AND_CALLBACK} function 
 *  to wait for the \e service-result. This function is indirect used in 
 *  \RNSA{SendEND_AND_WAIT} too.
 * 
 *  \ctx
 *  \param[in] timeout the maximum \e wait-time in seconds until a \e timeout-error is raised,
 *    possible values are:
 *  - \b >0, use this value as number of seconds.
 *  - <B>\MQ_TIMEOUT_DEFAULT = -1</B>, \copydoc MQ_TIMEOUT_DEFAULT.  \MQ_TIMEOUT_USER for \MQ_WAIT_ONCE
 *    or \MQ_TIMEOUT_MAX for \MQ_WAIT_FOREVER. \b default
 *  - <B>\MQ_TIMEOUT_USER = -2</B>, \copydoc MQ_TIMEOUT_USER
 *  - <B>\MQ_TIMEOUT_MAX = -3</B>, \copydoc MQ_TIMEOUT_MAX
 *  .
 *  \param[in] wait the \e operation-mode used to define the behaviour,
 *    possible values are:
 *  - <B>\MQ_WAIT_NO = 0</B>, don't wait for an event just do the check and comeback. if an
 *      Event is available process the event, but only one. If no Event is available
 *      return with #MQ_CONTINUE. \b default
 *  - <B>\MQ_WAIT_ONCE = 1</B>, wait maximum \e timeout seconds for only \e one event or raise
 *      a \e timeout-error if no event was found.
 *  - <B>\MQ_WAIT_FOREVER = 2</B>, wait maximum \e timeout seconds for a event. If an event was found
 *      process the event. If no event was found, raise a \e timeout-error. After the event was
 *      processed continue to listen for a the new event. 
 *      The function will only come back on \e error or on \e exit.
 *  .
 *  \retException
 *
\ifnot MAN
example: A typical server looks like:
\code
...
MqErrorCheck(MqLinkCreate(ctx,argv,alfa);
...
MqErrorCheck(MqProcessEvent(ctx, MQ_TIMEOUT, MQ_WAIT_FOREVER);
...
MqLinkDelete(ctx);
...
\endcode
\endif
 **/

MQ_EXTERN enum MqErrorE MQ_DECL MqProcessEvent (
  struct MqS * const ctx,
  MQ_TIME_T timeout,
  enum MqWaitOnEventE const wait
) __attribute__((nonnull(1)));

/** \} Mq_Service_C_API */

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                     B U F F E R - A P I                         ### */
/* ###                                                                 ### */
/* ####################################################################### */

/// \defgroup Mq_Buffer_C_API Mq_Buffer_C_API
/// \if MSGQUE
/// \anchor \NS{BufferObject}
/// \endif
/// \{
/// \brief create and manage dynamic, generic, mixed typed data.
///
/// The MqBufferS struct is used to store and manage #MqTypeE typed data in a
/// MqBufferU storage. If \libmsgque is working on any kind of data it is working on
/// MqBufferS objects or on a list of MqBufferS objects called MqBufferLS.

/// \brief the type is native and has a size of 1 byte
#define MQ_TYPE_IS_1_BYTE   (1<<0)

/// \brief the type is native and has a size of 2 bytes
#define MQ_TYPE_IS_2_BYTE   (1<<1)

/// \brief the type is native and has a size of 4 bytes
#define MQ_TYPE_IS_4_BYTE   (1<<2)

/// \brief the type is native and has a size of 8 bytes
#define MQ_TYPE_IS_8_BYTE   (1<<3)

/// \brief the type is native
#define MQ_TYPE_IS_NATIVE   (	MQ_TYPE_IS_1_BYTE | MQ_TYPE_IS_2_BYTE |	    \
				MQ_TYPE_IS_4_BYTE | MQ_TYPE_IS_8_BYTE	)

/// \brief a collection of all \e native-data-types supported
///
/// The \e type-identifier (TYPE) is a \e one-character-value (Y,O,S,I,W,F,D,B,C,L,U) for every 
/// \e native-data-type supported.
/// A \e buffer-data-package is type safe, this mean that every item has a \e type-prefix and every
/// \RNSA{ReadTYPE} or \RNSA{BufferGetTYPE} have to match the previous \RNSA{SendTYPE} with the same 
/// \e TYPE. One exception is allowed, the cast from and to the \C data-type (TYPE=C) is allowed.
/// The following type identifier's are available:
///  - \c Y : 1 byte signed character (\Y) 
///  - \c O : 1 byte boolean value using \yes or \no (\O)
///  - \c S : 2 byte signed short (\S)
///  - \c I : 4 byte signed integer (\I)
///  - \c W : 8 byte signed long long integer (\W)
///  - \c F : 4 byte float (\F)
///  - \c D : 8 byte double (\D)
///  - \c B : unsigned char array used for binary data (\B)
///  - \c C : string data using a \c \\0 at the end (\C)
///  - \c L : list type able to hold a list of all items from above
///  - \c U : typeless buffer able to hold a single item from above (\U)
///  .
/// \ifnot MAN
/// The type is a one byte character with the following syntax:
/// - bit 1 up to 4 has the size of the native type
/// - bit 5 up to 8 has the type, up to 16 types are allowed
/// .
/// \attention In the package the space for the type is only \e on char. If additional
/// space is needed the protocol have to be adjusted
/// \endif
/// \if MSGQUE
/// \anchor \NS{BufferIdentifer}
/// \endif
enum MqTypeE {
  MQ_BYTT = (1<<4 | MQ_TYPE_IS_1_BYTE),  ///< Y: 1 byte 'byte' type
  MQ_BOLT = (2<<4 | MQ_TYPE_IS_1_BYTE),  ///< O: 1 byte 'boolean' type
  MQ_SRTT = (3<<4 | MQ_TYPE_IS_2_BYTE),  ///< S: 2 byte 'short' type
  MQ_INTT = (4<<4 | MQ_TYPE_IS_4_BYTE),  ///< I: 4 byte 'int' type
  MQ_FLTT = (5<<4 | MQ_TYPE_IS_4_BYTE),  ///< F: 4 byte 'float' type
  MQ_WIDT = (6<<4 | MQ_TYPE_IS_8_BYTE),  ///< W: 8 byte 'long long int' type
  MQ_DBLT = (7<<4 | MQ_TYPE_IS_8_BYTE),  ///< D: 8 byte 'double' type
  MQ_BINT = (8<<4                    ),  ///< B: \e byte-array type
  MQ_STRT = (9<<4                    ),  ///< C: \e string type (e.g. with a \\0 at the end)
  MQ_LSTT = (10<<4                   ),  ///< L: list object type
  MQ_TRAT = (11<<4                   )   ///< T: transaction object type
};

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

/** \brief a union for all data items supported by #MqBufferS.
 * 
 * A \libmsgque data package is a collection of items of type #MqBufferU
 * with variable length data of type: #MQ_BIN, #MQ_STR and MQ_LST or a
 * a fixed length data of type #MQ_ATO.
 * The #MqBufferS object is using a #MqBufferU union 
 * and a #MqTypeE enum to define an item.
 **/
union MqBufferU {
  MQ_ATO *A;			///< atom for native data-types
  MQ_BIN  B;			///< arbitrary \e byte-array data
  MQ_STR  C;			///< C string like data (e.g. with a \\0 at the end)
  MQ_LST  L;			///< list object type data
  MQ_LST  R;			///< return object type data
  MQ_LST  T;			///< transaction object type data
};

/// \brief allocation style used for the data-segment in #MqBufferS.
enum MqAllocE {
  MQ_ALLOC_STATIC     = 0,	///< opposite from MQ_ALLOC_DYNAMIC)
  MQ_ALLOC_DYNAMIC    = 1,	///< dynamic allocation (e.g. SysMalloc, ...)
};

/// \brief initial size of the #MqBufferS::bls object
#define MQ_BLS_SIZE 50

/// \brief signature used in #MqBufferS::signature
#define MQ_MqBufferS_SIGNATURE 0x6021139

/// \brief a single object to store a \e native-type-data-item defined by \RNS{BufferIdentifer}
///
/// The \e buffer-object is returned by \RNSA{ReadU} and is used to save a 
/// \e typeless-temporary-data-item from the \e read-data-package. 
/// The lifetime of the \e buffer-object is only the current callback up to the next
/// read operation in the same \e parent-context.
struct MqBufferS {
  MQ_INT signature;		///< used to verify the \e pointer-data-type in a type-less programming languages
  struct MqS *context;		///< error object of the related msgque
  MQ_BIN data;                  ///< always point to the beginning of the data-segment
  MQ_SIZE size;                 ///< the size of the data-segment
  MQ_SIZE cursize;              ///< currently used size (always: \a cursize < \a size)
  MQ_SIZE numItems;             ///< the number of items in the data-segment (only if it is a package)
  union MqBufferU cur;		///< point to current position, used for 'appending to' or 'reading from' 
				///< a package (always: data <= cur.B <= data+cursize)
  enum MqAllocE alloc;		///< allocation style, MQ_DYNAMIC or MQ_STATIC
  enum MqTypeE type;            ///< type of the item stored into the data-segment

  /// \brief B)uffer L)ocal S)torage used in #MqBufferGetC to get a string from a #MQ_ATO data
  ///
  /// +1 to allow strings with MQ_BLS_SIZE=strlen(str) fit into the buffer with an additional
  /// '\\0' at the end
  MQ_BINB bls[MQ_BLS_SIZE+1];
};

/// \brief extract a MQ_BYT from a #MqBufferS object
/// \bufU
MQ_EXTERN MQ_BYT MQ_DECL MqBufU2BYT ( union MqBufferU bufU );
/// \brief extract a MQ_BOL from a #MqBufferS object
/// \bufU
MQ_EXTERN MQ_BOL MQ_DECL MqBufU2BOL ( union MqBufferU bufU );
/// \brief extract a MQ_SRT from a #MqBufferS object
/// \bufU
MQ_EXTERN MQ_SRT MQ_DECL MqBufU2SRT ( union MqBufferU bufU );
/// \brief extract a MQ_INT from a #MqBufferS object
/// \bufU
MQ_EXTERN MQ_INT MQ_DECL MqBufU2INT ( union MqBufferU bufU );
/// \brief extract a MQ_FLT from a #MqBufferS object
/// \bufU
MQ_EXTERN MQ_FLT MQ_DECL MqBufU2FLT ( union MqBufferU bufU );
/// \brief extract a MQ_WID from a #MqBufferS object
/// \bufU
MQ_EXTERN MQ_WID MQ_DECL MqBufU2WID ( union MqBufferU bufU );
/// \brief extract a MQ_DBL from a #MqBufferS object
/// \bufU
MQ_EXTERN MQ_DBL MQ_DECL MqBufU2DBL ( union MqBufferU bufU );
/// \brief extract a MQ_ATO from a #MqBufferS object
/// \bufU
MQ_EXTERN MQ_ATO MQ_DECL MqBufU2ATO ( union MqBufferU bufU );

/// \brief create a new #MqBufferS with \a size
/// \context
/// \param size initial size of the buffer, if \e size = 0 the initial size is set to an default value
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferCreate (
  struct MqS * const context,
  MQ_SIZE size
);

/// \brief delete a new #MqBufferS 
/// \param bufP a pointer to an struct MqBufferS * object
/// \attention \attDelete
MQ_EXTERN void MQ_DECL MqBufferDelete (
  struct MqBufferS ** const bufP
);

/// \brief reset a #MqBufferS to the length zero
/// \buf
MQ_EXTERN void MQ_DECL MqBufferReset (
  struct MqBufferS * const buf
);

/// \brief copy the #MqBufferS from \a srce to \a dest
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
  MQ_CBI  const val,
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
/// \if MSGQUE
/// \anchor \NS{BufferGetTYPE}
/// \endif
MQ_EXTERN enum MqErrorE MQ_DECL MqBufferGetY (
  struct MqBufferS * const buf,
  MQ_BYT * const valP
);

/// \copydoc MqBufferGetY
MQ_EXTERN enum MqErrorE MQ_DECL MqBufferGetO (
  struct MqBufferS * const buf,
  MQ_BOL * const valP
);

/// \copydoc MqBufferGetY
MQ_EXTERN enum MqErrorE MQ_DECL MqBufferGetS (
  struct MqBufferS * const buf,
  MQ_SRT * const valP
);

/// \copydoc MqBufferGetY
MQ_EXTERN enum MqErrorE MQ_DECL MqBufferGetI (
  struct MqBufferS * const buf,
  MQ_INT * const valP
);

/// \copydoc MqBufferGetY
MQ_EXTERN enum MqErrorE MQ_DECL MqBufferGetF (
  struct MqBufferS * const buf,
  MQ_FLT * const valP
);

/// \copydoc MqBufferGetY
MQ_EXTERN enum MqErrorE MQ_DECL MqBufferGetW (
  struct MqBufferS * const buf,
  MQ_WID * const valP
);

/// \copydoc MqBufferGetY
MQ_EXTERN enum MqErrorE MQ_DECL MqBufferGetD (
  struct MqBufferS * const buf,
  MQ_DBL * const valP
);

/// \brief function to read an #MQ_BIN from an #MQ_BUF object
/// \buf
/// \retval out the pointer to an MQ_BIN array object
/// \retval size the size of the array the array pointer
/// \retMqErrorE
/// \attention the return-pointer (\e out) is owned by the #MQ_BUF object -> never free this pointer
MQ_EXTERN enum MqErrorE MQ_DECL MqBufferGetB (
  struct MqBufferS * const buf,
  MQ_BIN  * const out,
  MQ_SIZE * const size
);

/// \brief function to read an #MQ_STR from an #MQ_BUF object
/// \buf
/// \retval out the pointer to an MQ_STR object
/// \retMqErrorE
/// \attention the return-pointer (\e out) is owned by the #MQ_BUF object -> never free this pointer
MQ_EXTERN enum MqErrorE MQ_DECL MqBufferGetC (
  struct MqBufferS * const buf,
  MQ_CST * const out
);

/// \brief return the type from a #MQ_BUF object on character identifer
/// \buf
/// \return the type from a #MqTypeE object as single character value
MQ_EXTERN char MQ_DECL MqBufferGetType (
  struct MqBufferS * const buf
);

/// \brief return the type from a #MQ_BUF object
/// \buf
/// \return the #MqTypeE object 
MQ_EXTERN enum MqTypeE MQ_DECL MqBufferGetType2 (
  struct MqBufferS * const buf
);

/// \brief return the type from a #MQ_BUF object as strint
/// \buf
/// \return the #MqTypeE object as string
MQ_EXTERN MQ_CST MQ_DECL MqBufferGetType3 (
  struct MqBufferS * const buf
);

/// \brief return the #MqErrorS object from a #MQ_BUF object
/// \buf
/// \return the #MqErrorS object
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
/// \param in the \e byte-array to set the buffer
/// \param len the length of the \e byte-array data
/// \retMqBufferS
MQ_EXTERN struct MqBufferS * MQ_DECL MqBufferSetB (
  struct MqBufferS * const buf,
  MQ_CBI  const in,
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

/// \brief append a single \a string to a #MqBufferS
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

/// \brief add \a str to the #MqBufferS
/// \buf
/// \param string the text to append to \e buf
/// \retval the size of the string appended to the MqBufferS object
MQ_EXTERN MQ_SIZE MQ_DECL MqBufferPush (
  register struct MqBufferS * const buf,
  MQ_CST const string
);

/// \brief delete \a str from the #MqBufferS
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

/// \brief log the whole #MqBufferS object to the stderr device
/// \context
/// \buf
/// \param prefix item to identify the output
MQ_EXTERN void MQ_DECL MqBufferLog (
  struct MqS const * const context,
  struct MqBufferS const * const buf,
  MQ_CST const prefix
);

/// \brief log the short #MqBufferS object data to the stderr device
/// \context
/// \buf
/// \param prefix item to identify the output
MQ_EXTERN void MQ_DECL MqBufferLogS (
  struct MqS const * const context,
  struct MqBufferS const * const buf,
  MQ_CST const prefix
);

/** \} */

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                 B U F F E R - L I S T - A P I                   ### */
/* ###                                                                 ### */
/* ####################################################################### */


/** \defgroup Mq_Buffer_List_C_API Mq_Buffer_List_C_API
 *  \{
 *  \brief create and manage a list of #MqBufferS objects.
 *
 *  the memory is allocated and extended dynamically. 
 */

/*****************************************************************************/
/*                                                                           */
/*                          buffer_list_init                                 */
/*                                                                           */
/*****************************************************************************/

/// \brief the object to manage an array of #MqBufferS items.
///
/// every #MqBufferLS is able to store a infinite number of #MqBufferS objects
/// using dynamic allocation. the #MqBufferS objects are stored in a flat
/// array of #MqBufferS object pointers. to access an object only the \a data 
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

/// \brief create and return a #MqBufferLS object using a \e va_list argument
MQ_EXTERN struct MqBufferLS * MQ_DECL MqBufferLCreateArgsVA (
  struct MqS * const context,
  va_list ap
);

/// \brief create and return a #MqBufferLS object using \e varargs arguments end with \c NULL
MQ_EXTERN struct MqBufferLS * MQ_DECL MqBufferLCreateArgsV (
  struct MqS * const context,
  ...
);

/// \brief create and return a a #MqBufferLS object using \e main startup arguments
MQ_EXTERN struct MqBufferLS * MQ_DECL MqBufferLCreateArgsVC (
  struct MqS * const context,
  int const   argc,
  MQ_CST      argv[]
);

/// \brief create an #MqBufferLS object as copy from an existing #MqBufferLS object
/// \param in the #MqBufferLS object to copy from
/// \return out the #MqBufferLS object to create
MQ_EXTERN struct MqBufferLS * MQ_DECL MqBufferLDup (
  struct MqBufferLS const * const in
);

/// \brief delete a dynamically created #MqBufferLS object
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
/// \context
/// \bufL0
/// \optionL
/// \retval var if \e opt is found set \e var to #MQ_YES otherwise #MQ_NO
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
/// \param doDelete if \e delete != 0 delete the #MqBufferS object, associated with the index, too
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
/// \return The index of the \e str found or -1 if not found. The return value can be used as startindex of following calls to #MqBufferLSearchC
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

/// \defgroup Mq_Error_C_API Mq_Error_C_API
/// \if MSGQUE
/// \anchor \NS{ErrorObject}
/// \endif
/// \{
/// \brief create and manage error messages.
///
/// the management is done with a #MqErrorS object created by #MqContextCreate.
/// every #MqS object is linked with only one #MqErrorS object and every #MqErrorS 
/// object is linked with only one #MqS object.
/// the #MqErrorS object is used to collect all data needed to handle an error.

/// \brief panic on error
///
/// This item is used as special meaning for the \c struct #MqErrorS 
/// argument of error-functions
#define MQ_ERROR_PANIC ((struct MqS* const)NULL)

/// \brief ignore error and do not generate any error-text (don't fill the error object)
///
/// This item is used as special meaning for the \c struct #MqErrorS 
/// argument of error-functions
#define MQ_ERROR_IGNORE ((struct MqS* const)0x1)

/// \brief print error to stderr
///
/// This item is used as special meaning for the \c struct #MqErrorS 
/// argument of error-functions
#define MQ_ERROR_PRINT ((struct MqS* const)0x2)

/// \brief check if the error pointer is a \e real pointer or just a flag
#define MQ_ERROR_IS_POINTER(e) (e>MQ_ERROR_PRINT)

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
) __attribute__((noreturn));

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
) __attribute__ ((noreturn, format (printf, 4, 5)));

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
	"internal ERROR, please contact your local support");

/// \brief clear the \e error and reset the \e context
MQ_EXTERN enum MqErrorE MQ_DECL MqErrorReset (
  struct MqS * const context
);

/*****************************************************************************/
/*                                                                           */
/*                         create an error message                           */
/*                                                                           */
/*****************************************************************************/

/// \brief set an error-code in #MqErrorS with vararg list argument
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

/// \brief set a error-code in #MqErrorS with vararg string argument
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

/// \brief set the \e context to an \e error but do \b not raise an \RNS{ErrorObject}
/// \ctx
/// \param[in] prefix the error-location like the current function-call or operation
/// \param[in] errnum the error-number
/// \param[in] message the error-message
MQ_EXTERN enum MqErrorE MQ_DECL MqErrorC (
  struct MqS * const ctx,
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

/// \brief append a vararg string to a #MqErrorS
/// \context
/// \format
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqErrorSAppendV (
  struct MqS const * const context,
  MQ_CST const fmt,
  ...
) __attribute__ ((format (printf, 2, 3)));

/// \brief append a string to a #MqErrorS
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
/*                              error_set/get                                */
/*                                                                           */
/*****************************************************************************/

/// \brief return the value of #MqErrorS code member
/// \context
MQ_EXTERN enum MqErrorE MQ_DECL MqErrorGetCode (
  struct MqS const * const context
);

/// \copydoc MqErrorGetCode
static mq_inline enum MqErrorE MqErrorGetCodeI (
  struct MqS const * const context
)
{
  return context->error.code;
}

/// \brief set the value of #MqErrorS code member
/// \context
/// \param code the \e code to set for \e error
MQ_EXTERN enum MqErrorE MQ_DECL MqErrorSetCode (
  struct MqS * const context,
  enum MqErrorE code
);

/// \return the \e error-message from the \e exception-object
MQ_EXTERN MQ_CST MQ_DECL MqErrorGetText (
  struct MqS const * const ctx
);

/// \brief print the \e error from the \e context to stderr and clear the \e error afterwards
MQ_EXTERN void MQ_DECL MqErrorPrint (
  struct MqS * const context
);

/// \return the \e error-number from the \e exception-object. The number can be used as exit-code.
MQ_EXTERN MQ_INT MQ_DECL MqErrorGetNum (
  struct MqS const * const ctx
);

/// \copydoc MqErrorGetNum
static mq_inline MQ_INT MqErrorGetNumI (
  struct MqS const * const ctx
)
{
  return ctx->error.num;
}

/// \brief set the value of the #MqErrorS object
/// \context
/// \param[in] num the error number to set
/// \param[in] code the error code to set
/// \param[in] message the error message to set
/// \param[in] errctx original source of the error
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqErrorSet (
  struct MqS * const context,
  MQ_INT num,
  enum MqErrorE code,
  MQ_CST const message,
  struct MqS *const errctx
);

/// \brief signal end of processing in an \RNSC{IEvent} callback
MQ_EXTERN enum MqErrorE MQ_DECL MqErrorSetCONTINUE (
  struct MqS * const context
);

/// \brief create the application exit flag
MQ_EXTERN enum MqErrorE MQ_DECL MqErrorSetEXITP (
  struct MqS * const context,
  MQ_CST const prefix
);

/// \brief finish the current \e callback, return to \e toplevel and \RNSA{Exit} the application
/// \details To exit a application from a callback is a difficult task because
/// the code is \e in-duty. To achive this goal a special \e exit-error-object
/// is created and reportet to the \e toplevel. If a \e transaction is ongoing
/// the \RNSA{SendRETURN} is \b not called and thus the transaction is not finished.
/// The calling application is informed later by a \e socket-down event.
#define MqErrorSetEXIT(ctx) MqErrorSetEXITP(ctx,__func__)

/// \brief check for an \e exit-error-object, return \yes or \no
/// \details A \e \e exit-error-object is set to signal a fatal error
/// which require an \e application-exit. The \e only source
/// of this kind of error is a \e link-target-abnormal-exit like a server/network
/// crash or an \RNSA{ErrorSetEXIT} function call. 
/// The \e link-target-abnormal-exit can only happen for functions doing a network-request like:
/// \RNSA{LinkCreate}, \RNSA{LinkCreateChild}, \RNSA{LinkConnect}, 
/// \RNSA{SendEND}, \RNSA{SendEND_AND_WAIT} or \RNSA{ProcessEvent}.
/// The goal of this function is to act on an \e exit-error-object
/// and is used to ignore this error using \RNSA{ErrorReset} and
/// later do a reconnect using \RNSA{LinkConnect}.\n
/// Read more using the code from <B><TT>theLink/example/LNG/Filter4.LNG</TT></B>
/// \ifnot MAN
/// \b Example: catch and ignore an EXIT return-code
/// \code
/// if (MqErrorCheckI (MqSendEND_AND_WAIT (ctx, "TOKS", MQ_TIMEOUT_USER))) { 
///   if (MqErrorIsEXIT (ctx)) MqErrorReset (ctx); 
/// } 
/// \endcode
/// \endif
MQ_EXTERN int MQ_DECL MqErrorIsEXIT (
  struct MqS * const context
);

/// \brief cleanup an \e exit-error
MQ_EXTERN enum MqErrorE MqErrorDeleteEXIT (
  struct MqS * const context
);

/*****************************************************************************/
/*                                                                           */
/*                              error_is/check                               */
/*                                                                           */
/*****************************************************************************/

/// \brief check on error
/// \return on #MQ_OK and #MQ_CONTINUE return \b 1 and an #MQ_ERROR return \b 0
#define MqErrorCheckI(PROC) (unlikely((PROC) >= MQ_ERROR))
/// \brief check \e return-code and <I>goto error</I> on error
#define MqErrorCheck(PROC) if (MqErrorCheckI(PROC)) goto error

/// \brief process error message
#define MqErrorSwitch(PROC) switch (PROC) {\
case MQ_OK: break; case MQ_ERROR: goto error; case MQ_CONTINUE: return MQ_CONTINUE;\
}
/// \brief process error message
#define MqErrorReturn(PROC) switch (PROC) {\
case MQ_OK: return MQ_OK; case MQ_CONTINUE: return MQ_CONTINUE; case MQ_ERROR: return MqErrorStack (MQ_CONTEXT_S);\
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

#if defined(_DEBUG)
/// \brief log the #MqErrorS for debugging
/// \context
/// \prefix
MQ_EXTERN void MQ_DECL MqErrorLog (
  struct MqS const * const context,
  MQ_CST const prefix
);
#endif

/// \brief copy a #MqErrorS from \a in to \a out
/// \param out the #MqS::error object created by #MqLinkCreate
/// \param in  the #MqS::error object created by #MqLinkCreate
MQ_EXTERN enum MqErrorE MQ_DECL MqErrorCopy (
  struct MqS * const out,
  struct MqS * const in
);

/** \} Mq_Error_C_API */

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                        R E A D - A P I                          ### */
/* ###                                                                 ### */
/* ####################################################################### */

/// \defgroup Mq_Read_C_API Mq_Read_C_API
/// \if MSGQUE
/// \anchor \NS{ReadData}
/// \endif
/// \{
/// \brief extract data from an incoming \e read-data-package.
///
/// \e Reading data is a passive task and the opposite of \e sending data which is an active task.
/// Passive mean that the \e read is triggered by an incoming data-package and not by the
/// software-work-flow or the user. There is one \e read function for every basic type
/// defined in \RNS{BufferIdentifer} plus a couple of help functions.
/// \attention Reading data is an atomic task and should not be split. Only one \e read-data-package 
/// is always in duty. As basic rule read first all data and start the processing afterwards.

/*****************************************************************************/
/*                                                                           */
/*                                read_reference                             */
/*                                                                           */
/*****************************************************************************/

/// \brief start to extract a \e list-items from the \e read-data-package.
///
/// Initialize the read with the current \e body-item or an optional \e buffer-object.
/// This command requires a final \RNS{ReadL_END} to finish the read.
/// \ctx
/// \param[in] buffer an optional \e buffer-object as result from a previous \RNSA{ReadU} call or \null to
///   use the next item from the \e read-data-package.
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqReadL_START (
  struct MqS * const ctx,
  struct MqBufferS * buffer
);

/// \brief finish to extract a \e list-items from the \e read-data-package.
/// \ctx
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqReadL_END (
  struct MqS * const ctx
);

/// \brief start to extract a \e longterm-transaction-item from the \e read-data-package.
///
/// Initialize the read with the \e current-item or an optional \e buffer-object.
/// The \e current-item have to be the first item in the \e read-data-package.
/// This command requires a final \RNS{ReadL_END} to finish the read.
/// \ctx
/// \param[in] buffer an optional \e buffer-object as result from a previous \RNSA{ReadU} call or \null to
///   use the next item from the \e read-data-package.
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqReadT_START (
  struct MqS * const ctx,
  struct MqBufferS * buffer
);

/// \brief finish to extract a \e longterm-transaction-item from the \e read-data-package.
/// \ctx
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqReadT_END (
  struct MqS * const ctx
);

/*****************************************************************************/
/*                                                                           */
/*                              read functions                               */
/*                                                                           */
/*****************************************************************************/

/// \brief read a \RNS{BufferIdentifer} from the \e read-data-package
/// \ctx
/// \param[out] val the value to read
/// \retException
/// \if MSGQUE
/// \anchor \NS{ReadTYPE}
/// \endif
MQ_EXTERN enum MqErrorE MQ_DECL MqReadY (
  struct MqS * const ctx,
  MQ_BYT * const val
) __attribute__((nonnull(1)));

/// \copydoc MqReadY
MQ_EXTERN enum MqErrorE MQ_DECL MqReadO (
  struct MqS * const ctx,
  MQ_BOL * const val
) __attribute__((nonnull(1)));

/// \copydoc MqReadY
MQ_EXTERN enum MqErrorE MQ_DECL MqReadS (
  struct MqS * const ctx,
  MQ_SRT * const val
) __attribute__((nonnull(1)));

/// \copydoc MqReadY
MQ_EXTERN enum MqErrorE MQ_DECL MqReadI (
  struct MqS * const ctx,
  MQ_INT * const val
) __attribute__((nonnull(1)));

/// \copydoc MqReadY
MQ_EXTERN enum MqErrorE MQ_DECL MqReadF (
  struct MqS * const ctx,
  MQ_FLT * const val
) __attribute__((nonnull(1)));

/// \copydoc MqReadY
MQ_EXTERN enum MqErrorE MQ_DECL MqReadW (
  struct MqS * const ctx,
  MQ_WID * const val
) __attribute__((nonnull(1)));

/// \copydoc MqReadY
MQ_EXTERN enum MqErrorE MQ_DECL MqReadD (
  struct MqS * const ctx,
  MQ_DBL * const val
) __attribute__((nonnull(1)));

/// \brief generic function to read an #MQ_STR object from the \e Read-Buffer object
/// \context
/// \retval out the string to return
/// \retMqErrorE
/// \attention the string returned has \e static allocation and have to be
/// duplicated for long term storage.
MQ_EXTERN enum MqErrorE MQ_DECL MqReadC (
  struct MqS * const context,
  MQ_CST * const out
) __attribute__((nonnull(1)));

/// \brief generic function to read an #MQ_BIN object from the \e Read-Buffer object
/// \context
/// \retval out the \e byte-array data to return
/// \retval len the length of the \e byte-array data
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqReadB (
  struct MqS * const context,
  MQ_BIN  * const out,
  MQ_SIZE * const len
) __attribute__((nonnull(1)));

/// \brief extract a \e body-item from the \e read-data-package
///
/// A \e body-item is a \e byte-array with a defined \e length and including the \e size, 
/// \e data-type and the \e native data object as information. The \e item extracted
/// can be saved into an external storage and be send later using \RNSA{SendN}.
/// \ctx
/// \param[out] val the \e body-item as \e byte-array
/// \param[out] len the \e byte-array-length of the \e val
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqReadN (
  struct MqS * const ctx,
  MQ_CBI  * const val,
  MQ_SIZE * const len
) __attribute__((nonnull(1)));

/// \brief extract the entire \e body-package from the \e read-data-package
///
/// A \e body is a \e byte-array with a defined \e length and including the \e number-of-items
/// and the \e body-items as information. The \e body extracted can be saved into an external 
/// storage or be used in a software tunnel (example: the \e agurad tool) and be ican send later 
/// using \RNSA{SendBDY}. 
/// \if C-STYLE
/// The memory is \e dynamic-allocated and have to be freed using #MqSysFree.
/// \endif
/// \ctx
/// \param[out] val the \e body as \e byte-array
/// \param[out] len the \e byte-array-length of the \e val
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqReadBDY (
  struct MqS * const ctx,
  MQ_BIN  * const val,
  MQ_SIZE * const len
) __attribute__((nonnull(1)));

/// \brief extract a \b temporary \RNS{BufferObject} from the \e read-data-package
///
/// The object returned is owned by the \e read-data-package and is \b only valid
/// up to the next call of any \RNS{ReadData} function. If a long-term object is required
/// use the C-API function: #MqBufferDup.
/// \ctx
/// \param[out] val the buffer
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqReadU (
  struct MqS * const ctx,
  struct MqBufferS ** val
) __attribute__((nonnull(1)));

/// \brief read an MqBufferLS object from all items of the \e Read-Buffer object
/// \context
/// \param[in,out] out data will be appended to \e *out, if \e *out != \c NULL the
///                    \e *out value will be used otherwise \e *out will be set to a new
///		       #MqBufferLS object
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqReadL (
  struct MqS * const context,
  struct MqBufferLS ** const out
) __attribute__((nonnull(1)));


/// \brief link two \e context-objects to direct pass a data item from one object to the other.
/// \ctx and the source of the copy
/// \param[in] otherCtx the \e other-context-object and the target of the copy
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqReadProxy (
  struct MqS * const ctx,
  struct MqS * const otherCtx
) __attribute__((nonnull(1)));

/*****************************************************************************/
/*                                                                           */
/*                                read_high                                  */
/*                                                                           */
/*****************************************************************************/

/// \brief undo the last \RNS{ReadData} function call
///
/// Put the internal position-pointer to the start of the last read \e body-item.
/// The next read function call will extract the same item again. Only \b one
/// undo level is supported.
/// \ctx
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqReadUndo (
  struct MqS * const ctx
);

/// \brief get the number of items left in the \e read-data-package
/// \ctx
/// \return the number of items as integer
MQ_EXTERN MQ_SIZE MQ_DECL MqReadGetNumItems (
  struct MqS const * const ctx
);

/// \brief check if an item exists in the \e read-data-package
/// \ctx
/// \return boolean, \yes or \no
MQ_EXTERN MQ_BOL MQ_DECL MqReadItemExists (
  struct MqS const * const ctx
);

/** \} Mq_Read_C_API */

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                        S E N D - A P I                          ### */
/* ###                                                                 ### */
/* ####################################################################### */

/** \defgroup Mq_Send_C_API Mq_Send_C_API
 *  \{
 *  \brief construct an outgoing \e send-data-package.
 *
 *  \e Sending data is an active task and the opposite of \e reading data which is an passive task.
 *  Active mean that the \e send is triggered by the
 *  software-work-flow or the user. There is one \e send function for every basic type
 *  defined in \RNS{BufferIdentifer} plus a couple of help functions.
 *  \attention Sending data is an atomic task and should not be split. Only one \e send-data-package 
 *  is always in duty. As basic rule do the processing first and send all the data afterwards.
 *  If the function \RNSA{SendEND_AND_WAIT} is used the application enter the event-loop and is still 
 *  able to work on other client/server links.
 *
\ifnot MAN
example 1: in the client, calling the \e service and wait for an answer
\code
void MyServiceCall(struct MqS * const context) { // the libmsgque object
    ...
    MqSendSTART(context);                          // init the Send-Buffer object
    MqSendI(context,int_value);                    // 1. argument: a MQ_INT value
    MqSendV(context,"num:%x",num);                 // 2. argument: a vararg string value
    MqSendB(context,mypicture,size);               // 3. argument: a \e byte-array picture of size length
    // call service "SRV1" and wait maximum 60 seconds for the results
    MqErrorCheck(MqSendEND_AND_WAIT(context,"SRV1",60);
    // ... get the results
    MqErrorCheck(MqReadI(context, ???);
    ...
}
\endcode

example 2: in the server, answer the \e service call
\code
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
\endcode
\endif
 **/

/*****************************************************************************/
/*                                                                           */
/*                              send functions                               */
/*                                                                           */
/*****************************************************************************/

/// \brief append a native \RNS{BufferIdentifer} value to the \e send-data-package.
/// \ctx
/// \param[in] value the value for appending
/// \retException
/// \if MSGQUE
/// \anchor \NS{SendTYPE}
/// \endif
MQ_EXTERN enum MqErrorE MQ_DECL MqSendY (
  struct MqS * const ctx,
  const MQ_BYT value
);

/// \copydoc MqSendY
MQ_EXTERN enum MqErrorE MQ_DECL MqSendO (
  struct MqS * const ctx,
  const MQ_BOL value
);

/// \copydoc MqSendY
MQ_EXTERN enum MqErrorE MQ_DECL MqSendS (
  struct MqS * const ctx,
  const MQ_SRT value
);

/// \copydoc MqSendY
MQ_EXTERN enum MqErrorE MQ_DECL MqSendI (
  struct MqS * const ctx,
  const MQ_INT value
);

/// \copydoc MqSendY
MQ_EXTERN enum MqErrorE MQ_DECL MqSendF (
  struct MqS * const ctx,
  const MQ_FLT value
);

/// \copydoc MqSendY
MQ_EXTERN enum MqErrorE MQ_DECL MqSendW (
  struct MqS * const ctx,
  const MQ_WID value
);

/// \copydoc MqSendY
MQ_EXTERN enum MqErrorE MQ_DECL MqSendD (
  struct MqS * const ctx,
  const MQ_DBL value
);

/// \copydoc MqSendY
MQ_EXTERN enum MqErrorE MQ_DECL MqSendC (
  struct MqS * const ctx,
  MQ_CST const value
);

/// \brief append a #MQ_BIN object to the \e Send-Buffer object
/// \context
/// \param in the \e byte-array data to send
/// \param len the size of the \e byte-array data to send
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqSendB (
  struct MqS * const context,
  MQ_CBI  const in,
  MQ_SIZE const len
);

/// \brief append a \e body-item to the \e send-data-package.
/// \details A \e body-item is a \e byte-array and including the \e item-size, 
/// \e item-type and the \e item-data as value.
/// The \e body-item is the result of a previous \RNS{ReadN} function call and can 
/// be used for saving into an external storage or for an additional operation like 
/// encryption.
/// \ctx
/// \param[in] value the \e body-item for appending
/// \param[in] len the size of the \e body-item-byte-array (C-API only)
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqSendN (
  struct MqS * const ctx,
  MQ_CBI  const value,
  MQ_SIZE const len
);

/// \brief send the \e entire-body to the \e link-target
/// \details The goal of this function is to send the \e entire-body of a service
/// request to the \e link-target. The \e entire-body is the result of a previous 
/// \RNSA{ReadBDY} function call and can be used to save the \e entire-body into an 
/// external storage or for an additional operation like encryption.
/// The \e SendBDY is a special kind of function because it combines the \e SendSTART, 
/// \e Send? and \e SendEND... style of function into one single command.
/// \ctx
/// \param[in] value the \e entire-body for appending
/// \param[in] len the size of the \e entire-body-byte-array (C-API only)
/// \retException 
MQ_EXTERN enum MqErrorE MQ_DECL MqSendBDY (
  struct MqS * const ctx,
  MQ_CBI  value,
  MQ_SIZE len
);

/// \brief append a #MQ_BUF object to the \e Send-Buffer object
/// \context
/// \param in the pointer to an #MqBufferS object to send
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqSendU (
  struct MqS * const context,
  struct MqBufferS * const in
);

/// \brief append a #MqBufferLS object to the \e Send-Buffer object
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

/// \brief initialize the \e send-data-package.
/// \details This function have to be the first statement in a \e send-data-block.
/// \ctx
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqSendSTART (
  struct MqS * const ctx
);

/// \brief finish the \e send-data-block, call the remote service and do \e not expect a return.
/// \details If an \b error is raised on the server during the service-processing the
/// error is send back as \e asynchronous-error-event and will be raised during the next enter into the
/// event-loop. Use \RNSC{IBgError} to control the \e asynchronous-error-event processing.
/// \ctx
/// \token
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqSendEND (
  struct MqS * const ctx,
  MQ_TOK const token
);

/// \brief finish the \e send-data-block, call the remote service and do \e wait for return.
/// \details If an \b error is raised on the server during the service-processing the function
/// will return this error immediately. During waiting for the return the \e event-loop is used
/// to process other events. Use \RNSC{IEvent} to add your tasks into the event loop.
/// \ctx
/// \token
/// \param_timeout
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqSendEND_AND_WAIT (
  struct MqS * const ctx,
  MQ_TOK const token,
  MQ_TIME_T timeout
);

/// \brief finish the \e send-data-block, call the remote service and do \e not-wait for return.
/// \details This function will \e never block and return immediately. If \e return-data or an
/// error is available the \e callback-function is called for processing. Use \RNS{ProcessEvent}
/// to enter the \e event-loop.
/// \ctx
/// \token
/// \param[in] callback the function to process the \e service-call results
/// \param[in] data a user defined additional \e data-argument for the \e callback function (C-API only)
/// \param[in] datafreeF the function to free the \e data-argument after use (C-API only)
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqSendEND_AND_CALLBACK (
  struct MqS * const ctx,
  MQ_TOK const token,
  MqServiceCallbackF const callback,
  MQ_PTR data,
  MqTokenDataFreeF datafreeF
);

/// \brief finish the \e send-data-block on the server and optional return the results.
/// \details Every \e service-handler have to use this function at the end to return the
/// results or the error. If an \e error is returned the local \e error-object is reset.
/// The following behaviour is used:
/// - if a shortterm transaction is ongoing this function return the data or the error to
///   the link target
/// - if the answer does not return any data no previous #MqSendSTART is required.
/// - if no transaction is ongoing this function does just return. If an error is available
///   report an asyncrone error to the link target.
/// .
/// \ctx
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqSendRETURN (
  struct MqS * const ctx
);

/// \brief send the data from the \RNS{ErrorObject} to the link target .
/// \details If an error is available the \e error-number and the \e error-text
/// is send to the link target. After send the error is reset. This function
/// only raise an error if the sending self fails.
/// \ctx
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqSendERROR (
  register struct MqS * const ctx
);

/*****************************************************************************/
/*                                                                           */
/*                              send_list                                    */
/*                                                                           */
/*****************************************************************************/

/** \brief start to append an embedded \e body-list-item to the \e send-data-package.
 *  \ctx
 *  \retException
 *
\ifnot MAN
example: create a \e body-list-item
\code
MqSendL_START(send);	    // start a list-item
MqSendI(send,myInt);        // first list-sub-item
MqSendC(send,"myString");   // second list- sub-item
// ... do additional MqSend?
MqSendL_END(send);	    // finish a list-item
\endcode
\endif
 */
MQ_EXTERN enum MqErrorE MQ_DECL MqSendL_START (
  struct MqS * const ctx
);

/// \brief finish to append an embedded \e body-list-item to the \e send-data-package.
/// \ctx
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqSendL_END (
  struct MqS * const ctx
);

/** 
\brief start to write a \e longterm-transaction-item to the \e send-data-package.

In difference to \RNSA{SendEND_AND_WAIT} and \RNSA{SendEND_AND_CALLBACK} a
\e longterm-transaction-call have to survive an application restart. To achieve
this goal two features have to be available to process the results:
-# a callback as \e known service created with \RNSA{ServiceCreate}
-# one or more \e data-item(s) to initialise the environment in the callback
.
The \e transaction-item have to be the first item in the \e data-package.
The callback is a \RNSA{ServiceIdentifier} and have to be defined 
with \RNSA{ServiceCreate} in the application setup code (like \RNSC{IServerSetup}) 
to be available after an application restart.
This command requires a final \RNSA{SendT_END} to finish the write.
The list of \e data-items between \e START and \e END have to be provided by the programmer 
and is used to initialise the environment in the \e callback. The data
is send to the \e link-target and returned as first item in the \e result-data-package.
Use \RNSA{ReadT_START} and \RNSA{ReadT_END} to extract the data.
\ctx
\param[in] callback a \RNSA{ServiceIdentifier} to identify the target
                    to process the results
\retException
*/
MQ_EXTERN enum MqErrorE MQ_DECL MqSendT_START (
  struct MqS * const ctx,
  MQ_TOK const callback
);

/// \brief finish to write a \e longterm-transaction-item to the \e send-data-package.
/// \ctx
/// \retException
MQ_EXTERN enum MqErrorE MQ_DECL MqSendT_END (
  struct MqS * const ctx
);

/** \} Mq_Send_C_API */

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                       S L A V E - A P I                         ### */
/* ###                                                                 ### */
/* ####################################################################### */

/// \defgroup Mq_Slave_C_API Mq_Slave_C_API
/// \{
/// \brief create and manage a slave context
/// 
/// The \e master-slave-link is used to create a mesh of nodes defined by
/// different \e parent-context.  <B>The master control the slave.</B>
///
/// The \e master-slave-link is used to perform the following tasks:
///  - report error messages from the \e slave-context to the \e master-context
///  - to create a \e slave-child-context if a \e master-child-context is created
///  - to delete a \e slave-context if a \e master-context is deleted
///  .
/// In difference to the \e client-server-link the \e master-slave-link connect
/// two independent \e parent-context in the same process or thread (e.g. node).
/// This leads to the restriction that only the \e master-context can be
/// a \e server-context because only one \e server-context per node is possible.
///\verbatim
///    node-0   |           node-1/2        |   node-3/4/5
///===================================================================
///
///| <- client/server link -> | <- client/server link -> |
///
///             | <-- master/slave link --> |
///
///                           |- client1-0 -|- server3 ...
///             |-  server1  -|             
///             |             |- client1-1 -|- server4 ...
///  client0-0 -|                           
///             |-  server2  -|- client1-2 -|- server5 ...  \endverbatim
///
/// <B>Definition of a "master-context"</B>\n
///  - the \e master-context is a \parent_context without a \child_context available.
///  - the \e master-context is a \client_context or a \server_context.
///  - the \e master-context is responsible to create or delete the \child_context of the \e slave.
///  - the \e master-context is responsible to delete the \parent_context of the \e slave.
///  - the \e link between the \e master-context and the \e slave-context is done using \RNSA{SlaveWorker}
///  .
/// 
/// <B>Definition of a "slave-context"</B>\n
///  - the \e slave-context is a \parent_context without a \child_context available.
///  - the \e slave-context is a \client_context.
///  - the \e slave-context lifetime is controlled by the \e master-context.
///  - the \e slave-context report all error-messages to the \e master-context.
///  - a special form of a \e slave-context is a \e worker-context
///  .
/// 
/// <B>Definition of a "worker-context"</B>\n
///  - the \e worker-context is a \e slave-context using the image of the \e master-context self.
///  - the \e master-context have to be a \server_context.
///  - the \e worker-context is created using \RNSA{SlaveWorker}
///  - the \e worker-context is identified by a \e unique integer starting with \c 0.
///  .
/// \if MSGQUE
/// \anchor \NS{SlaveContext}
/// \endif

/// \brief the maximum number of slave objects per master
/// \attention this number can be changed but \libmsgque have to be recompiled after
#define MQ_SLAVE_MAX 1024

/// \brief create a \e master/slave link using the image of the \e ctx object self.
///
/// The slave-context is started as an independent process or thread based on the \RNSC{startAs} argument.
/// \param[in] ctx the \e master context object as PARENT without a CHILD
/// \id
/// \param[in] args command-line arguments passed to the \e worker-client or the \e worker-server. all arguments prior the
/// first \b @ token are added to the \e worker-client and the other arguments to the \e worker-server.
/// \retException
MQ_EXTERN enum MqErrorE
MQ_DECL MqSlaveWorker (
  struct MqS * const  ctx,
  MQ_SIZE const	      id,
  struct MqBufferLS ** args
);

/// \brief create a \e master/slave link between the master-context object and the slave-context object
///
/// \param[in] ctx the \e master context object as PARENT without a CHILD
/// \id
/// \param[in] slave the \e slave context object as CLIENT-PARENT without a CHILD
/// \retException
/// \attention The \e slave-context is owned by the \e master-context.
/// This mean that no other external references should be used and the \e slave-context 
/// will be deleted if the \e master-context is be deleted.
MQ_EXTERN enum MqErrorE
MQ_DECL MqSlaveCreate (
  struct MqS * const  ctx,
  MQ_SIZE const       id,
  struct MqS * const  slave
);

/// \brief Delete a \e slave object from a \e master/slave link identified by \e id.
///
/// By default the \e slave-context object will be deleted if the \e master-context is deleted.
/// Use this function to delete the \e parent-slave-context explicit and brake the \e master/slave
/// link.  If \e id is invalid nothing will happen.  It is an \e error to delete a \e child-slave-context.
/// \param[in] ctx the \e master context object as PARENT without a CHILD
/// \id
/// \retException
/// \attention it is still possible to delete a \e child-slave-context using \RNSA{LinkDelete} but
/// this will break the internal master/slave order.
MQ_EXTERN enum MqErrorE
MQ_DECL MqSlaveDelete (
  struct MqS * const  ctx,
  MQ_SIZE const id
);

/// \brief get the \e slave-context from a \e master-context
///
/// \param[in] ctx the \e master context object as PARENT without a CHILD
/// \id
/// \return the \e slave-context or \c NULL if \e id is not valid or \e context is not a \e master-context.
MQ_EXTERN struct MqS *
MQ_DECL MqSlaveGet (
  struct MqS const * const  ctx,
  MQ_SIZE const id
);

/// \brief get the \e master-context from the \e slave-context
/// \ctx
/// \return the \e master-context or \null if the \e context is no \e slave-context
MQ_EXTERN struct MqS * MQ_DECL MqSlaveGetMaster (
  struct MqS const * const ctx
) __attribute__((nonnull));

/// \copydoc MqSlaveGetMaster
static mq_inline struct MqS* MqSlaveGetMasterI (
  struct MqS const * const ctx
) {
  return ctx->config.master;
}

/// \brief is the \e context a \e slave-context ?
/// \ctx
/// \return a boolean value, \yes or \no
MQ_EXTERN int MQ_DECL MqSlaveIs (
  struct MqS const * const ctx
) __attribute__((nonnull));

/// \copydoc MqSlaveIs
static mq_inline int MqSlaveIsI (
  struct MqS const * const ctx
)
{
  return (ctx->config.master != NULL);
}

/** \} slave api */

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                     S Y S T E M - A P I                         ### */
/* ###                                                                 ### */
/* ####################################################################### */

/// \defgroup Mq_System_C_API Mq_System_C_API
/// \{
/// \brief L)anguage A)bstraction L)ayer defnition
///
/// The \b LAL is required to give the target of embedding a change 
/// to modify the access to system functions. \e RUBY for example has it's
/// own Thread-Creation function.
/// 
/// All wrappers to access the underlying OS,Languags and system functions
/// are located in this section. Every external Function has additional
/// code to map the OS/System specific error codes/messages into
/// #MqS specific error codes/messages

/// process or thread identifer
typedef unsigned long MQ_IDNT;

/// \brief \b calloc syscall
/// \details additional info: <TT>man calloc</TT>
typedef MQ_PTR (*MqSysCallocF) (MQ_SIZE, MQ_SIZE);

/// \brief \b malloc syscall
/// \details additional info: <TT>man malloc</TT>
typedef MQ_PTR (*MqSysMallocF) (MQ_SIZE);

/// \brief \b strdup syscall
/// \details additional info: <TT>man strdup</TT>
typedef MQ_PTR (*MqSysStrDupF) (MQ_CST);

/// \brief \b realloc syscall
/// \details additional info: <TT>man realloc</TT>
typedef MQ_PTR (*MqSysReallocF) (MQ_PTR, MQ_SIZE);

/// \brief \b free syscall
/// \details additional info: <TT>man free</TT>
typedef void (*MqSysFreeF) (MQ_PTR);

/// \brief \b select syscall
/// \details read more at: <TT>man 2 select</TT>
/// \param[in] max the maximim file descriptor + 1 from the \e read, \e write or \e except input data
/// \param[in] read set of file descriptores
/// \param[in] write set of file descriptores
/// \param[in] except set of file descriptores
/// \param[in] timeout maximum time to wait for data
typedef int (*MqSysSelectF) (int, void*, void*, void*, struct timeval*);

/// \brief \b fork syscall
/// \details additional info: <TT>man fork</TT>
/// \retval the process identifer of the new child (in the parent process) or 0 (in the child process)
typedef MQ_IDNT (*MqSysForkF) (void);

/// signal type of the \e MqIdS data \e val
enum MqIdSE {
  MQ_ID_UNUSED  = 0,	      ///< empty struct
  MQ_ID_PROCESS = 1,	      ///< \e val has a process handle
  MQ_ID_THREAD  = 2	      ///< \e val has a thread handle
};

/// \brief data type for process/thread identification
/// \details This struct is used as storage for the process or the thread identification handle.
struct MqIdS {
  enum MqIdSE	type;	      ///< signal type of \e val data
  MQ_IDNT	val;	      ///< process or thread handle
};

/// \brief data used to initialize a new created thread
struct MqSysServerThreadMainS {
  struct MqS * tmpl;		  ///< calling (parent) context
  struct MqFactoryItemS * factory; ///< server configuration (memory belongs to caller)
  struct MqBufferLS * argv;	  ///< command-line arguments befor \b MQ_ALFA, owned by SysServerThread
  struct MqBufferLS * alfa;	  ///< command-line arguments after \b MQ_ALFA, owned by SysServerThread
};

/// \brief initialize a new created thred
/// \param[in] data configuration data for the new thread
MQ_EXTERN void MQ_DECL MqSysServerThreadMain (
  struct MqSysServerThreadMainS *data
);

/// \brief Interface between #MqS and the Operating-System
struct MqLalS {
  /// \copyall{MqSysCallocF}
  MqSysCallocF		  SysCalloc;
  /// \copyall{MqSysMallocF}
  MqSysMallocF		  SysMalloc;
  /// \copyall{MqSysReallocF}
  MqSysReallocF		  SysRealloc;
  /// \copyall{MqSysStrDupF}
  MqSysStrDupF		  SysStrDup;
  /// \copyall{MqSysFreeF}
  MqSysFreeF		  SysFree;
  /// \copyall{MqSysSelectF}
  MqSysSelectF		  SysSelect;
  /// \copyall{MqSysForkF}
  MqSysForkF		  SysFork;

  /// \syscall{spawn server create}
  /// \context
  /// \param[in]	argv  command-line arguments
  /// \param[in]	name  the name of the process
  /// \param[out]	idP   the process identifer
  /// \retMqErrorE
  ///
  /// \b Example:
  /// \dontinclude sys.c
  /// \skip sys_memory
  /// \skip SysServerSpawn
  /// \until MQ_ERROR_CAN_NOT_START_SERVER
  /// \until }
  enum MqErrorE (*SysServerSpawn) (
    struct MqS * const	  context, 
    char **		  argv, 
    MQ_CST		  name, 
    struct MqIdS *	  idP
  );

  /// \syscall{thread server create}
  /// \context
  /// \param[in,out]  factory	server configuration (memory will be freed)
  /// \param[in]	    argvP	command-line arguments befor #MQ_ALFA, owned by SysServerThread
  /// \param[in]	    alfaP	command-line arguments after #MQ_ALFA, owned by SysServerThread
  /// \param[in]	    name	the name of the thread
  /// \param[in]	    state	detachstate of the thread \c PTHREAD_CREATE_DETACHED or \c PTHREAD_CREATE_JOINABLE
  /// \param[out]	    idP		the thread identifer
  /// \retMqErrorE
  ///
  /// \b Example:
  /// \dontinclude sys.c
  /// \skip sys_memory
  /// \skip SysServerThread
  /// \until MQ_ERROR_NOT_SUPPORTED
  /// \until }
  enum MqErrorE (*SysServerThread) (
    struct MqS * const	  context, 
    struct MqFactoryItemS* factory,
    struct MqBufferLS **  argvP, 
    struct MqBufferLS **  alfaP, 
    MQ_CST		  name, 
    int			  state, 
    struct MqIdS *	  idP
  );

  /// \syscall{fork server create}
  /// \context
  /// \param[in,out]  factory	server configuration (memory will be freed)
  /// \param[in]	    argvP	command-line arguments befor #MQ_ALFA, owned by SysServerThread
  /// \param[in]	    alfaP	command-line arguments after #MQ_ALFA, owned by SysServerThread
  /// \param[in]	    name	the name of the thread
  /// \param[out]	    idP		the process identifer
  /// \retMqErrorE
  ///
  /// \b Example:
  /// \dontinclude sys.c
  /// \skip sys_memory
  /// \skip SysServerFork
  /// \until MQ_ERROR_NOT_SUPPORTED
  /// \until }
  enum MqErrorE (*SysServerFork) (
    struct MqS * const	  context, 
    struct MqFactoryItemS* factory,
    struct MqBufferLS **  argvP, 
    struct MqBufferLS **  alfaP, 
    MQ_CST		  name, 
    struct MqIdS *	  idP
  );
  /// \copyall{MqSysWait}
  ///
  /// \b Example:
  /// \dontinclude sys.c
  /// \skip sys_memory
  /// \skip SysWait
  /// \until MQ_OK
  /// \until }
  enum MqErrorE (*SysWait) (
    struct MqS * const	  context, 
    const struct MqIdS *  idP
  );
  /// \copyall{MqSysUSleep}
  ///
  /// \b Example:
  /// \dontinclude sys.c
  /// \skip sys_memory
  /// \skip SysUSleep
  /// \until }
  /// \until }
  enum MqErrorE (*SysUSleep) (
    struct MqS * const	  context, 
    unsigned int const	  usec
  );
  /// \copyall{MqSysSleep}
  ///
  /// \b Example:
  /// \dontinclude sys.c
  /// \skip sys_memory
  /// \skip SysSleep
  /// \until }
  /// \until }
  enum MqErrorE (*SysSleep) (
    struct MqS * const	  context, 
    unsigned int const	  sec
  );
  /// \brief \b daemonize the current process and save the resulting pid into the \e pidfile
  /// \context
  /// \param[in] pidfile	file to save the process identifer, can be used to kill the process later
  /// \retMqErrorE
  ///
  /// \b Example:
  /// \dontinclude sys.c
  /// \skip sys_memory
  /// \skip static enum MqErrorE SysDaemonize
  /// \until MQ_ERROR_NOT_SUPPORTED
  /// \until }
  enum MqErrorE (*SysDaemonize) (
    struct MqS * const	  context, 
    MQ_CST		  pidfile
  );
  /// \copyall{MqSysExit}
  ///
  /// \b Example:
  /// \dontinclude sys.c
  /// \skip sys_memory
  /// \skip static void SysExit
  /// \until exit(num)
  /// \until }
  void (*SysExit) (
    int			  isThread, 
    int			  num
  )  __attribute__((noreturn));
  /// \brief \b abort syscall
  /// \details additional info: <TT>man abort</TT>
  /// \retMqErrorE
  ///
  /// \b Example:
  /// \dontinclude sys.c
  /// \skip sys_memory
  /// \skip SysAbort
  /// \until }
  void (*SysAbort) (
    void
  ) __attribute__((noreturn));
};

/// Language Abstraction Layer in duty
extern struct MqLalS MqLal;

/// \syscall{calloc}
/// \details additional info: <TT>man calloc</TT>
///  \context
/// \param[in] nmemb the number of members in the memory block
/// \param[in] size the size of the new memory block
/// \return a pointer to the new memory block initialized with '0'
MQ_EXTERN MQ_PTR MQ_DECL MqSysCalloc (
  struct MqS * const context,
  MQ_SIZE nmemb,
  MQ_SIZE size
);

/// \syscall{malloc}
/// \details additional info: <TT>man malloc</TT>
///  \context
/// \param[in] size the size of the new memory block
/// \return a pointer to the new memory block
MQ_EXTERN MQ_PTR MQ_DECL MqSysMalloc (
  struct MqS * const context,
  MQ_SIZE size
);

/// \syscall{strdup}
/// \details additional info: <TT>man strdup</TT>
///  \context
/// \param[in] str the string to duplicate
/// \return a pointer to the new memory block
MQ_EXTERN MQ_STR MQ_DECL MqSysStrDup (
  struct MqS * const context,
  MQ_CST str
);

/// \syscall{realloc}
/// \details additional info: <TT>man realloc</TT>
/// \context
/// \param[in] buf the \b old memory block to extend
/// \param[in] size the size of the new memory block
/// \return a pointer to the new memory block
MQ_EXTERN MQ_PTR MQ_DECL MqSysRealloc (
  struct MqS * const context,
  MQ_PTR buf,
  MQ_SIZE size
);

/// \brief \b free syscall
/// \details additional info: <TT>man free</TT>
/// \param[in,out] tgt the memory block to delete and set to NULL
#define MqSysFree(tgt) \
    do { \
	if ( likely((tgt) != (NULL)) ) { \
	    (*MqLal.SysFree)((MQ_PTR)tgt); \
	    (tgt) = (NULL); \
	} \
    } while (0)

/// \syscall{free}
/// \details additional info: <TT>man free</TT>
/// Free memory of \e data if \e data is \b not \c NULL.
/// \param[in] data block to be freed
MQ_EXTERN void MQ_DECL MqSysFreeP(
  MQ_PTR    data
);

/// \syscall{wait for process or thread}
/// \context
/// \param[in] idP process or thread handle
/// \retMqErrorE
static mq_inline enum MqErrorE MqSysWait (
  struct MqS * const	context, 
  const struct MqIdS *  idP
)
{
  return (*MqLal.SysWait)(context, idP);
};

/// \syscall{usleep}
/// \context
/// \param[in] usec the micro (10^-6) seconds to sleep
/// \retMqErrorE
static mq_inline enum MqErrorE MqSysUSleep (
  struct MqS * const	context, 
  unsigned int const	usec
)
{
  return (*MqLal.SysUSleep)(context, usec);
};

/// \syscall{sleep}
/// \context
/// \param[in] sec the seconds to sleep
/// \retMqErrorE
static mq_inline enum MqErrorE MqSysSleep (
  struct MqS * const	context, 
  unsigned int const	sec
)
{
  return (*MqLal.SysSleep)(context, sec);
};

/// \syscall{exit}
/// \param[in] isThread	exit a thread?
/// \param[in] num exit code
/// \retMqErrorE
__attribute__((noreturn)) static mq_inline void MqSysExit (
  int			isThread, 
  int			num
)
{
  (*MqLal.SysExit)(isThread,num);
};

/// \syscall{basename}
/// \details additional info: <TT>man basename</TT>
/// \param[in] in name of the string to extract the basename from (value of \e in will not be changed)
/// \param[in] includeExtension add extension like '.exe' to the filename (#MQ_YES or #MQ_NO)
/// \return the basename of \e in (it is save to modify the basename for additional needs)
/// \attention the memory of the basename string returned is owned by the caller and have to be freed
MQ_EXTERN MQ_STR MQ_DECL MqSysBasename (
  MQ_CST const in, 
  MQ_BOL includeExtension
);

/// \syscall{gettimeofday}
/// \details additional info: <TT>man gettimeofday</TT>
/// \context
/// \retval tv the timeval object
/// \retval tz the timezone object
/// \retMqErrorE
MQ_EXTERN enum MqErrorE MQ_DECL MqSysGetTimeOfDay (
  struct MqS * const context,
  struct mq_timeval * tv,
  struct mq_timezone * tz
);

/// \}    Mq-LAL-API

/// \} Mq_System_C_API

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                        L O G - A P I                            ### */
/* ###                                                                 ### */
/* ####################################################################### */

/// \defgroup Mq_Log_C_API Mq_Log_C_API
/// \{
/// \brief log information to stderr with a common format.
///

/** \brief write a logging-message to stderr using the internal format
\details The logging is only done if '<TT>\RNSC{debug} >= level</TT>' and '<TT>\RNSC{isSilent} == \no</TT>' using the following format:
\verbatim
C> (NAME:PID:THREADID) [YYYY-MM-DD:HH-MM-SS] [DEBUGLEVEL-CONTEXTID-REFCOUNT-CONTEXTPTR-prefix]: message
\endverbatim
\ctx
\prefix
\level
\param[in] message string to log to stderr
*/
MQ_EXTERN void MQ_DECL MqLogC (
  struct MqS const * const ctx,
  MQ_CST const prefix,
  MQ_INT level,
  MQ_CST const message
);

/// \brief write a \e printf style logging-message to stderr using the internal format
/// \details Use the format and the restrictions from \RNSA{LogC}.
/// \ctx
/// \prefix
/// \level
/// \format
MQ_EXTERN void MQ_DECL MqLogV (
  struct MqS const * const ctx,
  MQ_CST const prefix,
  MQ_INT level,
  MQ_CST const fmt,
  ...
) __attribute__ ((format (printf, 4, 5)));

/// \brief write a \e vprintf style logging-message to stderr using the internal format
/// \details Use the format and the restrictions from \RNSA{LogC}.
/// \ctx
/// \prefix
/// \level
/// \format
/// \var_list
MQ_EXTERN void MQ_DECL MqLogVL (
  struct MqS const * const ctx,
  MQ_CST const prefix,
  MQ_INT level,
  MQ_CST const fmt,
  va_list var_list
);

#if defined(MQ_DISABLE_LOG)
#   define MqSetDebugLevel(context)
#   define MqDLogC(context,level,str)
#   define MqDLogV(context,level,fmt,VAR_ARGS)
#else

/// \brief define a variable \e debugLevel valid only in the current context
#define MqSetDebugLevel(context) debugLevel = context->config.debug

/// \brief log a plain string 
/// \context
/// \param level from \RNSC{debug}
/// \param str string to log
#   define MqDLogC(context,level,str) \
      if (unlikely(context != NULL && context->config.debug >= level)) MqLogC(context,__func__,level,str)

/// \brief log a plain string using the \e MqSetDebugLevel definition
/// \context
/// \param level from \RNSC{debug}
/// \param str string to log
#   define MqDLogCL(context,level,str) \
      if (unlikely(debugLevel >= level)) MqLogC(context,__func__,level,str)

/// \brief log a vararg string using formatting and checking for logging level
/// \context
/// \param level from \RNSC{debug}
/// \format
/// \param ... vararg string argument
/// \attention the prefix string is the calling function name
#   define MqDLogV(context, level,fmt,...) \
      if (unlikely(MQ_ERROR_IS_POINTER(context) && context->config.debug >= level)) MqLogV(context,__func__,level,fmt,__VA_ARGS__)

/// \brief log a vararg string using formatting and checking for logging level using the \e MqSetDebugLevel definition
/// \context
/// \param level from \RNSC{debug}
/// \format
/// \param ... vararg string argument
/// \attention the prefix string is the calling function name
#   define MqDLogVL(context, level,fmt,...) \
      if (unlikely(debugLevel >= level)) MqLogV(context,__func__,level,fmt,__VA_ARGS__)

#endif

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

/// \}	  Mq_Log_C_API

/* ####################################################################### */
/* ###                                                                 ### */
/* ###                     F I L T E R - A P I                         ### */
/* ###                                                                 ### */
/* ####################################################################### */

/**

\ingroup Mq_C_API
\defgroup Mq_Filter_C_API Mq_Filter_C_API
\brief modify data using filter technology

The filter mode is related to a special usage of the \libmsgque software called a command pipeline.
To define a filter create a \e server with:
 - \RNSC{isServer} or \RNSC{IServerSetup}
 .
and add a factory interface:
 - \RNSC{IFactory}
 .

Every filter has \b two context one belongs to the \e left command and one belongs to the \e right command: \verbatim
 <-- left cmd --> <------- filter -------> <-- right cmd -->

                  <-- left --><- right -->
                  <- server -><- client ->
                  <- master -><- slave -->
                  <-context1-><-context2->

  ... command1   @         filter         @   command2 ...
\endverbatim
The \e left context is created on application startup and the \e right context is created as \e slave of the
\e left context.
 - if the \b @ argument is followed by an \e normal command (server) a local pipeline is created: \verbatim
client @ filter @ server
\endverbatim
 - if the \b @ argument is followed by an option a non-local pipeline is created: \verbatim
<------------ host-1 -------------> <-- network --> <---------- host-2 ----------->
       <---- client arguments ---->                       <--- server arguments -->
                <-- filter arg. -->
                  <--- options --->

client @ filter @ --tcp --port 7777   ...........   server --tcp --port 7777 --fork
\endverbatim

<B>BI-DIRECTIONAL FILTER</B>\n

A bi-directional filter allow a \e data-flow in both directions and is used in a \e classical client/server application: \verbatim
    client ... <--> ... server
\endverbatim as a feature enhancement like a protocol-tunnelling: \verbatim
    client @ mq2tunnel ... <--> ... tunnel2mq @ server
\endverbatim or to convert the protocol into an other protocol: \verbatim
    client @ mq2soap ... <--> ... soap-server
\endverbatim
To define a \e bi-directional filter a couple of commands provide support:
 - \RNSA{ServiceCreate}
  - use the token \b +ALL to add a listener for \e all services. This feature is used
    for a tunnel to modify the body at all. (example: \c aguard)
  .
 - \RNSA{ServiceProxy}
  - use this function to link the \e left context with the \e right context identified with
    the slave-identifier \e id (default: 0). No data manipulation is performed.
  .
 - \RNSA{ServiceGetFilter}
  - in a filter service the current context is used to \e read the data. To \e send the
    data an other context, belonging to the \e other site of the communication, have to be used.
    This function return the context of the other site.
  .
 - \RNSA{ServiceGetToken}
  - if the token \b +ALL is used in \b ServiceCreate to add a \e generic service handler the current
    token is not known. This function return the current token.
  .
 - \RNSA{ServiceIsTransaction}
  - if the token \b +ALL is used in \b ServiceCreate to add a \e generic service handler the current
    transaction-status is not known. This function return the transaction-status as boolean with
    \b true (with-transaction) or \b false (without-transaction).
  - \b with-transaction: the package was send with \RNSA{SendEND_AND_WAIT} or \RNSA{SendEND_AND_CALLBACK}
  - \b without-transaction: the package was send with \RNSA{SendEND}
  .
 - \RNSA{ReadBDY}
  - read and return the entire body as binary array. Use this array to apply a transformation to the body at all like
    encryption (example: \c aguard) or to save the body in a persistent storage for later use like transaction support
    (example: \c atrans)
  .
 - \RNSA{SendBDY}
  - send a binary array, as returned by \b ReadBDY, to the filter target.
  .
 .

<B>ONE-DIRECTIONAL FILTER</B>\n

A one-directional filter is a special form of a bi-directional filter and allow a \e data-flow from the \e left to the \e right.
This filter is well known from the \b unix shell to link different commands together: \verbatim
    command1 | command2 | command3
\endverbatim
A \libmsgque command pipeline is created with the special character \b @ instead of \b | :
\verbatim
    msgcmd1 @ msgcmd2 @ msgcmd3
\endverbatim
with every command have to use \libmsgque.
To define a \libmsgque filter create a service handle with \RNSA{ServiceCreate} or
\RNSA{ServiceProxy} for the both special token:
 - \b +FTR : required to act on filter data rows. Every filter input data is a list of filter data rows and every row is a list of filter data columns. Every row is send to the following filter-command as \b +FTR service request.
 - \b +EOF : required to act on End-Of-Filter data and is called after all \b +FTR data was send. Sometimes the filter data can not be served as \b +FTR data (example: sorting of the input rows need to read all rows before the data can be send to the next filter command) and the\b +EOF token is used to continue send \b +FTR data rows.
 .
and send every data item with \RNSA{SendEND_AND_WAIT}.

\}

**/

/// \}	  Mq-C-API

// end c++ save definition
END_C_DECLS

#endif /* MQ_MSGQUE_H */





