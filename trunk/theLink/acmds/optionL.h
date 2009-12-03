/**
 *  \file       theLink/acmds/optionL.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef OPTION_L_H
#define OPTION_L_H

#include "mqconfig.h"
#include "msgque.h"

/*****************************************************************************/
/*                                                                           */
/*                    reading options from a MqBufferLS                      */
/*                                                                           */
/*****************************************************************************/

/** \addtogroup optionL_api
 * \{
 */

/// \brief storage for an index/argument pair used as array in OptLS
typedef struct OptLAS {
    MQ_SIZE	idx;	///< the NUM index from: <tt>\b -NUM ARGUMENT</tt>
    struct MqBufferS *	val;	///< the ARGUMENT from: <tt>-NUM \b ARGUMENT</tt>
} OptLAS;

/// \brief a pointer to the OptLAS object
typedef OptLAS* OptLASP;

/// \brief container for the OptLAS array
typedef struct OptLS {
    MQ_SIZE	size;	    ///< the total amount of the allocated OptLAS objects in the #data array
    MQ_SIZE	cursize;    ///< the used number of OptLAS objects in the #data array
    OptLASP	data;	    ///< the array of options filled in #OptLCreate
} OptLS;

/// \brief a pointer to the OptLS object
typedef OptLS* OptLSP;

enum MqErrorE
OptLCreate (
  struct MqS * const mqctx,
  struct MqBufferLS * const argv,
  struct OptLS ** const out
);

void
OptLDelete (
  OptLSP * out
);

/** \} optionL_api */

#endif /* OPTION_L_H */

