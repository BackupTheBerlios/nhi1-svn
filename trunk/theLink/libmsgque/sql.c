/**
 *  \file       theLink/libmsgque/factory.c
 *  \brief      \$Id: factory.c 319 2010-11-21 15:57:51Z aotto1968 $
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 319 $
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "main.h"
#include "factory.h"
#include "error.h"
#include "token.h"
#include "sqlite3.h"
#include "errno.h"

#define MQ_CONTEXT_S context

BEGIN_C_DECLS

struct MqFactoryS *defaultFactoryItem = NULL;

struct MqFactoryTransS {
  MQ_CST  storageDir;	      ///< main directory used for database files
  sqlite3 *db;		      ///< sqlite database connection handle
};

/// \brief a static Factory function return this enum as status
/// \details Use the #MqFactoryErrorMsg function to \copybrief MqFactoryErrorMsg
enum MqFactoryReturnE {
  MQ_FACTORY_RETURN_OK,
  MQ_FACTORY_RETURN_ERR,
  MQ_FACTORY_RETURN_ADD_ERR,
  MQ_FACTORY_RETURN_ADD_TRANS_ERR,
  MQ_FACTORY_RETURN_ADD_IDENT_IN_USE_ERR,
  MQ_FACTORY_RETURN_CREATE_FUNCTION_REQUIRED,
  MQ_FACTORY_RETURN_CALL_ERR,
  MQ_FACTORY_RETURN_COPY_ERR,
  MQ_FACTORY_RETURN_ITEM_IS_NULL,
  MQ_FACTORY_RETURN_INVALID_IDENT,
  MQ_FACTORY_RETURN_CALLOC_ERR,
  MQ_FACTORY_RETURN_SQL_ERR,
  MQ_FACTORY_RETURN_DEFAULT_ERR,
  MQ_FACTORY_RETURN_GET_ERR
};

MQ_CST sFactoryReturnMsg (
  enum MqFactoryReturnE const ret
) {
  switch (ret) {
   case MQ_FACTORY_RETURN_OK:
    return "OK";
   case MQ_FACTORY_RETURN_ERR:
    return "inspecified factory error";
   case MQ_FACTORY_RETURN_CREATE_FUNCTION_REQUIRED:	
    return "unable to define factory, create function is required";
   case MQ_FACTORY_RETURN_ADD_IDENT_IN_USE_ERR:
    return "factory identifer already in use";
   case MQ_FACTORY_RETURN_CALL_ERR:
    return "unable to call factory for identifer";
   case MQ_FACTORY_RETURN_ADD_ERR:
    return "unable to add a new factory";
   case MQ_FACTORY_RETURN_INVALID_IDENT:
    return "invalid identifier, value have to be set to a non-empty string";
   case MQ_FACTORY_RETURN_ADD_TRANS_ERR:
    return "unable to use the storage directory";
   case MQ_FACTORY_RETURN_CALLOC_ERR:
    return "unable to add transaction storage";
   case MQ_FACTORY_RETURN_ITEM_IS_NULL:
    return "item is NULL";
   case MQ_FACTORY_RETURN_SQL_ERR:
    return "factory sql error";
   case MQ_FACTORY_RETURN_COPY_ERR:
    return "unable to copy factory";
   case MQ_FACTORY_RETURN_DEFAULT_ERR:
    return "unable to create the default factory";
   case MQ_FACTORY_RETURN_GET_ERR:
    return "unable to get factory";
  }
  return "nothing";
}

/// \brief everything \e io need for local storage
struct MqFactoryThreadS {
  MQ_CST  storageDir;		///< main directory used for database files
  sqlite3 *db;			///< sqlite database connection handle
  sqlite3_stmt *sendInsert;	///< prepared sql statement
  sqlite3_stmt *sendSelect;	///< prepared sql statement
  sqlite3_stmt *readInsert;	///< prepared sql statement
  sqlite3_stmt *readSelect1;	///< prepared sql statement
  sqlite3_stmt *readSelect2;	///< prepared sql statement
  sqlite3_stmt *readDelete;	///< prepared sql statement
};

#define pFactoryCmp(s1,s2) strcmp(s1,s2)

/*****************************************************************************/
/*                                                                           */
/*                               factory_define                              */
/*                                                                           */
/*****************************************************************************/

static struct pFactorySpaceS {
  struct MqFactoryS *items;	    ///< array of the items
  MQ_SIZE size;			    ///< max possible number of items
  MQ_SIZE used;			    ///< number of used entries in the items
} space;

#define SPACE_INIT_SIZE 10

/*****************************************************************************/
/*                                                                           */
/*                              private                                      */
/*                                                                           */
/*****************************************************************************/

static MQ_STR sFactoryStrDup (
  MQ_CST s
) {
  MQ_STR result = (MQ_STR) sqlite3_malloc (strlen(s) + 1);
  if (result == NULL)
    return NULL;
  strcpy(result, s);
  return result;
}

static MQ_PTR sFactoryCalloc (
  MQ_SIZE  nelem, 
  MQ_SIZE  elsize
) {
  register MQ_PTR ptr;  

  if (nelem == 0 || elsize == 0)
    nelem = elsize = 1;
  
  ptr = sqlite3_malloc (nelem * elsize);
  if (ptr) {
    memset(ptr,'\0', nelem * elsize);
  }
  
  return ptr;
}

static void sFactoryInit (
  struct MqFactoryS * const item,
  MQ_CST ident,
  struct MqFactoryCreateS Create,
  struct MqFactoryDeleteS Delete
)
{
  item->ident = sFactoryStrDup(ident);
  item->called = MQ_NO;
  item->Create = Create;
  item->Delete = Delete;
}

/*****************************************************************************/
/*                                                                           */
/*                             private Add / Del                             */
/*                                                                           */
/*****************************************************************************/

static void sFactoryAddSpace (
  MQ_SIZE const add
);

static void sFactorySpaceDelAll (
  void
);

// *********************************************************

static void sFactoryPrepSqlFinalize (
  sqlite3 *db,
  sqlite3_stmt **prepSql
)
{
  if (prepSql == NULL || *prepSql == NULL) return;
  check_sqlite(sqlite3_finalize(*prepSql)) {
    MqErrorDbFactorySql(MQ_ERROR_PRINT, db);
  }
  *prepSql = NULL;
}

static enum MqErrorE
sFactoryDelDb (
  struct MqS * const context,
  struct MqFactoryThreadS * const factory_sys
)
{
  if (factory_sys != NULL) {
    sqlite3 * const db = factory_sys->db;

    sFactoryPrepSqlFinalize (db, &factory_sys->sendInsert);
    sFactoryPrepSqlFinalize (db, &factory_sys->sendSelect);
    sFactoryPrepSqlFinalize (db, &factory_sys->readInsert);
    sFactoryPrepSqlFinalize (db, &factory_sys->readSelect1);
    sFactoryPrepSqlFinalize (db, &factory_sys->readSelect2);
    sFactoryPrepSqlFinalize (db, &factory_sys->readDelete);

    if (factory_sys->storageDir != NULL) {
      sqlite3_free((void*)factory_sys->storageDir);
      factory_sys->storageDir = NULL;
    }
    if (factory_sys->db != NULL) {
      check_sqlite(sqlite3_close(db)) {
	return MqErrorDbFactorySql(context, db);
      }
      factory_sys->db = NULL;
    }
  }
  return MQ_OK;
}

static enum MqErrorE
sFactoryAddDb (
  struct MqS * const context, 
  MQ_CST const storageDir
)
{
  struct MqFactoryThreadS * const factory_sys = context->link.factory;
  char *errmsg = NULL;

  // transaction storage, as transaction-id the ROWID is used
  const static char SQL_SCT[] = "CREATE TABLE IF NOT EXISTS sendTrans (callback TEXT, numItems INTEGER, type INTEGER, data BLOB);";
  const static char SQL_RCT[] = "CREATE TABLE IF NOT EXISTS readTrans (ident TEXT,ctxId INTEGER,rmtTransId INTEGER,oldTransId INTEGER);";

  check_NULL (factory_sys) {
    return MqErrorDbFactoryMsg(context, MQ_FACTORY_RETURN_ADD_ERR, "factory not available");
  }

  check_sqlite (sqlite3_open(storageDir, &factory_sys->db)) {
    return MqErrorDbFactorySql(context, factory_sys->db);
  } 

  check_sqlite (sqlite3_exec(factory_sys->db, SQL_SCT, NULL, NULL, &errmsg)) {
    MqErrorDbFactorySql2(context, factory_sys->db, errmsg);
    sqlite3_free(errmsg);
    return MqErrorGetCodeI(context);
  } 

  check_sqlite (sqlite3_exec(factory_sys->db, SQL_RCT, NULL, NULL, &errmsg)) {
    MqErrorDbFactorySql2(context, factory_sys->db, errmsg);
    sqlite3_free(errmsg);
    return MqErrorGetCodeI(context);
  } 

  check_NULL (factory_sys->storageDir = sFactoryStrDup(storageDir)) {
    return MqErrorDbFactoryMsg(context, MQ_FACTORY_RETURN_ADD_ERR, "unable to duplicate 'storageDir' string");
  } 

  return MqErrorGetCodeI(context);
}

static void
sFactoryAddSpace (
  MQ_SIZE const add
)
{
  register MQ_SIZE newsize = space.used + add;

  if (newsize <= space.size)
    return;

  // add extra space
  newsize *= 2;

  // alloc new space
  space.items = (struct MqFactoryS *)
	sqlite3_realloc (space.items, (newsize * sizeof (*space.items)));
  memset(space.items+space.used, '\0', newsize-space.used);
  space.size = newsize;
}

static void
sFactorySpaceDelItem (
  MQ_SIZE id
)
{
  if (space.items[id].Create.data && space.items[id].Create.fFree) {
    (*space.items[id].Create.fFree) (&space.items[id].Create.data);
  }
  if (space.items[id].Delete.data && space.items[id].Delete.fFree) {
    (*space.items[id].Delete.fFree) (&space.items[id].Delete.data);
  }
  sqlite3_free((void*)space.items[id].ident);
}

static void
sFactorySpaceDelAll (
  void
)
{
  if (space.items != NULL) {
    MQ_SIZE id;

    // name == "-ALL"
    // delete the other name
    for (id=0; id<space.used; id++) {
      sFactorySpaceDelItem (id);
    }
    // set all items to zero
    memset(space.items, '\0', space.used * sizeof(struct MqFactoryS));
    space.used = 0;
    defaultFactoryItem = NULL;
  }
}

static struct MqFactoryS *
sFactoryAddName (
  MQ_CST const ident,
  struct MqFactoryCreateS Create,
  struct MqFactoryDeleteS Delete
)
{
  struct MqFactoryS *free = MqFactoryGet(ident);

  if (free != NULL) {
    // item is available, -> error
    MqErrorDbFactoryNum(MQ_ERROR_PRINT, MQ_FACTORY_RETURN_ADD_IDENT_IN_USE_ERR);
    return NULL;
  } else {
    // item not available, add new one
    sFactoryAddSpace (1);

    //free = space.items + space.used * sizeof(*space.items);
    free = space.items + space.used;

    space.used += 1;

    free->signature = MQ_MqFactoryS_SIGNATURE;
    free->ident = sFactoryStrDup(ident);

    free->Create = Create;
    free->Delete = Delete;

    return free;
  }
}

/*****************************************************************************/
/*                                                                           */
/*                              protected                                    */
/*                                                                           */
/*****************************************************************************/

void
pFactoryMark (
  struct MqS * const context,
  MqMarkF markF
)
{
	   struct MqFactoryS * start = space.items;
  register struct MqFactoryS * end = start + space.used;
  while (start < end--) {
    if (end->Create.data) (*markF)(end->Create.data);
    if (end->Delete.data) (*markF)(end->Delete.data);
  }
}

void 
pFactoryCtxItemSet (
  struct MqS * const context,
  struct MqFactoryS * const item
) {
  context->setup.factory = item;
  MqConfigUpdateName(context, item->ident);
}

/*****************************************************************************/
/*                                                                           */
/*                               public misc                                 */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE 
MqFactoryInvoke (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryS* item,
  struct MqS ** contextP
)
{
  struct MqS * mqctx;

  if (item != NULL) {
    // check for 'fCall'
    if (!item->Create.fCall) goto error2;
    // call the factory
    MqErrorCheck((item->Create.fCall)(tmpl, create, item, &mqctx));
    // we need to !cleanup! the factory
    mqctx->link.bits.doFactoryCleanup = MQ_YES;
    // child setup
    if (create == MQ_FACTORY_NEW_CHILD) {
      // child inherit "ignoreExit" from "template"
      mqctx->setup.ignoreExit = tmpl->setup.ignoreExit;
    }
    // ist the object the "first" object created?
    mqctx->statusIs = create == MQ_FACTORY_NEW_INIT ? MQ_STATUS_IS_INITIAL : MQ_STATUS_IS_DUP;
    // set Factory on a new object
    pFactoryCtxItemSet (mqctx, item);
  } else {
    goto error2;
  }

  *contextP = mqctx;
  return MQ_OK;

error:
  *contextP = NULL;
  if (create != MQ_FACTORY_NEW_INIT) {
    if (mqctx != NULL) {
      MqErrorCopy(tmpl, mqctx);
      MqContextDelete(&mqctx);
    }
    return MqErrorStack(tmpl);
  } else {
    if (mqctx != NULL) {
      MqErrorC(MQ_ERROR_PRINT, __func__, -1, MqErrorGetText(mqctx));
    }
    return MQ_ERROR;
  }

error2:
  *contextP = NULL;
  if (create != MQ_FACTORY_NEW_INIT || tmpl == MQ_ERROR_PRINT) {
    return MqErrorDbV2(tmpl, MQ_ERROR_CONFIGURATION_REQUIRED, "Factory", "MqSetupS::Factory");
  } else {
    return MQ_ERROR;
  }
}

enum MqErrorE
MqFactoryDefaultCreate (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryS* item,
  struct MqS  ** contextP
)
{
  *contextP = MqContextCreate (0, tmpl);
  // I don't know anything about the target
  // -> use the template as source of the setup
  if (create != MQ_FACTORY_NEW_INIT) MqSetupDup (*contextP, tmpl);
  return MQ_OK;
}

/*****************************************************************************/
/*                                                                           */
/*                              public ident                                 */
/*                                                                           */
/*****************************************************************************/

MQ_CST
MqFactoryDefaultIdent (
  void
) {
  return space.items[0].ident ? space.items[0].ident : "";
}

struct MqFactoryS *
MqFactoryGet (
  MQ_CST const ident
)
{
  if (ident == NULL || *ident == '\0') {
    return &space.items[0]; // return "default" item
  } else {
    register struct MqFactoryS * end = space.items + space.used;
    while (space.items < end-- && strcmp(end->ident,ident)) {}
    return end >= space.items ? end : NULL; // found : not found
  }
}

struct MqFactoryS *
MqFactoryAdd (
  MQ_CST	      const ident,
  MqFactoryCreateF    const fCreate,
  MQ_PTR	      const createData,
  MqFactoryDataFreeF  const createDatafreeF,
  MqFactoryDataCopyF  const createDataCopyF,
  MqFactoryDeleteF    const fDelete,
  MQ_PTR	      const deleteData,
  MqFactoryDataFreeF  const deleteDatafreeF,
  MqFactoryDataCopyF  const deleteDataCopyF
)
{
  struct MqFactoryCreateS Create = {fCreate, createData, createDatafreeF, createDataCopyF};
  struct MqFactoryDeleteS Delete = {fDelete, deleteData, deleteDatafreeF, deleteDataCopyF};
  if (ident == NULL || *ident == '\0') {
    MqErrorDbFactoryNum(MQ_ERROR_PRINT, MQ_FACTORY_RETURN_INVALID_IDENT);
    return NULL;
  }
  if (fCreate == NULL) {
    MqErrorDbFactoryNum(MQ_ERROR_PRINT, MQ_FACTORY_RETURN_CREATE_FUNCTION_REQUIRED);
    return NULL;
  }
  return sFactoryAddName (ident, Create, Delete);
}

struct MqFactoryS *
MqFactoryDefault (
  MQ_CST	      const ident,
  MqFactoryCreateF    const fCreate,
  MQ_PTR	      const createData,
  MqFactoryDataFreeF  const createDataFreeF,
  MqFactoryDataCopyF  const createDataCopyF,
  MqFactoryDeleteF    const fDelete,
  MQ_PTR	      const deleteData,
  MqFactoryDataFreeF  const deleteDataFreeF,
  MqFactoryDataCopyF  const deleteDataCopyF
)
{
  struct MqFactoryCreateS Create = {fCreate, createData, createDataFreeF, createDataCopyF};
  struct MqFactoryDeleteS Delete = {fDelete, deleteData, deleteDataFreeF, deleteDataCopyF};
  if (ident == NULL || *ident == '\0') {
    MqErrorDbFactoryNum(MQ_ERROR_PRINT, MQ_FACTORY_RETURN_INVALID_IDENT);
    return NULL;
  }
  if (fCreate == NULL) {
    MqErrorDbFactoryNum(MQ_ERROR_PRINT, MQ_FACTORY_RETURN_CREATE_FUNCTION_REQUIRED);
    return NULL;
  }
  sFactorySpaceDelItem (0);
  sFactoryInit (&space.items[0], ident, Create, Delete);
  defaultFactoryItem = &space.items[0];
  return defaultFactoryItem;
}

/*****************************************************************************/
/*                                                                           */
/*                               public item                                 */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
MqFactoryCall (
  struct MqFactoryS * const item,
  MQ_PTR const data,
  struct MqS ** ctxP
)
{
  *ctxP = NULL;
  if (item == NULL) {
    return MqErrorDbFactoryNum(MQ_ERROR_PRINT,MQ_FACTORY_RETURN_ITEM_IS_NULL);
  } else if (MqErrorCheckI(MqFactoryInvoke ((struct MqS * const)data, MQ_FACTORY_NEW_INIT, item, ctxP))) {
    return MqErrorDbFactoryNum(MQ_ERROR_PRINT,MQ_FACTORY_RETURN_CALL_ERR);
  }
  MqConfigUpdateName(*ctxP, item->ident);
  return MQ_OK;
}

MQ_PTR
MqFactoryItemGetCreateData(
  struct MqFactoryS  const * const item
)
{
  return item->Create.data;
}

MQ_PTR
MqFactoryItemGetDeleteData(
  struct MqFactoryS  const * const item
) {
  return item->Delete.data;
}

struct MqFactoryS *
MqFactoryCopy (
  struct MqFactoryS * const item,
  MQ_CST const ident
)
{
  if (item == NULL) {
    MqErrorDbFactoryNum(MQ_ERROR_PRINT, MQ_FACTORY_RETURN_ITEM_IS_NULL);
    return NULL;
  } else if (ident == NULL || *ident == '\0') {
    MqErrorDbFactoryNum(MQ_ERROR_PRINT, MQ_FACTORY_RETURN_INVALID_IDENT);
    return NULL;
  } else {
    struct MqFactoryCreateS Create = item->Create;
    struct MqFactoryDeleteS Delete = item->Delete;
    if (Create.data != NULL && Create.fCopy != NULL) {
      (*Create.fCopy) (&Create.data);
    }
    if (Delete.data != NULL && Delete.fCopy != NULL) {
      (*Delete.fCopy) (&Delete.data);
    }
    return sFactoryAddName (ident, Create, Delete);
  }
}

/*****************************************************************************/
/*                                                                           */
/*                            public Set / Get                               */
/*                                                                           */
/*****************************************************************************/

void
MqFactorySetCalled (
  struct MqFactoryS * const item,
  MQ_BOL const called
)
{
  item->called = called;
}

/*****************************************************************************/
/*                                                                           */
/*                             public context                                */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
MqFactoryCtxSetDb (
  struct MqS * const context,
  MQ_CST const storageDir
)
{
  MqErrorCheck (sFactoryDelDb(context, context->link.factory));
  MqErrorCheck (sFactoryAddDb(context, storageDir));
  return MqErrorGetCodeI(context);
error:
  return MqErrorStack(context);
}

enum MqErrorE 
MqFactoryCtxSet (
  struct MqS * const context,
  struct MqFactoryS * const item
) {
  check_NULL(item) {
    return MqErrorDbFactoryNum(context,MQ_FACTORY_RETURN_ITEM_IS_NULL);
  } else {
    context->setup.factory = item;
    MqConfigUpdateName(context, item->ident);
    return MQ_OK;
  }
}

struct MqFactoryS * const
MqFactoryCtxGet (
  struct MqS const * const context
)
{
  return context->setup.factory;
}

enum MqErrorE 
MqFactoryCtxIdentSet (
  struct MqS * const context,
  MQ_CST ident
) {
  struct MqFactoryS * const item = MqFactoryGet(ident);
  check_NULL(item) {
    return MqErrorDbFactoryNum(context,MQ_FACTORY_RETURN_ITEM_IS_NULL);
  }
  context->setup.factory = item;
  MqConfigUpdateName(context, ident);
  return MQ_OK;
}

MQ_CST 
MqFactoryCtxIdentGet (
  struct MqS const * const context
)
{
  return context && context->setup.factory ? context->setup.factory->ident : "";
}

/*****************************************************************************/
/*                                                                           */
/*                    protected context - sql layer                          */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
pFactoryCtxInsertSendTrans (
  struct MqS * const context, 
  MQ_TOK const callback, 
  MQ_BUF buf, 
  MQ_WID *transId
)
{
  struct MqFactoryThreadS * const factory_sys = context->link.factory;
  register sqlite3_stmt *hdl= factory_sys->sendInsert;
  check_NULL(factory_sys->db) {
    MqErrorCheck1 (sFactoryAddDb (context, ":memory:"));
  }
  check_NULL(hdl) {
    const static char sql[] = "INSERT INTO sendTrans (callback, numItems, type, data) VALUES (?, ?, ?, ?);";
    check_sqlite (sqlite3_prepare_v2(factory_sys->db, sql, -1, &factory_sys->sendInsert, NULL)) goto error;
    hdl = factory_sys->sendInsert;
  }
  check_sqlite (sqlite3_reset	   (hdl))					    goto error;
  check_sqlite (sqlite3_bind_text  (hdl,1,callback,HDR_TOK_LEN,SQLITE_TRANSIENT))   goto error;
  check_sqlite (sqlite3_bind_int   (hdl,2,buf->numItems))			    goto error;
  check_sqlite (sqlite3_bind_int   (hdl,3,buf->type))				    goto error;
  check_sqlite (sqlite3_bind_blob  (hdl,4,buf->data,buf->cursize,SQLITE_TRANSIENT)) goto error;
  if (sqlite3_step(hdl) != SQLITE_DONE) {
    return MqErrorDbFactorySql(context,factory_sys->db);
  }
  *transId = sqlite3_last_insert_rowid(factory_sys->db);
  return MQ_OK;
error:
  return MqErrorDbFactorySql(context,factory_sys->db);
error1:
  return MqErrorStack(context);
}

enum MqErrorE
pFactoryCtxSelectSendTrans (
  struct MqS * const context, 
  MQ_WID transId,
  MQ_BUF buf 
)
{
  struct MqFactoryThreadS * const factory_sys = context->link.factory;
  register sqlite3_stmt *hdl= factory_sys->sendSelect;
  check_NULL(factory_sys->db) {
    MqErrorCheck1 (sFactoryAddDb (context, ":memory:"));
  }
  check_NULL(hdl) {
    const static char sql[] = "SELECT callback, numItems, type, data FROM sendTrans WHERE rowid = ?;";
    check_sqlite (sqlite3_prepare_v2(factory_sys->db, sql, -1, &factory_sys->sendSelect, NULL)) goto error;
    hdl = factory_sys->sendSelect;
  }
  check_sqlite (sqlite3_reset(hdl))		    goto error;
  check_sqlite (sqlite3_bind_int64(hdl,1,transId))  goto error;
  switch (sqlite3_step(hdl)) {
    case SQLITE_ROW:
      break;
    case SQLITE_DONE:
      return MqErrorDbV(MQ_ERROR_ID_NOT_FOUND,"transaction",transId);
    default:
      return MqErrorDbFactorySql(context,factory_sys->db);
  }
  pTokenSetCurrent(context->link.srvT, (MQ_CST const)sqlite3_column_text(hdl, 0));
  MqBufferSetB(buf, sqlite3_column_blob(hdl, 3), sqlite3_column_bytes(hdl, 3));
  buf->numItems = sqlite3_column_int(hdl, 1);
  buf->type = sqlite3_column_int(hdl, 2);
  return MQ_OK;
error:
  return MqErrorDbFactorySql(context,factory_sys->db);
error1:
  return MqErrorStack(context);
}

enum MqErrorE
pFactoryCtxInsertReadTrans (
  struct MqS * const context, 
  MQ_CST const ident, 
  MQ_WID const rmtTransId, 
  MQ_WID const oldTransId, 
  MQ_WID *transIdP
)
{
  struct MqFactoryThreadS * const factory_sys = context->link.factory;
  register sqlite3_stmt *hdl= factory_sys->readInsert;
  check_NULL(factory_sys->db) {
    MqErrorCheck1 (sFactoryAddDb (context, ":memory:"));
  }
  check_NULL(hdl) {
    const static char sql[] = "INSERT INTO readTrans (ident, ctxId, rmtTransId, oldTransId ) VALUES (?, ?, ?, ?);";
    check_sqlite (sqlite3_prepare_v2(factory_sys->db, sql, -1, &factory_sys->readInsert, NULL)) goto error;
    hdl = factory_sys->readInsert;
  }
  check_sqlite(sqlite3_reset	  (hdl))			      goto error;
  check_sqlite(sqlite3_bind_text  (hdl,1,ident,-1,SQLITE_TRANSIENT))  goto error;
  check_sqlite(sqlite3_bind_int   (hdl,2,context->link.ctxId))	      goto error;
  check_sqlite(sqlite3_bind_int64 (hdl,3,rmtTransId))		      goto error;
  check_sqlite(sqlite3_bind_int64 (hdl,4,oldTransId))		      goto error;
  if (sqlite3_step(hdl) != SQLITE_DONE) {
    return MqErrorDbFactorySql(context,factory_sys->db);
  }
  *transIdP = sqlite3_last_insert_rowid(factory_sys->db);
  return MQ_OK;
error:
  return MqErrorDbFactorySql(context,factory_sys->db);
error1:
  return MqErrorStack(context);
}

enum MqErrorE
pFactoryCtxSelectReadTrans (
  struct MqS * const context, 
  MQ_WID transId
)
{
  struct MqFactoryThreadS * const factory_sys = context->link.factory;
  register sqlite3_stmt *hdl= factory_sys->readSelect1;
  check_NULL(factory_sys->db) {
    MqErrorCheck1 (sFactoryAddDb (context, ":memory:"));
  }
  check_NULL(hdl) {
    const static char sql[] = "SELECT ident, rmtTransId FROM readTrans WHERE rowid = ?;";
    check_sqlite (sqlite3_prepare_v2(factory_sys->db, sql, -1, &factory_sys->readSelect1, NULL)) goto error;
    hdl = factory_sys->readSelect1;
  }
  check_sqlite (sqlite3_reset(hdl))		    goto error;
  check_sqlite (sqlite3_bind_int64(hdl,1,transId))  goto error;
  switch (sqlite3_step(hdl)) {
    case SQLITE_ROW:
      break;
    case SQLITE_DONE:
      return MqErrorDbV(MQ_ERROR_ID_NOT_FOUND,"transaction",transId);
    default:
      return MqErrorDbFactorySql(context,factory_sys->db);
  }
  MqErrorCheck1 (MqSendC (context, (MQ_CST) sqlite3_column_text	(hdl, 0)));
  MqErrorCheck1 (MqSendW (context, sqlite3_column_int64	(hdl, 1)));
  return MQ_OK;
error:
  return MqErrorDbFactorySql(context,factory_sys->db);
error1:
  return MqErrorStack(context);
}

enum MqErrorE
pFactoryCtxDeleteReadTrans (
  struct MqS * const context, 
  MQ_WID transId,
  MQ_WID *oldTransId
)
{
  struct MqFactoryThreadS * const factory_sys = context->link.factory;
  register sqlite3_stmt * hdl = factory_sys->readSelect2;
  register sqlite3_stmt * del = factory_sys->readDelete;
  check_NULL(factory_sys->db) {
    MqErrorCheck1 (sFactoryAddDb (context, ":memory:"));
  }
  check_NULL(hdl) {
    const static char sql[] = "SELECT oldTransId FROM readTrans WHERE rowid = ?;";
    check_sqlite (sqlite3_prepare_v2(factory_sys->db, sql, -1, &factory_sys->readSelect2, NULL)) goto error;
    hdl = factory_sys->readSelect2;
  }
  // get oldTransId
  check_sqlite (sqlite3_reset(hdl))		    goto error;
  check_sqlite (sqlite3_bind_int64(hdl,1,transId))  goto error;
  switch (sqlite3_step(hdl)) {
    case SQLITE_ROW:
      break;
    case SQLITE_DONE:
      return MqErrorDbV(MQ_ERROR_ID_NOT_FOUND,"transaction",transId);
    default:
      return MqErrorDbFactorySql(context,factory_sys->db);
  }
  *oldTransId = sqlite3_column_int64 (hdl, 0);
  // delete row
  check_NULL(del) {
    const static char sql[] = "DELETE FROM readTrans WHERE rowid = ?;";
    check_sqlite (sqlite3_prepare_v2(factory_sys->db, sql, -1, &factory_sys->readDelete, NULL)) goto error;
    del = factory_sys->readDelete;
  }
  check_sqlite (sqlite3_reset(del))		    goto error;
  check_sqlite (sqlite3_bind_int64(del,1,transId))  goto error;
  if (sqlite3_step(hdl) != SQLITE_DONE) {
    return MqErrorDbFactorySql(context,factory_sys->db);
  }
  return MqErrorGetCodeI(context);
error:
  return MqErrorDbFactorySql(context,factory_sys->db);
error1:
  return MqErrorStack(context);
}

/*****************************************************************************/
/*                                                                           */
/*                           protected setup                                 */
/*                                                                           */
/*****************************************************************************/

static MqThreadKeyType factory_key = MqThreadKeyNULL;

static struct MqFactoryThreadS*
sFactoryThreadAlloc(
  struct MqS * const context
)
{
  struct MqFactoryThreadS * factory_sys = (struct MqFactoryThreadS *) MqThreadGetTLS(factory_key);
  if (factory_sys == NULL) {
    factory_sys = (struct MqFactoryThreadS *) MqSysCalloc (context, 1, sizeof (*factory_sys));
    if (factory_sys == NULL) {
      MqErrorStack(context);
    } else if (MqThreadSetTLSCheck(factory_key,factory_sys)) {
      MqSysFree(factory_sys);
      MqErrorC (context, __func__, -1, "unable to alloc Thread-Local-Storage data");
    }
  }
  return factory_sys;
}

enum MqErrorE
pFactoryThreadCreate (
  struct MqS * const context,
  struct MqFactoryThreadS ** const thread
) {
  if (*thread == NULL) {
    *thread = sFactoryThreadAlloc(context);
    MqDLogV(context,4,"TLS-Id<%p>\n", (void*)*thread);
  }
  return MqErrorStack(context);
}

void
pFactoryThreadDelete (
  struct MqFactoryThreadS ** thread
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
FactoryCreate (void)
{
  // setup sqlite 
  check_sqlite (sqlite3_initialize()) {
    MqPanicC(MQ_ERROR_PANIC, __func__, -1, "unable to use 'sqlite3_initialize'");
  }   

  // setup main proc TLS data
  MqThreadKeyCreate(factory_key);
}

void
FactoryDelete (void)
{

  // delete main proc TLS data
  struct MqFactoryThreadS * factory_sys;
  if (factory_key != MqThreadKeyNULL) {
    sFactoryDelDb (NQ_ERROR_PRINT, MqThreadGetTLS(factory_key));
    MqThreadSetTLS(factory_key, NULL);
    MqSysFree(factory_sys);
  }

  // delete sqlite 
  check_sqlite (sqlite3_shutdown()) {
    MqPanicC(MQ_ERROR_PANIC, __func__, -1, "unable to use 'sqlite3_shutdown'");
  }   
}

END_C_DECLS

