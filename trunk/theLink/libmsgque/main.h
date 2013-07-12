/**
 *  \file       theLink/libmsgque/main.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef MAIN_H
#   define MAIN_H

# if defined(_MSC_VER)
#   define _CRT_SECURE_NO_WARNINGS
# endif

#   include "mqconfig.h"

#   define MQ_PRIVATE
#   define MQ_PRIVATE_CONFIG_CONST

#   include "msgque.h"
#   include "debug.h"
#   include "msgque_private.h"


#   include <stdlib.h>
#if defined(MQ_IS_WIN32)
#   undef socklen_t
// socklen defined as "int" in "Ws2tcpip.h" at "sys_com.h" required for
// freeaddrinfo...getaddrinfo...gai_strerror
#   define  socklen_t int
#elif defined(__CYGWIN__)
#   include <sys/socket.h>
#else
#   include <unistd.h>
#endif
#   include <sys/types.h>
#   include <stdio.h>
#   include <ctype.h>
#   include <time.h>
#   include <string.h>

#include "sqlite3.h"

struct sockaddr;
struct sockaddr_in;

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                            msgque error codes                             */
/*                                                                           */
/*****************************************************************************/

#define MqMessageNum(n) (n+200)

/// \brief error-codes used by \libmsgque
enum MqMessageNumE {
    /*  0 */ MQ_ERROR_TIMEOUT,
    /*  1 */ MQ_ERROR_NOT_A_CLIENT,
    /*  2 */ MQ_ERROR_CAN_NOT_START_NON_PIPE_SERVER,
    /*  3 */ MQ_ERROR_CAN_NOT_START_CLIENT,
    /*  4 */ MQ_ERROR_CAN_NOT_START_SERVER,
    /*  5 */ MQ_ERROR_OPTION_REQUIRED,
    /*  6 */ MQ_ERROR_INVALID_HEADER,
    /*  7 */ MQ_ERROR_RETURN_WITHOUT_TRANSACTION,
    /*  8 */ MQ_ERROR_TOKEN_LENGTH,
    /*  9 */ MQ_ERROR_OPTION_WRONG,
    /* 10 */ MQ_ERROR_CAST_TO_NON_ATOM,
    /* 11 */ MQ_ERROR_SEND_END_WITHOUT_START,
    /* 12 */ MQ_ERROR_TYPE,
    /* 13 */ MQ_ERROR_REQUEST_ARGUMENTS,    
    /* 14 */ MQ_ERROR_CHECK_CLIENT,
    /* 15 */ MQ_ERROR_EXIT,
    /* 16 */ MQ_ERROR_CAN_NOT,
    /* 17 */ MQ_ERROR_WINSOCK,
    /* 18 */ MQ_ERROR_CREATE_CHILD,
    /* 19 */ MQ_ERROR_INVALID_OPTION,
    /* 20 */ MQ_ERROR_NO_FILTER,
    /* 21 */ MQ_ERROR_OPTION_ARG,
    /* 22 */ MQ_ERROR_OPTION_FORBIDDEN,
    /* 23 */ MQ_ERROR_WAIT_FOR_TOKEN,
    /* 24 */ MQ_ERROR_UNDO_FORBIDDEN,
    /* 25 */ MQ_ERROR_READ_HDL,
    /* 26 */ MQ_ERROR_NO_INIT,
    /* 27 */ MQ_ERROR_MASTER_SLAVE,
    /* 28 */ MQ_ERROR_ID_IN_USE,
    /* 29 */ MQ_ERROR_HANDSHAKE,
    /* 30 */ MQ_ERROR_SLAVE_CHILD_DELETE,
    /* 31 */ MQ_ERROR_ID_OUT_OF_RANGE,
    /* 32 */ MQ_ERROR_CONFIGURATION_REQUIRED, 
    /* 33 */ MQ_ERROR_MASTER_SLAVE_CLIENT,
    /* 34 */ MQ_ERROR_LINK_CREATE,
    /* 35 */ MQ_ERROR_CONNECTED,
    /* 36 */ MQ_ERROR_ITEM_IN_PACKAGE,
    /* 37 */ MQ_ERROR_NOT_SUPPORTED,
    /* 38 */ MQ_ERROR_START_ITEM_REQUIRED,
    /* 39 */ MQ_ERROR_TRANSACTION_REQUIRED,
    /* 40 */ MQ_ERROR_ID_NOT_FOUND,
    /* 41 */ MQ_ERROR_FEATURE_NOT_AVAILABLE,
    /* 42 */ MQ_ERROR_VALUE_INVALID,
    /* 43 */ MQ_ERROR_INVALID_SIZE,
    /* 44 */ MQ_ERROR_CALLBACK_REQUIRED,
    /* 45 */ MQ_MESSAGE_END,
};

#if defined(MQ_PRIVATE_IS_MAIN)

/// \brief error-messages used by \libmsgque
MQ_CST MqMessageText[MQ_MESSAGE_END+1] = {
    /* MQ_ERROR_TIMEOUT */			
	"TIMEOUT after '%lli' sec",
    /* MQ_ERROR_NOT_A_CLIENT */			
	"it is not allowed to use a server in a client context" ,
    /* MQ_ERROR_CAN_NOT_START_NON_PIPE_SERVER */		
	"can not start '%s' server using '@ server ...' syntax",
    /* MQ_ERROR_CAN_NOT_START_CLIENT */		
	"can not start PIPE client without '@ server ...' as argument",
    /* MQ_ERROR_CAN_NOT_START_SERVER */		
	"can not start server '%s'" ,
    /* MQ_ERROR_OPTION_REQUIRED */			
	"for a '%s' setup the option '%s' is required",
    /* MQ_ERROR_INVALID_HEADER */		
	"invalid header, expect <HDR+...>" ,
    /* MQ_ERROR_RETURN_WITHOUT_TRANSACTION */	
	"calling a service with return arguments without transaction, use MqSendEND_AND_WAIT instead of MqSendEND" ,
    /* MQ_ERROR_TOKEN_LENGTH */			
	"expect token as string of size 4 but got '%s'" ,
    /* MQ_ERROR_OPTION_WRONG */			
	"for a '%s' connection the option '%s' is wrong",
    /* MQ_ERROR_CAST_TO_NON_ATOM */		
	"it is not allowed to cast to a non atom (MQ_ATO) type" ,
    /* MQ_ERROR_SEND_END_WITHOUT_START */		
	"use first MqSendSTART and than MqSendEND" ,
    /* MQ_ERROR_TYPE */
	"invalid type, expect '%s' but got '%s'",
    /* MQ_ERROR_REQUEST_ARGUMENTS */
	"request more arguments than available",
    /* MQ_ERROR_CHECK_CLIENT */
	"unable to CHECK client connection, shutdown the server",
    /* MQ_ERROR_EXIT */
	"application EXIT request",
    /* MQ_ERROR_CAN_NOT */
	"can not '%s'",
    /* MQ_ERROR_WINSOCK */
	"can not '%s'-> ERR<%i>\ncan not find a usable WinSock DLL",
    /* MQ_ERROR_CREATE_CHILD */
	"[OKS] can not create CHILD",
    /* MQ_ERROR_INVALID_OPTION */
	"invalid [OPTION]... found:",
    /* MQ_ERROR_NO_FILTER */
       "no filter is available",
    /* MQ_ERROR_OPTION_ARG */
	"expect argument for option '%s'",
    /* MQ_ERROR_OPTION_FORBIDDEN */
	"for the '%s' setup the option '%s' is forbidden",
    /* MQ_ERROR_WAIT_FOR_TOKEN */
	"TIMEOUT: wait '%i' sec for token '%s' but got only token '%s'",
    /* MQ_ERROR_UNDO_FORBIDDEN */
	"maximum undo level reached",
    /* MQ_ERROR_READ_HDL */
	"the buffer handle '%i' is out of range",
    /* MQ_ERROR_NO_INIT */
	"unable to continue, the 'init' object is not available",
    /* MQ_ERROR_MASTER_SLAVE */
	"the '%s' have to be a 'parent' without 'child' objects",
    /* MQ_ERROR_ID_IN_USE */
	"'%s' identifer '%i' is already in use",
    /* MQ_ERROR_HANDSHAKE */
	"hand-shake-error",
    /* MQ_ERROR_SLAVE_CHILD_DELETE */
	"it is not allowed to delete a slave 'child' context",
    /* MQ_ERROR_ID_OUT_OF_RANGE */
	"'%s' identifer out of range (%i <= %i <= %i)",
    /* MQ_ERROR_CONFIGURATION_REQUIRED */
	"to use the '%s' feature the configuration '%s' is required",
    /* MQ_ERROR_MASTER_SLAVE_CLIENT */
	"the '%s' have to be a 'client' and a 'parent' without 'child' objects",
    /* MQ_ERROR_LINK_CREATE */
	"the 'client-server-link' was not created",
    /* MQ_ERROR_CONNECTED */
	"the '%s' object link is '%s' connected",
    /* MQ_ERROR_ITEM_IN_PACKAGE */
	"the '%s' item have to be the '%s' item in the data package",
    /* MQ_ERROR_NOT_SUPPORTED */
	"the feature is not supported in the current setup",
    /* MQ_ERROR_START_ITEM_REQUIRED */
	"the required item '%s' is not available in the data package",
    /* MQ_ERROR_TRANSACTION_REQUIRED */
	"found ReadT_START but no TRANSACTION is available",
    /* MQ_ERROR_ID_NOT_FOUND */
	"%s-id '%lld' not found",
    /* MQ_ERROR_FEATURE_NOT_AVAILABLE */
	"the feature '%s' is not available",
    /* MQ_ERROR_VALUE_INVALID */
	"the value '%s' is invalid for object '%s'",
    /* MQ_ERROR_INVALID_SIZE */
	"invalid '%s' size, expect '%d' but got '%d'",
    /* MQ_ERROR_CALLBACK_REQUIRED */
	"only use the function in a callback",
    /* MQ_MESSAGE_END */
	"END OF TEXT MESSAGE ARRAY",
};

#else
extern MQ_CST MqMessageText[MQ_MESSAGE_END];
#endif

/*****************************************************************************/
/*                                                                           */
/*                  msgque string to number convertion                       */
/*                                                                           */
/*****************************************************************************/

static mq_inline MQ_CST StringOrUnknown(MQ_CST str) {
  return str ? str : "unknown";
}

#define str2wid		mq_strtoll
#define str2int		strtol
#if defined(HAVE_STRTOD)
#  define str2dbl	strtod
#else
#  error function strtod is required
#endif
#if defined(HAVE_STRTOF)
#  define str2flt	strtof
#endif
#if defined(HAVE_STRTOUL)
#  define str2ptr	strtoul
#elif defined(HAVE_STRTOULL)
#  define str2ptr	strtoull
#endif

#if defined (MQ_HAS_THREAD)
# if defined(HAVE_PTHREAD) /* unix thread */
#  define MqThreadLocal __thread
#  define MqThreadSelf() pthread_self()
#  define MqThreadGetTLS(k) pthread_getspecific(k)
#  define MqThreadSetTLS(k,v) pthread_setspecific(k,v)
#  define MqThreadSetTLSCheck(k,v) (unlikely (pthread_setspecific(k,v) != 0))
#  define MqThreadKeyType pthread_key_t
#  define MqThreadType pthread_t
#  if defined(__CYGWIN__)
#    define MqThreadKeyNULL NULL
#  else
#    define MqThreadKeyNULL PTHREAD_KEYS_MAX
#  endif
#  define MqThreadKeyCreate(key) if (pthread_key_create(&key, NULL) != 0) { \
      MqPanicC(MQ_ERROR_PANIC,__func__,-1,"unable to 'pthread_key_create'"); \
    }
# else /* windows THREAD */
#  define MqThreadLocal __declspec(thread)
#  define MqThreadSelf() GetCurrentThreadId()
#  define MqThreadGetTLS(k) TlsGetValue(k)
#  define MqThreadSetTLS(k,v) TlsSetValue(k,v)
#  define MqThreadSetTLSCheck(k,v) (unlikely (TlsSetValue(k,v) == 0))
#  define MqThreadKeyType DWORD
#  define MqThreadType DWORD
#  define MqThreadKeyNULL TLS_OUT_OF_INDEXES
#  define MqThreadKeyCreate(key) if ((key = TlsAlloc()) == TLS_OUT_OF_INDEXES) { \
      MqPanicC(MQ_ERROR_PANIC,__func__,-1,"unable to 'TlsAlloc'"); \
    }
# endif
#else /* no THREAD */
#  define MqThreadLocal
#  define MqThreadKeyCreate(k)
#  define MqThreadGetTLS(k) k
#  define MqThreadSetTLS(k,v) k=v
#  define MqThreadSetTLSCheck(k,v) (unlikely ((k=v) == NULL))
#  define MqThreadKeyNULL NULL
#  define MqThreadKeyType void*
#  define MqThreadType void*
#  define MqThreadSelf() NULL
#endif // MQ_HAS_THREAD

/*****************************************************************************/
/*                                                                           */
/*                                misc                                       */
/*                                                                           */
/*****************************************************************************/

void pContextDeleteLOCK(struct MqS * const);

#define check_sqlite(E) if (unlikely((E) != SQLITE_OK))
#define check_NULL(E) if (unlikely((E) == NULL))
#define check_INT(E) if (unlikely((E) != 0))

#define U2INT(isBin,U) (likely(isBin) ? iBufU2INT(U) : str2int(U.C,NULL,16))

/*****************************************************************************/
/*                                                                           */
/*                                 config.h                                  */
/*                                                                           */
/*****************************************************************************/

#define buffersize_DEFAULT 4096
#define timeout_DEFAULT MQ_TIMEOUT10
#define storage_DEFAULT "#memdb#"

void pConfigSetParent ( struct MqS * const, struct MqS * const);
void pConfigSetMaster ( struct MqS * const, struct MqS * const, int);

/*****************************************************************************/
/*                                                                           */
/*                                 buffer.h                                  */
/*                                                                           */
/*****************************************************************************/

// can be ((*buf->cur.A).I)
static mq_inline MQ_INT iBufU2INT (
  union MqBufferU buf
) {
#if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
  MQ_ATO a;
  a.B[0] = (*buf.A).B[0];
  a.B[1] = (*buf.A).B[1];
  a.B[2] = (*buf.A).B[2];
  a.B[3] = (*buf.A).B[3];
  return a.I;
#else
  return (*buf.A).I;
#endif
}

// can be ((*buf->cur.A).T)
static mq_inline MQ_TRA iBufU2TRA (
  union MqBufferU buf
) {
#if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
  MQ_ATO a;
  memcpy(&a,buf.B,sizeof(MQ_TRA));
  return a.W;
#else
  return (*buf.A).T;
#endif
}

// can be (*in->cur.A)
static mq_inline MQ_ATO iBufU2ATO (
  union MqBufferU buf
) {
#if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
  MQ_ATO p;
  memcpy(&p,buf.B,sizeof(MQ_ATO));
  return p;
#else
  return *buf.A;
#endif
}

struct MqBufferS * pBufferCreateRef ( struct MqBufferS const * const);
void pBufferDeleteRef ( struct MqBufferS ** const);
void _pBufferNewSize ( register struct MqBufferS * const, MQ_CST const, MQ_SIZE const);
typedef enum MqErrorE ( *pBufferGetAF) ( struct MqBufferS * const, union MqBufferAtomU * const, enum MqTypeE const);
enum MqErrorE pBufferGetA1 ( struct MqBufferS * const, union MqBufferAtomU * const, enum MqTypeE const);
enum MqErrorE pBufferGetA2( struct MqBufferS * const, union MqBufferAtomU * const, enum MqTypeE const);
enum MqErrorE pBufferGetA4( struct MqBufferS * const, union MqBufferAtomU * const, enum MqTypeE const);
enum MqErrorE pBufferGetA8( struct MqBufferS * const, union MqBufferAtomU * const, enum MqTypeE const);
MQ_TRA pBufU2TRA ( union MqBufferU buf);
#define pBufferNewSize(buf, newSize) \
	if (unlikely(newSize > buf->size)) _pBufferNewSize(buf, __func__, newSize);
#define pBufferAddSize(buf, addSize) \
	{MQ_SIZE s=buf->cursize + addSize; pBufferNewSize(buf, s);}
#define MqBufferCreateStatic(NAME,SIZE) \
  static MQ_BINB data[SIZE+1]; \
  static struct MqBufferS tmp##NAME = { \
    MQ_MqBufferS_SIGNATURE,   /* signature */ \
    MQ_ERROR_PANIC,	      /* context   */ \
    data,		      /* data      */ \
    SIZE,		      /* size      */ \
    0,			      /* cursize   */ \
    0,			      /* numItems  */ \
    {NULL},		      /* cur       */ \
    { \
      MQ_ALLOC_STATIC,	      /* bits.alloc*/ \
      MQ_REF_LOCAL,	      /* bits.ref  */ \
      MQ_NO,		      /* user_2    */ \
      MQ_NO		      /* user_3    */ \
    }, \
    MQ_STRT		      /* type      */ \
  }; \
  struct MqBufferS *NAME = &tmp##NAME; \
  tmp##NAME.cur.B = &data[0];

/*****************************************************************************/
/*                                                                           */
/*                               generic_io.h                                */
/*                                                                           */
/*****************************************************************************/

#define GENERIC_SAVE_ERROR(generiC) ((generiC)&&(generiC)->error?(generiC)->error:MQ_ERROR_IGNORE)

struct GenericS {
  struct MqIoS * io;		///< link to 'io' object
  struct MqS * context;		///< my MqErrorS
  MQ_SOCK sock;                 ///< the socket
  char stringbuffer[20];	///< context temporar memory
};

enum MqErrorE pGenericCreate ( struct MqIoS * const, struct GenericS ** const);
void pGenericDelete ( struct GenericS ** const) __attribute__((nonnull));
enum MqErrorE GenericCreateSocket ( struct GenericS * const, int const, int const, int);
enum MqErrorE GenericServer ( struct GenericS * const, struct sockaddr * const, socklen_t const);
enum MqErrorE GenericConnect ( struct GenericS * const, struct sockaddr * const, socklen_t const, MQ_TIME_T);
enum MqErrorE GenericBind ( struct GenericS const * const, struct sockaddr*, socklen_t);
enum MqErrorE GenericGetSockName ( struct GenericS const * const, struct sockaddr*, socklen_t*);

/*****************************************************************************/
/*                                                                           */
/*                                   log.h                                   */
/*                                                                           */
/*****************************************************************************/

void pLog ( FILE*, MQ_CST const, ...) __attribute__ ((format (printf, 2, 3)));
void pLogHDR ( struct MqS * const, MQ_CST, const MQ_INT, struct MqBufferS * const);
void pLogBDY ( struct MqS * const, MQ_CST, const MQ_INT, struct MqBufferS * const);
void pLogMqBuffer ( struct MqS * const, MQ_CST, MQ_INT, struct MqBufferS*);
void pLogHEX ( struct MqS const * const, MQ_CST const, MQ_BINB const * , MQ_SIZE const);
MQ_STR pLogAscii ( MQ_STR, MQ_CST, MQ_SIZE);

/*****************************************************************************/
/*                                                                           */
/*                                  send.h                                   */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE pSendCreate ( struct MqS * const, struct MqSendS ** const) __attribute__((nonnull));
void pSendDelete ( struct MqSendS **) __attribute__((nonnull));
enum MqErrorE pSendSYSTEM ( struct MqS * const, MQ_TOK const);
enum MqErrorE pSendEND ( struct MqS * const, MQ_TOK const, const MQ_HDL);
enum MqErrorE pSendSYSTEM_RETR ( struct MqS * const);
void pSendBDY ( struct MqS * const, MQ_BINB const * const, MQ_SIZE const, enum MqHandShakeE const, MQ_SIZE);
enum MqErrorE pSendT (struct MqS * const context, const MQ_TRA);

/*****************************************************************************/
/*                                                                           */
/*                                 token.h                                   */
/*                                                                           */
/*****************************************************************************/

struct pTokenS* pTokenCreate (struct MqS * const);
void pTokenDelete ( register struct pTokenS ** const) __attribute__((nonnull));
enum MqErrorE pTokenCheckSystem ( struct pTokenS const * const);
enum MqErrorE pTokenInvoke ( struct pTokenS const * const);
mq_bool pTokenCheck ( struct pTokenS const * const, MQ_CST const);
enum MqErrorE pTokenAddHdl ( struct pTokenS const * const, MQ_CST const, struct MqCallbackS);
enum MqErrorE pTokenDelHdl ( struct pTokenS const * const, MQ_CST const);
void pTokenMark ( struct MqS * const, MqMarkF);

// can be (*((MQ_INT const *)(bin)))
static mq_inline MQ_INT pByte2INT (
  //MQ_BINB const * const bin
  MQ_CST const bin
) {
#if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
  MQ_INT i;
  memcpy(&i,bin,sizeof(MQ_INT));
  return i;
#else
  return (*((MQ_INT const *)(bin)));
#endif
}

#define TOKEN_LEN (HDR_TOK_LEN+1)

struct pTokenS {
  struct MqS * context;		///< link to msgque object
  MQ_STRB current[TOKEN_LEN];	///< kind the the current action
  struct pTokenSpaceS *loc;     ///< the local handler
};

static mq_inline MQ_CST pTokenGetCurrent (
  struct pTokenS const * const token
)
{
  return token->current;
}

static mq_inline void pTokenSetCurrent (
  struct pTokenS * const token,
  MQ_CST const str
)
{
  memcpy (token->current, str, HDR_TOK_LEN);
}

/*****************************************************************************/
/*                                                                           */
/*                                 buferL.h                                  */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE pBufferLSplitAlfa (struct MqBufferLS **, struct MqBufferLS **);
void pBufferLSetError (struct MqBufferLS * const, struct MqS * const);
struct MqBufferLS * pBufferLExtractOptions (struct MqBufferLS * const);

/*****************************************************************************/
/*                                                                           */
/*                                 error.h                                   */
/*                                                                           */
/*****************************************************************************/

void pErrorSetup ( struct MqS * const);
void pErrorCleanup ( struct MqS * const);
enum MqErrorE pErrorSet ( struct MqS * const, MQ_INT const, MQ_CST const, MQ_BOL const);
void pErrorAppendC ( struct MqS * const, MQ_CST const);
void pErrorReport( struct MqS * const);
void pErrorSync ( struct MqS * const, struct MqS * const);
enum MqErrorE MQ_DECL pErrorSetExitWithCheckP ( struct MqS * const, MQ_CST const);
#define pErrorSetExitWithCheck(ctx) pErrorSetExitWithCheckP(ctx,__func__)
void pErrorReset (struct MqS * const);

#define MqErrorDb(item) \
    MqErrorSGenV(MQ_CONTEXT_S,__func__,MQ_ERROR,MqMessageNum(item),MqMessageText[item])

#define MqErrorDb2(context,item) \
    MqErrorSGenV(context,__func__,MQ_ERROR,MqMessageNum(item),MqMessageText[item])

#define MqErrorDbV(item, ...) \
    MqErrorSGenV(MQ_CONTEXT_S,__func__,MQ_ERROR,MqMessageNum(item),MqMessageText[item], __VA_ARGS__)

#define MqErrorDbV2(context,item, ...) \
    MqErrorSGenV(context,__func__,MQ_ERROR,MqMessageNum(item),MqMessageText[item], __VA_ARGS__)

#define MqErrorDbFactoryMsg(context,num,msg) \
    MqErrorSGenV(context,__func__,MQ_ERROR,MqMessageNum(num+100),"%s",msg)

#define MqErrorDbFactoryNum(context,num) \
    MqErrorSGenV(context,__func__,MQ_ERROR,MqMessageNum(num+100),"%s",sFactoryReturnMsg(num))

#define MqErrorDbSql(context,db) \
    MqErrorSGenV(context,__func__,MQ_ERROR,MqMessageNum(sqlite3_extended_errcode(db)+200),"%s",sqlite3_errmsg(db))

#define MqErrorDbSql2(context,db,msg) \
    MqErrorSGenV(context,__func__,MQ_ERROR,MqMessageNum(sqlite3_extended_errcode(db)+200),"%s",msg)

#define MqErrorDbSqlMsg(context,num,msg) \
    MqErrorSGenV(context,__func__,MQ_ERROR,MqMessageNum(num+200),"%s",msg)

/*****************************************************************************/
/*                                                                           */
/*                                  sys.h                                    */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE SysFork (struct MqS *const, struct MqIdS *);
enum MqErrorE SysIgnorSIGCHLD (struct MqS * const);
enum MqErrorE SysAllowSIGCHLD (struct MqS * const);
enum MqErrorE SysUnlink (struct MqS * const, const MQ_STR);

#define MqSysFork() \
  (*MqLal.SysFork)()
#define MqSysAbort() \
  (*MqLal.SysAbort)()
#define MqSysServerSpawn(context, argv, name, idP) \
  (*MqLal.SysServerSpawn)(context, argv, name, idP)
#define MqSysServerThread(context, factory, argvP, alfaP, name, state, idP) \
  (*MqLal.SysServerThread)(context, factory, argvP, alfaP, name, state, idP)
#define MqSysServerFork(context, factory, argvP, alfaP, name, idP) \
  (*MqLal.SysServerFork)(context, factory, argvP, alfaP, name, idP)
#define MqSysDaemonize(context,pidfile) \
  (*MqLal.SysDaemonize)(context,pidfile)
#define MqSysIgnorSIGCHLD(context) \
  (*MqLal.SysIgnorSIGCHLD)(context)
#define MqSysAllowSIGCHLD(context) \
  (*MqLal.SysAllowSIGCHLD)(context)

/*****************************************************************************/
/*                                                                           */
/*                                 cache.h                                   */
/*                                                                           */
/*****************************************************************************/

typedef MQ_PTR ( *CacheCF) ( MQ_PTR const);
typedef void ( *CacheDF) ( MQ_PTR * const);

void pCacheCreate ( CacheCF, CacheDF, MQ_PTR, struct MqCacheS ** const);
void pCacheDelete ( struct MqCacheS ** const) __attribute__((nonnull));

/*****************************************************************************/
/*                                                                           */
/*                              cache_1                                      */
/*                                                                           */
/*****************************************************************************/

MQ_PTR
 pCachePop (
  register struct MqCacheS * const top
);

void
 pCachePush (
  register struct MqCacheS * const top,
  MQ_PTR data
);

typedef void ( MQ_DECL
  *MqCacheMarkF
) (
  MQ_PTR data,
  MqMarkF markF
);

void pCacheMark (
  register struct MqCacheS * const top,
  MqCacheMarkF cacheMarkF,
  MqMarkF markF
);

/*****************************************************************************/
/*                                                                           */
/*                                 event.h                                   */
/*                                                                           */
/*****************************************************************************/

struct MqEventS;

void pEventAdd ( MQ_CST, struct MqS * const , MQ_SOCK * const);
void pEventDel ( MQ_CST, struct MqS const * const );
void pEventGetList( struct MqS ***, MQ_SIZE * );

enum MqErrorE pEventCheck (struct MqS*const, struct MqEventS*, enum MqIoSelectE const, struct timeval*const);
enum MqErrorE pEventStart (struct MqS*const, struct MqEventS*, struct timeval const*const);
enum MqErrorE pEventCreate (struct MqS*const, struct MqEventS**const);
void pEventDelete ( struct MqS const * const );
struct MqS const * pEventSocketFind (struct MqS const * const, MQ_SOCK );

#if _DEBUG
void pEventLog( struct MqS const * const , struct MqEventS *, MQ_CST const);
#endif

#define DLogEvent(ev) printf("%s(%s:%d) -> pid<%i>, id<%li>, event<%p>, ref<%i>\n", __func__, __FILE__, __LINE__,\
      mq_getpid(), pthread_self(), ev, (ev!=NULL?ev->refCount:-1));fflush(stdout);

/*****************************************************************************/
/*                                                                           */
/*                                 mq_io.h                                   */
/*                                                                           */
/*****************************************************************************/

#if defined(MQ_IS_POSIX)
// used for "fd_set"
#   include <sys/select.h>
#endif

struct MqIoS {
  struct MqS * context;             ///< link to the 'msgque' object
  union {
#if defined(MQ_HAVE_UDS)
    struct UdsS *   udsSP;          ///< UDS object pointer
#endif
    struct TcpS *   tcpSP;          ///< TCP object pointer
    struct PipeS *  pipeSP;         ///< PIPE object pointer
  } iocom;                          ///< link to 'iocom' object
  fd_set fdset;                     ///< select need this
  MQ_SOCK *sockP;                   ///< pointer to the current socket
  struct MqIdS id;		    ///< (pid_t/mqthread_t) id of the server in "pipe" mode
  struct MqEventS * event;          ///< link to the according Event structure
  struct MqIoConfigS * config;      ///< "Io" part of the global configuration data
};

enum MqErrorE pIoCreate ( struct MqS * const, struct MqIoS ** const) __attribute__((nonnull));
void pIoDelete ( struct MqIoS ** const) __attribute__((nonnull));
void pIoShutdown ( struct MqIoS * const);
void pIoCloseSocket (MQ_CST, struct MqIoS * const);
enum MqErrorE pIoRead ( struct MqIoS const * const, register struct MqBufferS * const, MQ_SIZE const);
enum MqErrorE pIoSend ( struct MqIoS const * const, struct MqBufferS const * const);
enum MqErrorE pIoConnect ( struct MqIoS * const);
enum MqErrorE pIoSelect ( struct MqIoS * const, enum MqIoSelectE const, struct timeval * const);
enum MqErrorE pIoSelectAll ( struct MqIoS * const, enum MqIoSelectE const, struct timeval * const);
enum MqErrorE pIoSelectStart ( struct MqS * const, struct timeval const * const);
enum MqErrorE pReadInsertRmtTransId  ( struct MqS * const);

/*****************************************************************************/
/*                                                                           */
/*                                 io_misc                                   */
/*                                                                           */
/*****************************************************************************/

/// \brief howto start a new server
enum IoStartServerE {
  MQ_START_SERVER_AS_PIPE,
#if defined(HAVE_FORK)
  MQ_START_SERVER_AS_FORK,
  MQ_START_SERVER_AS_INLINE_FORK,
#endif
#if defined(MQ_HAS_THREAD)
  MQ_START_SERVER_AS_THREAD,
#endif
  MQ_START_SERVER_AS_SPAWN,
};

/// \brief start the PIPE Server 
///
/// with \e fMain and \e name we have the possibility to select
/// howto create a process
enum MqErrorE pIoStartServer (
  struct MqIoS * const io,
  enum IoStartServerE startType,
  MQ_SOCK * sockP,
  struct MqIdS * idP
);

/// \brief helper to convert #IoStartServerE into a human readable string
MQ_CST pIoLogStartType (
  enum IoStartServerE startType
);

MQ_STR pIoLogId (
  MQ_STR buf,
  MQ_INT size,
  struct MqIdS id
);

/// \brief helper to covert the #MqIoComE into a human readable string
MQ_CST pIoLogCom (
  enum MqIoComE com
);

enum MqErrorE pIoEventAdd (MQ_CST, struct MqIoS * const, MQ_SOCK *);

struct MqS const * pIoGetMsgqueFromSocket (
  struct MqIoS * const	io,
  MQ_SOCK sock
);

MQ_BOL pIoIsRemote (struct MqIoS * const);

static mq_inline MQ_BOL pIoCheck (
  struct MqIoS * const io
) {
  return (io && io->sockP ? (*(io->sockP) >= 0) : MQ_NO);
};

static mq_inline MQ_TIME_T pIoGetTimeout (
  struct MqIoS * const io
)
{
  return io->config->timeout;
};

#if _DEBUG
void
pIoLog (
  struct MqIoS * const  io,
  MQ_CST proc
);
#endif

/*****************************************************************************/
/*                                                                           */
/*                                 read.h                                    */
/*                                                                           */
/*****************************************************************************/

/// \brief special data needed to save the current-status for a LST object
struct ReadSaveS {
  struct ReadSaveS * save;	    ///< this is needed for recursion
  struct MqBufferS bdy;		    ///< used for BDY data (dynamic)
};

struct MqReadS {
  struct MqS * context;		    ///< link to the msgque object
  struct MqBufferS * hdrorig;	    ///< in a context->switch, poit to the original header data, need in "MqReadDUMP"
  struct MqBufferS * hdr;	    ///< used for HDR data (static)
  struct MqBufferS * bdy;	    ///< buffer in duty, will be "readBuf" or "tranBuf"
  struct MqBufferS * cur;	    ///< used as reference on \e bdy with the current data
  struct MqBufferS * readBuf;	    ///< read socket buffer, will be used for socket io
  struct MqBufferS * tranBuf;	    ///< transaction buffer, will be mapped into database
  enum MqHandShakeE handShake;	    ///< what kind of call is it?
  MQ_INT returnNum;		    ///< Return-Number
  struct MqCacheS * saveCache;	    ///< cache of <TT>struct ReadSaveS</TT> data
  struct ReadSaveS * save;	    ///< need for List objects
  enum MqTypeE type;		    ///< type of the item stored into the data-segment (InitialSet)
  MQ_BOL canUndo;		    ///< is an MqReadUndo allowed ?
  MQ_TRA transLId;		    ///< transaction-id (rowid from readTrans table) used for persistent-transaction
  MQ_TRA rmtTransId;		    ///< remote transaction-id (rowid from the remote readTrans table)
				    ///< if ">0LL" -> the value to use or "OLL" -> get value from the database
};

struct MqReadS* pReadCreate ( struct MqS * const);
void pReadDelete ( struct MqReadS **) __attribute__((nonnull));
enum MqErrorE pReadHDR (MQ_PTR, struct MqS**);
enum MqErrorE pReadTRA (MQ_PTR, struct MqS**);
void pReadSetType( struct MqS * const, MQ_BOL const);
void pReadL_CLEANUP (register struct MqS * const); 
MQ_TRA pReadGetTransId ( struct MqS * const);
enum MqErrorE pReadCreateTransId  ( register struct MqS * const);
enum MqErrorE pReadDeleteTransId  ( register struct MqS * const);
void pReadSetReturnNum ( struct MqS const * const, MQ_INT);
enum MqErrorE pReadDeleteTrans ( struct MqS * const);
enum MqErrorE pReadWord ( struct MqS * const, struct MqBufferS * const, register struct MqBufferS * const);
void pReadBDY ( struct MqS * const, MQ_BIN* const, MQ_SIZE* const, enum MqHandShakeE* const, MQ_SIZE* const) __attribute__((nonnull(1)));
enum MqErrorE pReadInsert ( register struct MqS*, MQ_WID*);
enum MqErrorE pReadT ( struct MqS * const, MQ_TRA * const);
void pReadLog ( register struct MqS const * const, MQ_CST const);

static mq_inline enum MqHandShakeE
pReadGetHandShake (
  struct MqS const * const context
)
{
  return context->link.read->handShake;
}

static mq_inline void
pReadSetHandShake (
  struct MqS const * const context,
  enum MqHandShakeE hs
)
{
  context->link.read->handShake = hs;
}

static mq_inline enum MqHandShakeE
pReadSwapHandShake (
  struct MqS const * const context,
  enum MqHandShakeE hs
)
{
  struct MqReadS * const read = context->link.read;
  enum MqHandShakeE const ret = read->handShake;
  read->handShake = hs;
  return ret;
}

/*****************************************************************************/
/*                                                                           */
/*                                 swap.h                                    */
/*                                                                           */
/*****************************************************************************/

void pSwap4 ( register MQ_INT*);
void pSwapBDY ( register MQ_BIN);

/*****************************************************************************/
/*                                                                           */
/*                                tcp_io.h                                   */
/*                                                                           */
/*****************************************************************************/

#define TCP_SAVE_ERROR(tcp) ((tcp)?IO_SAVE_ERROR((tcp)->io):NULL)

enum MqErrorE TcpCreate ( struct MqIoS * const, struct TcpS ** const);
void TcpDelete ( struct TcpS ** const) __attribute__((nonnull));
enum MqErrorE TcpServer ( register struct TcpS * const);
enum MqErrorE TcpConnect ( register struct TcpS * const);

/*****************************************************************************/
/*                                                                           */
/*                               pipe_io.h                                   */
/*                                                                           */
/*****************************************************************************/

#define PIPE_SAVE_ERROR(pipe) ((pipe)?IO_SAVE_ERROR((pipe)->io):NULL)

enum MqErrorE PipeCreate ( struct MqIoS * const, struct PipeS ** const);
void PipeDelete ( struct PipeS ** const pipeP) __attribute__((nonnull)); 
enum MqErrorE PipeServer ( struct PipeS * const);
enum MqErrorE PipeConnect ( struct PipeS * const);
MQ_PTR PipeGetOpt ( struct PipeS const * const, int const);
MQ_SOCK* PipeGetServerSocket ( struct PipeS * const);

/*****************************************************************************/
/*                                                                           */
/*                                uds_io.h                                   */
/*                                                                           */
/*****************************************************************************/

#define UDS_SAVE_ERROR(uds) ((uds)?IO_SAVE_ERROR((uds)->io):NULL)

#if defined(MQ_HAVE_UDS)

enum MqErrorE UdsCreate ( struct MqIoS * const, struct UdsS ** const);
void          UdsDelete ( struct UdsS ** const) __attribute__((nonnull));
enum MqErrorE UdsServer ( register struct UdsS * const);
enum MqErrorE UdsConnect ( register struct UdsS * const);

#else

#define UdsCreate(io,udsPtr) MQ_ERROR
#define UdsDelete(udsP)
#define UdsServer(uds) MQ_ERROR
#define UdsConnect(uds) MQ_ERROR

#endif

/*****************************************************************************/
/*                                                                           */
/*                                slave.h                                    */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE pSlaveCreate (struct MqS * const, struct MqLinkSlaveS ** const);
void pSlaveDelete (struct MqLinkSlaveS ** const);
void pSlaveShutdown (struct MqLinkSlaveS * const);

/*****************************************************************************/
/*                                                                           */
/*                                service.h                                  */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE pServiceStart (register struct MqS * const, EventReadF const, MQ_PTR);

/*****************************************************************************/
/*                                                                           */
/*                                factory.h                                  */
/*                                                                           */
/*****************************************************************************/

void pFactoryMark ( struct MqS * const, MqMarkF);
void pFactoryCtxItemSet ( struct MqS * const, struct MqFactoryS * const);
enum MqErrorE MqFactoryInvoke ( struct MqS * const, enum MqFactoryE, struct MqFactoryS*, struct MqS **);

/*****************************************************************************/
/*                                                                           */
/*                                link.h                                     */
/*                                                                           */
/*****************************************************************************/

struct pChildS {
    struct pChildS * left   ;
    struct MqS     * context;
    struct pChildS * right  ;
};

void pMqShutdown (MQ_CST, struct MqS * const);
void pLinkDisConnect ( struct MqS * const);
int pMqCheckOpt ( register struct MqBufferS * const);

/*****************************************************************************/
/*                                                                           */
/*                                trans.h                                    */
/*                                                                           */
/*****************************************************************************/

/// \brief the status of a transaction used in the \e status member of the \e MqTransItemS object.
enum MqTransE {
  MQ_TRANS_START,               ///< start of transaction
  MQ_TRANS_END,                 ///< end of transaction
  MQ_TRANS_CANCEL               ///< transaction was canceled
};

enum MqErrorE pTransCreate (struct MqS * const, struct MqTransS ** const );
void pTransDelete (struct MqTransS ** const);
MQ_HDL pTransPop ( struct MqTransS const * const , struct MqCallbackS);
void pTransPush ( struct MqTransS const * const, MQ_HDL) __attribute__((nonnull));
int pTransCheckStart ( struct MqTransS const * const, const MQ_HDL);
MQ_HDL pTransGetLast ( struct MqTransS const * const, const MQ_HDL);
struct MqReadS * pTransGetResult ( struct MqTransS const * const, const MQ_HDL);
enum MqTransE pTransGetStatus ( struct MqTransS const * const, const MQ_HDL);
enum MqHandShakeE pTransGetHandShake ( struct MqTransS const * const, const MQ_HDL);
enum MqErrorE pTransSetResult ( struct MqTransS const * const, enum MqTransE const, struct MqReadS * const);

/*****************************************************************************/
/*                                                                           */
/*                                sql.h                                      */
/*                                                                           */
/*****************************************************************************/

struct MqSqlS {
  MQ_CST  storageFile;		      ///< main directory used for database files
  sqlite3 *db;			      ///< sqlite database connection handle
  sqlite3_stmt *pSqlInsertSendTrans;  ///< prepared sql statement
  sqlite3_stmt *sendSelect;	      ///< prepared sql statement
  sqlite3_stmt *sendDelete;	      ///< prepared sql statement
  sqlite3_stmt *readInsert;	      ///< prepared sql statement
  sqlite3_stmt *pSqlSelectReadTrans;  ///< prepared sql statement
  sqlite3_stmt *readSelect2;	      ///< prepared sql statement
  sqlite3_stmt *readDelete;	      ///< prepared sql statement
  sqlite3_stmt *MqStorageSelect1;     ///< prepared sql statement
  sqlite3_stmt *MqStorageSelect2;     ///< prepared sql statement
  sqlite3_stmt *MqStorageCount;	      ///< prepared sql statement
};

enum MqErrorE pSqlInsertSendTrans ( struct MqS * const, MQ_TOK const, MQ_BUF, MQ_TRA*);
enum MqErrorE pSqlSelectSendTrans ( struct MqS * const, MQ_TRA, MQ_BUF);
enum MqErrorE pSqlDeleteSendTrans ( struct MqS * const, MQ_TRA, MQ_TRA*);
enum MqErrorE pSqlInsertReadTrans ( struct MqS * const, MQ_TRA const, MQ_TRA const, MQ_BUF const, MQ_BUF const, MQ_TRA*);
enum MqErrorE pSqlSelectReadTrans ( struct MqS * const);
enum MqErrorE pSqlDeleteReadTrans ( struct MqS * const, MQ_TRA, MQ_TRA*, MQ_TRA*);
enum MqErrorE pSqlCreate ( struct MqS * const, struct MqSqlS ** const);
void pSqlDelete (struct MqSqlS **);

END_C_DECLS

#endif // MAIN_H
