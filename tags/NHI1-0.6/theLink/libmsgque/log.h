/**
 *  \file       theLink/libmsgque/log.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef LOG_H
#   define LOG_H

BEGIN_C_DECLS

void pLog (
  FILE * channel,
  MQ_CST const fmt,
  ...
) __attribute__ ((format (printf, 2, 3)));

void pLogHDR (
  struct MqS * const context,
  MQ_CST prefix,
  const MQ_INT level,
  struct MqBufferS * const buf
);

void pLogBDY (
  struct MqS * const context,
  MQ_CST prefix,
  const MQ_INT level,
  struct MqBufferS * const buf
);

void pLogMqBuffer (
  struct MqS * const context,
  MQ_CST header,
  MQ_INT level,
  struct MqBufferS * buf
);

void pLogHEX ( struct MqS const * const, MQ_CST const, MQ_BINB const * , MQ_SIZE const);

MQ_STR pLogAscii ( MQ_STR buf, MQ_STR str, MQ_SIZE size);

END_C_DECLS

#endif // LOG_H


