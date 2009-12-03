/**
 *  \file       theLink/libmsgque/cache.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef CACHE_H
#define CACHE_H

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                              cache_init                                   */
/*                                                                           */
/*****************************************************************************/

typedef MQ_PTR (
  *CacheCF
) (
  MQ_PTR const
);

typedef void (
  *CacheDF
) (
  MQ_PTR * const
);

void
 pCacheCreate (
  CacheCF createF,
  CacheDF deleteF,
  MQ_PTR createD,
  struct MqCacheS ** const out
);

void
 pCacheDelete (
  struct MqCacheS ** const out
) __attribute__((nonnull));

/*****************************************************************************/
/*                                                                           */
/*                              cache_1                                      */
/*                                                                           */
/*****************************************************************************/

MQ_PTR
 pCachePop (
  register struct MqCacheS * const top
);

void
 pCachePush (
  register struct MqCacheS * const top,
  MQ_PTR data
);

END_C_DECLS

#endif
