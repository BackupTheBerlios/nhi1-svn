/**
 *  \file       theLink/libmsgque/read.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.sourceforge.net
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

/// \brief Return-Codes used in #sSend_RET_START function
enum MqReturnE {
    MQ_RETURN_OK	= 'O',		///< return an \b ok package
    MQ_RETURN_ERROR	= 'E',		///< return an \b error package
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

/// \brief get the return-code of the current Msgque packet from the \e MqReadS stack
/// \context
/// \return the return-code
enum MqReturnE pReadGetReturnCode (
  struct MqS const * const context
);

/// \brief get the return-number of the current Msgque packet from the \e MqReadS stack
/// \context
/// \return the return-number
MQ_INT pReadGetReturnNum (
  struct MqS const * const context
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
/// \param read a pointer to an MqReadS object
#define pRead_RET_END(read) MqReadL_END(read)

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


