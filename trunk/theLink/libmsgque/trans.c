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

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                               trans_init                                  */
/*                                                                           */
/*****************************************************************************/

struct MqTransS {
  struct MqS * context;		    ///< link to the msgque object
  MQ_SIZE   transIdR;		    ///< the largest currently used tranId number
  MQ_SIZE   transIdZ;		    ///< the size of the tranIdA array
  struct MqTransItemS ** transIdA;  ///< array of struct MqTransItemS * pointer's
  struct MqCacheS * transCache;	    ///< cache for MqTransItemSP
#ifdef _DEBUG
  MQ_HDL    shift;		    ///< shift transaction to random number
#endif
};

struct MqTransItemS {
  enum MqTransE status;		    ///< START, END, TIMEOUT or CANCEL
  struct MqReadS * result;	    ///< the result of the transaction (the answer)
  struct MqTransS const * trans;    ///< link to the \e MqTransS object
  MQ_TIME_T start;		    ///< time transaction start
  MQ_HDL last;			    ///< temporary storage of last transaction
  MQ_HDL transId;		    ///< unique transaction ID
  struct MqCallbackS callback;	    ///< callback function pointer
  MQ_TOK token;			    ///< callback token
  enum MqHandShakeE hs;		    ///< save the handshake
};

#ifdef _DEBUG
# define pu2pr(id) ((id)-trans->shift)
# define pr2pu(id) ((id)+trans->shift)
#else
# define pu2pr(id) (id)
# define pr2pu(id) (id)
#endif

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
#ifdef _DEBUG
  // this should be some kind of random
  trans->shift = (MQ_HDL) ((((long)context>>7) + ((long)trans>>7) + ((long)MqThreadSelf()>>7) + (long) mq_getpid()) & 127);
#endif

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

  // free "old" transactions not closed
  if (trans->transIdZ != 0) {
    MQ_HDL i;
    for (i=0; i<=trans->transIdR; i++) {
      if (trans->transIdA[i] != NULL)
	pTransPush (trans, i);
    }
  }

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
  item->last = pu2pr(trans->context->link.transSId);

  trans->transIdA[item->transId] = item;

//if (!strncmp(trans->context->config.name,"fs",2))
//  MqDLogV(trans->context,__func__,0,"trans<%p>, transH<%d>\n", trans, item->transId);

  return pr2pu(item->transId);
}

void
pTransPush (
  struct MqTransS const * const trans,
  MQ_HDL transId
)
{
//if (!strncmp(trans->context->config.name,"fs",2))
//  MqDLogV(trans->context,__func__,0,"trans<%p>, transH<%d>\n", trans, transId);

  struct MqTransItemS * item;
  transId = pu2pr(transId);
  item = trans->transIdA[transId];
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
  return (trans->transIdA[pu2pr(transId)]->status == MQ_TRANS_START);
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
  return trans->transIdA[pu2pr(transId)]->last;
}

struct MqReadS *
pTransGetResult (
  struct MqTransS const * const trans,
  const MQ_HDL transId
)
{
  struct MqTransItemS * const item = trans->transIdA[pu2pr(transId)];
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
  return trans->transIdA[pu2pr(transId)]->status;
}

enum MqHandShakeE
pTransGetHandShake (
  struct MqTransS const * const trans,
  const MQ_HDL transId
)
{
  return trans->transIdA[pu2pr(transId)]->hs;
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
  MQ_HDL puTrId = context->link.transSId;
  MQ_HDL transSId = pu2pr(puTrId);
  struct MqTransItemS * item;
  if (unlikely(transSId < 0 || transSId > trans->transIdR)) 
      return MqErrorV (context, __func__, -1, "invalid transaction-id '%i'", transSId);
  item = trans->transIdA[transSId];
  if (item == NULL) return MQ_OK;
  item->status = status;
  if (unlikely (item->callback.fCall != NULL)) {
    switch (pReadGetHandShake (context)) {
      case MQ_HANDSHAKE_OK: {
	enum MqErrorE ret;
	// from: MqSendEND_AND_CALLBACK
	ret = (*item->callback.fCall)(context, item->callback.data);
	pTransPush(trans, puTrId);
	MqErrorCheck(ret);
	return MQ_OK;
      }
      case MQ_HANDSHAKE_ERROR: {
	MQ_INT retNum;
	MQ_CST msg;

	// (*callback.fCall) is never called
	pTransPush(trans, puTrId);
	MqDLogC(context,5,"got ERROR return code\n");
	MqErrorCheck1 (MqReadI (context, &retNum));
	pReadSetReturnNum (context, retNum);
	
	// write HEADER
	MqErrorV (context, "callback-error", retNum, "<Num|%i>\n", retNum);
	// write ERROR-STACK
	while (MqReadItemExists (context)) {
	  MqErrorCheck1 (MqReadC (context, &msg));
	  pErrorAppendC (context, msg);
	}
error1:
	return MQ_ERROR;
      }
      case MQ_HANDSHAKE_START:
      case MQ_HANDSHAKE_TRANSACTION:
	MqPanicSYS(context);
    }
  } else {
    item->hs = pReadGetHandShake(context);
    if (result) {
      item->result = result;
      // from: MqSendEND_AND_WAIT
      context->link.read = (struct MqReadS * const) pCachePop (context->link.readCache);
    }
  }
  return MQ_OK;
error:
  return MqErrorStack(context);
}

static void sTransMark (struct MqCallbackS *cb, MqMarkF const markF) {
  if (cb->data != NULL) (*markF)(cb->data);
}

void pTransMark (
  struct MqS * const context,
  MqMarkF const markF
) 
{ 
  struct MqTransS const * const trans = context->link.trans;
  if (unlikely(trans == NULL)) return;
  pCacheMark(trans->transCache, (MqCacheMarkF) sTransMark, markF);
}

END_C_DECLS


