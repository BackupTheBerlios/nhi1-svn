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

void pFactoryMark (
  struct MqS * const context,
  MqMarkF markF
);

void pFactoryCtxItemSet (
  struct MqS * const context,
  struct MqFactoryItemS * const item
);

END_C_DECLS

#endif /* FACTORY_H */



