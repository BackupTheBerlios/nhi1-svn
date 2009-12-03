/**
 *  \file       theLink/acmds/asort.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

/** \addtogroup asort
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

/// \brief storage for the relation between \e key and the whole input \e line
struct SortKeyS {
    struct MqBufferS *	key;		///< the sort key as \b -NUM index from the input \e line
    struct MqBufferLS * line;		///< the whole input line
};

/// \brief the local \b context of the \ref asort tool
struct SortCtxS {
  struct MqS *	    msgque;		///< \msgqueI
  MQ_SIZE	    idx;		///< the column index from: <tt>\b -NUM TYPE</tt>
  enum MqTypeE	    type;		///< type of the column index from: <tt>-NUM \b TYPE</tt>
  struct SortKeyS   *sort;		///< the array of sort keys filled in #SortFilter
  MQ_SIZE	    sort_size;		///< the total amount of the allocated SortKeyS objects in the #sort array
  MQ_SIZE	    sort_cursize;	///< the used number of SortKeyS objects in the #sort array
};

/*****************************************************************************/
/*                                                                           */
/*                                context_init                               */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE SortCreate (
  struct MqConfigS * config,
  struct MqBufferLS * argv,
  struct MqBufferLS * alfa,
  struct MqS ** msgqueP
);

#endif /* CONTEXT_H */

/** \} */
