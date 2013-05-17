/**
 *  \file       theLink/libmsgque/read.c
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
/*                                read_init                                  */
/*                                                                           */
/*****************************************************************************/

#define MQ_CONTEXT_S context

/*****************************************************************************/
/*                                                                           */
/*                                read_native                                */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*                               read_static                                 */
/*                                                                           */
/*****************************************************************************/

static struct MqReadS*
sReadC (
  struct MqS * context
)
{
  return (struct MqReadS *) MqSysMalloc (context, sizeof (struct ReadSaveS));
}

static void
sReadD (
  struct MqReadS ** const ptr
)
{
  MqSysFree (*ptr);
}

///pReadCreate have to fit into 'CacheCF' function
struct MqReadS*
pReadCreate (
  struct MqS * const context
)
{
  register struct MqReadS * const read = (struct MqReadS *const) MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof (*read));
  read->context = context;

//MqDLogV(read->context,__func__,-1,"read<%p>\n", read);

  read->hdr = MqBufferCreate (context, HDR_SIZE + 1);
  read->readBuf = MqBufferCreate (context, 10000);
  read->tranBuf = MqBufferCreate (context, 256);
  read->canUndo = MQ_NO;
  read->handShake = MQ_HANDSHAKE_START;
  read->bdy = read->readBuf;

  // references
  read->cur = pBufferCreateRef (read->bdy);

  read->hdr->cursize = HDR_SIZE;

  // create the lst Cache data
  pCacheCreate ((CacheCF) sReadC, (CacheDF) sReadD, context, &read->saveCache);

  // cache the current package-type
  read->type = MQ_STRING_TYPE(context->config.isString);

  return read;
}

void
pReadDelete (
  struct MqReadS ** readP
)
{
  struct MqReadS * read = *readP;

  if (unlikely (read == NULL)) return;

  // rollback all open LST/RET blocks
  while (read->save != NULL) {
    MqReadL_END (read->context);
  }

  // delete cache
  pCacheDelete (&read->saveCache);

  MqBufferDelete (&read->hdr);
  MqBufferDelete (&read->readBuf);
  MqBufferDelete (&read->tranBuf);

  // references
  pBufferDeleteRef (&read->cur);

  MqSysFree (*readP);
}

static void
sReadListStart (
  struct MqS * const context,
  MQ_BUF buf
)
{
  register struct MqReadS * const read = context->link.read;
  register struct ReadSaveS * const save = (struct ReadSaveS *) pCachePop (read->saveCache);
  register struct MqBufferS * const bdy = read->bdy;

  save->save = read->save;
  save->bdy  = *bdy;
  read->save = save;

  bdy->type = read->type;
  bdy->data = bdy->cur.B = buf->data;
  bdy->size = buf->size;
  bdy->cursize = buf->cursize;
  bdy->numItems = U2INT(read->type==MQ_BINT, bdy->cur);
  bdy->cur.B += (HDR_INT_LEN + 1);
}

static void
sReadListEnd (
  struct MqS * const context
)
{
  register struct MqReadS * const read = context->link.read;
  struct ReadSaveS * const save = read->save;
  if (unlikely(save == NULL)) return;
  read->save = save->save;
  *read->bdy = save->bdy;
//MqDLogV(context,0,"read<%p>, save<%p>\n", read, save);
  pCachePush (read->saveCache, save);
}

enum MqErrorE
MqReadL_START (
  struct MqS * const context,
  struct MqBufferS * buf
)
{
  register struct MqReadS * const read = context->link.read;
  if (unlikely(read == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else if (buf == NULL && MqErrorCheckI(MqReadU(context,&buf))) {
    return MqErrorStack(context);
  } else if (buf->type != MQ_LSTT) {
    return MqErrorDbV(MQ_ERROR_TYPE, MqLogTypeName(MQ_LSTT), MqLogTypeName(buf->type));
  } else {
    sReadListStart (context, buf);
    return MQ_OK;
  }
}

enum MqErrorE
MqReadL_END (
  struct MqS * const context
)
{
  register struct MqReadS * const read = context->link.read;
  if (unlikely(read == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    sReadListEnd (context);
    return MQ_OK;
  }
}

enum MqErrorE
MqReadT_START (
  struct MqS * const context
)
{
  register struct MqReadS * const read = context->link.read;
  if (unlikely(read == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else if (read->transLId == 0LL) {
    return MqErrorDb(MQ_ERROR_TRANSACTION_REQUIRED);
  } else {
    read->bdy = read->tranBuf;
    return MQ_OK;
  }
}

enum MqErrorE
MqReadT_END (
  struct MqS * const context
)
{
  register struct MqReadS * const read = context->link.read;
  if (unlikely(read == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    read->bdy = read->readBuf;
    return MQ_OK;
  }
}

void pReadL_CLEANUP (
  register struct MqS * const context
)
{
  while (context->link.read->save != NULL) {
    MqReadL_END (context);
  }
}

/*****************************************************************************/
/*                                                                           */
/*                                read_native                                */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
pReadCreateTransId (
  register struct MqS * const context
)
{
  struct MqReadS * read = context->link.read;
  MqErrorCheck (pReadT (context, &read->transLId));
  MqErrorCheck (pSqlSelectSendTrans (context, read->transLId, read->tranBuf));
  return MQ_OK;
error:
  return MqErrorStack(context);
}

enum MqErrorE
pReadDeleteTransId (
  register struct MqS * const context
)
{
  struct MqReadS * read = context->link.read;
  return pSqlDeleteSendTrans (context, read->transLId, &read->transLId);
}

enum MqErrorE
MqStorageInsert (
  struct MqS * const context,
  MQ_TRA *transLIdP
)
{
  struct MqReadS * read = context->link.read;
  if (context->bits.onCallback == MQ_NO) {
    return MqErrorDb(MQ_ERROR_CALLBACK_REQUIRED);
  } else if (unlikely(read->transLId != 0LL)) {
    // this request is a logterm-transaction-request and this request is
    // already stored into the database
    if (transLIdP != NULL) *transLIdP = read->transLId;
    // the current transaction is now in the storage and have to be deleted.
    // the MqStorageSelect will setup the transaction again
    read->transLId = 0LL;
    read->rmtTransId = 0LL;
    read->handShake = MQ_HANDSHAKE_START;
  } else {
    // this request is a shortterm-transaction-request and this request need
    // to be saved into the database
    MqErrorCheck (
      pSqlInsertReadTrans (context, 
	0LL,		    /* old transaction id (used for "stack" transaction) */
	0LL,		    /* old remote transaction id (used for "stack" transaction) */
	read->hdrorig,	    /* header data used by MqReadDUMP (size = HDR_SIZE) */
	read->bdy,	    /* body data (size = bdy->cursize) */
	transLIdP
      )
    );
  }
  return MQ_OK;
error:
  return MqErrorStack(context);
}

enum MqErrorE
MqStorageDelete (
  struct MqS * const context,
  MQ_TRA transLId
)
{
  //if (context->link.read->transLId != transLId) {
    MQ_WID oldTransIdDummy, oldRmtTransIdDummy;
    // a longterm-transaction-request will be deleted in MqSendRETURN or MqSendERROR using
    // pReadDeleteTrans. Only the shortterm-transaction-request need to be deleted.
    // do not change the longterm-transaction-request data, because in the mean-time between
    // MqStoreInsert and MqStoreDelete a new longterm-transaction-request could be arrived
    MqErrorCheck(pSqlDeleteReadTrans(context,transLId,&oldTransIdDummy,&oldRmtTransIdDummy));
  //}
  return MQ_OK;
error:
  return MqErrorStack(context);
}

enum MqErrorE
pReadHDR (
  MQ_PTR env,
  struct MqS ** a_context
)
{
  register struct MqS * context = *a_context;
  register struct MqReadS * read = context->link.read;
  register int const string = context->config.isString;
  register struct HdrS * cur;
  register struct MqBufferS * bdy;
  MQ_SIZE ctxId, size;
  int debug;
  struct MqBufferS * hdr = read->hdr;

  // 2. read
  MqErrorSwitch (pIoRead (context->link.io, hdr, HDR_SIZE));
  cur = (struct HdrS *) hdr->cur.B;

//MqDLogV (context, __func__, 0, "START (%c%c%c%c)\n", cur->tok[0], cur->tok[1], cur->tok[2], cur->tok[3]);
//pReadLog(context, "pReadHDR->1");

  // check "HDR"
#ifdef MQ_SAVE
  if (unlikely (strncmp (cur->ID, "HDR", 3))) {
    MQ_BUF save = MqBufferCreate (MQ_ERROR_PANIC, 1000);
    do {
      MqBufferAppendC (save, (MQ_STR) hdr->data);
    } while (pIoRead (context->link.io, hdr, HDR_SIZE) == MQ_OK);
    *(hdr->data+hdr->cursize) = '\0';
    MqBufferAppendC (save, (MQ_STR) hdr->data);
    MqErrorDb (MQ_ERROR_INVALID_HEADER);
    MqErrorSAppendV (context, "but got: %s", (MQ_STR) save->data);
    MqBufferDelete (&save);
    return MQ_ERROR;
  }
#endif

  // 3. read binary data
  if (unlikely(string)) {
    ctxId  = str2int(cur->ctxId.S,NULL,16);
    size   = str2int(cur->size.S,NULL,16);
  } else {
    ctxId  = cur->ctxId.B;
    size   = cur->size.B;
    // 3.b fix byte-order
    if (context->link.bits.endian) {
      pSwap4 (&ctxId);
      pSwap4 (&size);
    }
  }

  // 4. set my context
  if (ctxId != context->link.ctxId) {
    // check if ctxId is out of range
    if (ctxId > context->link.ctxIdP->link.ctxIdR) 
	return MqErrorV (context, __func__, -1, "invalid context-id '%i'", ctxId);
    // do the context-switch if the message does !!not!! belongs to the
    // original context
    context = context->link.ctxIdP->link.ctxIdA[ctxId];
    // check if entry was already deleted
    if (context == NULL) return MQ_CONTINUE;
    *a_context = context;
  }

//MqDLogV (context, __func__, 0, "cur->ctxId.B<%i>, context->context.ctxId<%i>\n", cur->ctxId.B, context->context.ctxId);
//MqDLogV (context, __func__, 0, "hex\n<%s>\n", pLogHEX (hdr->data, sizeof (struct HdrS)));
  debug = context->config.debug;
  context->link.transSId = cur->trans;

  // 6. log message
  if (unlikely (debug >= 5)) {
    hdr->type = MQ_STRING_TYPE(string);
    pLogHDR (context, __func__, 5, hdr);
  }

  // 7. setup read
  read = context->link.read;
  read->handShake = (enum MqHandShakeE) cur->code;
  read->returnNum = 0;
  // MqReadDUMP need the original "header", save this header in the current context
  read->hdrorig = hdr;
  bdy = read->bdy = read->readBuf;

  // 8. read token
  pTokenSetCurrent (context->link.srvT, cur->tok);

  // 9. read BDY
  bdy->cursize = size;
  if (size > 0) {
    bdy->type = MQ_STRING_TYPE(string);
    if (unlikely (debug >= 6))
      MqLogV (context, __func__, 6, "BDY -> " MQ_FORMAT_Z " bytes\n", size);

    // 1. read
    MqErrorSwitch (pIoRead (context->link.io, bdy, bdy->cursize));

    // 2. read 'BDY' header
#ifdef MQ_SAVE
    if (unlikely (strncmp (bdy->cur.C, "BDY", 3))) {
      return MqErrorV (context, __func__, "expect 'BDY' but got '%s'", bdy->cur.C);
    }
#endif
    bdy->cur.B += BDY_NumItems_S;

    // 3. read NumItems
    if (unlikely(string)) {
      bdy->numItems = str2int(bdy->cur.C,NULL,16);
    } else {
      if (context->link.bits.endian) {
	pSwapBDY(bdy->data);
      }
      bdy->numItems = iBufU2INT(bdy->cur);
    }
    bdy->cur.B = (bdy->data + BDY_SIZE);

    // 4. if required, log package
    if (unlikely (debug >= 7 && size > BDY_SIZE))
      pLogBDY (context, __func__, 7, bdy);

    // 5. if in a longterm-transaction, read the transaction-item -> only on the !server!
    if (unlikely(read->handShake == MQ_HANDSHAKE_TRANSACTION)) {
      MQ_TRA rmtTransId, transLId;
      MqErrorCheck (pReadT (context, &rmtTransId));
      // use temporary variable "transLId" because the following "MqSendSTART" have to send data 
      // !without! transaction id
      if ( 
	MqErrorCheckI (
	  // collect all data necessary to setup the transaction !after! a crash
	  pSqlInsertReadTrans (context, 
	    read->transLId,		  /* old transaction id (used for "stack" transaction) */
	    read->rmtTransId,		  /* old remote transaction id (used for "stack" transaction) */
	    hdr,			  /* header data used by MqReadDUMP (size = HDR_SIZE) */
	    bdy,			  /* body data (size = bdy->cursize) */
	    &transLId
	  )
	) 
      ) {
	// on error, just return the error
	// now the "transLId" is "official" in use
	read->transLId = transLId;
	read->rmtTransId = rmtTransId;
	MqSendSTART(context);
	return MqSendRETURN (context);
      };
      // answer first call with an empty return package
      // a short-term-transaction (MqSendEND_AND_WAIT) require an answer.
      if (context->link.transSId != 0) {
	enum MqErrorE ret;
	read->transLId = 0LL;
	MqErrorCheck (MqSendSTART  (context));
	read->handShake = MQ_HANDSHAKE_START;
	ret = MqSendRETURN (context);
	read->handShake = MQ_HANDSHAKE_TRANSACTION;
	MqErrorCheck (ret);
	context->link.transSId = 0;
      }
      // now the "transLId" is "official" in use
      read->transLId = transLId;
      read->rmtTransId = rmtTransId;
    } else {
      read->transLId = 0LL;
      read->rmtTransId = 0LL;
    }
  }

  // reset data
  read->canUndo = MQ_NO;
  read->save = NULL;
  
  // 11. check if it's a System-pToken
  // this line have to use the 'return' because pTokenCheckSystem usually return
  // MQ_CONTINUE if a system token (_*) was found
  return pTokenCheckSystem (context->link.srvT);
error:
  return MqErrorStack (context);
}

static enum MqErrorE
sReadGEN (
  register struct MqS *context,
  MQ_TRA  transLId,
  MQ_BOL  string,
  MQ_BOL  endian,
  MQ_BIN  hdrB,
  MQ_SIZE hdrlen,
  MQ_BIN  bdyB,
  MQ_SIZE bdylen
)
{
  register struct MqReadS * read = context->link.read;
  register struct HdrS * cur;
  int debug = context->config.debug;
  struct MqBufferS * bdy;

  // 1. check environment context
  if (string != context->config.isString) {
    return MqErrorC(context,__func__,-1,"invalid configuration -> isString");
  }

  // 2. MqReadDUMP need the original "header", save this header in the current context
  read->hdrorig = MqBufferSetB (read->hdr, hdrB, hdrlen);
  cur = (struct HdrS*) read->hdrorig->data;

  // 3. log message
  if (unlikely (debug >= 5)) {
    read->hdr->type = MQ_STRING_TYPE(string);
    pLogHDR (context, __func__, 5, read->hdr);
  }

  // 4. setup read
  read->handShake = (enum MqHandShakeE) cur->code;
  read->returnNum = 0;

  // 5. after load from storage the "OLD" shortterm-transaction is invalid.
  // but we need the information later to forward data proper.
  // choose "-1" to signal this
  context->link.transSId = cur->trans > 0 ? -1 : 0;

  // 6. setup "bdy"
  bdy = read->bdy = MqBufferSetB(read->readBuf, bdyB, bdylen);

  // 7. read token
  pTokenSetCurrent (context->link.srvT, cur->tok);

  // 8. read BDY
  if (bdylen > 0) {
    bdy->type = MQ_STRING_TYPE(string);
    if (unlikely (debug >= 6)) {
      MqLogV (context, __func__, 6, "BDY -> " MQ_FORMAT_Z " bytes\n", bdylen);
    }
    bdy->cur.B += BDY_NumItems_S;

    // 8a. read NumItems
    if (unlikely(string)) {
      bdy->numItems = str2int(bdy->cur.C,NULL,16);
    } else {
      // if endian has changed -> swap bdy again (first swap was done in pReadHDR)
      if (endian != context->link.bits.endian) {
	pSwapBDY(bdy->data);
      }
      // body data already swapped !! (pSwapBDY...)
      bdy->numItems = iBufU2INT(bdy->cur);
    }
    bdy->cur.B = (bdy->data + BDY_SIZE);

    // 8b. if required, log package
    if (unlikely (debug >= 7 && bdylen > BDY_SIZE)) {
      pLogBDY (context, __func__, 7, bdy);
    }

    // 8c. if in a longterm-transaction, read the transaction-item on the !server!
    if (unlikely(read->handShake == MQ_HANDSHAKE_TRANSACTION)) {
      MQ_TRA rmtTransId;
      MqErrorCheck (pReadT (context, &rmtTransId));
      // now the "transLId" is "official" in use
      read->transLId = transLId;
      read->rmtTransId = rmtTransId;
    } else {
      read->transLId = 0LL;
      read->rmtTransId = 0LL;
    }
  }

  // 9. reset data
  read->canUndo = MQ_NO;
  read->save = NULL;
  
  return MQ_OK;
error:
  return MqErrorStack (context);
}

enum MqErrorE
pReadTRA (
  MQ_PTR env,
  struct MqS ** a_context
)
{
  register struct MqS * context = *a_context;
  if (unlikely(context->link.sql == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    // called by "pSqlSelectReadTrans", hdl as environment
    register sqlite3_stmt *hdl= (sqlite3_stmt*) env;

    // process data from database
    return sReadGEN (context, 
      (MQ_TRA)  sqlite3_column_int64(hdl,0),	/* transLId  */
      (MQ_BOL)  sqlite3_column_int(hdl,1),	/* string   */
      (MQ_BOL)  sqlite3_column_int(hdl,2),	/* endian   */
      (MQ_BIN)  sqlite3_column_blob(hdl,3),	/* hdrB	    */
      (MQ_SIZE) sqlite3_column_bytes(hdl,3),	/* hdrlen   */
      (MQ_BIN)  sqlite3_column_blob(hdl,4),	/* bdyB	    */
      (MQ_SIZE) sqlite3_column_bytes(hdl,4)	/* bdylen   */
    );
  }
}

/*****************************************************************************/
/*                                                                           */
/*                                read_dump                                  */
/*                                                                           */
/*****************************************************************************/

#pragma pack(1)
struct MqDumpS {
  MQ_INT      signature;
  MQ_SIZE     size;
  MQ_TRA      transLId;
  MQ_BOL      isString;
  MQ_BOL      endian;
  MQ_SIZE     bdylen;
  MQ_SIZE     hdrlen;
  struct HdrS hdr;
  struct BdyS bdy;
};
#pragma pack()

const static MQ_SIZE DMP_STA = 0;
const static MQ_SIZE DMP_HDR = sizeof(struct MqDumpS);
const static MQ_SIZE DMP_BDY = sizeof(struct MqDumpS) + sizeof(struct HdrS);

enum MqErrorE
pReadLOAD (
  MQ_PTR env,
  struct MqS ** a_context
)
{
  register struct MqS * context = *a_context;
  if (unlikely(context->link.read == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    // called by "pSqlSelectReadTrans", hdl as environment
    register struct MqDumpS * dump = (struct MqDumpS*) env;

    if (dump->signature != MQ_MqDumpS_SIGNATURE) {
      return MqErrorDbV(MQ_ERROR_VALUE_INVALID, "signature", "MqDumpS");
    }

    // process data from MqDumpS buffer
    return sReadGEN (context, 
      dump->transLId,
      dump->isString,
      dump->endian, 
      (MQ_BIN) &dump->hdr,
      dump->hdrlen,
      (MQ_BIN) &dump->bdy,
      dump->bdylen
    );
  }
}

enum MqErrorE
MqReadDUMP (
  struct MqS * const context,
  struct MqDumpS ** const out
)
{
  struct MqReadS * const read = context->link.read;
  if (unlikely(read == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    MQ_BUF hdr = read->hdrorig;
    MQ_BUF bdy = read->bdy;
    MQ_SIZE const llen = sizeof(struct MqDumpS) + read->bdy->cursize - sizeof(struct BdyS);
    register struct MqDumpS *dump = *out = (struct MqDumpS *) MqSysMalloc (context, llen);
    check_NULL(dump) return MqErrorStack(context);
    dump->signature   =	MQ_MqDumpS_SIGNATURE;
    dump->size	      = llen;
    dump->transLId    =	read->transLId;
    dump->isString    =	context->config.isString;
    dump->endian      = context->link.bits.endian;
    dump->hdrlen      =	hdr->cursize;
    dump->hdr	      = *(struct HdrS*)hdr->data;
    dump->bdylen      =	bdy->cursize;
    memcpy (&dump->bdy, bdy->data, bdy->cursize);
    // in a "longterm-transaction" with "MqReadDUMP" no return transaction is
    // possible because the transaction is stored or forwarded
    // with "MQ_HANDSHAKE_TRANSACTION" the transaction will be deleted
    // in "MqSendRETURN"
    read->handShake = MQ_HANDSHAKE_START;
    return MQ_OK;
  }
}

enum MqErrorE
MqReadLOAD (
  struct MqS * const context,
  struct MqDumpS * const in
)
{
  MqErrorCheck(pReadLOAD((MQ_PTR)in, (struct MqS**) &context));
  return MQ_OK;
error:
  return MqErrorStack(context);
}

MQ_SIZE
MqDumpSize (
  struct MqDumpS * const dump
)
{
  return dump->size;
}

int
MqDumpIsTransaction (
  struct MqDumpS * const dump
)
{
    return (dump->hdr.trans != 0);
}

MQ_STRB
MqDumpGetHandShake (
  struct MqDumpS * const dump
)
{
    return dump->hdr.code;
}

/*****************************************************************************/
/*                                                                           */
/*                                read_atom                                  */
/*                                                                           */
/*****************************************************************************/

#define pReadWordM(context, buf, out) { \
  register union MqBufferU bcur = buf->cur; \
  MQ_SIZE lsize; \
  /* check if an argument is available? and decrease the number of arguments */ \
  /* buf->numItems have to be an \b int !! because --0 have to be < 0 */ \
  if ((--buf->numItems) < 0) { \
    buf->numItems = 0; \
    return MqErrorDb2 (context,MQ_ERROR_REQUEST_ARGUMENTS); \
  } \
 \
  /* read "type" of the value */ \
  out->type = (enum MqTypeE) *bcur.B; \
  bcur.B += 2; \
 \
  /* read "length" of the value */ \
  lsize = (out->type & MQ_TYPE_IS_NATIVE); \
  /* if type is native than "lsize" has the length of the native type */ \
  /* if type is *not* native than "lsize" has the value of '0' */ \
  if (lsize == 0) { \
    /* if *not* native, one additional field is available -> the length */ \
    lsize = U2INT(buf->type==MQ_BINT,bcur); \
    bcur.B += (HDR_INT_LEN + 1); \
  } \
 \
  /* read argument */ \
  out->data = out->cur.B = bcur.B; \
 \
  bcur.B += (out->cursize = out->size = lsize) + 1; \
  buf->cur = bcur; \
}

enum MqErrorE
pReadWord (
  struct MqS * const context,
  struct MqBufferS * const buf,
  register struct MqBufferS * const out
)
{
  pReadWordM(context, buf, out);

/*
  register union MqBufferU bcur = buf->cur;
  MQ_SIZE lsize;
  // check if an argument is available? and decrease the number of arguments //
  // buf->numItems have to be an \b int !! because --0 have to be < 0 //
  if ((--buf->numItems) < 0) {
    return MqErrorDb2 (context,MQ_ERROR_REQUEST_ARGUMENTS);
  }

  // read "type" of the value //
  out->type = (enum MqTypeE) *bcur.B;
  bcur.B += 2;

  // read "length" of the value //
  lsize = (out->type & MQ_TYPE_IS_NATIVE);
  // if type is native than "lsize" has the length of the native type //
  // if type is *not* native than "lsize" has the value of '0' //
  if (lsize == 0) {
    lsize = U2INT(buf->type==MQ_BINT,bcur);
    bcur.B += (HDR_INT_LEN + 1);
  }

  // read argument //
  out->data = out->cur.B = bcur.B;

  bcur.B += (out->cursize = out->size = lsize);
  *bcur.B++ = '\0';
  buf->cur = bcur;
*/

  return MQ_OK;
}

static enum MqErrorE
sReadA1 (
  struct MqS * const context,
  union MqBufferAtomU * const out,
  enum MqTypeE const type
)
{
  struct MqReadS * const read = context->link.read;
  if (unlikely(read == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    register struct MqBufferS * const buf = read->bdy;
    register struct MqBufferS * const cur = read->cur;
    pReadWordM(context, buf, cur);
    //MqErrorCheck( pReadWord (context, buf, cur));
    MqErrorCheck( pBufferGetA1 (cur, out, type));
    read->canUndo = MQ_YES;
    return MQ_OK;

error:
    return MqErrorStack (context);
  }
}

static enum MqErrorE
sReadA2 (
  struct MqS * const context,
  union MqBufferAtomU * const out,
  enum MqTypeE const type
)
{
  struct MqReadS * const read = context->link.read;
  if (unlikely(read == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    register struct MqBufferS * const buf = read->bdy;
    register struct MqBufferS * const cur = read->cur;
    pReadWordM(context, buf, cur);
    //MqErrorCheck( pReadWord (context, buf, cur));
    MqErrorCheck( pBufferGetA2 (cur, out, type));
    read->canUndo = MQ_YES;
    return MQ_OK;

error:
    return MqErrorStack (context);
  }
}

static enum MqErrorE
sReadA4 (
  struct MqS * const context,
  union MqBufferAtomU * const out,
  enum MqTypeE const type
)
{
  struct MqReadS * const read = context->link.read;
  if (unlikely(read == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    register struct MqBufferS * const buf = read->bdy;
    register struct MqBufferS * const cur = read->cur;
    pReadWordM(context, buf, cur);
    //MqErrorCheck( pReadWord (context, buf, cur));
    MqErrorCheck( pBufferGetA4 (cur, out, type));
    read->canUndo = MQ_YES;
    return MQ_OK;

error:
    return MqErrorStack (context);
  }
}

static enum MqErrorE
sReadA8 (
  struct MqS * const context,
  union MqBufferAtomU * const out,
  enum MqTypeE const type
)
{
  struct MqReadS * const read = context->link.read;
  if (unlikely(read == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    register struct MqBufferS * const buf = read->bdy;
    register struct MqBufferS * const cur = read->cur;
    pReadWordM(context, buf, cur);
    //MqErrorCheck( pReadWord (context, buf, cur));
    MqErrorCheck( pBufferGetA8 (cur, out, type));
    read->canUndo = MQ_YES;
    return MQ_OK;

error:
    return MqErrorStack (context);
  }
}

/*****************************************************************************/
/*                                                                           */
/*                               read public                                 */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
MqReadL (
  struct MqS * const context,
  struct MqBufferLS ** const out
)
{
  struct MqReadS * const read = context->link.read;
  if (unlikely(read == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    struct MqBufferLS * const line = (*out == NULL ? MqBufferLCreate (read->bdy->numItems) : *out);
    struct MqBufferS * buf = NULL;
    while (read->bdy->numItems) {
      MqErrorCheck (MqReadU (context, &buf));
      MqBufferLAppend (line, MqBufferDup(buf), -1);
    }
    *out = line;
    read->canUndo = MQ_NO;
    return MQ_OK;
error:
    return MqErrorStack (context);
  }
}

enum MqErrorE
MqReadY (
  struct MqS * const context,
  MQ_BYT * const valP
)
{
  return sReadA1(context, (union MqBufferAtomU * const) valP, MQ_BYTT);
}

enum MqErrorE
MqReadO (
  struct MqS * const context,
  MQ_BOL * const valP
)
{
  return sReadA1(context, (union MqBufferAtomU * const) valP, MQ_BOLT);
}

enum MqErrorE
MqReadS (
  struct MqS * const context,
  MQ_SRT * const valP
)
{
  return sReadA2(context, (union MqBufferAtomU * const) valP, MQ_SRTT);
}

enum MqErrorE
MqReadI (
  struct MqS * const context,
  MQ_INT * const valP
)
{
  return sReadA4(context, (union MqBufferAtomU * const) valP, MQ_INTT);
}

enum MqErrorE
MqReadF (
  struct MqS * const context,
  MQ_FLT * const valP
)
{
  return sReadA4(context, (union MqBufferAtomU * const) valP, MQ_FLTT);
}

enum MqErrorE
MqReadW (
  struct MqS * const context,
  MQ_WID * const valP
)
{
  return sReadA8(context, (union MqBufferAtomU * const) valP, MQ_WIDT);
}

enum MqErrorE
pReadT (
  struct MqS * const context,
  MQ_TRA * const valP
)
{
  return sReadA8(context, (union MqBufferAtomU * const) valP, MQ_TRAT);
}

enum MqErrorE
MqReadD (
  struct MqS * const context,
  MQ_DBL * const valP
)
{
  return sReadA8(context, (union MqBufferAtomU * const) valP, MQ_DBLT);
}

enum MqErrorE
MqReadC (
  struct MqS * const context,
  MQ_CST * const out
)
{
  struct MqReadS * const read = context->link.read;
  if (unlikely(read == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    register struct MqBufferS * const buf = read->bdy;
    register struct MqBufferS * const cur = read->cur;
    pReadWordM(context, buf, cur);
    MqErrorCheck (MqBufferGetC (cur, out));
    read->canUndo = MQ_YES;
    return MQ_OK;
error:
    return MqErrorStack (context);
  }
}

enum MqErrorE
MqReadB (
  struct MqS * const context,
  MQ_BIN * const out,
  MQ_SIZE * const len
)
{
  struct MqReadS * const read = context->link.read;
  if (unlikely(read == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    register struct MqBufferS * const buf = read->bdy;
    register struct MqBufferS * const cur = read->cur;
    pReadWordM(context, buf, cur);
    *out = cur->data;
    *len = cur->cursize;
    read->canUndo = MQ_YES;
    return MQ_OK;
  }
}

enum MqErrorE
MqReadN (
  struct MqS * const context,
  MQ_CBI  * const out,
  MQ_SIZE * const len
)
{
  struct MqReadS * const read = context->link.read;
  if (unlikely(read == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    register struct MqBufferS * const buf = read->bdy;
    register union MqBufferU bcur = buf->cur;
    union MqBufferU bstart = bcur;
    MQ_SIZE lsize;
    enum MqTypeE type;

    /* check if an argument is available? and decrease the number of arguments */
    /* buf->numItems have to be an \b int !! because --0 have to be < 0 */
    if ((--buf->numItems) < 0) {
      return MqErrorDb2 (context,MQ_ERROR_REQUEST_ARGUMENTS);
    }
   
    /* read "type" of the value */
    type = (enum MqTypeE) *bcur.B;
    bcur.B += 2;
 
    /* read "length" of the value */
    lsize = (type & MQ_TYPE_IS_NATIVE);
    /* if type is native than "lsize" has the length of the native type */
    /* if type is *not* native than "lsize" has the value of '0' */
    if (lsize == 0) {
      lsize = U2INT(buf->type==MQ_BINT,bcur);
      bcur.B += (HDR_INT_LEN + 1);
    }
 
    bcur.B += lsize+1;
    buf->cur = bcur;
    *out = bstart.B;
    *len = (MQ_SIZE) (bcur.B-bstart.B);
    read->canUndo = MQ_YES;
    return MQ_OK;
  }
}

void
pReadBDY (
  struct MqS * const context,
  MQ_BIN * const out,
  MQ_SIZE * const len,
  enum MqHandShakeE * const hs,
  MQ_SIZE * const num
)
{
  struct MqReadS * const read = context->link.read;
  *out = read->bdy->data;
  *len = read->bdy->cursize;
  *hs  = read->handShake;
  // the transLId was already read in pReadHDR / sReadGEN -> need to add this item
  *num = read->handShake == MQ_HANDSHAKE_TRANSACTION ? read->bdy->numItems + 1 : read->bdy->numItems;
  // in a "longterm-transaction" with "MqReadDUMP" no return transaction is
  // required because the transaction is stored/forwarded
  read->handShake = MQ_HANDSHAKE_START;
}

enum MqErrorE
MqReadU (
  struct MqS * const context,
  struct MqBufferS ** out
)
{
  struct MqReadS * const read = context->link.read;
  if (unlikely(read == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    register struct MqBufferS * const buf = read->bdy;
    register struct MqBufferS * const cur = read->cur;
    pReadWordM(context, buf, cur);
    *out = cur;
    read->canUndo = MQ_YES;
    return MQ_OK;
  }
}

enum MqErrorE
MqReadProxy (
  struct MqS * const readctx,
  struct MqS * const sendctx
)
{
  if (readctx->config.isString == sendctx->config.isString) {
    MQ_CBI buf = NULL;
    MQ_SIZE len = 0;
    MqErrorCheck(MqReadN(readctx, &buf, &len));
    if (MqErrorCheckI(MqSendN(sendctx, buf, len))) {
      MqErrorCopy(readctx, sendctx);
      goto error;
    }
  } else {
    MQ_BUF buf = NULL;
    MqErrorCheck(MqReadU(readctx, &buf));
    if (MqErrorCheckI(MqSendU(sendctx, buf))) {
      MqErrorCopy(readctx, sendctx);
      goto error;
    }
  }
  return MQ_OK;
error:
  return MqErrorGetCodeI(readctx);
}

#define st(str) str==MQ_YES?"string=yes":"string=no"
#define et(str) end==MQ_YES?"endian=yes":"endian=no"

enum MqErrorE 
MqReadForward (
  struct MqS * const readctx,
  struct MqS * const sendctx,
  struct MqDumpS * const dump
) {
  MQ_TOK token;
  enum MqHandShakeE hs;
  MQ_BIN bdy; MQ_SIZE len;
  MQ_HDL RtransSId, StransSId;
  MQ_ATO num;

  if (sendctx->link.send == NULL) {
    return MqErrorDbV2(readctx,MQ_ERROR_CONNECTED, "msgque", "not");
  }

  // we need to know if the result is for a "shortterm-transaction"
  // a value < 0 signal an out-of-duty id, this happen if the read-buffer was
  // read from a storage and the original id is invalid.
  StransSId = sendctx->link.transSId > 0 ? sendctx->link.transSId : 0;
  
  // start to send package
  MqErrorCheck1 (MqSendSTART (sendctx));

  // is the "dump" parameter available?
  if (dump == NULL) {
    // NO -> read data from the "read-buffer"
    token = MqServiceGetToken(readctx);
    RtransSId = readctx->link.transSId; // "0" = no-transaction  or "-1" = shortterm-transaction

    pReadBDY (readctx, &bdy, &len, &hs, &num.I);
  } else if (dump->signature != MQ_MqDumpS_SIGNATURE) {
    return MqErrorDbV2(readctx,MQ_ERROR_VALUE_INVALID, "signature", "MqDumpS");
  } else if (dump->isString != sendctx->config.isString) {
    return MqErrorDbV2(readctx,MQ_ERROR_VALUE_INVALID, st(dump->isString), "MqDumpS");
/*
  } else if (dump->endian != sendctx->bits.endian) {
    return MqErrorDbV2(readctx,MQ_ERROR_VALUE_INVALID, et(dump->endian), "MqDumpS");
*/
  } else {
    // YES -> read data from the "dump-buffer"
    token = dump->hdr.tok;
    hs = (enum MqHandShakeE) dump->hdr.code;
    bdy = (MQ_BIN) &dump->bdy;
    len = dump->bdylen;
    RtransSId = dump->hdr.trans == 0 ? 0 : -1; // "0" = no-transaction  or "-1" = shortterm-transaction
    // read the bdy->numItems
    if (unlikely(dump->isString)) {
      num.I = str2int(dump->bdy.num.S,NULL,16);
    } else {
      if (dump->endian != sendctx->link.bits.endian) {
	pSwapBDY(bdy);
      }
#if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
      num.B[0] = dump->bdy.num.E[0];
      num.B[1] = dump->bdy.num.E[1];
      num.B[2] = dump->bdy.num.E[2];
      num.B[3] = dump->bdy.num.E[3];
#else
      num.I = dump->bdy.num.B;
#endif
    }
    // if the package is a "longterm-transaction increase the number
    if (hs == MQ_HANDSHAKE_TRANSACTION) ++num.I;
  }
  // fill the "send-buffer"
  pSendBDY (sendctx, bdy, len, hs, num.I);

  // direction: client -> server = call a service
  if (hs == MQ_HANDSHAKE_START || hs == MQ_HANDSHAKE_TRANSACTION) {
    if (RtransSId != 0) { // shortterm-transaction
      // use a transaction protection
      MqErrorCheck1 (MqSendEND_AND_WAIT (sendctx, token, MQ_TIMEOUT_USER));

      // start the answer
      MqErrorCheck(MqSendSTART (readctx));

      // if the "transSId != -1" was set than the data came from a !dump!.
      // no answer is possible because the initial call is already gone.
      // only for a !real! "shortterm-transaction" result (transSId != 0 and -1)
      // the results are important. as a silent agreement a "longterm-transaction"
      // only report an empty "_RET" first -> ignore the read/send -> this is faster.
      // an other silent agreement is that a "shortterm-transaction", used from 
      // a storage, only report an empty  "_RET" too!!
      if (RtransSId != -1 && hs != MQ_HANDSHAKE_TRANSACTION) {
	// BDY in + out
	pReadBDY (sendctx, &bdy, &len, &hs, &num.I);
	pSendBDY (readctx,  bdy,  len,  hs,  num.I);
	//MqErrorCheck(MqSendRETURN (readctx));
      }
    } else { // no-transaction
      MqErrorCheck1 (pSendEND (sendctx, token, 0));
    }

  // direction: server -> client = answer from the service
  } else {
    // only send return for a !real! request
    MqErrorCheck1 (pSendEND (sendctx, token, StransSId));
  }

  return MQ_OK;
error1:
  MqErrorCopy (readctx, sendctx);
  pErrorReset(sendctx);
error:
  return MqErrorStack(readctx);
}

/*****************************************************************************/
/*                                                                           */
/*                                 protected                                 */
/*                                                                           */
/*****************************************************************************/

void
pReadSetType(
  struct MqS * const context,
  MQ_BOL const type
)
{
  context->link.read->type = MQ_STRING_TYPE(type);
}

void
pReadSetReturnNum (
  struct MqS const * const context,
  MQ_INT retNum
)
{
  context->link.read->returnNum = retNum;
}

MQ_TRA
pReadGetTransId (
  struct MqS * const context
)
{
  return context->link.read->transLId;
}

enum MqErrorE
pReadDeleteTrans (
  struct MqS * const context 
)
{
  struct MqReadS * const read = context->link.read;
  if (read->transLId != 0LL) {
    // the following line is necessary, because without reset of "MQ_HANDSHAKE_TRANSACTION"
    // the transaction-end could be send double
    read->handShake = MQ_HANDSHAKE_START;
    MqErrorCheck(pSqlDeleteReadTrans(context,read->transLId,&read->transLId,&read->rmtTransId));
  }
  return MQ_OK;
error:
  return MqErrorStack(context);
}

enum MqErrorE
pReadInsertRmtTransId (
  struct MqS * const context
)
{
  struct MqReadS * const read = context->link.read;
  if (read->rmtTransId != 0LL) {
    MqErrorCheck (pSendT (context, read->rmtTransId));
  }
  return MQ_OK;
error:
  return MqErrorStack (context);
}

/*****************************************************************************/
/*                                                                           */
/*                                  public                                   */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
MqReadUndo (
  struct MqS * const context
)
{
  struct MqReadS * const read = context->link.read;
  if (unlikely(read == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    struct MqBufferS * const bdy = read->bdy;
    struct MqBufferS * const cur = read->cur;

    if (read->canUndo == MQ_NO) {
      return MqErrorDb(MQ_ERROR_UNDO_FORBIDDEN);
    } else {
      read->canUndo = MQ_NO;
    }
    
    bdy->numItems++; 
    bdy->cur.B -= ( 2 + cur->cursize + 1 +
      (cur->type & MQ_TYPE_IS_NATIVE ? 0 : HDR_INT_LEN + 1)
    );
    return MQ_OK;
  }
}

MQ_SIZE
MqReadGetNumItems (
  struct MqS const * const context
)
{
  return (context->link.read ? context->link.read->bdy->numItems : 0);
}

MQ_BOL
MqReadItemExists (
  struct MqS const * const context
)
{
  return (context->link.read && context->link.read->bdy->numItems != 0 ? MQ_YES : MQ_NO);
}

enum MqHandShakeE
MqReadGetHandShake (
  struct MqS const * const context
)
{
  return context->link.read->handShake;
}

/*****************************************************************************/
/*                                                                           */
/*                                 debugging                                 */
/*                                                                           */
/*****************************************************************************/

#ifdef _DEBUG

void
pReadLog (
  struct MqS const * const context,
  MQ_CST const prefix
)
{
  struct MqReadS * read = context->link.read;

  MqLogV (context, prefix, 0, ">>>> MqReadS (%p)\n", (void*) read);
  MqLogV (context, prefix, 0, "handShake = " "%c" "\n", read->handShake);
  MqLogV (context, prefix, 0, "returnNum  = " MQ_FORMAT_I "\n", read->returnNum);
  MqLogV (context, prefix, 0, "  >>>> read->hdr\n");
  MqBufferLog (context, read->hdr, prefix);
  MqLogV (context, prefix, 0, "  >>>> read->bdy\n");
  MqBufferLog (context, read->bdy, prefix);
  MqLogV (context, prefix, 0, "  >>>> read->cur\n");
  MqBufferLog (context, read->cur, prefix);
  MqLogV (context, prefix, 0, "<<<< MqReadS\n");
}

#endif

END_C_DECLS



