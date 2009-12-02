/**
 *  \file       libmsgque/token.h
 *  \brief      \$Id: token.h 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
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

END_C_DECLS

#endif /* TOKEN_H */


