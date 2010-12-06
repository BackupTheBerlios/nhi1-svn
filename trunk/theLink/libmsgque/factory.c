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

BEGIN_C_DECLS

#define pFactoryCmp(s1,s2) strcmp(s1,s2)

/*****************************************************************************/
/*                                                                           */
/*                               factory_define                              */
/*                                                                           */
/*****************************************************************************/

static void pFactoryDelHdl (MQ_CST const name);

struct pFactoryItemS {
  MQ_CST name;			    ///< the pFactory name (cast into an integer)
  struct MqFactoryS callback;	    ///< callback data
};

static struct pFactorySpaceS {
  struct pFactoryItemS *items;	    ///< array of the items
  MQ_SIZE size;			    ///< max possible number of items
  MQ_SIZE used;			    ///< number of used entries in the items
  MQ_INT sorted;		    ///< is the array sorted ?
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

static void sFactorySpaceDelItem (
  MQ_CST const name
);

/*****************************************************************************/
/*                                                                           */
/*                              factory_memory                                 */
/*                                                                           */
/*****************************************************************************/

void
FactorySpaceCreate (void)
{
  space.items = (struct pFactoryItemS *) MqSysCalloc (MQ_ERROR_PANIC, SPACE_INIT_SIZE, sizeof (*space.items));
  space.size = SPACE_INIT_SIZE;
  space.used = 0;
  space.sorted = 0;
}

void
FactorySpaceDelete (void)
{
  sFactorySpaceDelItem ("-ALL");
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
  space.items = (struct pFactoryItemS *)
	MqSysRealloc (MQ_ERROR_PANIC, space.items, (newsize * sizeof (*space.items)));
  memset(space.items+space.used, '\0', newsize-space.used);
  space.size = newsize;
}

static struct pFactoryItemS *
sFactorySpaceFindItem (
  register struct pFactoryItemS * start,
  register struct pFactoryItemS const * const end,
  register MQ_CST const name
)
{
  while (start < end && pFactoryCmp(start->name,name))
    start++;
  return (start >= end ? NULL : start);
}

static void
sFactorySpaceDelItem (
  MQ_CST const name
)
{
  register struct pFactoryItemS * start = space.items;
           struct pFactoryItemS * end = start + space.used;

  if (strncmp (name, "-ALL", HDR_TOK_LEN)) {
    // name != "-ALL"
    start = sFactorySpaceFindItem (start, end, name);

    if (unlikely (start == NULL))
      MqErrorV (MQ_ERROR_PANIC, __func__, -1, "item '%s' not found", name);

    // call the data item cleanup
    if (start->callback.Create.data && start->callback.Create.fFree) {
      (*start->callback.Create.fFree) (MQ_ERROR_PANIC, &start->callback.Create.data);
    }
    // move all items !after! start one to the left
    memmove(start, start+1, (end-start-1) * sizeof(*space.items));
    // set the last item to zero
    memset(end-1, '\0', sizeof(*space.items));
    // one item less
    space.used -= 1;
  } else {
    // name == "-ALL"
    // delete the other name
    while (start < end--) {
      if (end->callback.Create.data && end->callback.Create.fFree) {
	(*end->callback.Create.fFree) (MQ_ERROR_PANIC, &end->callback.Create.data);
      }
    }
    // set all items to zero
    memset(start, '\0', space.used * sizeof(struct pFactoryItemS));
    space.used = 0;
  }
}

/*****************************************************************************/
/*                                                                           */
/*                              factory_misc                                 */
/*                                                                           */
/*****************************************************************************/

static int
MQ_CDECL sFactoryCompare1 (
  const void *one,
  const void *two
)
{
  return pFactoryCmp (((struct pFactoryItemS *) one)->name, ((struct pFactoryItemS *) two)->name);
}

static int
MQ_CDECL sFactoryCompare2 (
  const void *key,
  const void *two
)
{
  return pFactoryCmp (((MQ_CST)key), ((struct pFactoryItemS *) two)->name);
}

static void
pFactoryAddHdl (
  MQ_CST const name,
  struct MqFactoryS callback
)
{
  struct pFactoryItemS *free;

  if (sFactorySpaceFindItem (space.items, space.items + space.used, name)) {
    pFactoryDelHdl (name);
  }
  sFactorySpaceAdd (1);

  free = space.items + space.used;

  space.sorted = 0;
  space.used += 1;

  free->name = mq_strdup(name);
  free->callback = callback;
}

static void
pFactoryDelHdl (
  MQ_CST const name
)
{
  sFactorySpaceDelItem (name);
}

/*****************************************************************************/
/*                                                                           */
/*                              protected                                    */
/*                                                                           */
/*****************************************************************************/

struct pFactoryItemS*
pFactoryItemGet (
  MQ_CST const name
)
{
  struct pFactoryItemS * item = NULL;

  if (unlikely (!space.sorted)) {
    qsort (space.items, space.used, sizeof (struct pFactoryItemS), sFactoryCompare1);
    space.sorted = 1;
  }

  if (space.used != 0) {
    item = (struct pFactoryItemS *) bsearch (name, space.items,
	space.used, sizeof (struct pFactoryItemS), sFactoryCompare2);
  }

  return item;
}

void
pFactoryMark (
  struct MqS * const context,
  MqMarkF markF
)
{
	   struct pFactoryItemS * start = space.items;
  register struct pFactoryItemS * end = start + space.used;
  while (start < end--) {
    if (end->callback.Create.data) (*markF)(end->callback.Create.data);
  }
}

struct MqFactoryS
pFactoryGet (
  struct pFactoryItemS* item
)
{
  return item->callback;
}


/*****************************************************************************/
/*                                                                           */
/*                                 public                                    */
/*                                                                           */
/*****************************************************************************/

void
MqFactoryCreate(
  MQ_CST           const name,
  MqFactoryCreateF const fCreate,
  MQ_PTR           const createData,
  MqTokenDataFreeF const createDatafreeF,
  MqTokenDataCopyF const createDatacopyF,
  MqFactoryDeleteF const fDelete,
  MQ_PTR           const deleteData,
  MqTokenDataFreeF const deleteDatafreeF,
  MqTokenDataCopyF const deleteDatacopyF
)
{
  struct MqFactoryS cb = {
    {fCreate, createData, createDatafreeF, createDatacopyF}, 
    {fDelete, deleteData, deleteDatafreeF, deleteDatacopyF}
  };
  if (name == NULL || fCreate == NULL) {
    MqErrorV (MQ_ERROR_PANIC, __func__, -1, "factory definition incomplete name=%s or function=%p not found", name, fCreate);
  }
  pFactoryAddHdl (name, cb);
}

void
MqFactoryDelete(
  MQ_CST const name
)
{
  pFactoryDelHdl (name);
}

void
MqFactoryCall (
  MQ_CST const name,
  struct MqS **ctxP
)
{
  struct pFactoryItemS * item = pFactoryItemGet (name);

  if (item == NULL) {
    MqErrorV (MQ_ERROR_PANIC, __func__, -1, "factory <%s> not found", name);
  };

  (*item->callback.Create.fCall) (MQ_ERROR_PANIC, MQ_FACTORY_NEW_INIT, item->callback.Create.data, ctxP);
}

END_C_DECLS
