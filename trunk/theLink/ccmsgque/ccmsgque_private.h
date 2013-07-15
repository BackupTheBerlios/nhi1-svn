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

#if defined (MQ_HAS_THREAD)
# if defined(HAVE_PTHREAD) /* unix thread */
#  define MqThreadLocal __thread
# else /* windows THREAD */
#  define MqThreadLocal __declspec(thread)
# endif
#else /* no THREAD */
#  define MqThreadLocal
#endif // MQ_HAS_THREAD


#endif /* MQ_CCMSGQUE_PRIVATE_H */

