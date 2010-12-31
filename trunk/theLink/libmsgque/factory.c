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

BEGIN_C_DECLS

struct MqFactoryItemS *defaultFactoryItem = NULL;

#define pFactoryCmp(s1,s2) strcmp(s1,s2)

/*****************************************************************************/
/*                                                                           */
/*                               factory_define                              */
/*                                                                           */
/*****************************************************************************/

static struct pFactorySpaceS {
  struct MqFactoryItemS *items;	    ///< array of the items
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

/*****************************************************************************/
/*                                                                           */
/*                              factory_memory                                */
/*                                                                           */
/*****************************************************************************/

void
FactorySpaceCreate (void)
{
  space.items = (struct MqFactoryItemS *) MqSysCalloc (MQ_ERROR_PANIC, SPACE_INIT_SIZE, sizeof (*space.items));
  space.size = SPACE_INIT_SIZE;
  space.used = 1;  // first item is always the default

  MqFactoryDefault("libmsgque", MqFactoryDefaultCreate, NULL, NULL, NULL, NULL, NULL);
}

void
FactorySpaceDelete (void)
{
  sFactorySpaceDelAll ();
  MqSysFree (space.items);
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
  space.items = (struct MqFactoryItemS *)
	MqSysRealloc (MQ_ERROR_PANIC, space.items, (newsize * sizeof (*space.items)));
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
    (*space.items[id].Create.fFree) (MQ_ERROR_PANIC, &space.items[id].Create.data);
  }
  if (space.items[id].Delete.data && space.items[id].Delete.fFree) {
    (*space.items[id].Delete.fFree) (MQ_ERROR_PANIC, &space.items[id].Delete.data);
  }
  MqSysFree(space.items[id].ident);
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
    memset(space.items, '\0', space.used * sizeof(struct MqFactoryItemS));
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
  struct MqFactoryItemS *find;

  if (MqFactoryItemGet(ident,&find) == MQ_FACTORY_RETURN_OK) {
    // item already available, PANIC if callback is NOT equal
    if (memcmp(&Create, &find->Create, sizeof(Create)) || memcmp(&Delete, &find->Delete, sizeof(Delete))) {
      return MQ_FACTORY_RETURN_ADD_IDENT_IN_USE_ERR;
    }
    // OK, item available and in use ... nothing to do
  } else {
    // item not available, add new one
    struct MqFactoryItemS *free;

    sFactorySpaceAdd (1);

    //free = space.items + space.used * sizeof(*space.items);
    free = space.items + space.used;

    space.used += 1;

    free->ident = MqSysStrDup(MQ_ERROR_PANIC, ident);

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
	   struct MqFactoryItemS * start = space.items;
  register struct MqFactoryItemS * end = start + space.used;
  while (start < end--) {
    if (end->Create.data) (*markF)(end->Create.data);
    if (end->Delete.data) (*markF)(end->Delete.data);
  }
}

void 
pFactoryCtxItemSet (
  struct MqS * const context,
  struct MqFactoryItemS * const item
) {
  context->setup.factory = item;
  MqConfigUpdateName(context, item->ident);
}

/*****************************************************************************/
/*                                                                           */
/*                                 public                                    */
/*                                                                           */
/*****************************************************************************/

enum MqFactoryReturnE
MqFactoryItemGet (
  MQ_CST const ident,
  struct MqFactoryItemS **itemP
)
{
	   struct MqFactoryItemS * start = space.items;
  register struct MqFactoryItemS * end = start + space.used;
  while (start < end-- && strcmp(end->ident,ident)) {}
  if (end >= start) {
    *itemP = end;
    return MQ_FACTORY_RETURN_OK;
  } else {
    *itemP = NULL;
    return MQ_FACTORY_RETURN_ITEM_GET_ERR;
  }
}

enum MqErrorE 
MqFactoryInvoke (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryItemS* item,
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
  MQ_CST           const ident,
  MqFactoryCreateF const fCreate,
  MQ_PTR           const createData,
  MqTokenDataFreeF const createDatafreeF,
  MqFactoryDeleteF const fDelete,
  MQ_PTR           const deleteData,
  MqTokenDataFreeF const deleteDatafreeF
)
{
  struct MqFactoryCreateS Create = {fCreate, createData, createDatafreeF};
  struct MqFactoryDeleteS Delete = {fDelete, deleteData, deleteDatafreeF};
  if (ident == NULL || fCreate == NULL) {
    return MQ_FACTORY_RETURN_ADD_DEF_ERR;
  }
  return pFactoryAddName (ident, Create, Delete);
}

enum MqFactoryReturnE
MqFactoryCopyDefault (
  MQ_CST const ident
)
{
  return pFactoryAddName (ident, space.items[0].Create, space.items[0].Delete);
}

enum MqFactoryReturnE
MqFactoryDefault (
  MQ_CST           const ident,
  MqFactoryCreateF const fCreate,
  MQ_PTR           const createData,
  MqTokenDataFreeF const createDatafreeF,
  MqFactoryDeleteF const fDelete,
  MQ_PTR           const deleteData,
  MqTokenDataFreeF const deleteDatafreeF
)
{
  struct MqFactoryCreateS Create = {fCreate, createData, createDatafreeF};
  struct MqFactoryDeleteS Delete = {fDelete, deleteData, deleteDatafreeF};
  if (ident == NULL || fCreate == NULL) {
    return MQ_FACTORY_RETURN_ADD_DEF_ERR;
  }
  // Del
  sFactorySpaceDelItem (0);
  // Add
  space.items[0].ident = MqSysStrDup(MQ_ERROR_PANIC, ident);
  space.items[0].Create = Create;
  space.items[0].Delete = Delete;
  defaultFactoryItem = &space.items[0];
  return MQ_FACTORY_RETURN_OK;
}

enum MqErrorE
MqFactoryDefaultCreate (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryItemS* item,
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
  enum MqFactoryReturnE ret = MQ_FACTORY_RETURN_OK;
  struct MqFactoryItemS *item;
  struct MqS * mqctx;
  *ctxP = NULL;
  MqFactoryErrorCheck (ret = MqFactoryItemGet (ident, &item));
  if (MqErrorCheckI(MqFactoryInvoke ((struct MqS * const)data, MQ_FACTORY_NEW_INIT, item, &mqctx))) {
    ret = MQ_FACTORY_RETURN_CALL_ERR;
  }
  MqConfigUpdateName(mqctx, item->ident);
  *ctxP = mqctx;
error:
  return ret;
}

enum MqFactoryReturnE
MqFactoryNew (
  MQ_CST           const ident,
  MqFactoryCreateF const fCreate,
  MQ_PTR           const createData,
  MqTokenDataFreeF const createDatafreeF,
  MqFactoryDeleteF const fDelete,
  MQ_PTR           const deleteData,
  MqTokenDataFreeF const deleteDatafreeF,
  MQ_PTR data,
  struct MqS ** ctxP
)
{
  enum MqFactoryReturnE ret;
  *ctxP = NULL;
  ret = MqFactoryAdd (ident, fCreate, createData, createDatafreeF, 
			fDelete, deleteData, deleteDatafreeF);
  if (MqFactoryErrorCheckI(ret)) return ret;
  return MqFactoryCall (ident, data, ctxP); 
}

MQ_PTR
MqFactoryItemGetCreateData(
  struct MqFactoryItemS  const * const item
)
{
  return item->Create.data;
}

MQ_PTR
MqFactoryItemGetDeleteData(
  struct MqFactoryItemS  const * const item
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
   case MQ_FACTORY_RETURN_ADD_DEF_ERR:	
    return "factory definition incomplete 'ident' or 'fCreate' not available";
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
   default:
    return "nothing";
  }
}

void MqFactoryErrorPanic (
  enum MqFactoryReturnE ret
)
{
  if (ret != MQ_FACTORY_RETURN_OK)
    MqPanicC(MQ_ERROR_PANIC, __func__, -1, MqFactoryErrorMsg(ret));
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
  return context->setup.factory ? context->setup.factory->ident : "";
}

END_C_DECLS

