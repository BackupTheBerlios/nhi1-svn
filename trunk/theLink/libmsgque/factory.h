/**
 *  \file       theLink/libmsgque/factory.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
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

struct MqFactoryItemS* MqFactoryItemGet (
  MQ_CST const name
);

void pFactoryMark (
  struct MqS * const context,
  MqMarkF markF
);

enum MqErrorE MQ_DECL pCallFactory (
  struct MqS * const context,
  enum MqFactoryE create,
  struct MqFactoryItemS* item,
  struct MqS ** contextP
);

END_C_DECLS

#endif /* FACTORY_H */



