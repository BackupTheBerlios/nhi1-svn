/**
 *  \file       theLink/gomsgque/src/gomsgque.c
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "gomsgque.h"
#include "_cgo_export.h"

static enum MqErrorE sServerSetup (
  struct MqS * const context,
  MQ_PTR const data
)
{
M0
}

void
gomsgque_ConfigSetServerSetup (
  struct MqS * const context
)
{
  MqConfigSetServerSetup(this.ctx, sServerSetup, nil, nil, nil)
}

