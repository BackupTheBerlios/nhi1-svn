/**
 *  \file       theLink/acmds/ajoin.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

/** \addtogroup ajoin
 * \{
 */

#ifndef JOIN_CONTEXT_H
#define JOIN_CONTEXT_H

#include "msgque.h"
#include "config.h"
#include "optionL.h"

/*****************************************************************************/
/*                                                                           */
/*                              context_basic                                */
/*                                                                           */
/*****************************************************************************/

/// \brief the local \b context of the \ref ajoin tool
struct JoinCtxS {
  struct MqS 	mqctx;		///< \msgqueI
  MQ_STR		delimiter;	///< string used to join columns together (from: -d DELIMITER)
  MQ_STR		format[256];    ///< array of format items for the first 256 columns
  enum MqTypeE		type[256];	///< the type of the format (from: -NUM FORMAT)
  OptLSP		formatlist;	///< the output from the #OptLCreate function used as storage for
					///  #format strings
  MQ_STR		free[256];      ///< array of temporary formats ready to be freed
};

/*****************************************************************************/
/*                                                                           */
/*                                context_init                               */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE JoinCreate (
  struct MqConfigS * config,
  struct MqBufferLS ** argvP,
  struct MqBufferLS ** alfaP,
  struct MqS ** msgqueP
);

#endif /* JOIN_CONTEXT_H */

/** \} */

