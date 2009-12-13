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

/// \brief the local \b context of the \ref server tool
/// \mqctx
struct TransCtxS {
  struct MqS	mqctx;	///< \mqctxI
  TCHDB		*db;	///< context specific data
  MQ_INT	min;
  MQ_INT	max;
};

/*****************************************************************************/
/*                                                                           */
/*                               Request Handler                             */
/*                                                                           */
/*****************************************************************************/

static enum MqErrorE ACLO (struct MqS *const, void*);

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

#define READ_W(n) MqErrorCheck(MqReadW(mqctx,&n))
#define READ_C(n) MqErrorCheck(MqReadC(mqctx,&n))
#define READ_N(n,l) MqErrorCheck(MqReadN(mqctx,&n,&l))

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
  MqErrorCheck (MqServiceDelete (mqctx, "_ALL"));
  MqErrorCheck (MqServiceCreate (mqctx, "AOPN", AOPN, NULL, NULL));

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
  SETUP_trans;

  if (trans->db != NULL) {
    tchdbdel(trans->db);
    trans->db = NULL;
  }

  return MQ_OK;
}

static enum MqErrorE
TransSetup (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  SETUP_trans;

  trans->db = tchdbnew();
  trans->min = 0;
  trans->max = 0;
  MqErrorCheck (MqServiceCreate (mqctx, "AOPN", AOPN, NULL, NULL));

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

  // parse the command-line
  struct MqBufferLS * args = MqBufferLCreateArgs (argc, argv);

  // add config data
  mqctx->setup.Child.fCreate	    = MqDefaultLinkCreate;
  mqctx->setup.Parent.fCreate	    = MqDefaultLinkCreate;
  mqctx->setup.fHelp		    = TransHelp;
  mqctx->setup.isServer		    = MQ_YES;
  mqctx->setup.ServerSetup.fFunc    = TransSetup;
  mqctx->setup.ServerCleanup.fFunc  = TransCleanup;
  MqConfigSetDefaultFactory (mqctx);

  // create the ServerCtxS
  MqErrorCheck(MqLinkCreate (mqctx, &args));

  // start event-loop and wait forever
  MqProcessEvent (mqctx, MQ_TIMEOUT, MQ_WAIT_FOREVER);

  // finish and exit
error:
  MqExit (mqctx);
}

/** \} trans */

