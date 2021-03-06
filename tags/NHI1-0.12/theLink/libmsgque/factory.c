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

BEGIN_C_DECLS

struct MqFactoryS *defaultFactoryItem = NULL;

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
/*                          factory_static                                     */
/*                                                                           */
/*****************************************************************************/

static void sFactorySpaceAdd (
  MQ_SIZE const add
);

static void sFactorySpaceDelAll (
  void
);

static MQ_STR sFactoryStrDup (
  MQ_CST s
) {
  MQ_STR result = (MQ_STR) sqlite3_malloc (strlen(s) + 1);
  if (result == (MQ_STR)0)
    return (MQ_STR)0;
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
  if (ptr) memset(ptr,'\0', nelem * elsize);
  
  return ptr;
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
  struct MqFactoryDeleteS Delete
)
{
  struct MqFactoryS *find;

  if (MqFactoryItemGet(ident,&find) == MQ_FACTORY_RETURN_OK) {
    // item already available, PANIC if callback is NOT equal
    if (memcmp(&Create, &find->Create, sizeof(Create)) || memcmp(&Delete, &find->Delete, sizeof(Delete))) {
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
pFactoryCall (
  MQ_CST const ident,
  MQ_PTR const data,
  MQ_BOL const called,
  struct MqS ** ctxP
)
{
  enum MqFactoryReturnE ret = MQ_FACTORY_RETURN_OK;
  struct MqFactoryS *item;
  struct MqS * mqctx;
  *ctxP = NULL;
  MqFactoryErrorCheck (ret = MqFactoryItemGet (ident, &item));
  if (MqErrorCheckI(MqFactoryInvoke ((struct MqS * const)data, MQ_FACTORY_NEW_INIT, item, &mqctx))) {
    ret = MQ_FACTORY_RETURN_CALL_ERR;
    goto error;
  }
  MqConfigUpdateName(mqctx, item->ident);
  *ctxP = mqctx;
  item->called = called;
error:
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
  MqFactoryDataCopyF  const deleteDataCopyF
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
  return pFactoryAddName (ident, Create, Delete);
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
    return pFactoryAddName (ident, Create, Delete);
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
  // Del
  sFactorySpaceDelItem (0);
  // Add
  space.items[0].ident = sFactoryStrDup(ident);
  space.items[0].Create = Create;
  space.items[0].Delete = Delete;
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
MqFactoryCall (
  MQ_CST const ident,
  MQ_PTR const data,
  struct MqS ** ctxP
)
{
  return pFactoryCall (ident, data, MQ_YES, ctxP);
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
  *ctxP = NULL;
  ret = MqFactoryAdd (ident, 
    fCreate, createData, createDataFreeF, createDataCopyF,
    fDelete, deleteData, deleteDataFreeF, deleteDataCopyF
  );
  if (MqFactoryErrorCheckI(ret)) return ret;
  return pFactoryCall (ident, data, MQ_NO, ctxP);
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
)
{
  return item->Delete.data;
}

MQ_CST MqFactoryErrorMsg (
  enum MqFactoryReturnE ret
)
{
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
  }
  return "nothing";
}

void MqFactoryErrorPanic (
  enum MqFactoryReturnE ret
)
{
  if (ret != MQ_FACTORY_RETURN_OK) {
    MqPanicC(MQ_ERROR_PANIC, __func__, -1, MqFactoryErrorMsg(ret));
  }
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
  return MqErrorC(context, __func__, -1, MqFactoryErrorMsg(ret));
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
  return MqErrorC(context, __func__, -1, MqFactoryErrorMsg(ret));
}

MQ_CST 
MqFactoryCtxIdentGet (
  struct MqS const * const context
)
{
  return context && context->setup.factory ? context->setup.factory->ident : "";
}

END_C_DECLS



