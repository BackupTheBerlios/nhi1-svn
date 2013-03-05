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

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "sqlite3.h"
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
#define SETUP_W(sh) \
  SETUP_brain; \
  MQ_WID key; \
  register sqlite3_stmt *hdl= brain->sh
#define SETUP_C(sh) \
  SETUP_brain; \
  MQ_CST key; MQ_SIZE klen; \
  register sqlite3_stmt *hdl= brain->sh
#define DbErrorCheck(f) \
  if (unlikely(f) != SQLITE_OK) { \
    MqErrorC (mqctx, __func__, sqlite3_extended_errcode(brain->db), sqlite3_errmsg(brain->db)); \
    goto error; \
  }
#define CHECK_ARGS(s) \
  if (MqReadGetNumItems(mqctx))  { \
    return MqErrorV (mqctx, __func__, -1, "usage: %s (%s)\n", __func__, s); \
  }
#define check_sqlite(E) \
  if (unlikely((E) != SQLITE_OK))
#define check_NULL(E) \
  if (unlikely((E) == NULL))

#define DB_PREPARE_MAX 10

/// \brief the local \b context of the \ref server tool
/// \mqctx
struct BrainCtxS {
  struct MqS	mqctx;		///< \mqctxI
  MQ_CST	storage;	///< storage file
  sqlite3	*db;		///< database handle
  sqlite3_stmt	*prepare[DB_PREPARE_MAX];	///< prepared statements
};

/*****************************************************************************/
/*                                                                           */
/*                               Request Handler                             */
/*                                                                           */
/*****************************************************************************/

#define DbFinalize(st) {sqlite3_finalize(st);st=NULL;}

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

#define READ_C(n) MQ_CST n; MqErrorCheck(MqReadC(mqctx,&n))

/*****************************************************************************/
/*                                                                           */
/*                                  Helper                                   */
/*                                                                           */
/*****************************************************************************/

static enum MqErrorE IdxGet ( struct MqS * const mqctx, MQ_INT *pidx ) {
  MqErrorCheck (MqReadI (mqctx, pidx));
  if (*pidx < 0 || *pidx > DB_PREPARE_MAX-1) {
    MqErrorV (mqctx, __func__, 1, "prepare index out of range: 0 <= idx <= %i", DB_PREPARE_MAX-1);
    goto error;
  }
error:
  return MqErrorStack(mqctx);
}

static enum MqErrorE IdxFinalize ( struct MqS * const mqctx, MQ_INT idx ) {
  SETUP_brain;
  if (brain->prepare[idx] != NULL) {
    DbErrorCheck(sqlite3_finalize(brain->prepare[idx]));
    brain->prepare[idx] = NULL;
  }
error:
  return MqErrorStack(mqctx);
}

static enum MqErrorE HdlGet ( struct MqS * const mqctx, sqlite3_stmt **phdl ) {
  SETUP_brain;
  MQ_INT idx;
  MqErrorCheck (IdxGet (mqctx, &idx));
  *phdl = brain->prepare[idx];
  check_NULL(*phdl) {
    MqErrorV (mqctx, __func__, 1, "the prepare-index '%i' was NOT defined", idx);
  }
error:
  return MqErrorStack(mqctx);
}

/*****************************************************************************/
/*                                                                           */
/*                               Service Handler                             */
/*                                                                           */
/*****************************************************************************/

static enum MqErrorE STEP ( ARGS ) {
  SETUP_brain;
  sqlite3_stmt *hdl=NULL;
  MQ_BUF buf;
  MQ_INT idx=0;

  MqErrorCheck(HdlGet(mqctx, &hdl));
  DbErrorCheck(sqlite3_reset(hdl));

  MqSendSTART(mqctx);
  while (MqReadItemExists(mqctx)) {
    idx+=1;
    MqErrorCheck(MqReadU(mqctx,&buf));
    switch (buf->type) {
      case MQ_BYTT:
	DbErrorCheck (sqlite3_bind_int (hdl,idx,buf->cur.A->Y));
	break;
      case MQ_BOLT:
	DbErrorCheck (sqlite3_bind_int (hdl,idx,buf->cur.A->O));
	break;
      case MQ_SRTT:
	DbErrorCheck (sqlite3_bind_int (hdl,idx,buf->cur.A->S));
	break;
      case MQ_INTT:
	DbErrorCheck (sqlite3_bind_int (hdl,idx,buf->cur.A->I));
	break;
      case MQ_WIDT:
	DbErrorCheck (sqlite3_bind_int64 (hdl,idx,buf->cur.A->W));
	break;
      case MQ_FLTT:
	DbErrorCheck (sqlite3_bind_double (hdl,idx,buf->cur.A->F));
	break;
      case MQ_DBLT:
	DbErrorCheck (sqlite3_bind_double (hdl,idx,buf->cur.A->D));
	break;
      case MQ_BINT:
	DbErrorCheck (sqlite3_bind_blob (hdl,idx,buf->cur.B,buf->cursize,SQLITE_TRANSIENT));
	break;
      case MQ_STRT:
	DbErrorCheck (sqlite3_bind_text (hdl,idx,buf->cur.C,buf->cursize,SQLITE_TRANSIENT));
	break;
      case MQ_LSTT: case MQ_TRAT:
	MqErrorC (mqctx, __func__, 1, "invalid protocoll");
	break;
    }
  }

label_step_1:
  switch (sqlite3_step(hdl)) {
    case SQLITE_ROW:
      switch () {
	case SQLITE_INTEGER:

	  break;
	case SQLITE_FLOAT:

	  break;
	case SQLITE_BLOB:
  
	  break;
	case SQLITE_TEXT:

	  break;
	case SQLITE_NULL:

	  break;
      }
      break;
    case SQLITE_DONE:
      break;
    case SQLITE_LOCKED:
    case SQLITE_BUSY:
      goto label_step_1;
    default:
      MqErrorC (mqctx, __func__, sqlite3_extended_errcode(brain->db), sqlite3_errmsg(brain->db));
      goto error;
  }

error:
  return MqSendRETURN(mqctx);
}

/*
static enum MqErrorE AKEP ( ARGS ) {
  SETUP_ADB;
  MQ_CBI val;
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

  MQ_CBI val;
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
    free((MQ_BIN)key);
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
    free((MQ_BIN)key);
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

*/

static enum MqErrorE EXEC ( ARGS ) {
  SETUP_brain;
  MQ_CST sql;
  MqErrorCheck(MqReadC(mqctx,&sql));
  DbErrorCheck(sqlite3_exec(brain->db,sql, NULL, NULL, NULL));

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE PREP ( ARGS ) {
  SETUP_brain;
  MQ_CST tail=NULL, sql=NULL;
  MQ_INT idx;
  for (idx=0; idx<DB_PREPARE_MAX && brain->prepare[idx]!=NULL; idx++);
  if (idx >= DB_PREPARE_MAX) {
    MqErrorV (mqctx, __func__, 1, "unable to find a free prepare entry, only %i entries are allowed.", DB_PREPARE_MAX);
    goto error;
  }
  MqErrorCheck(MqReadC(mqctx,&sql));
  DbErrorCheck(sqlite3_prepare_v2(brain->db, sql, -1, &brain->prepare[idx], &tail));
  if (tail != NULL) {
    MqErrorV (mqctx, __func__, 1, "leftover sql-statement: %s", tail);
    goto error;
  }
  MqSendSTART(mqctx);
  MqSendI(mqctx, idx);
error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE OPEN ( ARGS );

static enum MqErrorE CLOS ( ARGS ) {
  SETUP_brain;
  MqSendSTART(mqctx);

  // close the database
  MqSysFree (brain->storage);
  DbErrorCheck (sqlite3_close(brain->db));

  // add/remove services
  MqErrorCheck (MqServiceDelete (mqctx, "-ALL"));
  MqErrorCheck (MqServiceCreate (mqctx, "OPEN", OPEN, NULL, NULL));

error:
  brain->db = NULL;
  return MqSendRETURN(mqctx);
}

static enum MqErrorE OPEN ( ARGS ) {
  SETUP_brain;

  // open storage
  {
    MQ_CST dbstorage;
    MqErrorCheck(MqReadC(mqctx,&dbstorage));

    if (dbstorage == NULL || *dbstorage == '\0') {
      return MqErrorC (mqctx, __func__, 1, "storage-file is empty or invalid");
    }

    if (dbstorage[0] == '#') {
      // a "temporary" database require, by sqlite default, an "empty-string".
      // for me an "empty-string" is an error, I prefer "#tmpdb#"
      if (!strcmp(dbstorage,"#tmpdb#")) {
	dbstorage = "";
      } else if (!strcmp(dbstorage,"#memdb#")) {
	dbstorage = ":memory:";
      }
    }

    MqDLogV(mqctx,5,"try to open database '%s'\n", dbstorage);

    // open the database
    DbErrorCheck (sqlite3_open(dbstorage, &brain->db));
  }

  // add/remove services
  MqErrorCheck (MqServiceDelete (mqctx, "OPEN"));

  MqErrorCheck (MqServiceCreate (mqctx, "CLOS", CLOS, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "EXEC", EXEC, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "PREP", PREP, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "STEP", STEP, NULL, NULL));

/*
  MqErrorCheck (MqServiceCreate (mqctx, "AITI", AITI, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "AOTM", AOTM, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "AVAN", AVAN, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "ATRB", ATRB, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "ATRC", ATRC, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "ATRA", ATRA, NULL, NULL));
*/
/*
  MqErrorCheck (MqServiceCreate (mqctx, "AKEP", AKEP_W, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "AGET", AGET_W, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "AITN", AITN_W, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "AITA", AITA_W, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "AOUT", AOUT_W, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "ASIZ", ASIZ_W, NULL, NULL));
*/


  MqSendSTART(mqctx);
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

  MqSysFree (brain->storage);

  if (brain->db != NULL) {
    for (MQ_INT idx=0; idx<DB_PREPARE_MAX; idx++) {
      MqErrorCheck (IdxFinalize (mqctx, idx));
    }
    DbErrorCheck(sqlite3_close(brain->db))
  }

error:
  brain->db = NULL;
  return MqErrorStack(mqctx);
}

static enum MqErrorE
BrainSetup (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MqErrorCheck (MqServiceCreate (mqctx, "OPEN", OPEN, NULL, NULL));

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
  mqctx->setup.Child.fCreate	    = MqLinkDefault;
  mqctx->setup.Parent.fCreate	    = MqLinkDefault;
  mqctx->setup.fHelp		    = BrainHelp;
  mqctx->setup.isServer		    = MQ_YES;
  mqctx->setup.ServerSetup.fCall    = BrainSetup;
  mqctx->setup.ServerCleanup.fCall  = BrainCleanup;

  // create the ServerCtxS
  MqErrorCheck(MqLinkCreate (mqctx, &args));

  // start event-loop and wait forever
  MqProcessEvent (mqctx, MQ_TIMEOUT, MQ_WAIT_FOREVER);

  // finish and exit
error:
  MqExit (mqctx);
}

/** \} brain */

