/**
 *  \file       theLink/libmsgque/config.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef CONFIG_H
#   define CONFIG_H

BEGIN_C_DECLS

void pConfigSetParent (
  struct MqS * const context,
  struct MqS * const parent
);

void pConfigSetMaster (
  struct MqS * const context,
  struct MqS * const master,
  int master_id
);

void pSetupMark (
  struct MqS * const context,
  MqMarkF const markF
);

END_C_DECLS

#endif /* CONFIG_H */

