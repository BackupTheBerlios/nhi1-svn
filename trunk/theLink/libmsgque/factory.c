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
#include "sqlite3.h"
#include "errno.h"

BEGIN_C_DECLS

struct MqFactoryS *defaultFactoryItem = NULL;

struct MqFactoryTransS {
  MQ_CST  storageDir;   ///< main directory used for database files
  sqlite3 *db;		///< sqlite database connection handle
};

#define pFactoryCmp(s1,s2) strcmp(s1,s2)

// transaction storage, as transaction-id the ROWID is used
const static char SQL_CT[] = "CREATE TABLE IF NOT EXIST trans ( tData BLOB )";

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
/*                          factory_static                                     */
/*                                                                           */
/*****************************************************************************/

static void sFactorySpaceAdd (
  MQ_SIZE const add
);

static void sFactorySpaceDelAll (
  void
);

static enum MqFactoryReturnE sFactorySaveError (
  struct MqFactoryS * const item,
  enum MqFactoryReturnE ret,
  MQ_CST const error
) {
  strncpy(item->error, error, MQ_FACTORY_BUF-1);
  item->ret = ret;
  item->error[MQ_FACTORY_BUF-1] = '\0';
  return ret;
}

static enum MqFactoryReturnE sFactorySaveReturn (
  struct MqFactoryS * const item,
  enum MqFactoryReturnE ret
) {
  return sFactorySaveError(item, ret, MqFactoryReturnMsg(ret));
}

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
    sFactorySaveError(item, MQ_FACTORY_RETURN_CALLOC_ERR, strerror (errno));
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
/*                              factory_memory                               */
/*                                                                           */
/*****************************************************************************/

static enum MqFactoryReturnE
sFactoryDelTrans (
  struct MqFactoryS * const item
)
{
  if (item->Trans != NULL) {
    if (item->Trans->storageDir != NULL) {
      sqlite3_free((void*)item->Trans->storageDir);
    }
    if (item->Trans->db != NULL) {
      sqlite3_close(item->Trans->db);
      sqlite3_shutdown();
    }
    sqlite3_free((void*)item->Trans);
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
  char *errmsg = NULL;
  struct MqFactoryTransS *trans = item->Trans = sFactoryCallocItem(item, 1, sizeof(*item->Trans));

  check_NULL (item->Trans) goto end;

  check_sqlite (sqlite3_initialize()) {
    sFactorySaveError(item, MQ_FACTORY_RETURN_SET_STORAGE_DIR_ERROR, "unable to use 'sqlite3_initialize'");
    goto end;
  } 

  check_sqlite (sqlite3_open(storageDir, &trans->db)) {
    sFactorySaveError(item, MQ_FACTORY_RETURN_SET_STORAGE_DIR_ERROR, "unable to use 'sqlite3_open'");
    goto end;
  } 

  check_sqlite (sqlite3_exec(trans->db, SQL_CT, NULL, NULL, &errmsg)) {
    sFactorySaveError(item, MQ_FACTORY_RETURN_SET_STORAGE_DIR_ERROR, errmsg);
    sqlite3_free(errmsg);
    goto end;
  } 

  trans->storageDir = sFactoryStrDup(storageDir);

  check_NULL (trans->storageDir) {
    sFactorySaveError(item, MQ_FACTORY_RETURN_SET_STORAGE_DIR_ERROR, "unable to duplicate 'storageDir' string");
    goto end;
  } 

end:
  return item->ret;
}

/*****************************************************************************/
/*                                                                           */
/*                              factory_memory                               */
/*                                                                           */
/*****************************************************************************/

void
FactorySpaceCreate (void)
{
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
}

static void
sFactorySpaceAdd (
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

/*****************************************************************************/
/*                                                                           */
/*                              Add + Del                                    */
/*                                                                           */
/*****************************************************************************/

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
  if (space.items[id].Trans != NULL) {
    if (space.items[id].Trans->storageDir != NULL) {
      sqlite3_free((void*)space.items[id].Trans->storageDir);
    }
    if (space.items[id].Trans->db != NULL) {
      sqlite3_close(space.items[id].Trans->db);
      sqlite3_shutdown();
    }
    sqlite3_free((void*)space.items[id].Trans);
  }
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
pFactoryAddName (
  MQ_CST const ident,
  struct MqFactoryCreateS Create,
  struct MqFactoryDeleteS Delete,
  struct MqFactoryS ** factoryP
)
{
  struct MqFactoryS *find;

  if (MqFactoryItemGet(ident,&find) == MQ_FACTORY_RETURN_OK) {
    // item already available, PANIC if callback is NOT equal
    if (memcmp(&Create, &find->Create, sizeof(Create)) || memcmp(&Delete, &find->Delete, sizeof(Delete))) {
      return sFactorySaveError(item, MQ_FACTORY_RETURN_ADD_IDENT_IN_USE_ERR, "unable to call factory for identifer");
      return MQ_FACTORY_RETURN_ADD_IDENT_IN_USE_ERR;
    }
    // OK, item available and in use ... nothing to do
  } else {
    // item not available, add new one
    struct MqFactoryS *free;

    sFactorySpaceAdd (1);

    //free = space.items + space.used * sizeof(*space.items);
    free = space.items + space.used;

    space.used += 1;

    free->ident = sFactoryStrDup(ident);

    free->Create = Create;
    free->Delete = Delete;

    if (factoryP != NULL) *factoryP = free;
  }
  return MQ_FACTORY_RETURN_OK;
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

static enum MqFactoryReturnE
pFactoryCallItem (
  struct MqFactoryS * const item,
  MQ_PTR const data,
  struct MqS ** ctxP
)
{
  *ctxP = NULL;
  if (MqErrorCheckI(MqFactoryInvoke ((struct MqS * const)data, MQ_FACTORY_NEW_INIT, item, ctxP))) {
    sFactorySaveError(item, MQ_FACTORY_RETURN_CALL_ERR, "unable to call factory for identifer");
    goto end;
  }
  MqConfigUpdateName(*ctxP, item->ident);
end:
  return item->ret;
}

static enum MqFactoryReturnE
pFactoryCallIdent (
  MQ_CST const ident,
  MQ_PTR const data,
  struct MqS ** ctxP
)
{
  enum MqFactoryReturnE ret;
  struct MqFactoryS *item;
  check_Factory(ret = MqFactoryItemGet (ident, &item)) goto end;
  check_Factory(ret = pFactoryCallItem(item, data, ctxP)) goto end;
end:
  return ret;
}

/*****************************************************************************/
/*                                                                           */
/*                                 public                                    */
/*                                                                           */
/*****************************************************************************/

enum MqFactoryReturnE
MqFactoryItemGet (
  MQ_CST const ident,
  struct MqFactoryS **itemP
)
{
	   struct MqFactoryS * start = space.items;
  register struct MqFactoryS * end = start + space.used;
  *itemP = NULL;
  if (ident == NULL || *ident == '\0') {
    return MQ_FACTORY_RETURN_INVALID_IDENT;
  }
  while (start < end-- && strcmp(end->ident,ident)) {}
  if (end >= start) {
    *itemP = end;
    return MQ_FACTORY_RETURN_OK;
  } else {
    return MQ_FACTORY_RETURN_ITEM_GET_ERR;
  }
}

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
  return pFactoryAddName (ident, Create, Delete, factoryP);
}

enum MqFactoryReturnE
MqFactoryCopyDefault (
  MQ_CST const ident
)
{
  if (ident == NULL || *ident == '\0') {
    return MQ_FACTORY_RETURN_INVALID_IDENT;
  } else {
    struct MqFactoryCreateS Create = space.items[0].Create;
    struct MqFactoryDeleteS Delete = space.items[0].Delete;
    if (Create.data != NULL && Create.fCopy != NULL) {
      (*Create.fCopy) (&Create.data);
    }
    if (Delete.data != NULL && Delete.fCopy != NULL) {
      (*Delete.fCopy) (&Delete.data);
    }
    return pFactoryAddName (ident, Create, Delete, NULL);
  }
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

MQ_CST
MqFactoryDefaultIdent (
  void
) {
  return space.items[0].ident ? space.items[0].ident : "";
}

enum MqFactoryReturnE
MqFactoryCallIdent (
  MQ_CST const ident,
  MQ_PTR const data,
  struct MqS ** ctxP
)
{
  return pFactoryCallIdent (ident, data, ctxP);
}

enum MqFactoryReturnE
MqFactoryCallItem (
  struct MqFactoryS * const item,
  MQ_PTR const data,
  struct MqS ** ctxP
)
{
  return pFactoryCallItem (item, data, ctxP);
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
  check_Factory (ret = MqFactoryAdd (ident, 
    fCreate, createData, createDataFreeF, createDataCopyF,
    fDelete, deleteData, deleteDataFreeF, deleteDataCopyF,
    &item
  ));
  check_Factory (ret = pFactoryCallItem (item, data, ctxP));
  return ret;
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
   case MQ_FACTORY_RETURN_SET_STORAGE_DIR_ERROR:
    return "unable to use the storage directory";
   case MQ_FACTORY_RETURN_CALLOC_ERR:
    return "unable to calloc memory";
  }
  return "nothing";
}

MQ_CST MqFactoryErrorMsg (
  struct MqFactoryS const * const item
) {
  return item->error;
}

void MqFactoryErrorPanic (
  struct MqFactoryS const * const item
) {
  if (item->ret != MQ_FACTORY_RETURN_OK) {
    MqPanicC(MQ_ERROR_PANIC, __func__, -1, item->error);
  }
}

/*****************************************************************************/
/*                                                                           */
/*                                set/get                                    */
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
  check_Factory (sFactoryDelTrans(item)) goto end;
  check_Factory (sFactoryAddTrans(item, storageDir)) goto end;
end:
  return item->ret;
}

/*****************************************************************************/
/*                                                                           */
/*                                context                                    */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE 
MqFactoryCtxDefaultSet (
  struct MqS * const context,
  MQ_CST const ident
) {
  enum MqFactoryReturnE ret;
  MqFactoryErrorCheck(ret = MqFactoryCopyDefault(ident));
  MqFactoryCtxIdentSet(context, ident);
  return MQ_OK;
error:
  return MqErrorC(context, __func__, -1, MqFactoryReturnMsg(ret));
}

enum MqErrorE 
MqFactoryCtxIdentSet (
  struct MqS * const context,
  MQ_CST ident
) {
  enum MqFactoryReturnE ret;
  MqFactoryErrorCheck (ret = MqFactoryItemGet(ident, &context->setup.factory));
  MqConfigUpdateName(context, ident);
  return MQ_OK;
error:
  return MqErrorC(context, __func__, -1, MqFactoryReturnMsg(ret));
}

MQ_CST 
MqFactoryCtxIdentGet (
  struct MqS const * const context
)
{
  return context && context->setup.factory ? context->setup.factory->ident : "";
}

END_C_DECLS

