/**
 *  \file       theLink/libmsgque/send.c
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
#include "send.h"
#include "error.h"
#include "buffer.h"
#include "sys.h"
#include "log.h"
#include "mq_io.h"
#include "token.h"
#include "swap.h"
#include "read.h"
#include "trans.h"
#include "cache.h"
#include "read.h"

BEGIN_C_DECLS

// the 'sprintf' function overwrite the first character after the end with a '\0' character,
// we need to put back the BUFFER_CHAR after every write
// #define SEND_PS(cur,data) 
//     sprintf(cur, MQ_FORMAT_Xn(HDR_ATO_LEN), (MQ_PTRZ) data); 
//     *(cur+HDR_ATO_LEN) = BUFFER_CHAR;

//((MQ_INT*)(ptr)) = data;
//((MQ_STR)(ptr+HDR_INT_LEN)) = BUFFER_CHAR;

#define SEND_I(string,ptr,data) \
    if(unlikely(string)) { \
        sprintf((MQ_STR)ptr, MQ_FORMAT_XI(HDR_INT_LEN), (MQ_INT) data); \
	ptr += HDR_INT_LEN; \
    } else { \
	memcpy (ptr,&data,sizeof(MQ_INT)); \
	memcpy (ptr+4,"\0\0\0\0",sizeof(MQ_INT)); \
	ptr += HDR_INT_LEN; \
    }

#define SEND_IS(char,cur,data,len) \
    sprintf(cur, MQ_FORMAT_XI(len), (MQ_INT) data); \
    char = BUFFER_CHAR;

/*****************************************************************************/
/*                                                                           */
/*                               send_data                                   */
/*                                                                           */
/*****************************************************************************/

#define MQ_CONTEXT_S context

/** -brief special data needed for list objects
 */
struct SendSaveS {
  MQ_INT numItems;		///< number of objects in the msgque body
  struct SendSaveS * save;      ///< this is needed for recursion
  MQ_SIZE cursize;              ///< the current size of \e bdy->data
};

/** -brief everything needed for sending msgque packages
 */
struct MqSendS {
  struct MqS * context;	///< ...
  struct MqBufferS * buf;       ///< the sending buffer object
  struct SendSaveS * save;      ///< need for List objects
  struct MqCacheS * cache;	///< ...
  MQ_BOL haveStart;		///< #MqSendEND checks if #MqSendSTART was used
};

/*****************************************************************************/
/*                                                                           */
/*                               send_static                                 */
/*                                                                           */
/*****************************************************************************/

static struct SendSaveS *
sSendC (
  struct MqS * const context
)
{
  return (struct SendSaveS *) MqSysMalloc (context, sizeof (struct SendSaveS));
}

static void
sSendD (
  struct SendSaveS ** const ptr
)
{
  MqSysFree (*ptr);
}

/*****************************************************************************/
/*                                                                           */
/*                               send_init                                   */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
pSendCreate (
  struct MqS * const context,
  struct MqSendS ** const out
)
{
  register struct MqSendS * const  send = *out = (struct MqSendS *const) MqSysCalloc (context, 1, sizeof (*send));
  register MQ_STR ptr;

  send->context = context;

//MqDLogV(msgque, 0, ">>>>>>>>>>>>> CREATE send<%p>\n", send);

  send->buf = MqBufferCreate (context, 10000);
  send->haveStart = MQ_NO;

  ptr = (MQ_STR) send->buf->data;

  // Format HDR
  memcpy (ptr, "HDR" BUFFER_CHAR_S, HDR_CtxId_S);
  ptr[HDR_BdySize_S - 1] = BUFFER_CHAR;
  ptr[HDR_Token_S - 1] = BUFFER_CHAR;
  ptr[HDR_Trans_S - 1] = BUFFER_CHAR;
  ptr[HDR_Code_S - 1] = BUFFER_CHAR;
  ptr[HDR_SIZE - 1] = BUFFER_CHAR;

  // Format BDY
  ptr += HDR_SIZE;

  memcpy (ptr, "BDY" BUFFER_CHAR_S, BDY_NumItems_S);

  // create the cache data
  pCacheCreate ((CacheCF) sSendC, (CacheDF) sSendD, context, &send->cache);

  return MQ_OK;
}

void
pSendDelete (
  struct MqSendS ** sendP
)
{
  struct MqSendS * const send = *sendP;
  if (unlikely (send == NULL)) return;

  pCacheDelete (&send->cache);
  MqBufferDelete (&send->buf);
  MqSysFree (*sendP);
}

/*****************************************************************************/
/*                                                                           */
/*                               send_atom                                   */
/*                                                                           */
/*****************************************************************************/

static void
sSendLen (
  register struct MqBufferS * buf,
  MQ_SIZE const len,
  enum MqTypeE const type,
  MQ_BOL const isString
)
{
  register MQ_BIN ist = buf->cur.B - BUFFER_P2_PRENUM;

 /* set the type */
  *ist++ = (char) type;
  *ist++ = BUFFER_CHAR;

 /* set the size */
  SEND_I (isString, ist, len);
  *ist++ = BUFFER_CHAR;

 /* close the data */
  ist += len;
  *ist++ = BUFFER_CHAR;

  buf->numItems++;
  buf->cursize = (ist - buf->data);
  buf->cur.B = ist;
}

static enum MqErrorE
sSendA1 (
  struct MqSendS * const send,
  union MqBufferAtomU const in,
  MQ_BOL const isString,
  enum MqTypeE const type
)
{
  if (send == NULL) {
    return MqErrorDbV2(send->context, MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    register struct MqBufferS * buf = send->buf;
    if (unlikely(isString)) {
	MQ_SIZE len=0;
	pBufferAddSize (buf, (BUFFER_P2_PRENUM+50));
	buf->cur.B += BUFFER_P2_PRENUM;
	// MQ_BYTT and MQ_BOLT are using the same basic type "char"
	len = sprintf (buf->cur.C, MQ_FORMAT_Y, in.Y);
	sSendLen(buf, len, MQ_STRT, MQ_YES);
    } else {
	register struct istS {
	  MQ_BINB type;
	  MQ_BINB c0;
	  MQ_BINB B1[1];
	  MQ_BINB c1;
	} *ist;
	pBufferAddSize (buf, (BUFFER_P2_NATIVE+4));
	ist = (struct istS*) buf->cur.B;
	ist->type = type;
	ist->c0	= BUFFER_CHAR;
	ist->B1[0]= in.B1[0];
	ist->c1	= BUFFER_CHAR;
	buf->numItems++;
	buf->cur.B += sizeof(*ist);
	buf->cursize = (buf->cur.B - buf->data);
    }
    return MQ_OK;
  }
}

static enum MqErrorE
sSendA2 (
  struct MqSendS * const send,
  union MqBufferAtomU const in,
  MQ_BOL const isString,
  enum MqTypeE const type
)
{
  if (send == NULL) {
    return MqErrorDbV2(send->context, MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    register struct MqBufferS * buf = send->buf;
    if (unlikely(isString)) {
	MQ_SIZE len=0;
	pBufferAddSize (buf, (BUFFER_P2_PRENUM+50));
	buf->cur.B += BUFFER_P2_PRENUM;
	len = sprintf (buf->cur.C, MQ_FORMAT_S, in.S);
	sSendLen(buf, len, MQ_STRT, MQ_YES);
    } else {
  #pragma pack(1)
	register struct istS {
	  MQ_BINB type;
	  MQ_BINB c0;
  #if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
	  MQ_BINB B2[2];
  #else
	  MQ_SRT  S;
  #endif
	  MQ_BINB c1;
	} *ist;
  #pragma pack()
	pBufferAddSize (buf, (BUFFER_P2_NATIVE+4));
	ist = (struct istS*) buf->cur.B;
	ist->type = type;
	ist->c0	= BUFFER_CHAR;
  #if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
	memcpy(ist->B2, in.B2, 2);
  #else
	ist->S = in.S;
  #endif
	ist->c1	= BUFFER_CHAR;
	buf->numItems++;
	buf->cur.B += sizeof(*ist);
	buf->cursize = (buf->cur.B - buf->data);
    }
    return MQ_OK;
  }
}

static enum MqErrorE
sSendA4 (
  struct MqSendS * const send,
  union MqBufferAtomU const in,
  MQ_BOL const isString,
  enum MqTypeE const type
)
{
  if (send == NULL) {
    return MqErrorDbV2(send->context, MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    register struct MqBufferS * buf = send->buf;
    if (unlikely(isString)) {
	MQ_SIZE len=0;
	pBufferAddSize (buf, (BUFFER_P2_PRENUM+50));
	buf->cur.B += BUFFER_P2_PRENUM;
	if (type == MQ_INTT) {
	  len = sprintf (buf->cur.C, MQ_FORMAT_I, in.I);
	} else {
	  len = sprintf (buf->cur.C, MQ_FORMAT_F, in.F);
	}
	sSendLen(buf, len, MQ_STRT, MQ_YES);
    } else {
  #pragma pack(1)
	register struct istS {
	  MQ_BINB type;
	  MQ_BINB c0;
  #if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
	  MQ_BINB B4[4];
  #else
	  MQ_INT  I;
  #endif
	  MQ_BINB c1;
	} *ist;
  #pragma pack()
	pBufferAddSize (buf, (BUFFER_P2_NATIVE+4));
	ist = (struct istS*) buf->cur.B;
	ist->type = type;
	ist->c0	= BUFFER_CHAR;
  #if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
	memcpy(ist->B4, in.B4, 4);
  #else
	ist->I = in.I;
  #endif
	ist->c1	= BUFFER_CHAR;
	buf->numItems++;
	buf->cur.B += sizeof(*ist);
	buf->cursize = (buf->cur.B - buf->data);
    }
    return MQ_OK;
  }
}

static enum MqErrorE
sSendA8 (
  struct MqSendS * const send,
  union MqBufferAtomU const in,
  MQ_BOL const isString,
  enum MqTypeE const type
)
{
  if (send == NULL) {
    return MqErrorDbV2(send->context, MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    register struct MqBufferS * buf = send->buf;
    if (unlikely(isString)) {
	MQ_SIZE len=0;
	pBufferAddSize (buf, (BUFFER_P2_PRENUM+50));
	buf->cur.B += BUFFER_P2_PRENUM;
	if (type == MQ_DBLT) {
	  len = sprintf (buf->cur.C, MQ_FORMAT_D, in.D);
	} else {
	  len = sprintf (buf->cur.C, MQ_FORMAT_W, in.W);
	}
	sSendLen(buf, len, MQ_STRT, MQ_YES);
    } else {
  #pragma pack(1)
	register struct istS {
	  MQ_BINB type;
	  MQ_BINB c0;
  #if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
	  MQ_BINB B8[8];
  #else
	  MQ_WID  W;
  #endif
	  MQ_BINB c1;
	} *ist;
  #pragma pack()
	pBufferAddSize (buf, (BUFFER_P2_NATIVE+8));
	ist = (struct istS*) buf->cur.B;
	ist->type = type;
	ist->c0	= BUFFER_CHAR;
  #if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
	memcpy(ist->B8, in.B8, 8);
  #else
	ist->W = in.W;
  #endif
	ist->c1	= BUFFER_CHAR;
	buf->numItems++;
	buf->cur.B += sizeof(*ist);
	buf->cursize = (buf->cur.B - buf->data);
    }
    return MQ_OK;
  }
}

enum MqErrorE
MqSendY (
  struct MqS * const context,
  const MQ_BYT in
)
{
  union MqBufferAtomU ato;
  ato.Y = in;
  return sSendA1(context->link.send,ato,context->config.isString,MQ_BYTT);
}

enum MqErrorE
MqSendO (
  struct MqS * const context,
  const MQ_BOL in
)
{
  union MqBufferAtomU ato;
  ato.O = in;
  return sSendA1(context->link.send,ato,context->config.isString,MQ_BOLT);
}

enum MqErrorE
MqSendS (
  struct MqS * const context,
  const MQ_SRT in
)
{
  union MqBufferAtomU ato;
  ato.S = in;
  return sSendA2(context->link.send,ato,context->config.isString,MQ_SRTT);
}

enum MqErrorE
MqSendI (
  struct MqS * const context,
  const MQ_INT in
)
{
  union MqBufferAtomU ato;
  ato.I = in;
  return sSendA4(context->link.send,ato,context->config.isString,MQ_INTT);
}

enum MqErrorE
MqSendF (
  struct MqS * const context,
  const MQ_FLT in
)
{
  union MqBufferAtomU ato;
  ato.F = in;
  return sSendA4(context->link.send,ato,context->config.isString,MQ_FLTT);
}

enum MqErrorE
MqSendW (
  struct MqS * const context,
  const MQ_WID in
)
{
  union MqBufferAtomU ato;
  ato.W = in;
  return sSendA8(context->link.send,ato,context->config.isString,MQ_WIDT);
}

enum MqErrorE
MqSendD (
  struct MqS * const context,
  const MQ_DBL in
)
{
  union MqBufferAtomU ato;
  ato.D = in;
  return sSendA8(context->link.send,ato,context->config.isString,MQ_DBLT);
}

enum MqErrorE
MqSendC (
  struct MqS * const context,
  MQ_CST const in
)
{
  struct MqSendS * const send = context->link.send;
  if (unlikely(send == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    register struct MqBufferS * buf = context->link.send->buf;
    MQ_SIZE len = (in?strlen(in):0);
    pBufferAddSize (buf, BUFFER_P2_PRENUM+len);
    buf->cur.B += BUFFER_P2_PRENUM;
    memcpy (buf->cur.C, in, len);
    sSendLen(buf, len, MQ_STRT, context->config.isString);
    return MQ_OK;
  }
}

enum MqErrorE
MqSendB (
  struct MqS * const context,
  MQ_BINB const * const in,
  MQ_SIZE const len
)
{
  struct MqSendS * const send = context->link.send;
  if (unlikely(send == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    register struct MqBufferS * buf = context->link.send->buf;
    pBufferAddSize (buf, BUFFER_P2_PRENUM+len);
  // MK1
    buf->cur.B += BUFFER_P2_PRENUM;
    memcpy (buf->cur.B, in, len);
    sSendLen (buf, len, MQ_BINT, context->config.isString);
    return MQ_OK;
  }
}

enum MqErrorE
MqSendN (
  struct MqS * const context,
  MQ_BINB const * const in,
  MQ_SIZE const len
)
{
  struct MqSendS * const send = context->link.send;
  if (unlikely(send == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    register struct MqBufferS * const buf = send->buf;
    register const int len2 = len+1;
    pBufferAddSize (buf, len2);
    memcpy (buf->cur.B, in, len);
    buf->numItems++;
    buf->cursize += len2;
    buf->cur.B += len;
    return MQ_OK;
  }
}

enum MqErrorE
MqSendBDY (
  struct MqS * const context,
  MQ_BINB const * const in,
  MQ_SIZE const len
)
{
  struct MqSendS * const send = context->link.send;
  if (unlikely(send == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else if (len != 0) {
    register struct MqBufferS * const buf = send->buf;
    register MQ_SIZE const newlen = sizeof(struct HdrS) + len;
    pBufferNewSize (buf, newlen);
    memcpy (buf->data + sizeof(struct HdrS), in, len);
    buf->cur.B = buf->data + sizeof(struct HdrS) + BDY_NumItems_S;
    // read NumItems -> attention no ENDIAN conversion
    if (unlikely(context->config.isString)) {
      buf->numItems = str2int(buf->cur.C,NULL,16);
    } else {
      buf->numItems = iBufU2INT(buf->cur);
    }
    buf->cursize = newlen;
    buf->cur.B = buf->data + newlen;
  }
  return MQ_OK;
}

enum MqErrorE
MqSendU (
  struct MqS * const context,
  struct MqBufferS * const in
)
{
  struct MqSendS * const send = context->link.send;
  if (unlikely(send == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    // reset current buffer pointer to the data pointer
    in->cur.B = in->data;
    switch (in->type) {
      case MQ_BYTT: return MqSendY (context, in->cur.A->Y); break;
      case MQ_BOLT: return MqSendO (context, in->cur.A->O); break;
      case MQ_SRTT: return MqSendS (context, in->cur.A->S); break;
      case MQ_INTT: return MqSendI (context, in->cur.A->I); break;
      case MQ_FLTT: return MqSendF (context, in->cur.A->F); break;
      case MQ_WIDT: return MqSendW (context, in->cur.A->W); break;
      case MQ_DBLT: return MqSendD (context, in->cur.A->D); break;
      case MQ_STRT: return MqSendC (context, in->cur.C); break;
      case MQ_BINT: return MqSendB (context, in->cur.B, in->cursize); break;
      case MQ_LSTT: {
	MQ_BUF buf;
	MqReadL_START(context,in);
	MqSendL_START(context);
	while (MqReadItemExists(context)) {
	  MqReadU(context,&buf);
	  MqSendU(context,buf);
	}
	MqSendL_END(context);
	MqReadL_END(context);
	break;
      }
      case MQ_RETT: MqPanicSYS(context);
    }
    return MQ_OK;
  }
}

enum MqErrorE
MqSendL (
  struct MqS * const context,
  struct MqBufferLS const * const in
)
{
  struct MqSendS * const send = context->link.send;
  if (unlikely(send == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    register struct MqBufferS * const * start = in->data;
    struct MqBufferS * const * const end = in->data + in->cursize;
    for (; start<end; start++) {
      MqSendU (context, *start);
    }
    return MQ_OK;
  }
}

enum MqErrorE
MqSendVL (
  struct MqS * const context,
  MQ_CST const fmt,
  va_list ap
)
{
  struct MqSendS * const send = context->link.send;
  if (unlikely(send == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    register struct MqBufferS * buf = send->buf;
    MQ_SIZE len;
    pBufferAddSize (buf, 100);
    buf->cur.B += BUFFER_P2_PRENUM;
    buf->cursize += BUFFER_P2_PRENUM;
    len = MqBufferAppendVL (buf, fmt, ap);
    buf->cur.B -= len;
    sSendLen (buf, len, MQ_STRT, context->config.isString);
    return MQ_OK;
  }
}

enum MqErrorE
MqSendV (
  struct MqS * const context,
  MQ_CST const fmt,
  ...
)
{
  enum MqErrorE ret;
  va_list ap;
  va_start (ap, fmt);
  ret = MqSendVL (context, fmt, ap);
  va_end (ap);
  return ret;
}

/*****************************************************************************/
/*                                                                           */
/*                              send_compose                                 */
/*                                                                           */
/*****************************************************************************/

static mq_inline void pSendL_CLEANUP (
  register struct MqS * const context
) {
  if (context->link.send == NULL) return;
  while (context->link.send->save != NULL) { 
    MqSendL_END (context); 
  }
}

static mq_inline void
pSendSTART_CHECK (
  struct MqS * const context
)
{
  if (context->link.send->haveStart == MQ_NO) 
    MqSendSTART(context);
}

enum MqErrorE
MqSendSTART (
  struct MqS * const context
)
{
  if (context->link.send == NULL) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    struct MqSendS * const send = context->link.send;
    register struct MqBufferS * const buf = send->buf;
    register struct HdrS * const cur = (struct HdrS *) buf->data;

    buf->numItems = 0;
    send->haveStart = MQ_YES;
    send->save = NULL;

    // add FLAG data
    if (unlikely(context->config.isString)) {
      buf->type = MQ_STRT;
      SEND_IS (cur->charS, cur->ctxId.S, context->link.ctxId, HDR_INT_LEN);

  //MqDLogX (msgque, __func__, 0, "msgque->ctxId<%i>, cur->ctxId.B<%s>, string<%i>\n", 
  //    msgque->ctxId, cur->ctxId.S, msgque->config.isString);

    } else {
      buf->type = MQ_BINT;
      cur->ctxId.B = context->link.ctxId;

  //MqDLogX (msgque, __func__, 0, "msgque->ctxId<%i>, cur->ctxId.B<%i>, string<%i>\n", 
  //    msgque->ctxId, cur->ctxId.B, msgque->config.isString);
    }

    // intitialize the return-code
    cur->code = MQ_RETURN_OK;

    buf->cur.B = (((MQ_BIN)cur) + sizeof(struct HdrS) + BDY_SIZE);
    buf->cursize = (sizeof(struct HdrS) + BDY_SIZE);
    return MQ_OK;
  }
}

enum MqErrorE
MqSendEND (
  struct MqS * const context,
  MQ_TOK const token
)
{
  pSendL_CLEANUP (context);
  return pSendEND(context, token, 0);
}

enum MqErrorE
pSendEND (
  struct MqS * const context,
  MQ_TOK const token,
  MQ_HDL const trans
)
{
  if (context->link.send == NULL) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    register struct MqSendS * const send = context->link.send;
    register struct MqBufferS * const buf = send->buf;

  //MqDLogV(msgque,0,"trans<%p>\n", trans);

    MQ_SIZE cursize = buf->cursize - HDR_SIZE;
    register struct HdrSendS * const cur = (struct HdrSendS *) buf->data;

    // check if START was used 
    if (send->haveStart == MQ_NO) {
      context->link._trans = 0;
      return MqErrorDb(MQ_ERROR_SEND_END_WITHOUT_START);
    }
    send->haveStart = MQ_NO;

    // write pToken
    memcpy (cur->hdr.tok, token, HDR_TOK_LEN);

    // the Transaction token will allways be send as binary
    cur->hdr.trans = trans;

    // set some data
    if (unlikely(context->config.isString)) {
      if (cursize > BDY_SIZE) {
	  SEND_IS (cur->hdr.charO, cur->hdr.size.S, cursize, HDR_INT_LEN);
	  SEND_IS (cur->hdr.charE, cur->bdy.num.S, buf->numItems, HDR_INT_LEN);
      } else {
	  cursize = 0;
	  SEND_IS (cur->hdr.charO, cur->hdr.size.S, cursize, HDR_INT_LEN);
	  buf->cursize -= BDY_SIZE;
	  buf->cur.B -= BDY_SIZE;
      }
    } else {
      if (cursize > BDY_SIZE) {
	  cur->hdr.size.B = cursize;
	  cur->bdy.num.B = buf->numItems;
      } else {
	  cur->hdr.size.B = 0;
	  buf->cursize -= BDY_SIZE;
	  buf->cur.B -= BDY_SIZE;
      }
    }

  /*
  if (MQ_IS_SERVER(msgque)) {
  MqDLogX (msgque, __func__, 0, "token<%s>, ctxId<%i>, trans<%p>, string<%i>\n", 
      token, cur->hdr.ctxId.B, cur->hdr.trans, msgque->config.isString);
  MqDLogX (msgque, __func__, 0, "body-prefix<%s>\n", buf->data+sizeof(struct HdrS));
  MqDLogX (msgque, __func__, 0, "body-item<%s>\n", buf->data+sizeof(struct HdrS)+sizeof(struct BdyS)+1);
  }

  I1
  printLP(msgque)
    pLogHEX(context, __func__, buf->data, (buf->cursize > 1100 ? 1100 : buf->cursize));
  */

    MqErrorReturn (pIoSend (context->link.io, buf));
  }
}

enum MqErrorE
MqSendEND_AND_WAIT (
  struct MqS * const context,
  MQ_TOK const token,
  MQ_TIME_T const timeout
)
{
  struct MqTransS * const trans = context->link.trans;
  enum MqTransE status;
  static struct MqCallbackS empty = {NULL, NULL, NULL, NULL};
  struct MqReadS * read;

  // 1. create the transaktion
  MQ_HDL transH =  pTransPop (trans, empty);

  // 2. send the message
  pSendL_CLEANUP (context);
  switch (pSendEND (context, token, transH)) {
    case MQ_OK: 
      break; 
    case MQ_ERROR: 
      pTransPush (trans, transH);
      goto error; 
    case MQ_EXIT: 
      pTransPush (trans, transH);
      return MQ_EXIT; 
    case MQ_CONTINUE: 
      pTransPush (trans, transH);
      return MQ_CONTINUE;
  }

  // 3. wait until the transaction has finished
  context->link._trans = 0;
  do {
    switch (MqProcessEvent (context, timeout, MQ_WAIT_ONCE)) {
      case MQ_OK:
	break; 
      case MQ_ERROR: 
	pTransPush (trans, transH);
	goto error; 
      case MQ_EXIT: 
	pTransPush (trans, transH);
	return MQ_EXIT; 
      case MQ_CONTINUE: 
	pTransPush (trans, transH);
	return MQ_CONTINUE;
    }
  }
  while (pTransCheckStart (trans, transH));
  context->link._trans = pTransGetLast (trans, transH);

  // 4. replace 'read'
  read = pTransGetResult (trans, transH);
  if (read) {
    // the next 2 lines are a TRANSACTION -> do not split
    pCachePush (context->link.readCache, context->link.read);
    context->link.read = read;
  }

  // 5. clean transaction
  status = pTransGetStatus (trans, transH);
  pTransPush (trans, transH);
  switch (status) {
    case MQ_TRANS_END: 
      break;
    case MQ_TRANS_CANCEL:
      return MqErrorC (context, __func__, -1, "transaction CANCEL");
    case MQ_TRANS_START:
      MqPanicC (context, __func__, -1, "INTERNAL: invalid transaction return-status 'MQ_TRANS_START'");
  }

  // 6. check returnCode
  switch (pReadGetReturnCode (context)) {
    case MQ_RETURN_OK:
      return MQ_OK;
    case MQ_RETURN_ERROR: {
      MQ_CST msg;
      MqDLogC(context,5,"got ERROR from LINK target\n");
      MqErrorCheck(pRead_RET_START (context));
    // write my HEADER
      MqErrorV (context, "service-call-error", pReadGetReturnNum (context), 
				"<Tok|%s> <Num|%i>\n", token, pReadGetReturnNum (context));
    // write ERROR-STACK
      while (MqReadItemExists (context)) {
	MqErrorCheck (MqReadC (context, &msg));
	pErrorAppendC (context, msg);
      }
      pRead_RET_END (context);
      return MQ_ERROR;
    }
    case MQ_RETURN_TRANSACTION:
      MqPanicSYS (context);
  }

error:
  return MqErrorStack (context);
}

enum MqErrorE
MqSendEND_AND_CALLBACK (
  struct MqS * const context,
  MQ_TOK const token,
  MqTokenF const proc,
  MQ_PTR data,
  MqTokenDataFreeF datafreeF
)
{
//MqDLogX(context,__func__,0,"data<%p>\n", data);
  struct MqCallbackS cb = {proc, data, datafreeF, NULL};
  MQ_HDL transH = pTransPop (context->link.trans, cb);
  pSendL_CLEANUP (context);
  MqErrorReturn (pSendEND (context, token, transH)); 
}

enum MqErrorE
pSendSYSTEM_RETR (
  struct MqS * const context
)
{
  MqErrorReturn (pSendEND (context, "_SRT", context->link._trans));
}

/*****************************************************************************/
/*                                                                           */
/*                              SendList                                     */
/*                                                                           */
/*****************************************************************************/

static void
pSendListStart (
  struct MqS * const context
)
{
  register struct MqSendS * const send = context->link.send;
  register struct MqBufferS * const buf = send->buf;
  register struct SendSaveS * save;

  pBufferAddSize (buf, (BUFFER_P2_PRENUM + HDR_INT_LEN + 1));
// MK1
  buf->cursize += BUFFER_P2_PRENUM;
  buf->cur.B += BUFFER_P2_PRENUM;

  // zustand sichern !
  save = (struct SendSaveS*) pCachePop (send->cache);
  save->save = send->save;
  save->numItems = buf->numItems;
  save->cursize = buf->cursize;

  send->save = save;

  // Init
  buf->numItems = 0;
  buf->cur.B += HDR_INT_LEN+1;
  buf->cursize += HDR_INT_LEN+1;
}

static void
pSendListEnd (
  struct MqS * const context,
  enum MqTypeE const type
)
{
  register struct MqSendS * const send = context->link.send;
  register struct SendSaveS * const save = send->save;
  if (save != NULL) {
    register struct MqBufferS * const buf = send->buf;
    register MQ_BIN ist = (buf->data + save->cursize);
    MQ_BOL const isString = context->config.isString;
    MQ_INT const bodyLen = (MQ_INT) (buf->cursize - save->cursize);

    // set numItems
    SEND_I (isString, ist, buf->numItems);

    // restore "save" 
    send->save = save->save;
    buf->numItems = save->numItems;
    buf->cursize = save->cursize;
    buf->cur.B = buf->data + buf->cursize;

    // master setzen
    sSendLen (buf, bodyLen, type, isString);

    // free memory /ATTENTION send->save will be set ABOVE
    pCachePush (send->cache, save);
  }
}

enum MqErrorE
MqSendL_START (
  struct MqS * const context
)
{
  if (context->link.send == NULL) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    pSendListStart (context);
    return MQ_OK;
  }
}

enum MqErrorE
MqSendL_END (
  struct MqS * const context
)
{
  if (context->link.send == NULL) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    pSendListEnd (context, MQ_LSTT);
    return MQ_OK;
  }
}

/*****************************************************************************/
/*                                                                           */
/*                              send_RET                                     */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
MqSendERROR (
  register struct MqS * const context
)
{
  if (MqErrorGetCodeI(context) == MQ_ERROR) {
    pSendL_CLEANUP (context);
    pReadL_CLEANUP (context);
    MqErrorCheck(MqSendSTART (context));
    MqSendI (context, MqErrorGetNumI (context));
    MqSendC (context, MqErrorGetText (context));
    MqErrorReset (context);
    return MqSendEND (context, "_ERR");
error:
    return MqErrorStack(context);
  } else {
    return MQ_OK;
  }
}

enum MqErrorE
MqSendRETURN (
  struct MqS * const context
)
{
  if (context->link._trans == 0) {
    return MqErrorGetCodeI(context);
  } else {
    pSendL_CLEANUP (context);
    pReadL_CLEANUP (context);
    switch (MqErrorGetCodeI (context)) {
      case MQ_OK: 
	break;
      case MQ_ERROR:
	MqErrorCheck(MqSendSTART (context));
	MqDLogC(context,5,"send ERROR to LINK target and RESET\n");
	*(context->link.send->buf->data+HDR_Code_S) = (char) MQ_RETURN_ERROR;
	pSendListStart (context);
	MqSendI (context, MqErrorGetNumI (context));
	MqSendC (context, MqErrorGetText (context));
	pSendListEnd (context, MQ_RETT);
	MqErrorReset (context);
	break;
      case MQ_EXIT:
	return MQ_EXIT;
      case MQ_CONTINUE:
	MqPanicSYS (context);
    }
    pSendSTART_CHECK(context);
    return pSendEND (context, "_RET", context->link._trans);
error:
    return MqErrorStack(context);
  }
}

END_C_DECLS

