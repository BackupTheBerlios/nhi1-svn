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

#   define MQ_PRIVATE

/*
#if defined(DLL_EXPORT) && !defined(MQ_IGNORE_EXTERN)
#  define MQ_DDL_EXPORT __declspec(dllexport)
#else
#  define MQ_DDL_EXPORT
#endif
*/

#   include "mqconfig.h"

#   define MQ_PRIVATE_CONFIG_CONST

#   include "msgque.h"
#   include "debug.h"
#   include "msgque_private.h"


#   include <stdlib.h>
#if !defined(_MSC_VER)
#   include <unistd.h>
#endif
#   include <sys/types.h>
#   include <stdio.h>
#   include <ctype.h>
#   include <time.h>
#   include <string.h>


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
    /* 38 */ MQ_MESSAGE_END,
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
    /* MQ_MESSAGE_END */
	"END OF TEXT MESSAGE ARRAY"
};

#else
extern MQ_CST MqMessageText[MQ_MESSAGE_END];
#endif

/*****************************************************************************/
/*                                                                           */
/*                  msgque string to number convertion                       */
/*                                                                           */
/*****************************************************************************/

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
# if defined(HAVE_PTHREAD)
#  define MqThreadSelf() pthread_self()
#  define MqThreadGetTLS(k) pthread_getspecific(k)
#  define MqThreadSetTLS(k,v) pthread_setspecific(k,v)
#  define MqThreadSetTLSCheck(k,v) pthread_setspecific(k,v)
#  define MqThreadKeyType pthread_key_t
#  define MqThreadKeyNULL PTHREAD_KEYS_MAX
# else
#  define MqThreadSelf() GetCurrentThreadId()
#  define MqThreadGetTLS(k) TlsGetValue(k)
#  define MqThreadSetTLS(k,v) TlsSetValue(k,v)
#  define MqThreadSetTLSCheck(k,v) (TlsSetValue(k,v) == 0)
#  define MqThreadKeyType DWORD
#  define MqThreadKeyNULL TLS_OUT_OF_INDEXES
# endif
#endif // MQ_HAS_THREAD

END_C_DECLS

#endif // MAIN_H
