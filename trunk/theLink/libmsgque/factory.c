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

MQ_CST defaultFactory;

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
  struct MqFactoryItemS *item
)
{
  if (item->Create.data && item->Create.fFree) {
    (*item->Create.fFree) (MQ_ERROR_PANIC, &item->Create.data);
  }
  if (item->Delete.data && item->Delete.fFree) {
    (*item->Delete.fFree) (MQ_ERROR_PANIC, &item->Delete.data);
  }
  MqSysFree(item->name);
}

static void
sFactorySpaceDelAll (
  void
)
{
  struct MqFactoryItemS * start = space.items;
  struct MqFactoryItemS * end = start + space.used;

  // name == "-ALL"
  // delete the other name
  while (start < end--) {
    sFactorySpaceDelItem (end);
  }
  // set all items to zero
  memset(start, '\0', space.used * sizeof(struct MqFactoryItemS));
  space.used = 1;
  defaultFactory = NULL;
}

static void
pFactoryAddName (
  MQ_CST const name,
  struct MqFactoryCreateS Create,
  struct MqFactoryDeleteS Delete
)
{
  struct MqFactoryItemS *find = MqFactoryItemGet (name);

  if (find != NULL) {
    // item already available, PANIC if callback is NOT equal
    if (memcmp(&Create, &find->Create, sizeof(Create)) || memcmp(&Delete, &find->Delete, sizeof(Delete))) {
      MqPanicV(MQ_ERROR_PANIC,__FILE__,-1,"factory-entry '%s' already in use", name);
    }
    // OK, item available and in use ... nothing to do
  } else {
    // item not available, add new one
    struct MqFactoryItemS *free;

    sFactorySpaceAdd (1);

    //free = space.items + space.used * sizeof(*space.items);
    free = space.items + space.used;

    space.used += 1;

    free->name = MqSysStrDup(MQ_ERROR_PANIC, name);

    free->Create = Create;
    free->Delete = Delete;
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
	   struct MqFactoryItemS * start = space.items;
  register struct MqFactoryItemS * end = start + space.used;
  while (start < end--) {
    if (end->Create.data) (*markF)(end->Create.data);
    if (end->Delete.data) (*markF)(end->Delete.data);
  }
}

/*****************************************************************************/
/*                                                                           */
/*                                 public                                    */
/*                                                                           */
/*****************************************************************************/

struct MqFactoryItemS*
MqFactoryItemGet (
  MQ_CST const name
)
{
	   struct MqFactoryItemS * start = space.items;
  register struct MqFactoryItemS * end = start + space.used;
  while (start < end-- && strcmp(end->name,name)) {}
  return end >= start ? end : NULL;
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
    MqConfigSetFactoryItem (mqctx, item);
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
    return MQ_ERROR;
  }

error2:
  *contextP = NULL;
  if (create != MQ_FACTORY_NEW_INIT) {
    return MqErrorDbV2(tmpl, MQ_ERROR_CONFIGURATION_REQUIRED, "Factory", "MqSetupS::Factory");
  } else {
    return MQ_ERROR;
  }
}

void
MqFactoryAdd (
  MQ_CST           const name,
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
  if (name == NULL || fCreate == NULL) {
    MqPanicC (MQ_ERROR_PANIC, __func__, -1, "factory definition incomplete 'name' or 'fCreate' not available");
  }
  pFactoryAddName (name, Create, Delete);
}

void
MqFactoryDefault (
  MQ_CST           const name,
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
  if (name == NULL || fCreate == NULL) {
    MqPanicC (MQ_ERROR_PANIC, __func__, -1, "factory definition incomplete 'name' or 'fCreate' not available");
  }
  // Del
  sFactorySpaceDelItem (&space.items[0]);
  // Add
  space.items[0].name = MqSysStrDup(MQ_ERROR_PANIC, name);
  space.items[0].Create = Create;
  space.items[0].Delete = Delete;
  defaultFactory = space.items[0].name;
}

struct MqS *
MqFactoryCall (
  MQ_CST const name
)
{
  struct MqS * ctx = NULL;
  MqErrorCheck(MqFactoryInvoke (MQ_ERROR_PANIC, MQ_FACTORY_NEW_INIT, MqFactoryItemGet (name), &ctx));
  return ctx;
error:
  MqPanicV(MQ_ERROR_PANIC, __func__, -1, "unable to call main factory for identifer '%s'", name);
}

struct MqS *
MqFactoryNew (
  MQ_CST           const name,
  MqFactoryCreateF const fCreate,
  MQ_PTR           const createData,
  MqTokenDataFreeF const createDatafreeF,
  MqFactoryDeleteF const fDelete,
  MQ_PTR           const deleteData,
  MqTokenDataFreeF const deleteDatafreeF
)
{
  MqFactoryAdd (name, fCreate, createData, createDatafreeF, fDelete, deleteData, deleteDatafreeF);
  return MqFactoryCall (name); 
}

END_C_DECLS

