/**
 *  \file       theLink/libmsgque/slave.h
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */


#ifndef SLAVE_H
# define SLAVE_H

BEGIN_C_DECLS

enum MqErrorE pSlaveCreate (struct MqS * const, struct MqLinkSlaveS ** const);
void pSlaveDelete (struct MqLinkSlaveS ** const);
void pSlaveShutdown (struct MqLinkSlaveS * const);
void pSlaveMark (struct MqLinkSlaveS * const, MqMarkF const);

END_C_DECLS

#endif


