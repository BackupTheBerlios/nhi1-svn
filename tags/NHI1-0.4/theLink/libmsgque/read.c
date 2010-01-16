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
#include "read.h"
#include "error.h"
#include "buffer.h"
#include "sys.h"
#include "mq_io.h"
#include "log.h"
#include "send.h"
#include "token.h"
#include "swap.h"
#include "cache.h"

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                                read_init                                  */
/*                                                                           */
/*****************************************************************************/

#define MQ_CONTEXT_S context
#define TOKEN_LEN (HDR_TOK_LEN+1)

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
  struct MqS * context;	    ///< link to the msgque object
  struct MqBufferS * hdr;	    ///< used for HDR data (static)
  struct MqBufferS * bdy;	    ///< used for BDY data (dynamic)
  struct MqBufferS * cur;	    ///< used as reference on BUF with the current data
  enum MqReturnE returnCode;	    ///< Return-Code (O)K, (E)RROR or (W)ARNING
  MQ_INT returnNum;		    ///< Return-Number
  struct MqCacheS * saveCache;
  struct ReadSaveS * save;	    ///< need for List objects
  enum MqTypeE type;		    ///< type of the item stored into the data-segment (InitialSet)
  MQ_BOL canUndo;		    ///< is an MqReadUndo allowed ?
};

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

//MqDLogX(read->context,__func__,-1,"read<%p>\n", read);

  read->hdr = MqBufferCreate (context, HDR_SIZE + 1);
  read->bdy = MqBufferCreate (context, 10000);
  read->canUndo = MQ_NO;

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
  MqBufferDelete (&read->bdy);

  // references
  MqBufferDelete (&read->cur);

  MqSysFree (*readP);
}

enum MqErrorE
MqReadL_START (
  struct MqS * const context,
  MQ_BUF buf
)
{
  register struct MqReadS * const read = context->link.read;
  if (unlikely(read == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    struct ReadSaveS * save;
    register struct MqBufferS * const bdy = read->bdy;

    // get the buffer
    if (buf == NULL)
      MqErrorCheck(MqReadU(context,&buf));

    // check buffer type
    if (buf->type != MQ_LSTT)
      return MqErrorDbV(MQ_ERROR_TYPE, MqLogTypeName(MQ_LSTT), MqLogTypeName(buf->type));

    save = (struct ReadSaveS *) pCachePop (read->saveCache);
    
    save->save = read->save;
    save->bdy  = *bdy;
    read->save = save;

    bdy->type = read->type;
    bdy->data = bdy->cur.B = buf->data;
    bdy->size = buf->size;
    bdy->cursize = buf->cursize;
    bdy->numItems = (read->type==MQ_BINT ? iBufU2INT(bdy->cur) : str2int(bdy->cur.C,NULL,16));
    bdy->cur.B += (HDR_INT_LEN + 1);

error:
    return MqErrorStack(context);
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
    struct ReadSaveS * save = read->save;
    if (unlikely(save == NULL)) return MQ_OK;

    read->save = save->save;
    *read->bdy = save->bdy;

//MqDLogV(context,0,"read<%p>, save<%p>\n", read, save);
    pCachePush (read->saveCache, save);
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

enum MqErrorE
pRead_RET_START (
  struct MqS * const context
)
{
  register struct MqReadS * read = context->link.read;
  register struct MqBufferS * const bdy = read->bdy;
  struct MqBufferS * buf = NULL;
  struct ReadSaveS * save;

  MqErrorCheck (MqReadU (context, &buf));

  // check buffer type
  if (buf->type != MQ_RETT)
    return MqErrorDbV(MQ_ERROR_TYPE, MqLogTypeName(MQ_RETT), MqLogTypeName(buf->type));

  save = (struct ReadSaveS *) pCachePop (read->saveCache);
  
//MqDLogV(context,0,"read<%p>, save<%p>\n", read, save);

  save->save = read->save;
  save->bdy  = *bdy;
  read->save = save;

  // get -> returnNum
  if (likely (read->returnCode == MQ_RETURN_OK)) {
    read->returnNum = -1;
  } else {
    read->returnNum = (read->type==MQ_BINT ? iBufU2INT(buf->cur) : str2int(buf->cur.C,NULL,16));
    buf->cur.B += (HDR_INT_LEN + 1);
    buf->cursize -= (HDR_INT_LEN + 1);
  }

  // 3. fix ret
  bdy->type = read->type;
  bdy->data = bdy->cur.B = buf->cur.B;
  bdy->size = buf->size;
  bdy->cursize = buf->cursize;
  bdy->numItems = (read->type==MQ_BINT ? iBufU2INT(bdy->cur) : str2int(bdy->cur.C,NULL,16));
  bdy->cur.B += (HDR_INT_LEN + 1);
  return MQ_OK;

error:
  return MqErrorStack(context);
}

// \attention "pRead_RET_END" is a #define

/*****************************************************************************/
/*                                                                           */
/*                                read_native                                */
/*                                                                           */
/*****************************************************************************/

static enum MqErrorE
sReadFIX (
  struct MqS * const context,
  register struct MqBufferS * const buf,
  MQ_CST const prefix
)
{
  // 1. read
  MqErrorSwitch (pIoRead (context->link.io, buf, buf->cursize));

  // 2. read 'HDR'
#ifdef MQ_SAVE
  if (unlikely (strncmp (buf->cur.C, prefix, 3))) {
    MqErrorV (context, __func__, "expect '%3s' but got '%s'", prefix, buf->cur.C);
    goto error;
  }
#endif
  buf->cur.B += HDR_CtxId_S;

  return MQ_OK;

error:
  return MqErrorStack (context);
}

enum MqErrorE
pReadHDR (
  register struct MqS  * context,
  struct MqS ** a_context
)
{
  register struct MqReadS * read = context->link.read;
  MQ_SIZE ctxId;
  MQ_SIZE size;
  MQ_HDL trans;
  register int const string = context->config.isString;
  register struct HdrS * cur;
  int debug;
  register struct MqBufferS * bdy;
  register struct MqTokenS * srvT;

  // 1. preset context
  *a_context = context;

  // 2. read
  MqErrorSwitch (pIoRead (context->link.io, read->hdr, sizeof(struct HdrS)));
  cur = (struct HdrS *) read->hdr->cur.B;

//MqDLogX (context, __func__, 0, "START (%c%c%c%c)\n", cur->tok[0], cur->tok[1], cur->tok[2], cur->tok[3]);
//pReadLog(context, "pReadHDR->1");

  // check "HDR"
  if (cur->ID1 != 'H' || cur->ID2 != 'D' || cur->ID3 != 'R') {
    MQ_BUF save = MqBufferCreate (MQ_ERROR_PANIC, 1000);
    do {
      MqBufferAppendC (save, (MQ_STR) read->hdr->data);
    } while (pIoRead (context->link.io, read->hdr, sizeof(struct HdrS)) == MQ_OK);
    *(read->hdr->data+read->hdr->cursize) = '\0';
    MqBufferAppendC (save, (MQ_STR) read->hdr->data);
    MqErrorDb (MQ_ERROR_INVALID_HEADER);
    MqErrorSAppendV (context, "but got: %s", (MQ_STR) save->data);
    MqBufferDelete (&save);
    return MQ_ERROR;
  }

  // 3. read binary data
  if (unlikely(string)) {
    ctxId  = str2int(cur->ctxId.S,NULL,16);
    size   = str2int(cur->size.S,NULL,16);
  } else {
    ctxId  = cur->ctxId.B;
    size   = cur->size.B;
    // 3.b fix byte-order
    if (context->link.endian) {
      pSwap4 (&ctxId);
      pSwap4 (&size);
    }
  }

  trans = cur->trans;

  // 4. set my context
  if (ctxId != context->link.ctxId) {
    // check if ctxId is out of range
    if (ctxId > context->link.ctxIdP->link.ctxIdR) 
	return MqErrorV (context, __func__, -1, 
	    "invalid context-id '%i'", ctxId);
    // do the context-switch if the message does !!not!! belongs to the
    // original context
    context = context->link.ctxIdP->link.ctxIdA[ctxId];
    // check if entry was already deleted
    if (!context) return MQ_CONTINUE;
    *a_context = context;
  }
//MqDLogX (context, __func__, 0, "cur->ctxId.B<%i>, context->context.ctxId<%i>\n", cur->ctxId.B, context->context.ctxId);
//MqDLogX (context, __func__, 0, "hex\n<%s>\n", pLogHEX (read->hdr->data, sizeof (struct HdrS)));
  debug = context->config.debug;
  context->link._trans = trans;

  // 6. log message
  if (unlikely (debug >= 5)) {
    read->hdr->type = MQ_STRING_TYPE(string);
    pLogHDR (context, __func__, 5, read->hdr);
  }

  // 7. setup read
  read = context->link.read;
  read->returnCode = (enum MqReturnE) cur->code;
  read->returnNum = 0;
  bdy = read->bdy;

  // 8. read token
  srvT = context->link.srvT;
  cur->charT = '\0';
  pTokenSetCurrent (srvT, cur->tok);

  // 9. read BDY
  if ((bdy->cursize = size)) {
    bdy->type = MQ_STRING_TYPE(string);
    if (unlikely (debug >= 6))
      MqDLogX (context, __func__, 6, "BDY -> " MQ_FORMAT_Z " bytes\n", size);

    // 1. read
    MqErrorCheck (sReadFIX (context, bdy, "BDY"));

    // 2. read NumItems
    if (unlikely(string)) {
      bdy->numItems = str2int(bdy->cur.C,NULL,16);
    } else {
      if (context->link.endian) {
	pSwapBDY(bdy->data);
      }
      bdy->numItems = iBufU2INT(bdy->cur);
    }

    bdy->cur.B = (bdy->data + BDY_SIZE);

    if (unlikely (debug >= 7 && size > BDY_SIZE))
      pLogBDY (context, __func__, 7, bdy);
  }

  // reset data
  read->canUndo = MQ_NO;
  read->save = NULL;
  
  // 11. check if it's a System-pToken
  // this line have to use the 'return' because pTokenCheckSystem usually return
  // MQ_CONTINUE if a system token (_*) was found
  return pTokenCheckSystem (srvT);

error:
  return MqErrorStack (context);
}

#define pReadWordM(context, buf, out) { \
  register union MqBufferU bcur = buf->cur; \
  MQ_SIZE lsize; \
  /* check if an argument is available? and decrease the number of arguments */ \
  /* buf->numItems have to be an \b int !! because --0 have to be < 0 */ \
  if ((--buf->numItems) < 0) { \
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
    lsize = (buf->type == MQ_BINT ? iBufU2INT(bcur) : str2int(bcur.C, NULL, 16)); \
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
printI(buf->numItems);
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
printC(bcur.C)
printO(buf->type == MQ_BINT)
    lsize = (buf->type == MQ_BINT ? iBufU2INT(bcur) : str2int(bcur.C, NULL, 16));
    bcur.B += (HDR_INT_LEN + 1);
  }
printI(lsize)

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
  MQ_BIN * const out,
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
      lsize = (buf->type == MQ_BINT ? iBufU2INT(bcur) : str2int(bcur.C, NULL, 16));
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

enum MqErrorE
MqReadBDY (
  struct MqS * const context,
  MQ_BIN * const out,
  MQ_SIZE * const len
)
{
  struct MqReadS * const read = context->link.read;
  if (unlikely(read == NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  } else {
    *out = read->bdy->data;
    *len = read->bdy->cursize;
    return MQ_OK;
  }
}

enum MqErrorE
MqReadU (
  struct MqS * const context,
  MQ_BUF * const out
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
    MQ_BIN buf = NULL;
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
    }
    read->canUndo = MQ_NO;
    
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

enum MqReturnE
pReadGetReturnCode (
  struct MqS const * const context
)
{
  return context->link.read->returnCode;
}

MQ_INT
pReadGetReturnNum (
  struct MqS const * const context
)
{
  return context->link.read->returnNum;
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

  MqDLogX (context, prefix, 0, ">>>> MqReadS (%p)\n", (void*) read);
  MqDLogX (context, prefix, 0, "returnCode = " "%c" "\n", read->returnCode);
  MqDLogX (context, prefix, 0, "returnNum  = " MQ_FORMAT_I "\n", read->returnNum);
  MqDLogX (context, prefix, 0, "  >>>> read->hdr\n");
  MqBufferLog (context, read->hdr, prefix);
  MqDLogX (context, prefix, 0, "  >>>> read->bdy\n");
  MqBufferLog (context, read->bdy, prefix);
  MqDLogX (context, prefix, 0, "  >>>> read->cur\n");
  MqBufferLog (context, read->cur, prefix);
  MqDLogX (context, prefix, 0, "<<<< MqReadS\n");
}

#endif

END_C_DECLS





