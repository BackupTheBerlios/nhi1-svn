/**
 *  \file       theLink/libmsgque/link.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef MQ_LINK_H
#define MQ_LINK_H

BEGIN_C_DECLS

struct pChildS {
    struct pChildS * left   ;
    struct MqS     * context;
    struct pChildS * right  ;
};

void pMqShutdown (
  struct MqS * const context,
  MQ_CST prefix
);

void pLinkDisConnect (
  struct MqS * const context
);

END_C_DECLS

#endif /* MQ_LINK_H */


