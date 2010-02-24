/**
 *  \file       theLink/libmsgque/read.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef READ_H
#   define READ_H

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                            msgque return codes                            */
/*                                                                           */
/*****************************************************************************/

/// \brief the \e role of a data package in a service call
enum MqHandShakeE {
  MQ_HANDSHAKE_START	    = 'S',  ///< start  \b service-call
  MQ_HANDSHAKE_OK	    = 'O',  ///< return \b ok service-call
  MQ_HANDSHAKE_ERROR	    = 'E',  ///< return \b error service-call
  MQ_HANDSHAKE_TRANSACTION  = 'T',  ///< start  \b transaction
};

/*****************************************************************************/
/*                                                                           */
/*                                read_init                                  */
/*                                                                           */
/*****************************************************************************/

/** \brief creation of an  \e MqReadS object
 */
struct MqReadS* pReadCreate (
  struct MqS * const context
);

/** \brief deletion of an \e MqReadS object
 */
void pReadDelete (
  struct MqReadS ** readP
) __attribute__((nonnull));

/*****************************************************************************/
/*                                                                           */
/*                                read_misc                                  */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE pReadHDR (
  register struct MqS * context,
  struct MqS ** a_msgque
);

void pReadSetType(
  struct MqS * const context,
  MQ_BOL const type
);

void pReadL_CLEANUP (register struct MqS * const);

enum MqHandShakeE pReadGetHandShake (
  struct MqS const * const context
);

void pReadSetHandShake (
  struct MqS const * const context,
  enum MqHandShakeE hs
);

void pReadInitTransactionItem (
  struct MqS * const context
);

void pReadCleanupTransactionItem (
  struct MqS * const context
);

MQ_CST pReadGetTransactionToken (
  struct MqS * const context
);

void pReadSetReturnNum (
  struct MqS const * const context,
  MQ_INT retNum
);

/// \brief reading from a dynamic \e MqBufferS
/// \context
/// \buf
/// \retval out the \e MqBufferS object to be filled
/// \return error-code
/// \attention
/// - the \e out object itself will \b not be created and have to be available 
///   but the \b internal buffer memory is expanded as needed
/// - the numItems of the \e buf object will be decreased by 1
/// - at the end of \e out an \\0 will be attached
/// .
enum MqErrorE pReadWord (
  struct MqS * const context,
  struct MqBufferS * const buf,
  register struct MqBufferS * const out
);

/// \brief creating a \e RET reference for reading the return items
/// \context
/// \retMqErrorE
enum MqErrorE pRead_RET_START (
  struct MqS * const context
) __attribute__((nonnull));

/// \brief deleting a \e RET reference after reading the return items was finished
/// \context
void pRead_RET_END (
  struct MqS * const context
) __attribute__((nonnull));

/*****************************************************************************/
/*                                                                           */
/*                                 read.log                                  */
/*                                                                           */
/*****************************************************************************/

void pReadLog (
  register struct MqS const * const context,
  MQ_CST const prefix
);

END_C_DECLS

#endif /* READ_H */


