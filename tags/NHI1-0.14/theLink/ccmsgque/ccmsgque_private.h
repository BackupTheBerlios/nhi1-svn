/**
 *  \file       theLink/ccmsgque/ccmsgque_private.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef MQ_CCMSGQUE_PRIVATE_H
#define MQ_CCMSGQUE_PRIVATE_H

#define MQ_PRIVATE

#include "main.h"
#include "ccmsgque.h"

//#define ErrorStringToCC(err) throw MqCException(-1, MQ_ERROR, __func__, __FILE__, __LINE__, err)
#define ErrorStringToCC(err) throw runtime_error(err)

#endif /* MQ_CCMSGQUE_PRIVATE_H */

