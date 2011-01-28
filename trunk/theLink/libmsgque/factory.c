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
#include "error.h"
#include "errno.h"
#include "sqlite3.h"

#define MQ_CONTEXT_S context

BEGIN_C_DECLS

struct MqFactoryS *defaultFactoryItem = NULL;

/// \brief a static Factory function return this enum as status
/// \details Use the #MqFactoryErrorMsg function to \copybrief MqFactoryErrorMsg
enum MqFactoryReturnE {
  MQ_FACTORY_RETURN_OK,
  MQ_FACTORY_RETURN_ERR,
  MQ_FACTORY_RETURN_ADD_ERR,
  MQ_FACTORY_RETURN_ADD_IDENT_IN_USE_ERR,
  MQ_FACTORY_RETURN_CREATE_FUNCTION_REQUIRED,
  MQ_FACTORY_RETURN_CALL_ERR,
  MQ_FACTORY_RETURN_COPY_ERR,
  MQ_FACTORY_RETURN_ITEM_IS_NULL,
  MQ_FACTORY_RETURN_INVALID_IDENT,
  MQ_FACTORY_RETURN_CALLOC_ERR,
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
   case MQ_FACTORY_RETURN_CALLOC_ERR:
    return "unable to add transaction storage";
   case MQ_FACTORY_RETURN_ITEM_IS_NULL:
    return "item is NULL";
   case MQ_FACTORY_RETURN_COPY_ERR:
    return "unable to copy factory";
   case MQ_FACTORY_RETURN_DEFAULT_ERR:
    return "unable to create the default factory";
   case MQ_FACTORY_RETURN_GET_ERR:
    return "unable to get factory";
  }
  return "nothing";
}

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
MqFactoryGetCalled (
  MQ_CST const ident
)
{
  struct MqFactoryS * item = MqFactoryGet(ident);
  if (item != NULL) {
    item->called = MQ_YES;
  }
  return item;
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
MqFactoryNew (
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
/*                           protected setup                                 */
/*                                                                           */
/*****************************************************************************/

void
FactoryCreate (void)
{
  // setup main proc global data ! not thread safe !
  space.items = (struct MqFactoryS *) sFactoryCalloc (SPACE_INIT_SIZE, sizeof (*space.items));
  space.size = SPACE_INIT_SIZE;
  space.used = 1;  // first item is always the default

  MqFactoryDefault("libmsgque", MqFactoryDefaultCreate, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

void
FactoryDelete (void)
{
  // delete main proc global data ! not thread safe !
  sFactorySpaceDelAll ();
  sqlite3_free (space.items);
}

END_C_DECLS

