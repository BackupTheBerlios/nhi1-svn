/**
 *  \file       theLink/acmds/acut.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

/** \addtogroup acut
 * \{
 */

#ifndef CONTEXT_H
#define CONTEXT_H

#include "msgque.h"
#include "config.h"

/*****************************************************************************/
/*                                                                           */
/*                              context_basic                                */
/*                                                                           */
/*****************************************************************************/

/// \brief the local \b context of the \ref acut tool
struct CutCtxS {

  struct MqS *	msgque;	      ///< \msgqueI
  MQ_INT	fields[256];  ///< fields used to cut, from the <tt>-f \b FIELDS</tt> option

};

/*****************************************************************************/
/*                                                                           */
/*                                context_init                               */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE CutCreate (
  struct MqConfigS  * config,
  struct MqBufferLS ** argvP,
  struct MqBufferLS ** alfaP,
  struct MqS ** msgqueP
);

#endif /* CONTEXT_H */

/** \} server */
