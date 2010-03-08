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

struct pTokenS* pTokenCreate (struct MqS * const);

void pTokenDelete (
  register struct pTokenS ** const tokenP
) __attribute__((nonnull));

/*****************************************************************************/
/*                                                                           */
/*                              token_misc                                   */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE pTokenCheckSystem (
  struct pTokenS const * const token
);

/*
MQ_SIZE pTokenGetUsed (
  register struct pTokenS const * const token
);
*/

enum MqErrorE pTokenInvoke (
  struct pTokenS const * const token
);

int pTokenCheck (
  struct pTokenS const * const token,
  MQ_CST const str
);

enum MqErrorE pTokenAddHdl (
  struct pTokenS const * const token,
  MQ_CST const name,
  struct MqCallbackS callback
);

enum MqErrorE pTokenDelHdl (
  struct pTokenS const * const token,
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

#define TOKEN_LEN (HDR_TOK_LEN+1)

struct pTokenS {
  struct MqS * context;		///< link to msgque object
  MQ_STRB current[TOKEN_LEN];	///< kind the the current action
  struct pTokenSpaceS *loc;     ///< the local handler
};

static mq_inline MQ_CST pTokenGetCurrent (
  struct pTokenS const * const token
)
{
  return token->current;
}

static mq_inline void pTokenSetCurrent (
  struct pTokenS * const token,
  MQ_CST const str
)
{
  memcpy (token->current, str, HDR_TOK_LEN);
}

END_C_DECLS

#endif /* TOKEN_H */

