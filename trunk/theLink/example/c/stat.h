/**
 *  \file       theLink/example/c/stat.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

/** \addtogroup stat_api
 * \{
 */

#ifndef STAT_H
#   define STAT_H

#include "mqconfig.h"
#include "msgque.h"

/*****************************************************************************/
/*                                                                           */
/*                        stat context storage                               */
/*                                                                           */
/*****************************************************************************/

/// \brief the statistic context object used to calculate the statistics
///
/// The StatCtxS object is used together with an #StatTimerS object to calculate
/// the statistics in the #StatCtxCalc function using the initial date/time
/// values from the #StatTimerS and the current date/time values updating the
/// #StatCtxS::val object.
///
typedef struct StatCtxS {
  struct MqS * mqctx;	      ///> link
  struct MqBufferS * name;    ///< identify the measurement point
  MQ_DBL count;		      ///< the number of times this measurement point was calculated
  MQ_DBL val;		      ///< the average time value
} StatCtxS;

/// \brief pointer to an #StatCtxS object
typedef struct StatCtxS *StatCtxSP;

/// \brief pointer to an #StatTimerS object
typedef struct StatTimerS *StatTimerSP;

/*****************************************************************************/
/*                                                                           */
/*                                stat_init                                  */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE StatInit (
  StatTimerSP timer
);

enum MqErrorE StatInit2 (
  StatTimerSP timer,
  time_t tv_sec,
  MQ_INT tv_usec,
  MQ_INT tz_minuteswest,
  MQ_INT tz_dsttime
);

/*****************************************************************************/
/*                                                                           */
/*                                stat_get                                   */
/*                                                                           */
/*****************************************************************************/

struct mq_timeval * StatGetTV (
  StatTimerSP const timer
);

struct mq_timezone * StatGetTZ (
  StatTimerSP const timer
);

/*****************************************************************************/
/*                                                                           */
/*                                stat_ctx                                   */
/*                                                                           */
/*****************************************************************************/

StatCtxSP StatCtxCreate (
  struct MqS * const mqctx,
  const MQ_STR name,
  MQ_INT count
);

void StatCtxDelete (
  StatCtxSP *contextP
);

void StatCtxPrint (
  StatCtxSP context
);

void StatCtxCalc (
  StatCtxSP context,
  StatTimerSP const timer
);

StatTimerSP StatCreate (
  struct MqS * const mqctx
);

enum MqErrorE StatDelete (
  StatTimerSP *timertP
);

enum MqErrorE StatInit (
  StatTimerSP timer
);

void StatLog (
  const char *prefix,
  StatTimerSP const timer
);

#endif /* STAT_H */

/** \} stat_api */






