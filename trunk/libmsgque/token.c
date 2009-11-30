/**
 *  \file       libmsgque/token.c
 *  \brief      \$Id: token.c 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
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

#define TOKEN_LEN (HDR_TOK_LEN+1)
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

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                               token_define                                */
/*                                                                           */
/*****************************************************************************/

struct MqTokenS;

struct pTokenItemS {
  MQ_INT name;			///< the pToken name (cast into an integer)
  struct MqCallbackS callback;	///< callback data
};

struct pTokenSpaceS {
  struct MqS * context;  ///< link to the error object
  struct pTokenItemS *items;    ///< array of the items
  struct pTokenItemS *lastItem; ///< save the last item
  MQ_SIZE size;                 ///< max possible number of items
  MQ_SIZE used;                 ///< number of used entries in the items
  MQ_INT sorted;		///< is the array sorted ?
};

struct MqTokenS {
  struct MqS * context;	///< link to msgque object
  MQ_STRB current[TOKEN_LEN];	///< kind the the current action
  struct pTokenSpaceS *loc;     ///< the local handler
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

struct MqTokenS*
pTokenCreate (
  struct MqS * const context
)
{
  struct MqTokenS * const token = 
    (struct MqTokenS *const) MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof (*token));

  token->context = context;
  token->loc = sTokenSpaceCreate (context, 10);

  return token;
}

void
pTokenDelete (
  register struct MqTokenS ** const tokenP
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
    sTokenSpaceDelItem ((*spaceP), "_ALL");
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

  if (strncmp (name, "_ALL", HDR_TOK_LEN)) {
    start = sTokenSpaceFindItem (start, end, pByte2INT(name));

    if (unlikely (start == NULL))
      return MqErrorV (space->context, __func__, -1, "item '%s' not found", name);

    // call the data item cleanup
    if (start->callback.data && start->callback.fFree) {
      (*start->callback.fFree) (space->context, &start->callback.data);
    }
    // one item less
    space->used -= 1;
    // move all ites !after! start one to the left
    memmove(start, start+1, end-start-1);
    // set the last item to zero
    memset(start+space->used, '\0', sizeof(struct pTokenItemS));
  } else {
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
  register struct MqTokenS const * const token
)
{
  return token->loc->used;
}
*/

// "MQ_INLINE" because the proc in only used once
enum MqErrorE
pTokenInvoke (
  register struct MqTokenS const * const token
)
{
  register struct pTokenItemS *item = NULL;
  register struct pTokenSpaceS * const space = token->loc;
  const MQ_INT icurrent = pByte2INT(token->current);

  // clear error buffer
  MqErrorReset (token->context);

  if (unlikely (!space->sorted)) {
    qsort (space->items, space->used, sizeof (struct pTokenItemS), sTokenCompare1);
    space->sorted = 1;
  }
  // ATTENTION: token->lastItem was filled in pTokenCreate
  if (sTokenCompare2 (&icurrent, space->lastItem) == 0) {
    item = space->lastItem;
  }

  if (unlikely (item == NULL)) {
    item = (struct pTokenItemS *) bsearch (&icurrent, space->items,
	space->used, sizeof (struct pTokenItemS), sTokenCompare2);
  }

  if (likely (item != NULL)) {
    space->lastItem = item;
  } else {
    return MqErrorV (token->context, __func__, -1, "token <%s> not found", token->current);
  }

/*
    MqDLogV(context,1,"current<%s>, currentPtr->proc<%p>, currentPtr->data<%p>\n", 
	token->current, currentPtr->callback.fFunc, currentPtr->callback.data);
*/

  return (item->callback.fFunc (token->context, item->callback.data));
}

enum MqErrorE
pTokenAddHdl (
  struct MqTokenS const * const token,
  MQ_CST const name,
  struct MqCallbackS callback
)
{
  struct MqS * const context = token->context;
  register struct pTokenSpaceS * const space = token->loc;
  register struct pTokenItemS *free;

  MQ_INT nameI = pByte2INT(name); 

  MqDLogV(context, 4, "HANDEL %s ADD %s: proc<%p>, data<%p>\n",
          (token == context->link.srvT ? "SERVICE" : "RETURN"), name, callback.fFunc, callback.data);

  if (sTokenSpaceFindItem (space->items, space->items + space->used, nameI))
    return MqErrorV (context, __func__, -1, "item '%s' already available", name);

  MqErrorCheck (sTokenSpaceAdd (space, 1));

  free = space->items + space->used;

  space->sorted = 0;
  space->used += 1;

  free->name = nameI;
  free->callback = callback;

error:
  return MqErrorStack (context);
}

enum MqErrorE
pTokenDelHdl (
  struct MqTokenS const * const token,
  MQ_CST const name
)
{
  MqDLogV (token->context, 6, "HANDEL DEL %s: token<%p>\n", name, token);
  return sTokenSpaceDelItem (token->loc, name);
}

enum MqErrorE
pTokenCheckSystem (
  struct MqTokenS const * const token
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
    case 'F': {                // _FTR: SERVER get filter data
      struct MqSendS * send = context->link.send;
      struct MqS * const myFilter = MqSlaveGet (context, 0);
      if (context->setup.FilterFTR.fFunc) {
	// if a filer follow, use this filter to send data
	if (myFilter != NULL) {
	  context->link.send = myFilter->link.send;
	}
	(*context->setup.FilterFTR.fFunc) (context, context->setup.FilterFTR.data);
	context->link.send = send;
      } else if (myFilter != NULL) {
	// default filter, just pass the args
	MQ_BUF buf;
	context->link.send = myFilter->link.send;
	MqSendSTART(context);
	while (MqReadItemExists(context)) {
	  MqReadU(context, &buf);
	  MqSendU(context, buf);
	}
	MqSendFTR(context, MQ_TIMEOUT_USER);
	context->link.send = send;
      } else {
	MqErrorDb2 (context, MQ_ERROR_TOKEN_FTR);
      }
      MqSendSTART (context);
      MqErrorCheck (MqSendRETURN (context));
      break;
    }
    case 'E':
    {   
	curr++;
	switch (*curr) {
	    case 'O': {		// _EOF: SERVER EndOfFile for input stream
	      struct MqSendS * send = context->link.send;
	      struct MqS * const myFilter = MqSlaveGet (context, 0);
	      enum MqErrorE ret = MQ_OK;
	      if (myFilter != NULL) {
		MqSendSTART(myFilter);
	      }
	      if (context->setup.FilterEOF.fFunc) {
		// if a filer follow, use this filter to send data
		if (myFilter != NULL) {
		  context->link.send = myFilter->link.send;
		}
		ret = (*context->setup.FilterEOF.fFunc) (context, context->setup.FilterEOF.data); 
		context->link.send = send;
	      } 
	      if (ret == MQ_OK && myFilter != NULL) {
		MqSendEOF(myFilter, MQ_TIMEOUT_USER);
	      }
	      // default answer
	      MqSendSTART (context);
	      MqErrorCheck (MqSendRETURN (context));
	      break;
	    }
	    case 'R': {		// _ERR: CLIENT on error in SERVER
	      MQ_CST errtext;
	      MQ_INT errnum;
	      MqErrorCheck (MqReadI (context, &errnum));
	      MqErrorCheck (MqReadC (context, &errtext));
	      MqErrorSet (context, errnum, MQ_ERROR, errtext);

	      if (context->setup.BgError.fFunc != NULL) {
		MqDLogCL (context, 5, "call BqError\n");
		MqErrorCheck ((*context->setup.BgError.fFunc) (context, context->setup.BgError.data));
	      }
	      MqErrorCheck (context->error.code);
	      break;
	    }
	}
      break;
    }
    case 'I':                  // _IAA:: SERVER, (I) (A)m (A)vailable
      // -> client need the rmtCtx pointer from the server
      // -> server need 'binary mode' and 'endian mode' from the client
      if (MQ_IS_SERVER_PARENT (context)) {
	MQ_BOL myendian, mystring;
	struct MqBufferS * name = NULL;

        MqConfigSetIsString(context, MQ_YES);

	// read the binary mode
	MqReadO(context, &mystring);

	// read the other endian and set my context->link.endian
	MqReadO(context, &myendian);
# if defined(WORDS_BIGENDIAN)
	context->link.endian = (myendian ? MQ_NO : MQ_YES);
# else
	context->link.endian = (myendian ? MQ_YES : MQ_NO);
# endif

	// read server name
	MqReadU(context, &name);
	if (name->cursize > 0) {
	  MqConfigSetName (context, name->cur.C);
	}

	// send my endian back
        MqSendSTART (context);
# if defined(WORDS_BIGENDIAN)
	MqSendO (context, MQ_YES);
# else
        MqSendO (context, MQ_NO);
# endif
	MqErrorCheck (pSendSYSTEM_RETR (context));

	// set the binary mode
	MqConfigSetIsString(context, mystring);
	pReadSetType(context, mystring);

	break;
      }
      break;
    case 'O':                  // _OKS: SERVER , CHILD statup OK
      if (MQ_IS_SERVER (context)) {
	struct MqS * fcontext;
	MQ_INT i;
	MQ_BUF name;
	// need client-code to handel ContextCreate request
	MqErrorCheck1(pCallFactory(context, MQ_FACTORY_NEW_CHILD, context->setup.Factory, &fcontext));
	MqConfigSetParent (fcontext, context);
        MqErrorCheck (MqReadI (context, &i));
	if (i != -1) MqConfigSetDebug(fcontext, i);
        MqErrorCheck (MqReadI (context, &i));
	if (i) MqConfigSetIsSilent(fcontext, MQ_YES);
	MqErrorCheck (MqReadU (context, &name));
	if (name->cursize > 0) {
	  MqConfigSetName (fcontext, name->cur.C);
	}
        if (MqErrorCheckI(MqLinkCreate (fcontext, NULL))) {
	  // if the error happen !after! fmsgque was created, use this code
	  MqErrorCopy(context, fcontext);
	  MqContextDelete(&fcontext);
        }
error1:
	MqSendSTART (context);
        MqErrorCheck (MqSendRETURN (context));
	// no MqBufferLDelete(&argv) because the CHILD will cleanup argv;
      }
      break;
    case 'P':                   // _PEO: the (P)rocess (E)vent (O)k
      break;
    case 'S':
      curr++;
      switch (*curr) {
        case 'H':		// _SHD: SERVER shutdown request from remote Mq
	  // shutdown all dependenig objects
	  pMqShutdown(context);
	  // the deletion will be handled in "pEventStart"
	  return pErrorSetEXIT (context, __func__);
        case 'R':              // _SRT: System return
          pTransSetResult (context->link.trans, MQ_TRANS_END, NULL);
          break;
      }
      break;
    default:                   // got a wrong message
      goto wrong_token;
  }

  return MQ_CONTINUE;

wrong_token:
  MqErrorV (context, __func__, -1,
	    "unknown Mq-System-Token \"%s\" found", context->link.srvT->current);

error:
  return MqErrorStack (context);
}

/*****************************************************************************/
/*                                                                           */
/*                            token_set/get                                  */
/*                                                                           */
/*****************************************************************************/

MQ_CST
pTokenGetCurrent (
  struct MqTokenS const * const token
)
{
  return token->current;
}

void
pTokenSetCurrent (
  struct MqTokenS * const token,
  MQ_CST const str
)
{
  memcpy (token->current, str, HDR_TOK_LEN);
}

int
pTokenCheck (
  struct MqTokenS const * const token,
  MQ_CST const str
)
{
  return (!pTokenCmpD(token->current,str));
}

END_C_DECLS

