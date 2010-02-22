/**
 *  \file       theLink/libmsgque/token.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef TOKEN_H
#   define TOKEN_H

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                             token_init                                    */
/*                                                                           */
/*****************************************************************************/

struct MqTokenS* pTokenCreate (struct MqS * const);

void pTokenDelete (
  register struct MqTokenS ** const tokenP
) __attribute__((nonnull));

/*****************************************************************************/
/*                                                                           */
/*                              token_misc                                   */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE pTokenCheckSystem (
  struct MqTokenS const * const token
);

/*
MQ_SIZE pTokenGetUsed (
  register struct MqTokenS const * const token
);
*/

enum MqErrorE pTokenInvoke (
  register struct MqTokenS const * const token
);

MQ_CST pTokenGetCurrent (
  struct MqTokenS const * const token
);

void pTokenSetCurrent (
  struct MqTokenS * const token,
  MQ_CST const str
);

int pTokenCheck (
  struct MqTokenS const * const token,
  MQ_CST const str
);

enum MqErrorE pTokenAddHdl (
  struct MqTokenS const * const token,
  MQ_CST const name,
  struct MqCallbackS callback
);

enum MqErrorE pTokenDelHdl (
  struct MqTokenS const * const token,
  MQ_CST const name
);

// can be (*((MQ_INT const *)(bin)))
static mq_inline MQ_INT pByte2INT (
  //MQ_BINB const * const bin
  MQ_CST const bin
) {
#if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
  MQ_INT i;
  memcpy(&i,bin,sizeof(MQ_INT));
  return i;
#else
  return (*((MQ_INT const *)(bin)));
#endif
}

END_C_DECLS

#endif /* TOKEN_H */


