/**
 *  \file       theLink/libmsgque/cache.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "main.h"

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                               cache_init                                  */
/*                                                                           */
/*****************************************************************************/

struct MqCacheItemS;

struct MqCacheItemS {
  struct MqCacheItemS * next;	///< link to the next item
  MQ_PTR data;			///< data to cache
};

struct MqCacheS {
  struct MqCacheItemS * used;	///< used cache items
  struct MqCacheItemS * free;	///< free cache items
  CacheCF createF;              ///< function to create 'data'
  CacheDF deleteF;              ///< function to delete 'data'
  MQ_PTR createD;               ///< additional create data item
};

void
pCacheCreate (
  CacheCF createF,
  CacheDF deleteF,
  MQ_PTR createD,
  struct MqCacheS ** const out
)
{
  register struct MqCacheS * const cache = (struct MqCacheS * const) MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof (*cache));
  cache->createF = createF;
  cache->deleteF = deleteF;
  cache->createD = createD;
  *out = cache;
}

void
pCacheDelete (
  struct MqCacheS ** const topP
)
{
  struct MqCacheS * const top = *topP;
  struct MqCacheItemS * item, * next;
  MQ_PTR ptr;
  if (unlikely(top==NULL)) return;
  // 1. free
  item = top->free;
  while (item) {
    next = item->next;
    (*(top->deleteF)) (&item->data);
    ptr = (MQ_PTR) item;
    MqSysFree (ptr);
    item = next;
  }
  // 2. used (e.g. no data)
  item = top->used;
  while (item) {
    next = item->next;
    ptr = (MQ_PTR) item;
    MqSysFree (ptr);
    item = next;
  }
  MqSysFree (*topP);
}

/*****************************************************************************/
/*                                                                           */
/*                               cache_1                                     */
/*                                                                           */
/*****************************************************************************/

MQ_PTR
pCachePop (
  register struct MqCacheS * const top
)
{
  MQ_PTR data;
  struct MqCacheItemS * const free = top->free;
  if (free) {
    data = free->data;
    free->data = NULL;
    top->free = free->next;
    free->next = top->used;
    top->used = free;
  } else {
    data = (*(top->createF)) (top->createD);
  }
  return data;
}

void
pCachePush (
  register struct MqCacheS * const top,
  MQ_PTR data
)
{
  struct MqCacheItemS * item = top->used;
  if (item) {
    top->used = item->next;
  } else {
    item = (struct MqCacheItemS *) MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof (*item));
  }
  item->data = data;
  item->next = top->free;
  top->free = item;
}

void 
pCacheMark (
  register struct MqCacheS * const top,
  MqCacheMarkF cacheMarkF,
  MqMarkF markF
)
{
  struct MqCacheItemS * item = top->used;
  while (item) {
    (*cacheMarkF)(item->data, markF);
    item = item->next;
  }
}

END_C_DECLS





