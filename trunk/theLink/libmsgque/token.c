/**
 *  \file       theLink/libmsgque/token.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "main.h"
#include "token.h"
#include "sys.h"
#include "error.h"
#include "log.h"
#include "send.h"
#include "buffer.h"
#include "bufferL.h"
#include "read.h"
#include "mq_io.h"
#include "log.h"
#include "trans.h"
#include "cache.h"
#include "link.h"
#include "config.h"

BEGIN_C_DECLS

#define MQ_CONTEXT_S token->context

#if defined(WORDS_BIGENDIAN)
#   define pTokenCmpD(s1,s2) (pByte2INT(s1) - pByte2INT(s2))
#else
#   define pTokenCmpD(s1,s2) (pByte2INT(s2) - pByte2INT(s1))
#endif

#if defined(WORDS_BIGENDIAN)
#   define pTokenCmpI(i1,i2) (i1 - i2)
#else
#   define pTokenCmpI(i1,i2) (i2 - i1)
#endif

static enum MqErrorE pTokenDefaultTRT (struct MqS * const context, MQ_PTR const data);

/*****************************************************************************/
/*                                                                           */
/*                               token_define                                */
/*                                                                           */
/*****************************************************************************/

struct pTokenS;

struct pTokenItemS {
  MQ_INT name;			///< the pToken name (cast into an integer)
  struct MqCallbackS callback;	///< callback data
};

struct pTokenSpaceS {
  struct MqS * context;		///< link to the error object
  struct pTokenItemS *items;    ///< array of the items
  struct pTokenItemS *lastItem; ///< save the last item
  struct MqCallbackS all;	///< data of the "+ALL" item
  MQ_SIZE size;                 ///< max possible number of items
  MQ_SIZE used;                 ///< number of used entries in the items
  MQ_INT sorted;		///< is the array sorted ?
};

/*****************************************************************************/
/*                                                                           */
/*                          token_static                                     */
/*                                                                           */
/*****************************************************************************/

static struct pTokenSpaceS* sTokenSpaceCreate (
  struct MqS * const context,
  MQ_SIZE const add
);

static void sTokenSpaceFree (
  struct pTokenSpaceS ** const spaceP
);

static enum MqErrorE sTokenSpaceAdd (
  register struct pTokenSpaceS * const space,
  MQ_SIZE const add
);

static enum MqErrorE sTokenSpaceDelItem (
  struct pTokenSpaceS * const space,
  MQ_CST const name
);

/*****************************************************************************/
/*                                                                           */
/*                               token_init                                  */
/*                                                                           */
/*****************************************************************************/

struct pTokenS*
pTokenCreate (
  struct MqS * const context
)
{
  static struct MqCallbackS cb = {pTokenDefaultTRT, NULL, NULL, NULL};
  struct pTokenS * const token = 
    (struct pTokenS *const) MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof (*token));

  token->context = context;
  token->loc = sTokenSpaceCreate (context, 10);

  pTokenAddHdl (token, "+TRT", cb);

  return token;
}

void
pTokenDelete (
  register struct pTokenS ** const tokenP
)
{
  if (unlikely(tokenP == NULL || *tokenP == NULL)) {
    return;
  } else {
    sTokenSpaceFree (&(*tokenP)->loc);
    MqSysFree (*tokenP);
  }
}

/*****************************************************************************/
/*                                                                           */
/*                              token_memory                                 */
/*                                                                           */
/*****************************************************************************/

static struct pTokenSpaceS *
sTokenSpaceCreate (
  struct MqS * const context,
  MQ_SIZE const add
)
{
  // create the structure
  register struct pTokenSpaceS *space = (struct pTokenSpaceS *) 
    MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof (*space));

  // filling the items
  space->items = (struct pTokenItemS *) MqSysCalloc (MQ_ERROR_PANIC, add, sizeof (*space->items));
  space->size = add;
  space->used = 0;
  space->sorted = 0;
  space->lastItem = space->items;
  space->context = context;

  // space back
  return space;
}

static enum MqErrorE
sTokenSpaceAdd (
  register struct pTokenSpaceS * const space,
  MQ_SIZE const add
)
{
  register MQ_SIZE newsize = space->used + add;

  if (newsize <= space->size)
    return MQ_OK;

  // add extra space
  newsize *= 2;

  // alloc new space
  space->items = (struct pTokenItemS *)
	MqSysRealloc (MQ_ERROR_PANIC, space->items, (newsize * sizeof (*space->items)));
  memset(space->items+space->used, '\0', newsize-space->used);
  space->size = newsize;
  space->lastItem = space->items;

  return MQ_OK;
}

static void
sTokenSpaceFree (
  struct pTokenSpaceS ** const spaceP
)
{
  if (unlikely(spaceP == NULL || *spaceP == NULL)) {
    return;
  } else {
    sTokenSpaceDelItem ((*spaceP), "-ALL");
    MqSysFree ((*spaceP)->items);
    MqSysFree (*spaceP);
  }
}

static struct pTokenItemS *
sTokenSpaceFindItem (
  register struct pTokenItemS * start,
  register struct pTokenItemS const * const end,
  register MQ_INT const nameI
)
{
  while (start < end && start->name != nameI)
    start++;
  return (start >= end ? NULL : start);
}

static enum MqErrorE
sTokenSpaceDelItem (
  struct pTokenSpaceS * const space,
  MQ_CST const name
)
{
  register struct pTokenItemS * start = space->items;
           struct pTokenItemS * end = start + space->used;

  if (strncmp (name, "-ALL", HDR_TOK_LEN)) {
    // name != "-ALL"
    start = sTokenSpaceFindItem (start, end, pByte2INT(name));

    if (unlikely (start == NULL))
      return MqErrorV (space->context, __func__, -1, "item '%s' not found", name);

    // call the data item cleanup
    if (start->callback.data && start->callback.fFree) {
      (*start->callback.fFree) (space->context, &start->callback.data);
    }
    // move all items !after! start one to the left
    memmove(start, start+1, (end-start-1) * sizeof(*space->items));
    // set the last item to zero
    memset(end-1, '\0', sizeof(*space->items));
    // one item less
    space->used -= 1;
  } else {
    // name == "-ALL"
    // delete the "+ALL" token
    if (space->all.fCall != NULL) {
      space->all.fCall = NULL;
      if (space->all.data && space->all.fFree) {
	(*space->all.fFree) (space->context, &space->all.data);
      }
    }
    // delete the other token
    while (start < end--) {
      if (end->callback.data && end->callback.fFree) {
	(*end->callback.fFree) (space->context, &end->callback.data);
      }
    }
    // set all items to zero
    memset(start, '\0', space->used * sizeof(struct pTokenItemS));
    space->used = 0;
  }

  return MQ_OK;
}

/*****************************************************************************/
/*                                                                           */
/*                              token_misc                                   */
/*                                                                           */
/*****************************************************************************/

static int
MQ_CDECL sTokenCompare1 (
  const void *one,
  const void *two
)
{
  return pTokenCmpI (((struct pTokenItemS *) one)->name, ((struct pTokenItemS *) two)->name);
}

static int
MQ_CDECL sTokenCompare2 (
  const void *key,
  const void *two
)
{
  return pTokenCmpI (*((MQ_INT*)key), ((struct pTokenItemS *) two)->name);
}

/*
MQ_SIZE 
pTokenGetUsed (
  register struct pTokenS const * const token
)
{
  return token->loc->used;
}
*/

// "MQ_INLINE" because the proc in only used once
enum MqErrorE
pTokenInvoke (
  struct pTokenS const * const token
)
{
  struct MqS * const context = token->context;
  register struct pTokenItemS * item = NULL;
  register struct pTokenSpaceS * const space = token->loc;
  const MQ_INT icurrent = pByte2INT(token->current);

  // clear error buffer
  MqErrorReset (context);

  if (unlikely (!space->sorted)) {
    qsort (space->items, space->used, sizeof (struct pTokenItemS), sTokenCompare1);
    space->sorted = 1;
  }
  // ATTENTION: space->lastItem was filled in pTokenCreate
  for (;;) {
    if (sTokenCompare2 (&icurrent, space->lastItem) == 0) {
      item = space->lastItem;
      if (item != NULL) break;
    }

    // search all items
    if (space->used != 0) {
      item = (struct pTokenItemS *) bsearch (&icurrent, space->items,
	  space->used, sizeof (struct pTokenItemS), sTokenCompare2);
      if (item != NULL) break;
    }

    // search "+ALL" items
    if (space->all.fCall != NULL) {
      return (space->all.fCall (context, space->all.data));
    }

    // nothing found -> break
    return MqErrorV (context, __func__, -1, "token <%s> not found", token->current);
  };

  space->lastItem = item;

/*
    MqDLogV(context,1,"current<%s>, currentPtr->proc<%p>, currentPtr->data<%p>\n", 
	token->current, currentPtr->callback.fCall, currentPtr->callback.data);
*/
  
  if (item->callback.fCall != NULL) {
    return MqCallbackCall (context, item->callback);
  } else {
    return MqSendRETURN(context);
  }
}

enum MqErrorE
pTokenAddHdl (
  struct pTokenS const * const token,
  MQ_CST const name,
  struct MqCallbackS callback
)
{
  struct MqS * const context = token->context;
  register struct pTokenSpaceS * const space = token->loc;
  register struct pTokenItemS *free;

  MqDLogV(context, 5, "HANDEL SERVICE ADD %s: proc<%p>, data<%p>\n",
    name, (void*) callback.fCall, callback.data);

  if (!strncmp (name, "+ALL", HDR_TOK_LEN)) {
    if (space->all.fCall != NULL) {
      MqErrorCheck (pTokenDelHdl (token, name));
    }
    space->all = callback;
  } else {
    MQ_INT nameI = pByte2INT(name); 
    if (sTokenSpaceFindItem (space->items, space->items + space->used, nameI)) {
      MqErrorCheck (pTokenDelHdl (token, name));
    }
    MqErrorCheck (sTokenSpaceAdd (space, 1));

    free = space->items + space->used;

    space->sorted = 0;
    space->used += 1;

    free->name = nameI;
    free->callback = callback;
  }

error:
  return MqErrorStack (context);
}

enum MqErrorE
pTokenDelHdl (
  struct pTokenS const * const token,
  MQ_CST const name
)
{
  struct MqS * const context = token->context;
  MqDLogV (context, 6, "HANDEL DEL %s: token<%p>\n", name, (void*) token);
  if (strncmp (name, "+ALL", HDR_TOK_LEN)) {
    static struct MqCallbackS cb = {pTokenDefaultTRT, NULL, NULL, NULL};
    // name != "+ALL"
    MqErrorCheck (sTokenSpaceDelItem (token->loc, name));
    // re-initialize the "longterm-transaction-result-handler"
    if (!strncmp (name, "-ALL", HDR_TOK_LEN)) {
      MqErrorCheck (pTokenAddHdl (token, "+TRT", cb));
    }
  } else {
    // name == "+ALL"
    struct pTokenSpaceS * const loc = token->loc;
    loc->all.fCall = NULL;
    if (loc->all.data && loc->all.fFree) {
      (*loc->all.fFree) (context, &loc->all.data);
    }
  }
error:
  return MqErrorStack(context);
}

static enum MqErrorE
pTokenDefaultTRT (
  struct MqS * const context,
  MQ_PTR const data
) 
{
  switch (pReadGetHandShake (context)) {
    case MQ_HANDSHAKE_OK: {
      pTokenSetCurrent (context->link.srvT, pReadGetTransactionToken(context));
      return pTokenInvoke (context->link.srvT);
    }
    case MQ_HANDSHAKE_ERROR: {
      MQ_INT retNum;
      MQ_CST msg;
      MQ_BUF tmp;

      // (*callback.fCall) is never called
      MqDLogC(context,5,"got ERROR return code\n");
      MqErrorCheck (MqReadU (context, &tmp));
      MqErrorCheck (MqReadI (context, &retNum));
      pReadSetReturnNum (context, retNum);
      
      // write HEADER
      MqErrorV (context, "callback-error", retNum, "<Token|%s>, <Num|%i>\n", 
	pReadGetTransactionToken(context), retNum);
      // write ERROR-STACK
      while (MqReadItemExists (context)) {
	MqErrorCheck (MqReadC (context, &msg));
	pErrorAppendC (context, msg);
      }
error:
      return MQ_ERROR;
    }
    case MQ_HANDSHAKE_START:
    case MQ_HANDSHAKE_TRANSACTION:
      break;
  }
  return MqErrorDb2(context, MQ_ERROR_HANDSHAKE);
}

enum MqErrorE
pTokenCheckSystem (
  struct pTokenS const * const token
)
{
  register struct MqS * const context = token->context;
  register MQ_CST curr = token->current;
  const int MqSetDebugLevel(context);

  // 1. check on USER defined token
  MqDLogVL (context, 5, "check token<%s>\n", curr);
  if (likely (*curr != '_'))
    return MQ_OK;

  // 3. work on msgque-system-token
  curr++;
  switch (*curr) {
    case 'R': {                // _RET: return from a service
      MqErrorCheck(pTransSetResult (context->link.trans, MQ_TRANS_END, context->link.read));
      break;
    }
    case 'E':
    {   
	curr++;
	switch (*curr) {
	    case 'R': {		// _ERR: CLIENT on error in SERVER
	      MQ_CST errtext;
	      MQ_INT errnum;
	      MqErrorCheck (MqReadI (context, &errnum));
	      MqErrorCheck (MqReadC (context, &errtext));
	      if (context->config.master != NULL) {
		MqErrorSet (context->config.master, errnum, MQ_ERROR, errtext, NULL);
		MqSendERROR (context->config.master);
	      } else {
		MqErrorSet (context, errnum, MQ_ERROR, errtext, NULL);
		if (context->setup.BgError.fCall != NULL) {
		  MqDLogCL (context, 5, "call BqError\n");
		  MqErrorCheck (MqCallbackCall(context, context->setup.BgError));
		}
		// check "error.code" again because "setup.BgError.fCall" could clean it
		MqErrorCheck (context->error.code);
	      }
	      break;
	    }
	}
      break;
    }
    case 'I': 
      curr++;
      switch (*curr) {
	case 'A': {	      // _IAA:: SERVER, (I) (A)m (A)vailable
	  // -> client need the rmtCtx pointer from the server
	  // -> server need 'binary mode' and 'endian mode' from the client
	  if (MQ_IS_SERVER_PARENT (context)) {
	    MQ_CST name = NULL;
	    MQ_BOL myendian, mystring;

	    MqConfigSetIsString(context, MQ_YES);

	    // read the binary mode
	    MqReadO(context, &mystring);

	    // read the other endian and set my context->link.bits.endian
	    MqReadO(context, &myendian);
# if defined(WORDS_BIGENDIAN)
	    context->link.bits.endian = (myendian ? MQ_NO : MQ_YES);
# else
	    context->link.bits.endian = (myendian ? MQ_YES : MQ_NO);
# endif

	    // read the other endian and set my context->link.bits.endian
	    MqReadC(context, &name);
	    context->link.targetIdent = mq_strdup(name);

	    // read server name
	    MqReadC(context, &name);
	    if (*name != '\0')
	      MqConfigSetSrvName (context, name);

	    // send my endian back
	    MqSendSTART (context);
# if defined(WORDS_BIGENDIAN)
	    MqSendO (context, MQ_YES);
# else
	    MqSendO (context, MQ_NO);
# endif
	    MqSendC (context, context->setup.ident);
	    MqErrorCheck (pSendSYSTEM_RETR (context));

	    // set the binary mode
	    MqConfigSetIsString(context, mystring);
	    pReadSetType(context, mystring);
	  }
	  break;
	}
      }
      break;
    case 'O':                  // _OKS: SERVER , CHILD statup OK
      if (MQ_IS_SERVER (context)) {
	struct MqS * childctx;
	MQ_INT i;
	// need client-code to handel ContextCreate request
	MqErrorCheck1(pCallFactory(context, MQ_FACTORY_NEW_CHILD, context->setup.Factory, &childctx));
	pConfigSetParent (childctx, context);
        MqErrorCheck (MqReadI (context, &i));
	if (i != -1) MqConfigSetDebug(childctx, i);
        MqErrorCheck (MqReadI (context, &i));
	if (i) MqConfigSetIsSilent(childctx, MQ_YES);
	if (MqReadItemExists(context)) {
	  MQ_CST name;
	  MqErrorCheck (MqReadC (context, &name));
	  if (MqSlaveGet(context, 0)) {
	    MqConfigSetSrvName (childctx, name);
	  } else {
	    MqConfigSetName (childctx, name);
	  }
	}
        if (MqErrorCheckI(MqLinkCreate (childctx, NULL))) {
	  // if the error happen !after! fmsgque was created, use this code
	  MqErrorCopy(context, childctx);
	  MqContextDelete(&childctx);
        }
error1:
	MqSendSTART (context);
        MqErrorCheck (MqSendRETURN (context));
	// no MqBufferLDelete(&argv) because the CHILD will cleanup argv;
      }
      break;
    case 'P':
      curr++;
      switch (*curr) {
	  case 'E':		// _PEO: the (P)rocess (E)vent (O)k
	    break;
	  case 'I':		// _PIN: the (PIN)G
	    MqSendSTART (context);
	    while (MqReadItemExists(context)) {
	      MqErrorCheck (MqReadProxy (context, context));
	    }
	    MqErrorCheck (MqSendRETURN (context));
	    break;
      }
      break;
    case 'S':
      curr++;
      switch (*curr) {
        case 'H':		// _SHD: SERVER shutdown request from remote Mq
	  // delete link, but without memory free
	  if (context->bits.onExit == MQ_NO) {
	    if (context->setup.ignoreExit == MQ_NO) {
	      context->refCount++;
	      MqLinkDelete(context);
	      context->refCount--;
	      return pErrorSetExitWithCheck(context);
	    } else {
	      pMqShutdown(context,__func__);
	    }
	  }
	  break;
        case 'R':              // _SRT: System return
          MqErrorCheck (pTransSetResult (context->link.trans, MQ_TRANS_END, NULL));
          break;
      }
      break;
    default:                   // got a wrong message
      return MqErrorV (context, __func__, -1,
            "unknown Mq-System-Token \"%s\" found", context->link.srvT->current);
  }

  return MQ_CONTINUE;
error:
  return MqErrorStack (context);
}

/*****************************************************************************/
/*                                                                           */
/*                            token_set/get                                  */
/*                                                                           */
/*****************************************************************************/

int
pTokenCheck (
  struct pTokenS const * const token,
  MQ_CST const str
)
{
  return (!pTokenCmpD(token->current,str));
}

void
pTokenMark (
  struct MqS * const context,
  MqMarkF markF
)
{
  struct pTokenS const * const token = context->link.srvT;
  if (token != NULL) {
    struct pTokenSpaceS * const space = token->loc;
	     struct pTokenItemS * start = space->items;
    register struct pTokenItemS * end = start + space->used;
    while (start < end--) {
      if (end->callback.data) (*markF)(end->callback.data);
    }
  }
}

END_C_DECLS

