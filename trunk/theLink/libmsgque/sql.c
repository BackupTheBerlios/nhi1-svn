/**
 *  \file       theLink/libmsgque/sql.c
 *  \brief      \$Id$
 *  
 *  (C) 2011 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "main.h"

#include "sqlite3.h"

#define MQ_CONTEXT_S context

BEGIN_C_DECLS

struct MqSqlS {
  MQ_CST  storageFile;		///< main directory used for database files
  sqlite3 *db;			///< sqlite database connection handle
  sqlite3_stmt *sendInsert;	///< prepared sql statement
  sqlite3_stmt *sendSelect;	///< prepared sql statement
  sqlite3_stmt *sendDelete;	///< prepared sql statement
  sqlite3_stmt *readInsert;	///< prepared sql statement
  sqlite3_stmt *readSelect1;	///< prepared sql statement
  sqlite3_stmt *readSelect2;	///< prepared sql statement
  sqlite3_stmt *readDelete;	///< prepared sql statement
};

/*****************************************************************************/
/*                                                                           */
/*                             private Add / Del                             */
/*                                                                           */
/*****************************************************************************/

static void sSqlFinalize (
  sqlite3 *db,
  sqlite3_stmt **prepSql
)
{
  if (prepSql == NULL || *prepSql == NULL) return;
  check_sqlite(sqlite3_finalize(*prepSql)) {
    MqErrorDbSql(MQ_ERROR_PRINT, db);
  }
  *prepSql = NULL;
}

static enum MqErrorE
sSqlDelDb (
  struct MqS * const context,
  struct MqSqlS * const sql_sys
)
{
  if (sql_sys != NULL) {
    sqlite3 * const db = sql_sys->db;

    if (sql_sys->storageFile != NULL) {
      sqlite3_free((void*)sql_sys->storageFile);
      sql_sys->storageFile = NULL;
    }
    if (sql_sys->db != NULL) {

      sSqlFinalize (db, &sql_sys->sendInsert);
      sSqlFinalize (db, &sql_sys->sendSelect);
      sSqlFinalize (db, &sql_sys->sendDelete);
      sSqlFinalize (db, &sql_sys->readInsert);
      sSqlFinalize (db, &sql_sys->readSelect1);
      sSqlFinalize (db, &sql_sys->readSelect2);
      sSqlFinalize (db, &sql_sys->readDelete);

      check_sqlite(sqlite3_close(db)) {
	return MqErrorDbSql(context, db);
      }
      sql_sys->db = NULL;
    }
  }
  return MQ_OK;
}

static enum MqErrorE
sSqlAddDb (
  struct MqS * const context, 
  MQ_CST const storageFile
)
{
  struct MqSqlS * const sql_sys = context->link.sql;
  char *errmsg = NULL;

  // transaction storage, as transaction-id the ROWID is used
  const static char SQL_SCT[] = "CREATE TABLE IF NOT EXISTS sendTrans (callback TEXT, numItems INTEGER, type INTEGER, data BLOB);";
  const static char SQL_RCT[] = "CREATE TABLE IF NOT EXISTS readTrans (ident TEXT,ctxId INTEGER,rmtTransId INTEGER,oldTransId INTEGER);";

  if (storageFile == NULL || *storageFile == '\0') {
    return MqErrorDbV(MQ_ERROR_NULL_NOT_ALLOWED, "storageFile");
  }

  check_NULL (sql_sys) {
    return MqErrorDbV(MQ_ERROR_FEATURE_NOT_AVAILABLE, "sql link");
  }

  check_sqlite (sqlite3_open(storageFile, &sql_sys->db)) {
    return MqErrorDbSql(context, sql_sys->db);
  } 

  check_sqlite (sqlite3_exec(sql_sys->db, SQL_SCT, NULL, NULL, &errmsg)) {
    MqErrorDbSql2(context, sql_sys->db, errmsg);
    sqlite3_free(errmsg);
    return MqErrorGetCodeI(context);
  } 

  check_sqlite (sqlite3_exec(sql_sys->db, SQL_RCT, NULL, NULL, &errmsg)) {
    MqErrorDbSql2(context, sql_sys->db, errmsg);
    sqlite3_free(errmsg);
    return MqErrorGetCodeI(context);
  } 

  check_NULL (sql_sys->storageFile = MqSysStrDup(context, storageFile)) {
    return MqErrorStack(context);
  } 

  return MqErrorGetCodeI(context);
}

/*****************************************************************************/
/*                                                                           */
/*                             public context                                */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
MqSqlSetDb (
  struct MqS * const context,
  MQ_CST const storageFile
)
{
  check_NULL (context->link.sql) {
    return MqErrorDbV(MQ_ERROR_FEATURE_NOT_AVAILABLE, "sql link");
  } else if (storageFile == NULL || context->link.sql->storageFile == NULL || 
	      strcmp(context->link.sql->storageFile,storageFile)) {
    // only change database if  "storageFile" is new
    MqErrorCheck (sSqlDelDb(context, context->link.sql));
    MqErrorCheck (sSqlAddDb(context, storageFile));
  }
  return MqErrorGetCodeI(context);
error:
  return MqErrorStack(context);
}

/*****************************************************************************/
/*                                                                           */
/*                    protected context - sql layer                          */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
pSqlInsertSendTrans (
  struct MqS * const context, 
  MQ_TOK const callback, 
  MQ_BUF buf, 
  MQ_WID *transId
)
{
  struct MqSqlS * const sql_sys = context->link.sql;
  register sqlite3_stmt *hdl= sql_sys->sendInsert;
  check_NULL(sql_sys->db) {
    MqErrorCheck1 (sSqlAddDb (context, ":memory:"));
  }
  check_NULL(hdl) {
    const static char sql[] = "INSERT INTO sendTrans (callback, numItems, type, data) VALUES (?, ?, ?, ?);";
    check_sqlite (sqlite3_prepare_v2(sql_sys->db, sql, -1, &sql_sys->sendInsert, NULL)) goto error;
    hdl = sql_sys->sendInsert;
  }
  check_sqlite (sqlite3_reset	   (hdl))					    goto error;
  check_sqlite (sqlite3_bind_text  (hdl,1,callback,HDR_TOK_LEN,SQLITE_TRANSIENT))   goto error;
  check_sqlite (sqlite3_bind_int   (hdl,2,buf->numItems))			    goto error;
  check_sqlite (sqlite3_bind_int   (hdl,3,buf->type))				    goto error;
  check_sqlite (sqlite3_bind_blob  (hdl,4,buf->data,buf->cursize,SQLITE_TRANSIENT)) goto error;
  if (sqlite3_step(hdl) != SQLITE_DONE) {
    return MqErrorDbSql(context,sql_sys->db);
  }
  *transId = sqlite3_last_insert_rowid(sql_sys->db);
  return MQ_OK;
error:
  return MqErrorDbSql(context,sql_sys->db);
error1:
  return MqErrorStack(context);
}

enum MqErrorE
pSqlSelectSendTrans (
  struct MqS * const context, 
  MQ_WID transId,
  MQ_BUF buf 
)
{
  struct MqSqlS * const sql_sys = context->link.sql;
  register sqlite3_stmt *hdl= sql_sys->sendSelect;
  check_NULL(sql_sys->db) {
    MqErrorCheck1 (sSqlAddDb (context, ":memory:"));
  }
  check_NULL(hdl) {
    const static char sql[] = "SELECT callback, numItems, type, data FROM sendTrans WHERE rowid = ?;";
    check_sqlite (sqlite3_prepare_v2(sql_sys->db, sql, -1, &sql_sys->sendSelect, NULL)) goto error;
    hdl = sql_sys->sendSelect;
  }
  check_sqlite (sqlite3_reset(hdl))		    goto error;
  check_sqlite (sqlite3_bind_int64(hdl,1,transId))  goto error;
  switch (sqlite3_step(hdl)) {
    case SQLITE_ROW:
      break;
    case SQLITE_DONE:
      return MqErrorDbV(MQ_ERROR_ID_NOT_FOUND,"transaction",transId);
    default:
      return MqErrorDbSql(context,sql_sys->db);
  }
  pTokenSetCurrent(context->link.srvT, (MQ_CST const)sqlite3_column_text(hdl, 0));
  MqBufferSetB(buf, (const unsigned char*)sqlite3_column_blob(hdl, 3), sqlite3_column_bytes(hdl, 3));
  buf->numItems = sqlite3_column_int(hdl, 1);
  buf->type = (enum MqTypeE) sqlite3_column_int(hdl, 2);
  return MQ_OK;
error:
  return MqErrorDbSql(context,sql_sys->db);
error1:
  return MqErrorStack(context);
}

enum MqErrorE
pSqlDeleteSendTrans (
  struct MqS * const context, 
  MQ_WID *transIdP
)
{
  struct MqSqlS * const sql_sys = context->link.sql;
  register sqlite3_stmt * hdl = sql_sys->sendDelete;
  check_NULL(sql_sys->db) {
    MqErrorCheck1 (sSqlAddDb (context, ":memory:"));
  }
  check_NULL(hdl) {
    const static char sql[] = "DELETE FROM sendTrans WHERE rowid = ?;";
    check_sqlite (sqlite3_prepare_v2(sql_sys->db, sql, -1, &sql_sys->sendDelete, NULL)) goto error;
    hdl = sql_sys->sendDelete;
  }
  check_sqlite (sqlite3_reset(hdl))		     goto error;
  check_sqlite (sqlite3_bind_int64(hdl,1,*transIdP)) goto error;
  if (sqlite3_step(hdl) != SQLITE_DONE) {
    return MqErrorDbSql(context,sql_sys->db);
  }
  *transIdP = 0LL;
  return MqErrorGetCodeI(context);
error:
  return MqErrorDbSql(context,sql_sys->db);
error1:
  return MqErrorStack(context);
}

enum MqErrorE
pSqlInsertReadTrans (
  struct MqS * const context, 
  MQ_CST const ident, 
  MQ_WID const rmtTransId, 
  MQ_WID const oldTransId, 
  MQ_WID *transIdP
)
{
  struct MqSqlS * const sql_sys = context->link.sql;
  register sqlite3_stmt *hdl= sql_sys->readInsert;
  check_NULL(sql_sys->db) {
    MqErrorCheck1 (sSqlAddDb (context, ":memory:"));
  }
  check_NULL(hdl) {
    const static char sql[] = "INSERT INTO readTrans (ident, ctxId, rmtTransId, oldTransId ) VALUES (?, ?, ?, ?);";
    check_sqlite (sqlite3_prepare_v2(sql_sys->db, sql, -1, &sql_sys->readInsert, NULL)) goto error;
    hdl = sql_sys->readInsert;
  }
  check_sqlite(sqlite3_reset	  (hdl))			      goto error;
  check_sqlite(sqlite3_bind_text  (hdl,1,ident,-1,SQLITE_TRANSIENT))  goto error;
  check_sqlite(sqlite3_bind_int   (hdl,2,context->link.ctxId))	      goto error;
  check_sqlite(sqlite3_bind_int64 (hdl,3,rmtTransId))		      goto error;
  check_sqlite(sqlite3_bind_int64 (hdl,4,oldTransId))		      goto error;
  if (sqlite3_step(hdl) != SQLITE_DONE) {
    return MqErrorDbSql(context,sql_sys->db);
  }
  *transIdP = sqlite3_last_insert_rowid(sql_sys->db);
  return MQ_OK;
error:
  return MqErrorDbSql(context,sql_sys->db);
error1:
  return MqErrorStack(context);
}

enum MqErrorE
pSqlSelectReadTrans (
  struct MqS * const context, 
  MQ_WID transId
)
{
  struct MqSqlS * const sql_sys = context->link.sql;
  register sqlite3_stmt *hdl= sql_sys->readSelect1;
  check_NULL(sql_sys->db) {
    MqErrorCheck1 (sSqlAddDb (context, ":memory:"));
  }
  check_NULL(hdl) {
    const static char sql[] = "SELECT ident, rmtTransId FROM readTrans WHERE rowid = ?;";
    check_sqlite (sqlite3_prepare_v2(sql_sys->db, sql, -1, &sql_sys->readSelect1, NULL)) goto error;
    hdl = sql_sys->readSelect1;
  }
  check_sqlite (sqlite3_reset(hdl))		    goto error;
  check_sqlite (sqlite3_bind_int64(hdl,1,transId))  goto error;
  switch (sqlite3_step(hdl)) {
    case SQLITE_ROW:
      break;
    case SQLITE_DONE:
      return MqErrorDbV(MQ_ERROR_ID_NOT_FOUND,"transaction",transId);
    default:
      return MqErrorDbSql(context,sql_sys->db);
  }
  MqErrorCheck1 (MqSendC (context, (MQ_CST) sqlite3_column_text	(hdl, 0)));
  MqErrorCheck1 (MqSendW (context, sqlite3_column_int64	(hdl, 1)));
  return MQ_OK;
error:
  return MqErrorDbSql(context,sql_sys->db);
error1:
  return MqErrorStack(context);
}

enum MqErrorE
pSqlDeleteReadTrans (
  struct MqS * const context, 
  MQ_WID *transIdP,
  MQ_WID *oldTransId
)
{
  struct MqSqlS * const sql_sys = context->link.sql;
  register sqlite3_stmt * hdl = sql_sys->readSelect2;
  check_NULL(sql_sys->db) {
    MqErrorCheck1 (sSqlAddDb (context, ":memory:"));
  }
  check_NULL(hdl) {
    const static char sql[] = "SELECT oldTransId FROM readTrans WHERE rowid = ?;";
    check_sqlite (sqlite3_prepare_v2(sql_sys->db, sql, -1, &sql_sys->readSelect2, NULL)) goto error;
    hdl = sql_sys->readSelect2;
  }
  // get oldTransId
  check_sqlite (sqlite3_reset(hdl))		      goto error;
  check_sqlite (sqlite3_bind_int64(hdl,1,*transIdP))  goto error;
  switch (sqlite3_step(hdl)) {
    case SQLITE_ROW:
      break;
    case SQLITE_DONE:
      return MqErrorDbV(MQ_ERROR_ID_NOT_FOUND,"transaction",*transIdP);
    default:
      return MqErrorDbSql(context,sql_sys->db);
  }
  *oldTransId = sqlite3_column_int64 (hdl, 0);
  // delete row
  hdl = sql_sys->readDelete;
  check_NULL(hdl) {
    const static char sql[] = "DELETE FROM readTrans WHERE rowid = ?;";
    check_sqlite (sqlite3_prepare_v2(sql_sys->db, sql, -1, &sql_sys->readDelete, NULL)) goto error;
    hdl = sql_sys->readDelete;
  }
  check_sqlite (sqlite3_reset(hdl))		      goto error;
  check_sqlite (sqlite3_bind_int64(hdl,1,*transIdP))  goto error;
  if (sqlite3_step(hdl) != SQLITE_DONE) {
    return MqErrorDbSql(context,sql_sys->db);
  }
  *transIdP = 0LL;
  return MqErrorGetCodeI(context);
error:
  return MqErrorDbSql(context,sql_sys->db);
error1:
  return MqErrorStack(context);
}

/*****************************************************************************/
/*                                                                           */
/*                           protected setup                                 */
/*                                                                           */
/*****************************************************************************/

static MqThreadKeyType sql_key = MqThreadKeyNULL;

static struct MqSqlS*
sSqlAlloc(
  struct MqS * const context
)
{
  struct MqSqlS * sql_sys = (struct MqSqlS *) MqThreadGetTLS(sql_key);
  if (sql_sys == NULL) {
    sql_sys = (struct MqSqlS *) MqSysCalloc (context, 1, sizeof (*sql_sys));
    if (sql_sys == NULL) {
      MqErrorStack(context);
    } else if (MqThreadSetTLSCheck(sql_key,sql_sys)) {
      MqSysFree(sql_sys);
      MqErrorC (context, __func__, -1, "unable to alloc Thread-Local-Storage data");
    }
  }
  return sql_sys;
}

enum MqErrorE
pSqlCreate (
  struct MqS * const context,
  struct MqSqlS ** const thread
) {
  if (*thread == NULL) {
    *thread = sSqlAlloc(context);
    MqDLogV(context,4,"TLS-Id<%p>\n", (void*)*thread);
  }
  return MqErrorStack(context);
}

void
pSqlDelete (
  struct MqSqlS ** thread
) {
  if (*thread == NULL) {
    return;
  } else {
    *thread = NULL;
  }
}

/*****************************************************************************/
/*                                                                           */
/*                           protected setup                                 */
/*                                                                           */
/*****************************************************************************/

void
SqlCreate (void)
{
  MqThreadKeyCreate(sql_key);
}

void
SqlDelete (void)
{
  if (sql_key != MqThreadKeyNULL) {
    struct MqSqlS * sql_sys = (struct MqSqlS *) MqThreadGetTLS(sql_key);
    sSqlDelDb (MQ_ERROR_PRINT, sql_sys);
    MqThreadSetTLS(sql_key, NULL);
    MqSysFree(sql_sys);
  }
}

END_C_DECLS


