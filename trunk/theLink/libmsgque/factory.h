/**
 *  \file       theLink/libmsgque/token.h
 *  \brief      \$Id: token.h 250 2010-10-16 18:14:03Z aotto1968 $
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 250 $
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef FACTORY_H
#   define FACTORY_H

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                              token_misc                                   */
/*                                                                           */
/*****************************************************************************/

/// \brief initialize a #MqFactoryS object to \c NULL
#define MqFactoryS_NULL { {NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL} }

struct pFactoryItemS;

struct pFactoryItemS* pFactoryItemGet (
  MQ_CST const name
);

struct MqFactoryS pFactoryGet (
  struct pFactoryItemS *item
);

void pFactoryMark (
  struct MqS * const context,
  MqMarkF markF
);

struct pFactoryItemS*
pFactoryItemGetWithCheck (
  struct MqS * context,
  MQ_CST const name
);

enum MqErrorE MQ_DECL pCallFactory (
  struct MqS * const context,
  enum MqFactoryE create,
  struct pFactoryItemS* item,
  struct MqS ** contextP
);

END_C_DECLS

#endif /* FACTORY_H */

