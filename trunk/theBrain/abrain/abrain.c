/**
 *  \file       theBrain/abrain/abrain.c
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
#include <tcadb.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "msgque.h"
#include "debug.h"

/** \ingroup validation
 *  \defgroup brain brain
 *  \{
 *  \brief server \brain_desc
 *
 *  \verbinclude brain.help
 */

/// link to the MqErrorS object
#define ARGS  struct MqS * const mqctx, void *data
#define BRAINCTX ((struct BrainCtxS*const)mqctx)
#define MQCTX ((struct MqS*const)brain)
#define MQ_CONTEXT_S mqctx
#define SETUP_brain struct BrainCtxS*const brain = BRAINCTX
#define SETUP_FDB \
  SETUP_brain; \
  MQ_WID key; \
  TCFDB * const fdb = brain->db->fdb;
#define SETUP_ADB \
  SETUP_brain; \
  MQ_BIN key; MQ_SIZE klen; \
  TCADB * const adb = brain->db;
#define DbErrorCheck(f) \
  if (!(f))  { \
    DbError (brain, __func__); \
    goto error; \
  }
#define CHECK_ARGS(s) \
  if (MqReadGetNumItems(mqctx))  { \
    return MqErrorV (mqctx, __func__, -1, "usage: %s (%s)\n", __func__, s); \
  }

/// \brief the local \b context of the \ref server tool
/// \mqctx
struct BrainCtxS {
  struct MqS	mqctx;	///< \mqctxI
  TCADB		*db;	///< context specific data
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
BrainHelp (const char * base)
{
  fprintf(stderr, "usage: %s [OPTION]... [ARGUMENT]...\n", base);
  fputs("\n", stderr);
  fputs("  This tool is the database server of NHI1. Read more at:\n", stderr);
  fputs("  http://openfacts2.berlios.de/wikien/index.php/BerliosProject:NHI1_-_TheBrain\n", stderr);
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
static enum MqErrorE
DbError(struct BrainCtxS *brain, MQ_CST func){
  int ecode = -1;
  MQ_CST estr = "failed";
  switch(brain->db->omode){
      break;
    case ADBOHDB:
      ecode = tchdbecode(brain->db->hdb);
      estr = tchdberrmsg(ecode);
      break;
    case ADBOBDB:
      ecode = tcbdbecode(brain->db->bdb);
      estr = tcbdberrmsg(ecode);
      break;
    case ADBOFDB:
      ecode = tcfdbecode(brain->db->fdb);
      estr = tcfdberrmsg(ecode);
      break;
    case ADBOTDB:
      ecode = tctdbecode(brain->db->tdb);
      estr = tctdberrmsg(ecode);
      break;
    default:
      break;
  }
  return MqErrorC (MQCTX, func, ecode, estr);
}

#define READ_W(n) MqErrorCheck(MqReadW(mqctx,&n))
#define READ_C(n) MqErrorCheck(MqReadC(mqctx,&n))
#define READ_N(n,l) MqErrorCheck(MqReadN(mqctx,&n,&l))

static enum MqErrorE APUT ( ARGS ) {
  SETUP_ADB;
  MQ_BIN val;
  MQ_SIZE vlen;

  MqSendSTART(mqctx);
  while (MqReadItemExists(mqctx)) {
    READ_N (key,klen);
    READ_N (val,vlen);
    DbErrorCheck (tcadbput(adb, key, klen, val, vlen));
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE APUT_F ( ARGS ) {
  SETUP_FDB;

  MQ_BIN val;
  MQ_SIZE vlen;

  MqSendSTART(mqctx);
  while (MqReadItemExists(mqctx)) {
    READ_W (key);
    READ_N (val,vlen);
    DbErrorCheck (tcfdbput(fdb, key, val, vlen));
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AKEP ( ARGS ) {
  SETUP_ADB;
  MQ_BIN val;
  MQ_SIZE vlen;

  MqSendSTART(mqctx);
  while (MqReadItemExists(mqctx)) {
    READ_N (key,klen);
    READ_N (val,vlen);
    DbErrorCheck (tcadbputkeep(adb, key, klen, val, vlen));
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AKEP_F ( ARGS ) {
  SETUP_FDB;

  MQ_BIN val;
  MQ_SIZE vlen;

  MqSendSTART(mqctx);
  while (MqReadItemExists(mqctx)) {
    READ_W (key);
    READ_N (val,vlen);
    DbErrorCheck (tcfdbputkeep(fdb, key, val, vlen));
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AGET ( ARGS ) {
  SETUP_ADB;
  MQ_BIN val;
  MQ_SIZE vlen;

  MqSendSTART(mqctx);
  while (MqReadItemExists(mqctx)) {
    READ_N (key,klen);
    DbErrorCheck(val = tcadbget(adb, key, klen, &vlen));
    MqSendN(mqctx, val, vlen);
    free(val);
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AGET_F ( ARGS ) {
  SETUP_FDB;

  MQ_BIN val;
  MQ_SIZE vlen;

  MqSendSTART(mqctx);
  while (MqReadItemExists(mqctx)) {
    READ_W (key);
    DbErrorCheck(val=tcfdbget(fdb, key, &vlen));
    MqSendN(mqctx, val, vlen);
    free(val);
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AITI ( ARGS ) {
  SETUP_brain;

  MqSendSTART(mqctx);
  DbErrorCheck(tcadbiterinit(brain->db));

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AITN ( ARGS ) {
  SETUP_ADB;

  MQ_INT num=999999;
  if (MqReadItemExists(mqctx)) {
    MqErrorCheck (MqReadI (mqctx, &num));
  }

  MqSendSTART(mqctx);
  while (num-- >= 0 && (key=tcadbiternext(adb, &klen)) != NULL) {
    MqSendN(mqctx, key, klen);
    free(key);
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AITN_F ( ARGS ) {
  SETUP_FDB;

  MQ_INT num=999999;
  if (MqReadItemExists(mqctx)) {
    MqErrorCheck (MqReadI (mqctx, &num));
  }

  MqSendSTART(mqctx);
  while (num-- >= 0 && (key=(MQ_WID)tcfdbiternext(fdb)) != 0) {
    MqSendW(mqctx, key);
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AITA ( ARGS ) {
  SETUP_ADB;
  MQ_BIN val;
  MQ_SIZE vlen;

  MQ_INT num=999999;
  if (MqReadItemExists(mqctx)) {
    MqErrorCheck (MqReadI (mqctx, &num));
  }

  MqSendSTART(mqctx);
  while ( num-- >= 0 && (key=tcadbiternext(adb, &klen)) != NULL ) {
    MqSendN(mqctx, key, klen);
    DbErrorCheck(val=tcadbget(adb, key, klen, &vlen));
    MqSendN(mqctx, val, vlen);
    free(key);
    free(val);
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AITA_F ( ARGS ) {
  SETUP_FDB;

  MQ_BIN val;
  MQ_SIZE vlen;

  MQ_INT num=999999;
  if (MqReadItemExists(mqctx)) {
    MqErrorCheck (MqReadI (mqctx, &num));
  }

  MqSendSTART(mqctx);
  while ( num-- >= 0 && (key=tcfdbiternext(fdb)) != 0 ) {
    MqSendW(mqctx, key);
    DbErrorCheck(val=tcfdbget(fdb, key, &vlen));
    MqSendN(mqctx, val, vlen);
    free(val);
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AOUT ( ARGS ) {
  SETUP_ADB;

  MqSendSTART(mqctx);
  while (MqReadItemExists(mqctx)) {
    READ_N (key,klen);
    DbErrorCheck(tcadbout(adb, key, klen));
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AOUT_F ( ARGS ) {
  SETUP_FDB;

  MqSendSTART(mqctx);
  while (MqReadItemExists(mqctx)) {
    READ_W (key);
    DbErrorCheck(tcfdbout(fdb, key));
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE ASIZ ( ARGS ) {
  SETUP_ADB;
  MQ_INT size;

  MqSendSTART(mqctx);
  while (MqReadItemExists(mqctx)) {
    READ_N (key,klen);
    size = tcadbvsiz(adb, key, klen);
    DbErrorCheck(size!=-1);
    MqSendI(mqctx,size);
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE ASIZ_F ( ARGS ) {
  SETUP_FDB;
  MQ_INT size;

  MqSendSTART(mqctx);
  while (MqReadItemExists(mqctx)) {
    READ_W (key);
    size = tcfdbvsiz(fdb, key);
    DbErrorCheck(size!=-1);
    MqSendI(mqctx,size);
  }

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AOTM ( ARGS ) {
  SETUP_brain;
  MQ_CST params = NULL;

  MqSendSTART(mqctx);
  if (MqReadItemExists(mqctx)) {
    MqErrorCheck (MqReadC (mqctx, &params));
  }

  DbErrorCheck (tcadboptimize(brain->db, params));

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AVAN ( ARGS ) {
  SETUP_brain;
  MqSendSTART(mqctx);
  DbErrorCheck (tcadbvanish(brain->db));
error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE ATRB ( ARGS ) {
  SETUP_brain;
  MqSendSTART(mqctx);
  DbErrorCheck (tcadbtranbegin(brain->db));
error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE ATRC ( ARGS ) {
  SETUP_brain;
  MqSendSTART(mqctx);
  DbErrorCheck (tcadbtrancommit(brain->db));
error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE ATRA ( ARGS ) {
  SETUP_brain;
  MqSendSTART(mqctx);
  DbErrorCheck (tcadbtranabort(brain->db));
error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE AOPN ( ARGS ) {
  SETUP_brain;
  MQ_CST fname;

  READ_C (fname);

  // open the database
  DbErrorCheck (tcadbopen(brain->db, fname));

  // add/remove services
  MqErrorCheck (MqServiceDelete (mqctx, "AOPN"));
  MqErrorCheck (MqServiceCreate (mqctx, "ACLO", ACLO, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "AITI", AITI, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "AOTM", AOTM, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "AVAN", AVAN, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "ATRB", ATRB, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "ATRC", ATRC, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "ATRA", ATRA, NULL, NULL));

  if (brain->db->omode == ADBOFDB) {
    MqErrorCheck (MqServiceCreate (mqctx, "APUT", APUT_F, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "AKEP", AKEP_F, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "AGET", AGET_F, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "AITN", AITN_F, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "AITA", AITA_F, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "AOUT", AOUT_F, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ASIZ", ASIZ_F, NULL, NULL));
  } else {
    MqErrorCheck (MqServiceCreate (mqctx, "APUT", APUT, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "AKEP", AKEP, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "AGET", AGET, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "AITN", AITN, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "AITA", AITA, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "AOUT", AOUT, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ASIZ", ASIZ, NULL, NULL));
  }

  MqSendSTART(mqctx);
error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE ACLO ( ARGS ) {
  SETUP_brain;
  MqSendSTART(mqctx);

  // open the database
  DbErrorCheck (tcadbclose(brain->db));

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
BrainCleanup (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  SETUP_brain;

  if (brain->db != NULL) {
    tcadbdel(brain->db);
    brain->db = NULL;
  }

  return MQ_OK;
}

static enum MqErrorE
BrainSetup (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  SETUP_brain;

  brain->db = tcadbnew();
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
  struct MqS * const mqctx = MqContextCreate(sizeof(struct BrainCtxS), NULL);

  // parse the command-line
  struct MqBufferLS * args = MqBufferLCreateArgs (argc, argv);

  // add config data
  mqctx->setup.Child.fCreate	    = MqDefaultLinkCreate;
  mqctx->setup.Parent.fCreate	    = MqDefaultLinkCreate;
  mqctx->setup.fHelp		    = BrainHelp;
  mqctx->setup.isServer		    = MQ_YES;
  mqctx->setup.ServerSetup.fFunc    = BrainSetup;
  mqctx->setup.ServerCleanup.fFunc  = BrainCleanup;
  MqConfigSetDefaultFactory (mqctx);

  // create the ServerCtxS
  MqErrorCheck(MqLinkCreate (mqctx, &args));

  // start event-loop and wait forever
  MqProcessEvent (mqctx, MQ_TIMEOUT, MQ_WAIT_FOREVER);

  // finish and exit
error:
  MqExit (mqctx);
}

/** \} brain */


