/**
 *  \file       theLink/libmsgque/binary.c
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

#define MQ_CONTEXT_S context

/*****************************************************************************/
/*                                                                           */
/*                                   main                                    */
/*                                                                           */
/*****************************************************************************/

#pragma pack(1)
struct MqBinaryS {
  MQ_SIZE len;
  MQ_BINB data[];
};
#pragma pack(0)

#pragma pack(1)
struct MqBdyS {
  enum MqHandShakeE   handShake;
  MQ_SIZE	      numItems;
  MQ_HDL	      transSId;
  MQ_STRB	      token[4];
  MQ_BINB	      data[];
};
#pragma pack(0)

enum MqErrorE
MqReadBDY (
  struct MqS * const context,
  struct MqBinaryS ** const binaryP
)
{
  struct MqReadS * const read = context->link.read;
  if (unlikely(read == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    MQ_SIZE const llen = sizeof(struct MqBinaryS) + sizeof(struct MqBdyS) + read->bdy->cursize;
    MQ_BIN data = (MQ_BIN) MqSysMalloc (MQ_ERROR_PANIC, llen);
    struct MqBinaryS *binary = (struct MqBinaryS*) data;
    struct MqBdyS *bdy = (struct MqBdyS*) binary->data
    binary->len = llen;
    memcpy (bdy->data, read->bdy->data, read->bdy->cursize);
    // in a "longterm-transaction" with "MqReadBDY" no return transaction is
    // required because the transaction is forwarded
    read->handShake = MQ_HANDSHAKE_START;
    return MQ_OK;
  }
}

enum MqErrorE
MqSendBDY (
  struct MqS * const context,
  struct MqBinaryS * const binary
)
{
  if (unlikely(context->link.send == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    MQ_BIN  in  = binary->data;
    MQ_SIZE len = binary->len;
    struct HdrS const * cur = (struct HdrS const *) in;
    in  += HDR_SIZE;
    len -= HDR_SIZE;
    // send data
    MqSendSTART (context);
    pSendBDY (context, in, len, cur->code);
    switch (cur->code) {
      case MQ_HANDSHAKE_START:
        // used for "MqServiceIsTransaction" to return the right values (aguard)
        context->link.transSId = cur->trans;
      case MQ_HANDSHAKE_TRANSACTION:
        cur->trans != 0 ?
          MqSendEND_AND_WAIT (context, cur->tok, MQ_TIMEOUT_USER) :
            pSendEND(context, cur->tok, 0);
        break;
      case MQ_HANDSHAKE_OK:
      case MQ_HANDSHAKE_ERROR:
        pSendEND (context, cur->tok, cur->trans);
        break;
    }
  }
  return MqErrorStack(context);
}

/*****************************************************************************/
/*                                                                           */
/*                                  public                                   */
/*                                                                           */
/*****************************************************************************/

MQ_SIZE
MqBinarySize (
  struct MqBinaryS *binary
)
{
  return binary->len;
}

void
MqBinaryDelete (
  struct MqBinaryS **binary
)
{
  MqSysFree(*binary);
}

END_C_DECLS

