/**
 *  \file       theLink/libmsgque/msgque_private.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef MQ_PRIVATE_H
#define MQ_PRIVATE_H

#if defined(MQ_HAS_THREAD)
# if defined(HAVE_PTHREAD)
#  include <pthread.h>
#  define mqthread_t pthread_t
# elif defined(MQ_IS_WIN32)
#  define mqthread_t unsigned long
# endif
#endif /* MQ_HAS_THREAD */

#include <sys/types.h>

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                            msgque/definition                              */
/*                                                                           */
/*****************************************************************************/

struct UdsS;
struct TcpS;
struct PipeS;
struct GenericS;
struct MqCacheS;
struct MqTransItemS;

#define MQ_IS_SERVER(msgque) (msgque->setup.isServer == MQ_YES)
#define MQ_IS_SERVER_PARENT(msgque) (MQ_IS_SERVER(msgque) && MQ_IS_PARENT(msgque))
#define MQ_IS_CLIENT(msgque) (msgque->setup.isServer == MQ_NO)
#define MQ_IS_CLIENT_PARENT(msgque) (MQ_IS_CLIENT(msgque) && MQ_IS_PARENT(msgque))
#define MQ_IS_CHILD(msgque)  (msgque->config.parent != NULL)
#define MQ_IS_SLAVE(msgque)  (msgque->config.master != NULL)
#define MQ_IS_PARENT(msgque) (msgque->config.parent == NULL)

#define MQ_STRING_TYPE(isString) (unlikely(isString) ? MQ_STRT : MQ_BINT)

/// \brief get a \e MqErrorS object from a \e MgS in a \e save manner
#define MQ_SAVE_ERROR(msgque) ((msgque)&&(msgque)->error?(msgque)->error:MQ_ERROR_IGNORE)

/// \brief separator between Message-Queue-Package-Data
#define BUFFER_CHAR         '\0'
#define BUFFER_CHAR_S       "\0"

enum MqIdSE {
  MQ_ID_UNUSED  = 0,
  MQ_ID_PROCESS = 1,
#if defined(MQ_HAS_THREAD)
  MQ_ID_THREAD  = 2
#endif
};

union MqIdSU {
  pid_t	process;	
#if defined(MQ_HAS_THREAD)
  mqthread_t	thread;
#endif
};

/// \brief data type for process/thread identification
///  
/// The values of \e pid_t and \e mqthread_t have to fit into this storage
struct MqIdS {
  enum MqIdSE type;
  union MqIdSU val;
};

/// \brief 'select' type for reading or writing
enum MqIoSelectE {
  MQ_SELECT_RECV = (1 << 0),    ///< 'select' for reading
  MQ_SELECT_SEND = (1 << 1),	///< 'select' for writing
} ;

/// \brief prototype for an Event-Callback function
typedef enum MqErrorE (
  *EventCreateF
) (
  struct MqS * const
);

/*****************************************************************************/
/*                                                                           */
/*                          protocol reference                               */
/*                                                                           */
/*****************************************************************************/

/**
\defgroup protocol INTERNAL: Package - Protocol - Reference
\{
Every data-package send from a client to the server and back is a combination of
a \b fix size HEADER and a \b variable size BODY.

The fix size HEADER (defined in \ref HdrS) was defined as:

\verbatim
HDR+CONTEXT+BODYSIZE+TOKEN+TRANSACTION+RETURNCODE+
\endverbatim

If "BODYSIZE > 0" the BODY_ITEM_HEADER (defined in \ref BdyS) is appended to HEADER:

\verbatim
BDY+BODY_ITEM_SIZE+BODY_ITEM_LIST
\endverbatim

with BODY_ITEM_SIZE as the number of items in the BODY_ITEM_LIST.

If "BODY_ITEM_SIZE > 0" the BODY_ITEM_SIZE * BODY_DATA_HEADER are appended to BODY_ITEM_HEADER

\verbatim
BODY_DATA_TYPE+BODY_DATA_SIZE+BODY_DATA
\endverbatim

with BODY_DATA_SIZE as the number of bytes in BODY_DATA.

If RETURNCODE == \e MQ_RETURN_ERROR (defined in \e MqReturnE) the \e last BODYITEM is defined 
as RETURNITEM with a RETURNITEM_HEADER:

\verbatim
RETURN_NUMBER+RETURN_MESSAGE_SIZE+...data...
\endverbatim

*/

/// \brief max size of a native handle (Type: #MQ_INT - 4 byte)
#define HDR_HDL_LEN	4
/// \brief max size of an hex integer in characters (Type: #MQ_INT)
#define HDR_INT_LEN	8
/// \brief max size of an pToken (Type: char[4])
#define HDR_TOK_LEN	4
/// \brief size of the return code (N)othing, (O)k, (W)arning, (E)rror (Type: char)
#define HDR_RET_LEN	1

/*
 *	Header-Prefix
 */

#define HDR_CtxId_S	    (3					+ 1)        ///< 4  : "HDR" string[3]
#define	HDR_BdySize_S	    (HDR_CtxId_S	+ HDR_INT_LEN	+ 1)        ///< 13 : "CONTEXT" integer
#define HDR_Token_S	    (HDR_BdySize_S	+ HDR_INT_LEN	+ 1)        ///< 22 : "BODYSIZE" integer
#define HDR_Trans_S	    (HDR_Token_S	+ HDR_TOK_LEN	+ 1)        ///< 27 : "TOKEN" string[4]
#define HDR_Code_S	    (HDR_Trans_S	+ HDR_HDL_LEN	+ 1)        ///< 32 : "TRANSACTION" pointer
#define HDR_SIZE	    (HDR_Code_S		+ HDR_RET_LEN	+ 1)        ///< 34 : "RETURNCODE" char

/*
 *	Body-Prefix
 */

#define BDY_NumItems_S	    (3				    + 1)    ///< 4  : "BDY" string[3]
#define BDY_SIZE	    (BDY_NumItems_S + HDR_INT_LEN   + 1)    ///< 13 : "BODY_ITEM_SIZE" integer

/*
 *      Return-Header (RET)
 */

#define RET_NumItems_S     (0              + HDR_INT_LEN   + 1)	    ///< 9  : "RETURN_NUMBER" integer
#define RET_SIZE           (RET_NumItems_S + HDR_INT_LEN   + 1)	    ///< 18 : "RETURN_MASSAGE_SIZE" integer

/*
 *      Body-Object-Header (BOH)
 */

/// \brief the HEADER size of an BODYITEM

//  Definition:                 TYP + CHAR + LENGTH      + CHAR 
#   define BUFFER_P2_PRENUM    (1   + 1    + HDR_INT_LEN + 1   )
 
//  Definition:                 TYP + CHAR  
#   define BUFFER_P2_NATIVE    (1   + 1    )

/*
 *	Misc stuff
 */ 

#define validateHDR(buf) \
    printf("VALIDATE(HDR) : %c%c%c %c ctxId %c size %c token %c transptr %c\n", \
	buf->data[0], buf->data[1], buf->data[2], \
	buf->data[HDR_CtxId_S - 1], \
	buf->data[HDR_BdySize_S - 1], buf->data[HDR_Token_S - 1], \
	buf->data[HDR_Trans_S - 1], buf->data[HDR_SIZE - 1])


/*****************************************************************************/
/*                                                                           */
/*                                msgque/packed                              */
/*                                                                           */
/*****************************************************************************/

#pragma pack(1)

/// \brief \e integer representation
union HdrIU {
    MQ_INT	B		; ///< \e binary representation of a integer
    MQ_STRB	S[HDR_INT_LEN]	; ///< \e string representation of a integer
};

/// \brief The protocol HEADER mapped to a \e struct
struct HdrS {
    MQ_STRB	    ID1	    ;   ///< <TT>size: 1 .............. -> total: 1 .. -> character .. -> "H"</TT>
    MQ_STRB	    ID2	    ;   ///< <TT>size: 1 .............. -> total: 2 .. -> character .. -> "D"</TT>
    MQ_STRB	    ID3	    ;   ///< <TT>size: 1 .............. -> total: 3 .. -> character .. -> "R"</TT>
    MQ_STRB	    charI   ;   ///< <TT>size: 1 .............. -> total: 4 .. -> character .. -> "+"</TT>
    union HdrIU	    ctxId   ;   ///< <TT>size: HDR_INT_LEN=8 .. -> total: 12 . -> integer .... -> "CONTEXT"</TT>
    MQ_STRB	    charS   ;   ///< <TT>size: 1 .............. -> total: 13 . -> character .. -> "+"</TT>
    union HdrIU	    size    ;   ///< <TT>size: HDR_INT_LEN=8 .. -> total: 21 . -> integer .... -> "BODYSIZE"</TT>
    MQ_STRB	    charO   ;   ///< <TT>size: 1 .............. -> total: 22 . -> character .. -> "+"</TT>
    MQ_STRB	    tok[4]  ;   ///< <TT>size: 4 .............. -> total: 26 . -> string[4] .. -> "TOKEN" </TT>
    MQ_STRB	    charT   ;   ///< <TT>size: 1 .............. -> total: 27 . -> character .. -> "+"</TT>
    MQ_HDL	    trans   ;   ///< <TT>size: 4 .............. -> total: 31 . -> handle ..... -> "TRANSACTION"</TT>
    MQ_STRB	    charC   ;   ///< <TT>size: 1 .............. -> total: 32 . -> character .. -> "+"</TT>
    MQ_STRB	    code    ;   ///< <TT>size: 1 .............. -> total: 33 . -> character .. -> "RETURNCODE"</TT>
    MQ_STRB	    charE   ;   ///< <TT>size: 1 .............. -> total: 34 . -> character .. -> "+"</TT>
};

/// \brief The protocol HEADER-BODY part mapped to a \e struct
struct BdyS {
    MQ_STRB	    ID[3]   ;   ///< <TT>size: 3 .............. -> total: 3 .. -> string[3] .. -> "BDY" </TT>
    MQ_STRB	    charN   ;   ///< <TT>size: 1 .............. -> total: 4 .. -> character .. -> "+"</TT>
    union HdrIU	    num	    ;   ///< <TT>size: HDR_INT_LEN=8 .. -> total: 12 . -> integer .... -> "BODYITEM_SIZE"</TT>
    MQ_STRB	    charE   ;   ///< <TT>size: 1 .............. -> total: 13 . -> character .. -> "+"</TT>
};

/// \brief the entire HEADER 
struct HdrSendS {
    struct HdrS	    hdr	    ;   ///< <TT>size: 38 ............. -> total: 38 . -> HdrS ....... -> "HEADER" </TT>
    struct BdyS	    bdy	    ;   ///< <TT>size: 13 ............. -> total: 59 . -> BdyS ....... -> "HEADER-BODY" </TT>
};

#pragma pack()

/// \}

/*****************************************************************************/
/*                                                                           */
/*                                msgque/misc                                */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE pWaitOnEvent (struct MqS * const, const enum MqIoSelectE, const MQ_TIME_T);

enum MqErrorE pUSleep (
  struct MqS * const context,
  long const usec
);

enum MqErrorE MQ_DECL pCallFactory (
  struct MqS * const context,
  enum MqFactoryE create,
  struct MqFactoryS factory,
  struct MqS ** contextP
);

void pConfigSetName (
  struct MqS * const context,
  MQ_STR  data
);

static mq_inline struct MqS* pMqGetFirstParent(
  struct MqS * const context
)
{
  return context->link.ctxIdP;
}

#if (HDR_TOK_LEN+1) == 5
#define ppTokenCopy(i1,i2) memcpy(i1,i2,5);
#else
#   error invalid TOKEN_LEN
#endif

END_C_DECLS

#endif /* MQ_PRIVATE_H */


