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
  MQ_TRA transId;		    ///< transaction-id (rowid from readTrans table) used for persistent-transaction
  MQ_TRA rmtTransId;		    ///< remote transaction-id (rowid from the remote readTrans table)
				    ///< if ">0LL" -> the value to use or "OLL" -> get value from the database
};

#pragma pack(1)
struct MqReadDmpS {
  MQ_TRA    transId;
  MQ_BOL    isString;
  MQ_BOL    endian;
  MQ_SIZE   hdrlen;
  MQ_SIZE   bdylen;
};
#pragma pack()

const static MQ_SIZE DMP_STA = 0;
const static MQ_SIZE DMP_HDR = sizeof(struct MqReadDmpS);
const static MQ_SIZE DMP_BDY = sizeof(struct MqReadDmpS) + sizeof(struct HdrS);


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
  MqBufferDelete (&read->cur);

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
  } else if (read->transId == 0LL) {
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
  register struct MqS  * context
)
{
  struct MqReadS * read = context->link.read;
  MqErrorCheck (pReadT (context, &read->transId));
  MqErrorCheck (pSqlSelectSendTrans (context, read->transId, read->tranBuf));
  return MQ_OK;
error:
  return MqErrorStack(context);
}

enum MqErrorE
pReadDeleteTransId (
  register struct MqS  * context
)
{
  struct MqReadS * read = context->link.read;
  return pSqlDeleteSendTrans (context, read->transId, &read->transId);
}

enum MqErrorE
MqStorageInsert (
  struct MqS * const context,
  MQ_TRA *transIdP
)
{
  struct MqReadS * read = context->link.read;
  if (unlikely(read->transId != 0LL)) {
    // this request is a logterm-transaction-request and this request is
    // already stored into the database
    if (transIdP != NULL) *transIdP = read->transId;
    // the current transaction is now in the storage and have to be deleted.
    // the MqStorageSelect will setup the transaction again
    read->transId = 0LL;
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
	transIdP
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
  MQ_TRA transId
)
{
  if (context->link.read->transId != transId) {
    MQ_WID oldTransIdDummy, oldRmtTransIdDummy;
    // a longterm-transaction-request will be deleted in MqSendRETURN or MqSendERROR using
    // pReadDeleteTrans. Only the shortterm-transaction-request need to be deleted.
    // do not change the longterm-transaction-request data, because in the mean-time between
    // MqStoreInsert and MqStoreDelete a new longterm-transaction-request could be arrived
    MqErrorCheck(pSqlDeleteReadTrans(context,transId,&oldTransIdDummy,&oldRmtTransIdDummy));
  }
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
  struct MqReadS * read = context->link.read;
  MQ_SIZE ctxId;
  MQ_SIZE size;
  register int const string = context->config.isString;
  register struct HdrS * cur;
  int debug;
  register struct MqBufferS * bdy;
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
  context->link._trans = cur->trans;

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

    // 5. if in a longterm-transaction, read the transaction-item on the !server!
    if (unlikely(read->handShake == MQ_HANDSHAKE_TRANSACTION)) {
      MQ_TRA rmtTransId, transId;
      MqErrorCheck (pReadT (context, &rmtTransId));
      // use temporary variable "transId" because the following "MqSendSTART" have to send data 
      // !without! transaction id
      MqErrorCheck (
	// collect all data necessary to setup the transaction !after! a crash
	pSqlInsertReadTrans (context, 
	  read->transId,		  /* old transaction id (used for "stack" transaction) */
	  read->rmtTransId,		  /* old remote transaction id (used for "stack" transaction) */
	  hdr,				  /* header data used by MqReadDUMP (size = HDR_SIZE) */
	  bdy,				  /* body data (size = bdy->cursize) */
	  &transId
	)
      );
      // answer first call with an empty return package
      if (context->link._trans != 0) {
	read->transId = 0LL;
	enum MqErrorE ret;
	MqErrorCheck (MqSendSTART  (context));
	read->handShake = MQ_HANDSHAKE_START;
	ret = MqSendRETURN (context);
	read->handShake = MQ_HANDSHAKE_TRANSACTION;
	MqErrorCheck (ret);
	context->link._trans = 0;
      }
      // now the "transId" is "official" in use
      read->transId = transId;
      read->rmtTransId = rmtTransId;
    } else {
      read->transId = 0LL;
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
  MQ_TRA  transId,
  MQ_BOL  string,
  MQ_BOL  endian,
  MQ_BIN  hdrB,
  MQ_SIZE hdrlen,
  MQ_BIN  bdyB,
  MQ_SIZE bdylen
)
{
  struct MqReadS * read = context->link.read;
  register struct HdrS * cur = (struct HdrS *) hdrB;
  int debug = context->config.debug;
  struct MqBufferS * bdy;

  // 1. check environment context
  if (string != context->config.isString) {
    return MqErrorC(context,__func__,-1,"invalid configuration -> isString");
  }

  // MqReadDUMP need the original "header", save this header in the current context
  read->hdrorig = MqBufferSetB (read->hdr, hdrB, hdrlen);

  // 6. log message
  if (unlikely (debug >= 5)) {
    read->hdr->type = MQ_STRING_TYPE(string);
    pLogHDR (context, __func__, 5, read->hdr);
  }

  // 7. setup read
  read->handShake = (enum MqHandShakeE) cur->code;
  read->returnNum = 0;

  // is a shortterm transaction ongoing?
  // after a transaction was send to storage, it is !not! garanteed that
  // the initial context still exists. only a longterm-transaction is able
  // to forward results back to the initial context.
  // -> make the shortterm-transaction invalid
  if (unlikely(read->handShake == MQ_HANDSHAKE_TRANSACTION)) {
    context->link._trans = 0;
  } else {
    context->link._trans = cur->trans;
  }

  // setup "bdy"
  bdy = read->bdy = MqBufferSetB(read->readBuf, bdyB, bdylen);

  // 8. read token
  pTokenSetCurrent (context->link.srvT, cur->tok);

  // 9. read BDY
  if (bdylen > 0) {
    bdy->type = MQ_STRING_TYPE(string);
    if (unlikely (debug >= 6)) {
      MqLogV (context, __func__, 6, "BDY -> " MQ_FORMAT_Z " bytes\n", bdylen);
    }
    bdy->cur.B += BDY_NumItems_S;

    // 3. read NumItems
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

    // 4. if required, log package
    if (unlikely (debug >= 7 && bdylen > BDY_SIZE)) {
      pLogBDY (context, __func__, 7, bdy);
    }

    // 5. if in a longterm-transaction, read the transaction-item on the !server!
    if (unlikely(read->handShake == MQ_HANDSHAKE_TRANSACTION)) {
      MQ_TRA rmtTransId;
      MqErrorCheck (pReadT (context, &rmtTransId));
      // now the "transId" is "official" in use
      read->transId = transId;
      read->rmtTransId = rmtTransId;
    } else {
      read->transId = 0LL;
      read->rmtTransId = 0LL;
    }
  }

  // reset data
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
      (MQ_TRA)  sqlite3_column_int64(hdl,0),	/* transId  */
      (MQ_BOL)  sqlite3_column_int(hdl,1),	/* string   */
      (MQ_BOL)  sqlite3_column_int(hdl,2),	/* endian   */
      (MQ_BIN)  sqlite3_column_blob(hdl,3),	/* hdrB	    */
      (MQ_SIZE) sqlite3_column_bytes(hdl,3),	/* hdrlen   */
      (MQ_BIN)  sqlite3_column_blob(hdl,4),	/* bdyB	    */
      (MQ_SIZE) sqlite3_column_bytes(hdl,4)	/* bdylen   */
    );
  }
}

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
    register struct MqReadDmpS * dump = (struct MqReadDmpS*) env;

    // process data from MqReadDmpS buffer
    return sReadGEN (context, 
      dump->transId,
      dump->isString,
      dump->endian, 
      (MQ_BIN)env+DMP_HDR,
      dump->hdrlen,
      (MQ_BIN)env+DMP_BDY,
      dump->bdylen
    );
  }
}

enum MqErrorE
MqReadDUMP (
  struct MqS * const context,
  MQ_BIN  * const out,
  MQ_SIZE * const len
)
{
  struct MqReadS * const read = context->link.read;
  if (unlikely(read == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    struct MqReadDmpS dump = {
      read->transId, 
      context->config.isString, 
      context->link.bits.endian,
      read->hdrorig->cursize,
      read->bdy->cursize
    };
    MQ_SIZE const lbdy = read->bdy->cursize;
    MQ_SIZE const llen = sizeof(struct MqReadDmpS) + HDR_SIZE + lbdy;
    MQ_BIN data = (MQ_BIN) MqSysMalloc (context, llen);
    check_NULL(data) return MqErrorStack(context);
    memcpy (data+DMP_STA, &dump,		DMP_HDR);
    memcpy (data+DMP_HDR, read->hdrorig->data,	read->hdrorig->cursize);
    memcpy (data+DMP_BDY, read->bdy->data,	read->bdy->cursize);
    *out = data;
    *len = llen;
    // in a "longterm-transaction" with "MqReadDUMP" no return transaction is
    // possible because the transaction is stored or forwarded
    read->handShake = MQ_HANDSHAKE_START;
    return MQ_OK;
  }
}

enum MqErrorE
MqReadLOAD (
  struct MqS * context,
  MQ_CBI  in,
  MQ_SIZE len
)
{
  MqErrorCheck(pReadLOAD((MQ_PTR)in, &context));
  return MQ_OK;
error:
  return MqErrorStack(context);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

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

/*****************************************************************************/
/*                                                                           */
/*                                read_atom                                  */
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
/*                               read functions                              */
/*                                                                           */
/*****************************************************************************/

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
    *len = bcur.B-bstart.B;
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
  // the transId was already read in pReadHDR / sReadGEN -> need to add this item
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

enum MqErrorE 
MqReadForward (
  struct MqS * const readctx,
  struct MqS * const sendctx
) {
  MQ_TOK const token = MqServiceGetToken(readctx);
  enum MqHandShakeE hs;
  MQ_BIN bdy; MQ_SIZE len, num;

  MqErrorCheck1 (MqSendSTART (sendctx));

  pReadBDY (readctx, &bdy, &len, &hs, &num);
  pSendBDY (sendctx,  bdy,  len,  hs, num);

  // continue with the original transaction
  if (readctx->link._trans != 0) {
    // use a transaction protection
    MqErrorCheck1 (MqSendEND_AND_WAIT (sendctx, token, MQ_TIMEOUT_USER));
    // send the answer
    MqErrorCheck(MqSendSTART (readctx));
    // BDY in + out
    pReadBDY (sendctx,  &bdy, &len, &hs, &num);
    pSendBDY (readctx,  bdy,  len,  hs,  num);
  } else {
    // use a transaction protection
    MqErrorCheck1 (MqSendEND (sendctx, token));
  }
  return MQ_OK;
error1:
  MqErrorCopy (readctx, sendctx);
error:
  return MqErrorStack(readctx);
}

/*****************************************************************************/
/*                                                                           */
/*                                 read.high                                 */
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

void
pReadSetType(
  struct MqS * const context,
  MQ_BOL const type
)
{
  context->link.read->type = MQ_STRING_TYPE(type);
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
pReadGetHandShake (
  struct MqS const * const context
)
{
  return context->link.read != NULL ? context->link.read->handShake : MQ_HANDSHAKE_START;
}

void
pReadSetHandShake (
  struct MqS const * const context,
  enum MqHandShakeE hs
)
{
  if (context->link.read != NULL) context->link.read->handShake = hs;
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
  return context->link.read->transId;
}

enum MqErrorE
pReadDeleteTrans (
  struct MqS * const context 
)
{
  struct MqReadS * const read = context->link.read;
  if (read->transId != 0LL) {
    read->handShake = MQ_HANDSHAKE_START;
    MqErrorCheck(pSqlDeleteReadTrans(context,read->transId,&read->transId,&read->rmtTransId));
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

