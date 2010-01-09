/**
 *  \file       theBrain/atrans/atrans.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include <tcutil.h>
#include <tchdb.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <myconf.h>

#include "msgque.h"
#include "debug.h"

/** \ingroup validation
 *  \defgroup trans trans
 *  \{
 *  \brief server \trans_desc
 *
 *  \verbinclude trans.help
 */

/// link to the MqErrorS object
#define ARGS  struct MqS * const mqctx, void *data
#define TRANSCTX ((struct TransCtxS*const)mqctx)
#define MQCTX ((struct MqS*const)trans)
#define MQ_CONTEXT_S mqctx
#define SETUP_trans struct TransCtxS*const trans = TRANSCTX
#define SETUP_HDB \
  SETUP_trans; \
  MQ_BIN key; MQ_SIZE klen; \
  TCHDB * const db = trans->db;
#define DbErrorCheck(f) \
  if (!(f))  { \
    DbError (trans, __func__); \
    goto error; \
  }
#define CHECK_ARGS(s) \
  if (MqReadGetNumItems(mqctx))  { \
    return MqErrorV (mqctx, __func__, -1, "usage: %s (%s)\n", __func__, s); \
  }

struct TransItmS {
  MQ_STRB token[5];
  MQ_BOL  isTrans;
  MQ_BUF  data;
};

/// \brief the local \b context of the \ref server tool
/// \mqctx
struct TransCtxS {
  struct MqS	    mqctx;	///< \mqctxI
  TCHDB		    *db;	///< context specific data
  struct TransItmS  **itm;
  MQ_STR	    dbstr;	///< database-connection string
  MQ_INT	    rIdx;
  MQ_INT	    wIdx;
  MQ_INT	    size;
  MQ_BFL	    args;
};

/*****************************************************************************/
/*                                                                           */
/*                               Request Handler                             */
/*                                                                           */
/*****************************************************************************/

/// \brief display help using \b -h or \b --help command-line option
/// \param base the executable usually: <tt>basename(argv[0])</tt>
static void __attribute__ ((noreturn))
TransHelp (const char * base)
{
  fprintf(stderr, "usage: %s [OPTION]... [ARGUMENT]...\n", base);
  fputs("\n", stderr);
  fputs("  This tool is the database server of NHI1. Read more at:\n", stderr);
  fputs("  http://openfacts2.berlios.de/wikien/index.php/BerliosProject:NHI1_-_TheTrans\n", stderr);
  fputs("\n", stderr);
  fprintf(stderr, "  %s [ARGUMENT]... syntax:\n", base);
  fprintf(stderr, "    aclient [OPTION]... %c %s [OPTION]... [ARGUMENT]\n", MQ_ALFA, base);
  fputs("\n", stderr);
  fputs(MqHelp (NULL), stderr);
  fputs("\n", stderr);
  fprintf(stderr,"  %s [OPTION]:\n", base);
  fputs("    -h, --help       print this help\n", stderr);
  fputs("\n", stderr);

  exit(EXIT_SUCCESS);
}

/* Store a record into an abstract database object. */
static inline enum MqErrorE
DbError(struct TransCtxS *trans, MQ_CST func){
  int ecode = tchdbecode(trans->db);
  MQ_CST estr;
  if (ecode) {
    estr = tchdberrmsg(ecode);
  } else {
    ecode = -1;
    estr = "unknown error";
  }
  return MqErrorC (MQCTX, func, ecode, estr);
}

/* Open an abstract database. */
/*
static bool tchdbopen2(TCHDB *hdb, const char *name){
  TCLIST *elems = tcstrsplit(name, "#");
  char *path = tclistshift2(elems);
  bool rv = true;
  if(!path){
    tclistdel(elems);
    return false;
  }
  int dbgfd = -1;
  int64_t bnum = -1;
  bool owmode = true;
  bool ocmode = true;
  bool otmode = false;
  bool onlmode = false;
  bool onbmode = false;
  int8_t apow = -1;
  int8_t fpow = -1;
  bool tlmode = false;
  bool tdmode = false;
  bool tbmode = false;
  bool ttmode = false;
  int32_t rcnum = -1;
  int64_t xmsiz = -1;
  int32_t dfunit = -1;
  int ln = TCLISTNUM(elems);
  for(int i = 0; i < ln; i++){
    const char *elem = TCLISTVALPTR(elems, i);
    char *pv = strchr(elem, '=');
    if(!pv) continue;
    *(pv++) = '\0';
    if(!tcstricmp(elem, "dbgfd")){
      dbgfd = tcatoi(pv);
    } else if(!tcstricmp(elem, "bnum")){
      bnum = tcatoix(pv);
    } else if(!tcstricmp(elem, "mode")){
      owmode = strchr(pv, 'w') || strchr(pv, 'W');
      ocmode = strchr(pv, 'c') || strchr(pv, 'C');
      otmode = strchr(pv, 't') || strchr(pv, 'T');
      onlmode = strchr(pv, 'e') || strchr(pv, 'E');
      onbmode = strchr(pv, 'f') || strchr(pv, 'F');
    } else if(!tcstricmp(elem, "apow")){
      apow = tcatoix(pv);
    } else if(!tcstricmp(elem, "fpow")){
      fpow = tcatoix(pv);
    } else if(!tcstricmp(elem, "opts")){
      if(strchr(pv, 'l') || strchr(pv, 'L')) tlmode = true;
      if(strchr(pv, 'd') || strchr(pv, 'D')) tdmode = true;
      if(strchr(pv, 'b') || strchr(pv, 'B')) tbmode = true;
      if(strchr(pv, 't') || strchr(pv, 'T')) ttmode = true;
    } else if(!tcstricmp(elem, "rcnum")){
      rcnum = tcatoix(pv);
    } else if(!tcstricmp(elem, "xmsiz")){
      xmsiz = tcatoix(pv);
    } else if(!tcstricmp(elem, "dfunit")){
      dfunit = tcatoix(pv);
    }
  }
  tclistdel(elems);
  if(tcstribwm(path, ".tch") || tcstribwm(path, ".hdb")){
    if(dbgfd >= 0) tchdbsetdbgfd(hdb, dbgfd);
    tchdbsetmutex(hdb);
    int opts = 0;
    if(tlmode) opts |= HDBTLARGE;
    if(tdmode) opts |= HDBTDEFLATE;
    if(tbmode) opts |= HDBTBZIP;
    if(ttmode) opts |= HDBTTCBS;
    tchdbtune(hdb, bnum, apow, fpow, opts);
    tchdbsetcache(hdb, rcnum);
    if(xmsiz >= 0) tchdbsetxmsiz(hdb, xmsiz);
    if(dfunit >= 0) tchdbsetdfunit(hdb, dfunit);
    int omode = owmode ? HDBOWRITER : HDBOREADER;
    if(ocmode) omode |= HDBOCREAT;
    if(otmode) omode |= HDBOTRUNC;
    if(onlmode) omode |= HDBONOLCK;
    if(onbmode) omode |= HDBOLCKNB;
    if(!tchdbopen(hdb, path, omode)){
      rv = false;
    }
  }
  TCFREE(path);
  return rv;
}
*/

#define READ_W(n) MqErrorCheck(MqReadW(mqctx,&n))
#define READ_C(n) MqErrorCheck(MqReadC(mqctx,&n))
#define READ_N(n,l) MqErrorCheck(MqReadN(mqctx,&n,&l))

/*

static enum MqErrorE APUT ( ARGS ) {
  SETUP_HDB;
  MQ_BIN val;
  MQ_SIZE vlen;

  MqSendSTART(mqctx);
  while (MqReadItemExists(mqctx)) {
    READ_N (key,klen);
    READ_N (val,vlen);
    DbErrorCheck (tchdbput(db, key, klen, val, vlen));
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AKEP ( ARGS ) {
  SETUP_HDB;
  MQ_BIN val;
  MQ_SIZE vlen;

  MqSendSTART(mqctx);
  while (MqReadItemExists(mqctx)) {
    READ_N (key,klen);
    READ_N (val,vlen);
    DbErrorCheck (tchdbputkeep(db, key, klen, val, vlen));
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AGET ( ARGS ) {
  SETUP_HDB;
  MQ_BIN val;
  MQ_SIZE vlen;

  MqSendSTART(mqctx);
  while (MqReadItemExists(mqctx)) {
    READ_N (key,klen);
    DbErrorCheck(val = tchdbget(db, key, klen, &vlen));
    MqSendN(mqctx, val, vlen);
    free(val);
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AITI ( ARGS ) {
  SETUP_trans;

  MqSendSTART(mqctx);
  DbErrorCheck(tchdbiterinit(trans->db));

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AITN ( ARGS ) {
  SETUP_HDB;

  MQ_INT num=999999;
  if (MqReadItemExists(mqctx)) {
    MqErrorCheck (MqReadI (mqctx, &num));
  }

  MqSendSTART(mqctx);
  while (num-- >= 0 && (key=tchdbiternext(db, &klen)) != NULL) {
    MqSendN(mqctx, key, klen);
    free(key);
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AITA ( ARGS ) {
  SETUP_HDB;
  MQ_BIN val;
  MQ_SIZE vlen;

  MQ_INT num=999999;
  if (MqReadItemExists(mqctx)) {
    MqErrorCheck (MqReadI (mqctx, &num));
  }

  MqSendSTART(mqctx);
  while ( num-- >= 0 && (key=tchdbiternext(db, &klen)) != NULL ) {
    MqSendN(mqctx, key, klen);
    DbErrorCheck(val=tchdbget(db, key, klen, &vlen));
    MqSendN(mqctx, val, vlen);
    free(key);
    free(val);
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AOUT ( ARGS ) {
  SETUP_HDB;

  MqSendSTART(mqctx);
  while (MqReadItemExists(mqctx)) {
    READ_N (key,klen);
    DbErrorCheck(tchdbout(db, key, klen));
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE ASIZ ( ARGS ) {
  SETUP_HDB;
  MQ_INT size;

  MqSendSTART(mqctx);
  while (MqReadItemExists(mqctx)) {
    READ_N (key,klen);
    size = tchdbvsiz(db, key, klen);
    DbErrorCheck(size!=-1);
    MqSendI(mqctx,size);
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AOTM ( ARGS ) {
  SETUP_trans;
  MQ_WID bnum=0; 
  MQ_BYT apow=0, fpow=0, opts=0;

  MqSendSTART(mqctx);
  if (MqReadItemExists(mqctx)) MqErrorCheck (MqReadW (mqctx, &bnum));
  if (MqReadItemExists(mqctx)) MqErrorCheck (MqReadY (mqctx, &apow));
  if (MqReadItemExists(mqctx)) MqErrorCheck (MqReadY (mqctx, &fpow));
  if (MqReadItemExists(mqctx)) MqErrorCheck (MqReadY (mqctx, &opts));

  DbErrorCheck (tchdboptimize(trans->db, bnum, apow, fpow, opts));

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AVAN ( ARGS ) {
  SETUP_trans;
  MqSendSTART(mqctx);
  DbErrorCheck (tchdbvanish(trans->db));
error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE ATRB ( ARGS ) {
  SETUP_trans;
  MqSendSTART(mqctx);
  DbErrorCheck (tchdbtranbegin(trans->db));
error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE ATRC ( ARGS ) {
  SETUP_trans;
  MqSendSTART(mqctx);
  DbErrorCheck (tchdbtrancommit(trans->db));
error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE ATRA ( ARGS ) {
  SETUP_trans;
  MqSendSTART(mqctx);
  DbErrorCheck (tchdbtranabort(trans->db));
error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AOPN ( ARGS ) {
  SETUP_trans;
  MQ_CST fname;

  READ_C (fname);

  // open the database
  DbErrorCheck (tchdbopen2(trans->db, fname));

  // add/remove services
  MqErrorCheck (MqServiceDelete (mqctx, "AOPN"));
  MqErrorCheck (MqServiceCreate (mqctx, "ACLO", ACLO, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "AITI", AITI, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "AOTM", AOTM, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "AVAN", AVAN, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "ATRB", ATRB, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "ATRC", ATRC, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "ATRA", ATRA, NULL, NULL));

  MqErrorCheck (MqServiceCreate (mqctx, "APUT", APUT, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "AKEP", AKEP, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "AGET", AGET, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "AITN", AITN, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "AITA", AITA, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "AOUT", AOUT, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "ASIZ", ASIZ, NULL, NULL));

  MqSendSTART(mqctx);
error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE ACLO ( ARGS ) {
  SETUP_trans;
  MqSendSTART(mqctx);

  // open the database
  DbErrorCheck (tchdbclose(trans->db));

  // add/remove services
  MqErrorCheck (MqServiceDelete (mqctx, "-ALL"));
  MqErrorCheck (MqServiceCreate (mqctx, "AOPN", AOPN, NULL, NULL));

error:
  return MqSendRETURN(mqctx);
}
*/

static enum MqErrorE TransEvent (
  struct MqS * const mqctx,
  MQ_PTR const data
)
{
  register SETUP_trans;

  // check if a transaction is available
  if (trans->rIdx == trans->wIdx) {
    // no transaction available
    if (mqctx->link.onShutdown == MQ_YES) {
      return MQ_EXIT;
    }
  } else {
    struct MqS * ftr;
    register struct TransItmS * itm;
    // transaction available
    // check if the filter is available, if not start the filter
    switch (MqServiceGetFilter(mqctx, 0, &ftr)) {
      case MQ_OK:
	break;
      case MQ_ERROR:
	// write error message but ignore the error
	MqErrorReset(mqctx);
	goto end;
      case MQ_EXIT:
      case MQ_CONTINUE:
	goto error;
    }

    // extract the first (oldest) transaction from the store
    itm = trans->itm[trans->rIdx++];

    // send the transaction to the filter, on error write message but do not stop processing
    MqErrorCheck1 (MqSendSTART(ftr));
    MqErrorCheck1 (MqSendBDY(ftr, itm->data->cur.B, itm->data->cursize));
    MqErrorCheck1 (
      itm->isTrans ?
	MqSendEND_AND_WAIT(ftr, itm->token, MQ_TIMEOUT_USER) :
	  MqSendEND(ftr, itm->token)
    );

    // delete the transaction from the store
    MqBufferReset(itm->data);
error:
    return MqErrorStack(mqctx);
error1:
    MqErrorPrint(ftr);
    MqErrorReset(ftr);
    MqErrorReset(mqctx);
    return MQ_OK;
  }
end:
  return MQ_OK;
}

static enum MqErrorE TransIn ( ARGS ) {
  MQ_BIN bdy;
  MQ_SIZE len;
  SETUP_trans;
  register struct TransItmS * itm;
  MqErrorCheck(MqReadBDY(mqctx, &bdy, &len));
  
  // add space if space is empty
  if (trans->rIdx-1==trans->wIdx || (trans->rIdx==0 && trans->wIdx==trans->size-1)) {
    MQ_SIZE i;
    MqSysRealloc(MQ_ERROR_PANIC, trans->itm, sizeof(struct TransItmS*)*trans->size*2);
    for (i=0;i<trans->rIdx;i++) {
      trans->itm[trans->wIdx++] = trans->itm[i];
      trans->itm[i] = NULL;
    }
    for (i=trans->wIdx;i<trans->size;i++) {
      trans->itm[i] = NULL;
    }
    trans->size*=2;
  }

  itm = trans->itm[trans->wIdx];

  // create storage if NULL
  if (itm == NULL) {
    trans->itm[trans->wIdx] = itm = MqSysCalloc(MQ_ERROR_PANIC, 1, sizeof(struct TransItmS));
  }
  if (itm->data == NULL) {
    itm->data = MqBufferCreate(MQ_ERROR_PANIC, len);
  }
  MqBufferSetB(itm->data, bdy, len);
  strncpy(itm->token, MqServiceGetToken(mqctx), 5);
  itm->isTrans = MqServiceIsTransaction(mqctx);
  trans->wIdx++;
error:
  return MqSendRETURN(mqctx);
}

/*****************************************************************************/
/*                                                                           */
/*                                context_init                               */
/*                                                                           */
/*****************************************************************************/

static enum MqErrorE
TransCleanup (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_SIZE i;
  SETUP_trans;

  if (trans->db != NULL) {
    tchdbdel(trans->db);
    trans->db = NULL;
  }
  
  for (i=0;i<trans->size;i++) {
    if (trans->itm[i] != NULL) {
      MqBufferDelete (&trans->itm[i]->data);
      MqSysFree(trans->itm[i]);
    }
  }
  return MQ_OK;
}

static enum MqErrorE
TransSetup (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  register SETUP_trans;

  //trans->db = tchdbnew();

  // init the chache
  trans->itm = (struct TransItmS**)MqSysCalloc(MQ_ERROR_PANIC,100,sizeof(struct TransItmS*));
  trans->rIdx = 0;
  trans->wIdx = 0;
  trans->size = 100;

  // open the database file
  //DbErrorCheck (tchdbopen2(trans->db, trans->dbstr));

  // SERVER: listen on every token (+ALL)
  MqErrorCheck (MqServiceCreate (mqctx, "+ALL", TransIn, NULL, NULL));

error:
  return MqErrorStack(mqctx);
}

/*****************************************************************************/
/*                                                                           */
/*                                  main                                     */
/*                                                                           */
/*****************************************************************************/

/// \brief main entry-point for the tool
/// \param argc the number of command-line arguments
/// \param argv the command-line arguments as an array of strings
/// \return the exit number
int
main (
  const int argc,
  MQ_CST argv[]
)
{
  // the parent-context
  struct MqS * const mqctx = MqContextCreate(sizeof(struct TransCtxS), NULL);
  struct TransCtxS * const trans = (struct TransCtxS*) mqctx;

  // parse the command-line
  MQ_BFL args = MqBufferLCreateArgs (argc, argv);

  // extract the connection items from "args"
  MQ_BFL ts = trans->args = MqBufferLDup(args);
  MQ_SIZE num;
  MQ_SIZE const max = ts->cursize-1;
  for (num=0; 
    num<max && *ts->data[num]->cur.C != MQ_ALFA; 
      num++) {;}
  MqBufferLDeleteItem (mqctx, ts, 0, num+1, MQ_YES);
  if (ts->cursize <= 0) {
    MqErrorC(mqctx, __func__, -1, "unable to extract the connection items from the command-line parameters");
    goto error;
  }
  MqErrorCheck (MqBufferLCheckOptionC (mqctx, args, "--db", &trans->dbstr));

  // add config data
  mqctx->setup.Child.fCreate	    = MqLinkDefault;
  mqctx->setup.Parent.fCreate	    = MqLinkDefault;
  mqctx->setup.fHelp		    = TransHelp;
  mqctx->setup.isServer		    = MQ_YES;
  mqctx->setup.ServerSetup.fFunc    = TransSetup;
  mqctx->setup.ServerCleanup.fFunc  = TransCleanup;
  mqctx->setup.ignoreExit	    = MQ_YES;
  MqConfigSetDefaultFactory (mqctx);
  MqConfigSetEvent (mqctx, TransEvent, NULL, NULL, NULL);

  // create the ServerCtxS
  MqErrorCheck(MqLinkCreate (mqctx, &args));

  // start event-loop and wait forever
  MqProcessEvent (mqctx, MQ_TIMEOUT, MQ_WAIT_FOREVER);

  // finish and exit
error:
  MqExit (mqctx);
}

/** \} trans */


