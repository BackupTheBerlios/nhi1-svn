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

BEGIN_C_DECLS

struct MqFactoryS *defaultFactoryItem = NULL;

struct MqFactoryTransS {
  MQ_CST  storageDir;	      ///< main directory used for database files
  sqlite3 *db;		      ///< sqlite database connection handle
  sqlite3_stmt *sendInsert;   ///< prepared sql statement
  sqlite3_stmt *sendSelect;   ///< prepared sql statement
  sqlite3_stmt *readInsert;   ///< prepared sql statement
  sqlite3_stmt *readSelect1;  ///< prepared sql statement
  sqlite3_stmt *readSelect2;  ///< prepared sql statement
  sqlite3_stmt *readDelete;   ///< prepared sql statement
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

static enum MqFactoryReturnE sFactoryErrorTxt (
  struct MqFactoryS * const item,
  enum MqFactoryReturnE ret,
  MQ_CST const error
) {
  strncpy(item->error, error, MQ_FACTORY_BUF-1);
  item->ret = ret;
  item->error[MQ_FACTORY_BUF-1] = '\0';
  return ret;
}

#define sFactoryErrorSql(item, ret) \
  (MqSqliteError(MQ_ERROR_PRINT, item->Trans),sFactoryErrorTxt(item, ret, sqlite3_errmsg(item->Trans->db)))

/*
static enum MqFactoryReturnE sFactoryErrorSql (
  struct MqFactoryS * const item,
  enum MqFactoryReturnE ret
) {
  return sFactoryErrorTxt (item, ret, sqlite3_errmsg(item->Trans->db));
}
*/

static enum MqFactoryReturnE sFactoryErrorNum (
  struct MqFactoryS * const item,
  enum MqFactoryReturnE ret
) {
  return sFactoryErrorTxt(item, ret, MqFactoryReturnMsg(ret));
}

// *********************************************************

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

MQ_PTR sFactoryCallocItem (
  struct MqFactoryS * const item,
  MQ_SIZE nmemb,
  MQ_SIZE size
)
{
  MQ_PTR ptr = sFactoryCalloc(nmemb, size);
  if (unlikely (ptr == NULL)) {
    sFactoryErrorTxt(item, MQ_FACTORY_RETURN_CALLOC_ERR, strerror (errno));
  }
  return ptr;
}

void sFactoryInit (
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
  item->ret = MQ_FACTORY_RETURN_OK;
  *item->error = '\0';
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

static enum MqFactoryReturnE
sFactoryDelTrans (
  struct MqFactoryS * const item
)
{
  register struct MqFactoryTransS *trans = item->Trans;
  if (trans != NULL) {
    if (trans->storageDir != NULL) {
      sqlite3_free((void*)trans->storageDir);
    }
    if (trans->sendInsert != NULL) {
      check_sqlite(sqlite3_finalize(trans->sendInsert)) {
	MqSqliteError(MQ_ERROR_PRINT, trans);
      }
    }
    if (trans->sendSelect != NULL) {
      check_sqlite(sqlite3_finalize(trans->sendSelect)) {
	MqSqliteError(MQ_ERROR_PRINT, trans);
      }
    }
    if (trans->db != NULL) {
      check_sqlite(sqlite3_close(trans->db)) {
	MqSqliteError(MQ_ERROR_PRINT, trans);
      }
    }
    sqlite3_free((void*)trans);
    item->Trans = NULL;
  }
  return item->ret;
}

static enum MqFactoryReturnE
sFactoryAddTrans (
  struct MqFactoryS * const item,
  MQ_CST const storageDir
)
{
  const static enum MqFactoryReturnE errnum = MQ_FACTORY_RETURN_ADD_TRANS_ERR;
  char *errmsg = NULL;
  struct MqFactoryTransS *trans = item->Trans = sFactoryCallocItem(item, 1, sizeof(*item->Trans));

  // transaction storage, as transaction-id the ROWID is used
  const static char SQL_SCT[] = "CREATE TABLE IF NOT EXISTS sendTrans ( callback TEXT, numItems INTEGER, type INTEGER, data BLOB );";
  const static char SQL_RCT[] = "CREATE TABLE IF NOT EXISTS readTrans (ident TEXT,ctxId INTEGER,rmtTransId INTEGER,oldTransId INTEGER);";

  check_NULL (item->Trans) goto end;

  check_sqlite (sqlite3_open(storageDir, &trans->db)) {
    sFactoryErrorTxt(item, errnum, sqlite3_errmsg(trans->db));
    goto end;
  } 

  check_sqlite (sqlite3_exec(trans->db, SQL_SCT, NULL, NULL, &errmsg)) {
    sFactoryErrorTxt(item, errnum, errmsg);
    sqlite3_free(errmsg);
    goto end;
  } 

  check_sqlite (sqlite3_exec(trans->db, SQL_RCT, NULL, NULL, &errmsg)) {
    sFactoryErrorTxt(item, errnum, errmsg);
    sqlite3_free(errmsg);
    goto end;
  } 

  check_NULL (trans->storageDir = sFactoryStrDup(storageDir)) {
    sFactoryErrorTxt(item, errnum, "unable to duplicate 'storageDir' string");
    goto end;
  } 

end:
  return item->ret;
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
  sFactoryDelTrans(&space.items[id]);
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

static enum MqFactoryReturnE
sFactoryAddName (
  MQ_CST const ident,
  struct MqFactoryCreateS Create,
  struct MqFactoryDeleteS Delete,
  struct MqFactoryS ** factoryP
)
{
  struct MqFactoryS *free = MqFactoryGet(ident);

  if (free != NULL) {
    // item is available, -> error
    return sFactoryErrorNum(free, MQ_FACTORY_RETURN_ADD_IDENT_IN_USE_ERR);
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

    if (factoryP != NULL) *factoryP = free;
  }

  // add the default-transaction-storage -> in memory
  return sFactoryAddTrans (free, ":memory:");
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

MQ_CST MqFactoryReturnMsg (
  enum MqFactoryReturnE const ret
) {
  switch (ret) {
   case MQ_FACTORY_RETURN_OK:		
    return "OK";
   case MQ_FACTORY_RETURN_CREATE_FUNCTION_REQUIRED:	
    return "unable to define factory, create function is required";
   case MQ_FACTORY_RETURN_ADD_IDENT_IN_USE_ERR:
    return "factory identifer already in use";
   case MQ_FACTORY_RETURN_CALL_ERR:
    return "unable to call factory for identifer";
   case MQ_FACTORY_RETURN_ITEM_GET_ERR:
    return "unable to find factory for identifer";
   case MQ_FACTORY_RETURN_NEW_ERR:
    return "unable to create a new factory and return an object";
   case MQ_FACTORY_RETURN_DEFAULT_ERR:
    return "unable to create a new default factory";
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
   case MQ_FACTORY_RETURN_GET_ERR:
    return "unable to get factory for ident";
  }
  return "nothing";
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

enum MqFactoryReturnE
MqFactoryAdd (
  MQ_CST	      const ident,
  MqFactoryCreateF    const fCreate,
  MQ_PTR	      const createData,
  MqFactoryDataFreeF  const createDatafreeF,
  MqFactoryDataCopyF  const createDataCopyF,
  MqFactoryDeleteF    const fDelete,
  MQ_PTR	      const deleteData,
  MqFactoryDataFreeF  const deleteDatafreeF,
  MqFactoryDataCopyF  const deleteDataCopyF,
  struct MqFactoryS ** factoryP
)
{
  struct MqFactoryCreateS Create = {fCreate, createData, createDatafreeF, createDataCopyF};
  struct MqFactoryDeleteS Delete = {fDelete, deleteData, deleteDatafreeF, deleteDataCopyF};
  if (ident == NULL || *ident == '\0') {
    return MQ_FACTORY_RETURN_INVALID_IDENT;
  }
  if (fCreate == NULL) {
    return MQ_FACTORY_RETURN_CREATE_FUNCTION_REQUIRED;
  }
  return sFactoryAddName (ident, Create, Delete, factoryP);
}

enum MqFactoryReturnE
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
    return MQ_FACTORY_RETURN_INVALID_IDENT;
  }
  if (fCreate == NULL) {
    return MQ_FACTORY_RETURN_CREATE_FUNCTION_REQUIRED;
  }
  sFactorySpaceDelItem (0);
  sFactoryInit (&space.items[0], ident, Create, Delete);
  defaultFactoryItem = &space.items[0];
  return MQ_FACTORY_RETURN_OK;
}

enum MqFactoryReturnE
MqFactoryNew (
  MQ_CST	      const ident,
  MqFactoryCreateF    const fCreate,
  MQ_PTR	      const createData,
  MqFactoryDataFreeF  const createDataFreeF,
  MqFactoryDataCopyF  const createDataCopyF,
  MqFactoryDeleteF    const fDelete,
  MQ_PTR	      const deleteData,
  MqFactoryDataFreeF  const deleteDataFreeF,
  MqFactoryDataCopyF  const deleteDataCopyF,
  MQ_PTR data,
  struct MqS ** ctxP
)
{
  enum MqFactoryReturnE ret;
  struct MqFactoryS * item;
  *ctxP = NULL;
  MqFactoryCheck (ret = MqFactoryAdd (ident, 
    fCreate, createData, createDataFreeF, createDataCopyF,
    fDelete, deleteData, deleteDataFreeF, deleteDataCopyF,
    &item
  )) return ret;
  return MqFactoryCall (item, data, ctxP);
}

/*****************************************************************************/
/*                                                                           */
/*                               public item                                 */
/*                                                                           */
/*****************************************************************************/

enum MqFactoryReturnE
MqFactoryCall (
  struct MqFactoryS * const item,
  MQ_PTR const data,
  struct MqS ** ctxP
)
{
  *ctxP = NULL;
  if (item == NULL) {
    return MQ_FACTORY_RETURN_ITEM_IS_NULL;
  } else if (MqErrorCheckI(MqFactoryInvoke ((struct MqS * const)data, MQ_FACTORY_NEW_INIT, item, ctxP))) {
    sFactoryErrorNum(item, MQ_FACTORY_RETURN_CALL_ERR);
    goto end;
  }
  MqConfigUpdateName(*ctxP, item->ident);
end:
  return item->ret;
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

MQ_CST MqFactoryErrorMsg (
  struct MqFactoryS const * const item
) {
  return item->error;
}

void MqFactoryPanicReturn (
  enum MqFactoryReturnE const ret
) {
  if (ret != MQ_FACTORY_RETURN_OK) {
    MqPanicC(MQ_ERROR_PANIC, __func__, ret, MqFactoryReturnMsg(ret))
  }
}

void MqFactoryPanicItem (
  struct MqFactoryS const * const item
) {
  if (item->ret != MQ_FACTORY_RETURN_OK) {
    MqPanicC(MQ_ERROR_PANIC, __func__, item->ret, item->error);
  }
}

enum MqFactoryReturnE
MqFactoryCopy (
  struct MqFactoryS * const item,
  MQ_CST const ident
)
{
  if (item == NULL) {
    return MQ_FACTORY_RETURN_ITEM_IS_NULL;
  } else if (ident == NULL || *ident == '\0') {
    return MQ_FACTORY_RETURN_INVALID_IDENT;
  } else {
    struct MqFactoryCreateS Create = item->Create;
    struct MqFactoryDeleteS Delete = item->Delete;
    if (Create.data != NULL && Create.fCopy != NULL) {
      (*Create.fCopy) (&Create.data);
    }
    if (Delete.data != NULL && Delete.fCopy != NULL) {
      (*Delete.fCopy) (&Delete.data);
    }
    return sFactoryAddName (ident, Create, Delete, NULL);
  }
}

/*****************************************************************************/
/*                                                                           */
/*                            public Set / Get                               */
/*                                                                           */
/*****************************************************************************/

enum MqFactoryReturnE
MqFactorySetCalled (
  struct MqFactoryS * const item,
  MQ_BOL const called
)
{
  item->called = called;
  return MQ_FACTORY_RETURN_OK;
}

enum MqFactoryReturnE
MqFactorySetTrans (
  struct MqFactoryS * const item,
  MQ_CST const storageDir
)
{
  MqFactoryCheck (sFactoryDelTrans(item)) goto end;
  MqFactoryCheck (sFactoryAddTrans(item, storageDir)) goto end;
end:
  return item->ret;
}

/*****************************************************************************/
/*                                                                           */
/*                             public context                                */
/*                                                                           */
/*****************************************************************************/

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

enum MqErrorE
pFactoryCtxInsertSendTrans (
  struct MqS * const context, 
  MQ_TOK const callback, 
  MQ_BUF buf, 
  MQ_WID *transId
)
{
  struct MqFactoryTransS *trans = context->setup.factory->Trans;
  register sqlite3_stmt *hdl= trans->sendInsert;
  check_NULL(hdl) {
    const static char sql[] = "INSERT INTO sendTrans (callback, numItems, type, data) VALUES (?, ?, ?, ?);";
    check_sqlite (sqlite3_prepare_v2(trans->db, sql, -1, &trans->sendInsert, NULL)) goto error;
    hdl = trans->sendInsert;
  }
  check_sqlite (sqlite3_reset	   (hdl))					    goto error;
  check_sqlite (sqlite3_bind_text  (hdl,1,callback,HDR_TOK_LEN,SQLITE_TRANSIENT))   goto error;
  check_sqlite (sqlite3_bind_int   (hdl,2,buf->numItems))			    goto error;
  check_sqlite (sqlite3_bind_int   (hdl,3,buf->type))				    goto error;
  check_sqlite (sqlite3_bind_blob  (hdl,4,buf->data,buf->cursize,SQLITE_TRANSIENT)) goto error;
  if (sqlite3_step(hdl) != SQLITE_DONE) {
    return MqErrorDbFactoryMsg(context,sqlite3_errmsg(trans->db));
  }
  *transId = sqlite3_last_insert_rowid(trans->db);
  return MQ_OK;
error:
  return MqErrorDbFactoryMsg(context,sqlite3_errmsg(trans->db));
}

enum MqErrorE
pFactoryCtxSelectSendTrans (
  struct MqS * const context, 
  MQ_WID transId,
  MQ_BUF buf 
)
{
  struct MqFactoryTransS *trans = context->setup.factory->Trans;
  register sqlite3_stmt * hdl = trans->sendSelect;
  check_NULL(hdl) {
    const static char sql[] = "SELECT callback, numItems, type, data FROM sendTrans WHERE rowid = ?;";
    check_sqlite (sqlite3_prepare_v2(trans->db, sql, -1, &trans->sendSelect, NULL)) goto error;
    hdl = trans->sendSelect;
  }
  check_sqlite (sqlite3_reset(hdl))		    goto error;
  check_sqlite (sqlite3_bind_int64(hdl,1,transId))  goto error;
  switch (sqlite3_step(hdl)) {
    case SQLITE_ROW:
      break;
    case SQLITE_DONE:
      return MqErrorV(context,__func__,MQ_ERROR_FACTORY,"transaction-id '%lld' not found",transId);
    default:
      return MqErrorDbFactoryMsg(context,sqlite3_errmsg(trans->db));
  }
  pTokenSetCurrent(context->link.srvT, (MQ_CST const)sqlite3_column_text(hdl, 1));
  MqBufferSetB(buf, sqlite3_column_blob(hdl, 4), sqlite3_column_bytes(hdl, 3));
  buf->numItems = sqlite3_column_int(hdl, 2);
  buf->type = sqlite3_column_int(hdl, 3);
  return MQ_OK;
error:
  return MqErrorDbFactoryMsg(context,sqlite3_errmsg(trans->db));
}

enum MqErrorE
pFactoryCtxInsertReadTrans (
  struct MqS * const context, 
  MQ_CST const ident, 
  MQ_WID const rmtTransId, 
  MQ_WID const oldTransId, 
  MQ_WID *transId
)
{
  struct MqFactoryTransS *trans = context->setup.factory->Trans;
  register sqlite3_stmt *hdl= trans->readInsert;
  check_NULL(hdl) {
    const static char sql[] = "INSERT INTO readTrans (ident, ctxId, rmtTransId, oldTransId ) VALUES (?, ?, ?, ?);";
    check_sqlite (sqlite3_prepare_v2(trans->db, sql, -1, &trans->readInsert, NULL)) goto error;
    hdl = trans->readInsert;
  }
  check_sqlite(sqlite3_reset	  (hdl))			      goto error;
  check_sqlite(sqlite3_bind_text  (hdl,1,ident,-1,SQLITE_TRANSIENT))  goto error;
  check_sqlite(sqlite3_bind_int   (hdl,2,context->link.ctxId))	      goto error;
  check_sqlite(sqlite3_bind_int64 (hdl,3,rmtTransId))		      goto error;
  check_sqlite(sqlite3_bind_int64 (hdl,4,oldTransId))		      goto error;
  if (sqlite3_step(hdl) != SQLITE_DONE) {
    return MqErrorDbFactoryMsg(context,sqlite3_errmsg(trans->db));
  }
  *transId = sqlite3_last_insert_rowid(trans->db);
  return MQ_OK;
error:
  return MqErrorDbFactoryMsg(context,sqlite3_errmsg(trans->db));
}

enum MqErrorE
pFactoryCtxSelectReadTrans (
  struct MqS * const context, 
  MQ_WID transId
)
{
  struct MqFactoryTransS *trans = context->setup.factory->Trans;
  register sqlite3_stmt * hdl = trans->readSelect1;
  check_NULL(hdl) {
    const static char sql[] = "SELECT ident, rmtTransId FROM readTrans WHERE rowid = ?;";
    check_sqlite (sqlite3_prepare_v2(trans->db, sql, -1, &trans->readSelect1, NULL)) goto error;
    hdl = trans->readSelect1;
  }
  check_sqlite (sqlite3_reset(hdl))		    goto error;
  check_sqlite (sqlite3_bind_int64(hdl,1,transId))  goto error;
  switch (sqlite3_step(hdl)) {
    case SQLITE_ROW:
      break;
    case SQLITE_DONE:
      return MqErrorV(context,__func__,MQ_ERROR_FACTORY,"transaction-id '%lld' not found",transId);
    default:
      return MqErrorDbFactoryMsg(context,sqlite3_errmsg(trans->db));
  }
  MqErrorCheck1 (MqSendC (context, (MQ_CST) sqlite3_column_text	(hdl, 1)));
  MqErrorCheck1 (MqSendW (context, sqlite3_column_int64	(hdl, 2)));
  return MQ_OK;
error:
  return MqErrorDbFactoryMsg(context,sqlite3_errmsg(trans->db));
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
  struct MqFactoryTransS *trans = context->setup.factory->Trans;
  register sqlite3_stmt * hdl = trans->readSelect2;
  register sqlite3_stmt * del = trans->readDelete;
  check_NULL(hdl) {
    const static char sql[] = "SELECT oldTransId FROM readTrans WHERE rowid = ?;";
    check_sqlite (sqlite3_prepare_v2(trans->db, sql, -1, &trans->readSelect2, NULL)) goto error;
    hdl = trans->readSelect2;
  }
  // get oldTransId
  check_sqlite (sqlite3_reset(hdl))		    goto error;
  check_sqlite (sqlite3_bind_int64(hdl,1,transId))  goto error;
  switch (sqlite3_step(hdl)) {
    case SQLITE_ROW:
      break;
    case SQLITE_DONE:
      return MqErrorV(context,__func__,MQ_ERROR_FACTORY,"transaction-id '%lld' not found",transId);
    default:
      return MqErrorDbFactoryMsg(context,sqlite3_errmsg(trans->db));
  }
  *oldTransId = sqlite3_column_int64 (hdl, 1);
  // delete row
  check_NULL(del) {
    const static char sql[] = "DELETE FROM readTrans WHERE rowid = ?;";
    check_sqlite (sqlite3_prepare_v2(trans->db, sql, -1, &trans->readDelete, NULL)) goto error;
    del = trans->readDelete;
  }
  check_sqlite (sqlite3_reset(del))		    goto error;
  check_sqlite (sqlite3_bind_int64(del,1,transId))  goto error;
  if (sqlite3_step(hdl) != SQLITE_DONE) {
    return MqErrorDbFactoryMsg(context,sqlite3_errmsg(trans->db));
  }
  return MqErrorGetCodeI(context);
error:
  return MqErrorDbFactoryMsg(context,sqlite3_errmsg(trans->db));
}

/*****************************************************************************/
/*                                                                           */
/*                           protected setup                                 */
/*                                                                           */
/*****************************************************************************/

void
FactorySpaceCreate (void)
{
  check_sqlite (sqlite3_initialize()) {
    MqPanicC(MQ_ERROR_PANIC, __func__, -1, "unable to use 'sqlite3_initialize'");
  }   

  space.items = (struct MqFactoryS *) sFactoryCalloc (SPACE_INIT_SIZE, sizeof (*space.items));
  space.size = SPACE_INIT_SIZE;
  space.used = 1;  // first item is always the default

  MqFactoryDefault("libmsgque", MqFactoryDefaultCreate, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

void
FactorySpaceDelete (void)
{
  sFactorySpaceDelAll ();
  sqlite3_free (space.items);

  check_sqlite (sqlite3_shutdown()) {
    MqPanicC(MQ_ERROR_PANIC, __func__, -1, "unable to use 'sqlite3_shutdown'");
  }   
}

END_C_DECLS

