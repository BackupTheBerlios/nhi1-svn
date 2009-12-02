/**
 *  \file       libmsgque/log.h
 *  \brief      \$Id: log.h 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef LOG_H
#   define LOG_H

BEGIN_C_DECLS

void
 pLogHDR (
  struct MqS * const context,
  MQ_CST prefix,
  const MQ_INT level,
  struct MqBufferS * const buf
);

void
 pLogBDY (
  struct MqS * const context,
  MQ_CST prefix,
  const MQ_INT level,
  struct MqBufferS * const buf
);

void
 pLogMqBuffer (
  struct MqS * const context,
  MQ_CST header,
  MQ_INT level,
  struct MqBufferS * buf
);

void pLogHEX ( struct MqS const * const, MQ_CST const, MQ_BINB const * , MQ_SIZE const);

END_C_DECLS

#endif // LOG_H


