/**
 *  \file       libmsgque/slave.h
 *  \brief      \$Id: slave.h 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */


#ifndef SLAVE_H
# define SLAVE_H

BEGIN_C_DECLS

enum MqErrorE pSlaveCreate (struct MqS * const, struct MqLinkSlaveS ** const);
void pSlaveDelete (struct MqLinkSlaveS ** const);
void pSlaveShutdown (struct MqLinkSlaveS * const);

END_C_DECLS

#endif


