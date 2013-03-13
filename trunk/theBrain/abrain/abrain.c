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
#include <ctype.h>

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
#define SETUP_db sqlite3*const db = brain->db
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
    return MqErrorV (mqctx, __func__, SQLITE_ERROR, "usage: %s (%s)\n", __func__, s); \
  }
#define check_sqlite(E) \
  if (unlikely((E) != SQLITE_OK))
#define check_NULL(E) \
  if (unlikely((E) == NULL))

#define DB_PREPARE_MAX 100

/// \brief the local \b context of the \ref server tool
/// \mqctx
struct BrainCtxS {
  struct MqS	mqctx;			    ///< \mqctxI
  MQ_CST	storage;		    ///< storage file
  sqlite3	*db;			    ///< database handle
  sqlite3_stmt	*prepStmt[DB_PREPARE_MAX];  ///< array prepared statement pointer
  MQ_STR	inType[DB_PREPARE_MAX];	    ///< array prepared statement input types string
  MQ_STR	inEnd[DB_PREPARE_MAX];	    ///< array prepared statement input types end pointer
  MQ_STR	outType[DB_PREPARE_MAX];    ///< array prepared statement output types string
  MQ_STR	outEnd[DB_PREPARE_MAX];	    ///< array prepared statement output types end pointer
  MQ_INT	prepare_start;		    ///< point to the next empty prepare empty
  MQ_BUF	buf;			    ///< context local storage
};

/*****************************************************************************/
/*                                                                           */
/*                               Request Handler                             */
/*                                                                           */
/*****************************************************************************/

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
    MqErrorV (mqctx, __func__, SQLITE_ERROR, "prepare index out of range: 0 <= idx <= %i", DB_PREPARE_MAX-1);
    goto error;
  }
error:
  return MqErrorStack(mqctx);
}

inline static enum MqErrorE IdxFinalize ( struct MqS * const mqctx, MQ_INT idx ) {
  SETUP_brain;
  if (brain->prepStmt[idx] != NULL) {
    DbErrorCheck(sqlite3_finalize(brain->prepStmt[idx]));
    brain->prepStmt[idx] = NULL;
    MqSysFree(brain->inType[idx]);
    brain->inEnd[idx] = NULL;
    MqSysFree(brain->outType[idx]);
    brain->outEnd[idx] = NULL;
  }
error:
  return MqErrorStack(mqctx);
}

inline static enum MqErrorE HdlGet ( 
  struct MqS * const mqctx, 
  sqlite3_stmt **phdl, 
  MQ_CST *pInType,
  MQ_CST *pInEnd,
  MQ_CST *pOutType, 
  MQ_CST *pOutEnd 
) {
  SETUP_brain;
  MQ_INT idx;
  MqErrorCheck (IdxGet (mqctx, &idx));
  check_NULL(*phdl = brain->prepStmt[idx]) {
    MqErrorV (mqctx, __func__, SQLITE_ERROR, "the prepare-index '%i' was NOT defined", idx);
  }
  *pInType = brain->inType[idx];
  *pInEnd = brain->inEnd[idx];
  *pOutType = brain->outType[idx];
  *pOutEnd = brain->outEnd[idx];
error:
  return MqErrorStack(mqctx);
}

static enum MqErrorE ctxCleanup ( struct MqS * const mqctx ) {
  SETUP_brain;
  MqSysFree (brain->storage);
  if (brain->db != NULL) {
    for (MQ_INT idx=0; idx<brain->prepare_start; idx++) {
      MqErrorCheck (IdxFinalize (mqctx, idx));
    }
    brain->prepare_start=0;
    DbErrorCheck(sqlite3_close(brain->db))
    brain->db=NULL;
  }
error:
  brain->db = NULL;
  return MqErrorStack(mqctx);
}

/*****************************************************************************/
/*                                                                           */
/*                               Service Handler                             */
/*                                                                           */
/*****************************************************************************/

inline static enum MqTypeE GetTypeE ( MQ_STRB t ) {
  switch (t) {
    case 'C': return MQ_STRT;
    case 'I': return MQ_INTT;
    case 'D': return MQ_DBLT;
    case 'W': return MQ_WIDT;
    case 'B': return MQ_BINT;
    case 'Y': return MQ_BYTT;
    case 'O': return MQ_BOLT;
    case 'S': return MQ_SRTT;
    case 'F': return MQ_FLTT;
    case 'L': return MQ_LSTT;
    case 'T': return MQ_TRAT;
  }
  return MQ_STRT;
}

inline static MQ_STRB GetTypeS ( enum MqTypeE const ntype ) {
  switch (ntype) {
    case MQ_STRT: return 'C';
    case MQ_INTT: return 'I';
    case MQ_DBLT: return 'D';
    case MQ_WIDT: return 'W';
    case MQ_BINT: return 'B';
    case MQ_BYTT: return 'Y';
    case MQ_BOLT: return 'O';
    case MQ_SRTT: return 'S';
    case MQ_FLTT: return 'F';
    case MQ_LSTT: return 'L';
    case MQ_TRAT: return 'T';
  }
  return '*';
}

inline static enum MqTypeE GetTypeD ( sqlite3_stmt *hdl, MQ_INT idx ) {
  switch (sqlite3_column_type(hdl,idx)) {
    case SQLITE_INTEGER:
      switch (sqlite3_column_bytes(hdl,idx)) {
	case 1: return MQ_BYTT;
	case 2: return MQ_SRTT;
	case 3: 
	case 4: return MQ_INTT;
	case 6: 
	case 8: return MQ_WIDT;
      }
      break;
    case SQLITE_FLOAT: return MQ_DBLT;
    case SQLITE_BLOB:  return MQ_BINT;
    case SQLITE_TEXT:  return MQ_STRT;
    case SQLITE_NULL:  return MQ_BINT;
  }
  return MQ_BINT;
}

#define case1(t,f,c) \
  case t##T: { \
    t dat; \
    MqErrorCheck(MqBufferGet ## c(buf,&dat)); \
    DbErrorCheck(f(hdl,idx,dat)); \
    break; \
  }

inline static enum MqErrorE GetFromDBnat ( 
  struct MqS * const mqctx, 
  enum MqTypeE ntype,
  sqlite3_stmt *hdl, 
  MQ_INT idx
) {
  switch (ntype) {
    case MQ_STRT: return MqSendC(mqctx,(MQ_CST)sqlite3_column_text(hdl,idx));
    case MQ_INTT: return MqSendI(mqctx,(MQ_INT)sqlite3_column_int(hdl,idx));
    case MQ_SRTT: return MqSendS(mqctx,(MQ_SRT)sqlite3_column_int(hdl,idx));
    case MQ_BYTT: return MqSendY(mqctx,(MQ_BYT)sqlite3_column_int(hdl,idx));
    case MQ_BOLT: return MqSendY(mqctx,(MQ_BOL)sqlite3_column_int(hdl,idx));
    case MQ_WIDT: return MqSendW(mqctx,(MQ_WID)sqlite3_column_int64(hdl,idx));
    case MQ_DBLT: return MqSendD(mqctx,(MQ_DBL)sqlite3_column_double(hdl,idx));
    case MQ_FLTT: return MqSendF(mqctx,(MQ_FLT)sqlite3_column_double(hdl,idx));
    case MQ_BINT: return MqSendB(mqctx,sqlite3_column_blob(hdl,idx),sqlite3_column_bytes(hdl,idx));
    case MQ_LSTT: case MQ_TRAT:
      return MqErrorV (mqctx, __func__, SQLITE_ERROR, "invalid protocoll item '%c'", GetTypeS(ntype));
  }
  return MQ_BINT;
}

inline static enum MqErrorE GetFromDBary ( 
  struct MqS * const mqctx, 
  enum MqTypeE ntype,
  sqlite3_stmt *hdl, 
  MQ_INT idx
) {
  switch (ntype) {
    case MQ_BINT:
      return MqSendB(mqctx,(MQ_BIN)sqlite3_column_blob(hdl,idx),sqlite3_column_bytes(hdl,idx));
    case MQ_STRT:
      return MqSendC(mqctx,(MQ_CST)sqlite3_column_text(hdl,idx));
    default:
      return MqSendU(mqctx,
	MqBufferCastTo( 
	  MqBufferSetC(BRAINCTX->buf, (MQ_CST)sqlite3_column_text(hdl, idx)),
	  ntype
	)
      );
  }
}

static enum MqErrorE STEP ( ARGS ) {
  SETUP_brain;
  MQ_INT idx=0;
  MQ_BUF buf;
  sqlite3_stmt *hdl=NULL;
  MQ_CST inType=NULL,  inEnd=NULL;
  MQ_CST outType=NULL, outEnd=NULL;
  enum MqTypeE ntype;

  MqErrorCheck(HdlGet(mqctx, &hdl, &inType, &inEnd, &outType, &outEnd));
  DbErrorCheck(sqlite3_reset(hdl));

  MqSendSTART(mqctx);
  while (MqReadItemExists(mqctx)) {
    idx+=1;
    MqErrorCheck(MqReadU(mqctx,&buf));
    if (inType && inType < inEnd) {
      ntype = GetTypeE(*inType);
      if (buf->type != ntype && buf->type != MQ_STRT) {
	MqErrorV(mqctx,__func__,SQLITE_ERROR,
	  "the buffer type '%c' does not match database type '%c'", 
	    MqBufferGetType(buf), *inType);
	goto error;
      }
      inType++;
    } else {
      ntype = buf->type;
    }
    switch (buf->type) {
      case MQ_BYTT: DbErrorCheck(sqlite3_bind_int(hdl,idx,buf->cur.A->Y)); break;
      case MQ_BOLT: DbErrorCheck(sqlite3_bind_int(hdl,idx,buf->cur.A->O)); break;
      case MQ_SRTT: DbErrorCheck(sqlite3_bind_int(hdl,idx,buf->cur.A->S)); break;
      case MQ_INTT: DbErrorCheck(sqlite3_bind_int(hdl,idx,buf->cur.A->I)); break;
      case MQ_WIDT: DbErrorCheck(sqlite3_bind_int64(hdl,idx,buf->cur.A->W)); break;
      case MQ_FLTT: DbErrorCheck(sqlite3_bind_double(hdl,idx,buf->cur.A->F)); break;
      case MQ_DBLT: DbErrorCheck(sqlite3_bind_double(hdl,idx,buf->cur.A->D)); break;
      case MQ_STRT: 
	switch (ntype) {
	  case1(MQ_BYT,sqlite3_bind_int,Y)
	  case1(MQ_BOL,sqlite3_bind_int,O)
	  case1(MQ_SRT,sqlite3_bind_int,S)
	  case1(MQ_INT,sqlite3_bind_int,I)
	  case1(MQ_WID,sqlite3_bind_int64,W)
	  case1(MQ_FLT,sqlite3_bind_double,F)
	  case1(MQ_DBL,sqlite3_bind_double,D)
	  case MQ_STRT: 
	    DbErrorCheck(sqlite3_bind_text (hdl,idx,buf->cur.C,buf->cursize,SQLITE_TRANSIENT)); 
	    break;
	  case MQ_BINT: 
	    if (buf->cursize == 0) {
	      DbErrorCheck (sqlite3_bind_null (hdl,idx));
	    } else {
	      DbErrorCheck (sqlite3_bind_blob (hdl,idx,buf->cur.B,buf->cursize,SQLITE_TRANSIENT));
	    }
	    break;
	  case MQ_LSTT: case MQ_TRAT:
	    MqErrorV (mqctx, __func__, SQLITE_ERROR, "invalid protocoll item '%c'", GetTypeS(ntype));
	}
	break;
      case MQ_BINT:
	if (buf->cursize == 0) {
	  DbErrorCheck (sqlite3_bind_null (hdl,idx));
	} else {
	  DbErrorCheck (sqlite3_bind_blob (hdl,idx,buf->cur.B,buf->cursize,SQLITE_TRANSIENT));
	}
	break;
      case MQ_LSTT: case MQ_TRAT:
	MqErrorV (mqctx, __func__, SQLITE_ERROR, "invalid protocoll item '%c'", GetTypeS(ntype));
	break;
    }
  }

  MQ_CST pos;
  MqSendSTART(mqctx);
  while (true) {
    switch (sqlite3_step(hdl)) {
      case SQLITE_ROW: {
	MqSendL_START(mqctx);
	for (idx=0,pos=outType; idx<sqlite3_column_count(hdl); idx++,pos++) {
	  ntype = pos && pos < outEnd ? GetTypeE(*pos) : GetTypeD(hdl,idx);
	  switch (sqlite3_column_type(hdl,idx)) {
	    case SQLITE_INTEGER:
	    case SQLITE_FLOAT:
	    case SQLITE_NULL:
	      MqErrorCheck(GetFromDBnat(mqctx, ntype, hdl, idx));
	      break;
	    case SQLITE_BLOB:
	    case SQLITE_TEXT:
	      MqErrorCheck(GetFromDBary(mqctx, ntype, hdl, idx));
	      break;
	  }
	}
	MqSendL_END(mqctx);
	continue;
	break;
      }
      case SQLITE_DONE:
	goto error;
      case SQLITE_LOCKED:
      case SQLITE_BUSY:
	continue;
      default:
	MqErrorC (mqctx, __func__, sqlite3_extended_errcode(brain->db), sqlite3_errmsg(brain->db));
	DbErrorCheck(sqlite3_reset(hdl));
	goto error;
    }
  }

error:
  sqlite3_reset(hdl);
  return MqSendRETURN(mqctx);
}

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
  MQ_CST sql,end,type,start;
  MQ_INT idx;
  MQ_BUF buf;

  // parse prefix
  MqErrorCheck(MqReadU(mqctx,&buf));
  if (buf->type == MQ_INTT) {
    MqErrorCheck(MqBufferGetI(buf,&idx));
    MqErrorCheck(IdxFinalize(mqctx, idx));
    MqErrorCheck(MqReadU(mqctx,&buf));
  } else {
    idx = brain->prepare_start;
  }
  MqErrorCheck(MqBufferGetC(buf,&sql));
  end = sql + buf->cursize;

  // process values
  MqSendSTART(mqctx);
  while (sql != NULL && *sql != '\0') {
    // get the index
    for (; idx<DB_PREPARE_MAX && brain->prepStmt[idx]!=NULL; idx++);
    if (idx >= DB_PREPARE_MAX) {
      MqErrorV (mqctx, __func__, SQLITE_ERROR, "unable to find a free prepare entry - only '%i' entries are allowed.", DB_PREPARE_MAX);
      goto error;
    }
    // find 'in:???' prefix
    brain->inType[idx]  = NULL;
    brain->inEnd[idx]   = NULL;
    brain->outType[idx] = NULL;
    brain->outEnd[idx]  = NULL;
    // 1. search for '/' as beginn of comment
    for (start=sql; start<end && *start != '/' && *start != ';'; start++);
    // 2. '/*' as 2cnd char
    if (start == end || *start++ != '/' || *start++ != '*') goto update;

  // find 'in:???' prefix
    // in1. 'WS' before "TYPE:???"
    for (; start<end && isspace(*start); start++);
    // in2. 'in:' the prefix
    if (start >= end-3 || strncmp(start, "in:", 3)) goto out;
    start+=3;
    // in3. 'ASCII' as beginn of comment
    for (type=start; start<end && isupper(*start); start++); 
    if (start == end) goto out;
    // in4. we have everything
    brain->inType[idx] = MqSysStrNDup(mqctx, type, (start-type-1));
    brain->inEnd[idx] = brain->inType[idx] + (start-type);
    *brain->inEnd[idx] = '\0';

out:
  // find 'out:???' prefix
    // out1. 'WS' before "TYPE:???"
    for (; start<end && isspace(*start); start++);
    // out2. 'out:' the prefix
    if (start >= end-4 || strncmp(start, "out:", 4)) goto update;
    start+=4;
    // out3. 'ASCII' as beginn of comment
    for (type=start; start<end && isupper(*start); start++); 
    if (start == end) goto update;
    // out4. we have everything
    brain->outType[idx] = MqSysStrNDup(mqctx, type, (start-type-1));
    brain->outEnd[idx] = brain->outType[idx] + (start-type);
    *brain->outEnd[idx] = '\0';

update:
    // update the database
    brain->prepare_start=idx+1;
    DbErrorCheck(sqlite3_prepare_v2(brain->db, sql, -1, &brain->prepStmt[idx], &sql));
    MqSendI(mqctx, idx);
  }
error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE FINA ( ARGS ) {
  MQ_INT idx;
  MqErrorCheck(IdxGet(mqctx, &idx));
  MqErrorCheck(IdxFinalize(mqctx, idx));
error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE OPEN ( ARGS );

static enum MqErrorE CLOS ( ARGS ) {
  MqErrorCheck (ctxCleanup(mqctx));
  MqErrorCheck (MqServiceDelete (mqctx, "-ALL"));
  MqErrorCheck (MqServiceCreate (mqctx, "OPEN", OPEN, NULL, NULL));

error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE OPEN ( ARGS ) {
  SETUP_brain;

  // open storage
  {
    MQ_CST dbstorage;
    MqErrorCheck(MqReadC(mqctx,&dbstorage));

    if (dbstorage == NULL) {
      return MqErrorC (mqctx, __func__, SQLITE_ERROR, "storage-file is empty or invalid");
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
  MqErrorCheck (MqServiceCreate (mqctx, "FINA", FINA, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "STEP", STEP, NULL, NULL));

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
  MqBufferDelete(&brain->buf);
  MqErrorCheck(ctxCleanup(mqctx));
error:
  return MqErrorStack(mqctx);
}

static enum MqErrorE
BrainSetup (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  SETUP_brain;
  brain->prepare_start=0;
  brain->buf = MqBufferCreate(mqctx, 10);
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

