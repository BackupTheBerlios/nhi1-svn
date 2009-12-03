/**
 *  \file       theLink/libmsgque/trans.c
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
#include "trans.h"
#include "sys.h"
#include "error.h"
#include "log.h"
#include "read.h"
#include "cache.h"

#define TOKEN_LEN (HDR_TOK_LEN+1)

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                               trans_init                                  */
/*                                                                           */
/*****************************************************************************/

struct MqTransS {
  struct MqS * context;	    ///< link to the msgque object
  MQ_SIZE   transIdR;		    ///< the largest currently used tranId number
  MQ_SIZE   transIdZ;		    ///< the size of the tranIdA array
  struct MqTransItemS ** transIdA;  ///< array of struct MqTransItemS * pointer's
  struct MqCacheS * transCache;	    ///< cache for MqTransItemSP
};

struct MqTransItemS {
  enum MqTransE status;		    ///< START, END, TIMEOUT or CANCEL
  struct MqReadS * result;	    ///< the result of the transaction (the answer)
  struct MqTransS const * trans;    ///< link to the \e MqTransS object
  MQ_TIME_T start;		    ///< time transaction start
  MQ_HDL last;			    ///< temporary storage of last transaction
  MQ_HDL transId;		    ///< unique transaction ID
  struct MqCallbackS callback;	    ///< callback function pointer
};

/*****************************************************************************/
/*                                                                           */
/*                               C R E A T E                                 */
/*                                                                           */
/*****************************************************************************/

static struct MqTransItemS *
pTransItemCreate (
  struct MqTransS * const trans
)
{
  struct MqTransItemS * const item = (struct MqTransItemS *) MqSysMalloc (MQ_ERROR_PANIC, sizeof (*item));
  trans->transIdR++;
  item->transId = trans->transIdR;
  if (trans->transIdR >= trans->transIdZ) {
    trans->transIdA = (struct MqTransItemS **) MqSysRealloc (MQ_ERROR_PANIC, 
          trans->transIdA, (trans->transIdZ + 100) * sizeof(item));
    memset (trans->transIdA + trans->transIdZ, 0, 100 * sizeof(item));
    trans->transIdZ += 100;
  }
  return trans->transIdA[item->transId] = item;
}

static void
pTransItemDelete (
  struct MqTransItemS ** const itemP
)
{
  struct MqTransItemS * const item = *itemP;
  struct MqTransS const * const trans = item->trans;
  if (unlikely(item == NULL))
    return;
  if (item->result)
    pReadDelete (&item->result);
  trans->transIdA[item->transId] = NULL;
  MqSysFree(*itemP);
}

enum MqErrorE
pTransCreate (
  struct MqS * const context,
  struct MqTransS ** const out
)
{
  struct MqTransS * const trans = (struct MqTransS *) MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof (*trans));
  *out = trans;
  trans->context = context;

  pCacheCreate ((CacheCF) pTransItemCreate, (CacheDF) pTransItemDelete, trans, &trans->transCache);

  return MQ_OK;
}

void
pTransDelete (
  struct MqTransS ** const transP
)
{
  struct MqTransS * const trans = *transP;

  if (unlikely(trans == NULL)) return;

  pCacheDelete (&trans->transCache);

  MqSysFree (trans->transIdA);
  MqSysFree (*transP);
}

MQ_HDL
pTransPop (
  struct MqTransS const * const trans,
  struct MqCallbackS callback
)
{
  struct MqTransItemS * const item = (struct MqTransItemS *) pCachePop (trans->transCache);

  item->status = MQ_TRANS_START;
  item->result = NULL;
  item->trans = trans;
  item->start = time (NULL);
  item->callback = callback;
  item->last = trans->context->link._trans;

  trans->transIdA[item->transId] = item;

  return item->transId;
}

void
pTransPush (
  struct MqTransS const * const trans,
  MQ_HDL transId
)
{
  struct MqTransItemS * const item = trans->transIdA[transId];
  if (unlikely(item == NULL)) return;
  if (item->callback.data && item->callback.fFree)
    (*item->callback.fFree)(trans->context, &item->callback.data);
  pCachePush (trans->transCache, item);
  trans->transIdA[transId] = NULL;
}

/*****************************************************************************/
/*                                                                           */
/*                              trans_misc                                   */
/*                                                                           */
/*****************************************************************************/

int
pTransCheckStart (
  struct MqTransS const * const trans,
  const MQ_HDL transId
)
{
  return (trans->transIdA[transId]->status == MQ_TRANS_START);
}

/*****************************************************************************/
/*                                                                           */
/*                              trans_get                                    */
/*                                                                           */
/*****************************************************************************/

MQ_HDL
pTransGetLast (
  struct MqTransS const * const trans,
  const MQ_HDL transId
)
{
  return trans->transIdA[transId]->last;
}

struct MqReadS *
pTransGetResult (
  struct MqTransS const * const trans,
  const MQ_HDL transId
)
{
  struct MqTransItemS * const item = trans->transIdA[transId];
  struct MqReadS * const ret = item->result;
  item->result = NULL;
  return ret;
}

enum MqTransE
pTransGetStatus (
  struct MqTransS const * const trans,
  const MQ_HDL transId
)
{
  return trans->transIdA[transId]->status;
}

/*****************************************************************************/
/*                                                                           */
/*                              trans_set                                    */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
pTransSetResult (
  struct MqTransS const * const trans,
  enum MqTransE const status,
  struct MqReadS * const result
)
{
  struct MqS * const context = trans->context;
  MQ_HDL _trans = context->link._trans;
  struct MqTransItemS * item;
  if (unlikely(_trans < 0 || _trans > trans->transIdR)) 
      return MqErrorV (context, __func__, -1, "invalid transaction-id '%i'", _trans);
  item = trans->transIdA[_trans];
  if (item == NULL) return MQ_CONTINUE;
  item->status = status;
  if (item->callback.fFunc) {
    switch (pReadGetReturnCode (context)) {
      case MQ_RETURN_OK: {
	enum MqErrorE ret;
	// from: MqSendEND_AND_CALLBACK
	ret = (*item->callback.fFunc)(context, item->callback.data);
	pTransPush(trans, _trans);
	MqErrorCheck(ret);
	return MQ_OK;
      }
      case MQ_RETURN_ERROR: {
	MQ_CST msg;
	// (*callback.fFunc) is never called
	pTransPush(trans, _trans);
	MqDLogV(context,5,"%s\n","got ERROR return code");
	MqErrorCheck1(pRead_RET_START (context));
	// write HEADER
	MqErrorV (context, "callback-error", pReadGetReturnNum (context),
		"<Num|%i>\n", pReadGetReturnNum (context));
	// write ERROR-STACK
	while (MqReadGetNumItems (context)) {
	  MqErrorCheck1 (MqReadC (context, &msg));
	  pErrorAppendC (context, msg);
	}
error1:
	pRead_RET_END (context);
	return MQ_ERROR;
      }
    }
  } else if (result) {
    item->result = result;
    // from: MqSendEND_AND_WAIT
    context->link.read = (struct MqReadS * const) pCachePop (context->link.readCache);
  }
  return MQ_OK;

error:
  return MqErrorStack(context);
}

END_C_DECLS



