/**
 *  \file       tests/stat.c
 *  \brief      \$Id: stat.c 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

/** \defgroup stat_api struct StatCtxS - collect and display statistical data
 *  \ingroup support
 *  \{
 *  \brief the \ref stat_api is used the handle statistical time data.
 *
 *  A statistical task is a collection and computation of time values
 *  in reference to an initial time. For computation of statistical
 *  values 2 objects are used:
 *   - StatCtxS, a container used for collection of the statistical data
 *   - MyStatTimerS, a measurement object used for collecting statistic steps and
 *             used to increment resulting data in StatCtxS
 *   .
 *
 *  example: get a statistic about the execution time of different tasks -> \b tests/client.c
 *
 */

#include "stat.h"
#include "debug.h"
#include <stdlib.h>
#include <string.h>

#if !defined(_MSC_VER)
#   include <sys/time.h>
#endif

/*****************************************************************************/
/*                                                                           */
/*                               stat_init                                   */
/*                                                                           */
/*****************************************************************************/

/// \brief the statistic timer object used to store date/time values for calculate the statistics
///
/// The timer is initialized with #StatInit and #StatInit2 and is later
/// used in #StatCtxCalc to calculate the statistics.
typedef struct StatTimerS {
  struct MqS * mqctx;    ///< pointer the the current #MqS object
  struct mq_timeval tv;         ///< storage for the timeval part of Stat
  struct mq_timezone tz;        ///< storage for the timezone part of Stat
} StatTimerS;

/// \brief create a statistic timer object StatTimerS
/// \msgque
/// \return the new created StatTimerS object
StatTimerSP
StatCreate (
  struct MqS * const mqctx
)
{
  register StatTimerSP stat = MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof (*stat));
  stat->mqctx = mqctx;
  return stat;
}

/// \brief initialize a statistic timer object #StatTimerS with the current date/time
/// \sta_timer
/// \retMqErrorE
enum MqErrorE
StatInit (
  StatTimerSP timer
)
{
  return SysGetTimeOfDay (timer->mqctx, &timer->tv, &timer->tz);
}

/// \brief initialize a statistic timer object #StatTimerS with a given date/time
/// \sta_timer
/// \param tv_sec timeval seconds
/// \param tv_usec timeval useconds
/// \param tz_minuteswest timezone minuteswest
/// \param tz_dsttime timezone dsttime
/// \retMqErrorE
enum MqErrorE
StatInit2 (
  StatTimerSP timer,
  time_t tv_sec,
  MQ_INT tv_usec,
  MQ_INT tz_minuteswest,
  MQ_INT tz_dsttime
)
{
  timer->tv.tv_sec = (long) tv_sec;
  timer->tv.tv_usec = tv_usec;
  timer->tz.tz_minuteswest = tz_minuteswest;
  timer->tz.tz_dsttime = tz_dsttime;

  return MQ_OK;
}

/// \brief delete a statistic timer object #StatTimerS
/// \param timerP a pointer to an StatTimerSP object
/// \retMqErrorE
/// \attDelete
enum MqErrorE
StatDelete (
  StatTimerSP *timerP
)
{
  if (!*timerP)
    return MQ_OK;
  MqSysFree (*timerP);

  return MQ_OK;
}

/*****************************************************************************/
/*                                                                           */
/*                                stat_get                                   */
/*                                                                           */
/*****************************************************************************/

/// \brief get the timeval object from the statistic context
/// \sta_timer
/// \retval the timeval object
struct mq_timeval *
StatGetTV (
  StatTimerSP const timer
)
{
  return &timer->tv;
}

/// \brief get the timezone object from the statistic context
/// \sta_timer
/// \retval the timezone object
struct mq_timezone *
StatGetTZ (
  StatTimerSP const timer
)
{
  return &timer->tz;
}

/*****************************************************************************/
/*                                                                           */
/*                                stat_ctx                                   */
/*                                                                           */
/*****************************************************************************/

/// \brief create a new statistic context object #StatCtxS
/// \msgque
/// \param name identify this object
/// \param count the initial value for #StatCtxS::count (should usually be 0)
/// \return the new created statistic context object
StatCtxSP
StatCtxCreate (
  struct MqS * const mqctx,
  const MQ_STR name,
  MQ_INT count
)
{
  register StatCtxSP context = MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof (*context));
  context->mqctx = mqctx;

  context->name = MqBufferCreateC (mqctx, name);

  context->count = count;
  context->val = 0.0;

  return context;
}

/// \brief delete a statistic context object #StatCtxS
/// \param contextP a pointer to an StatCtxSP object
/// \attDelete
void
StatCtxDelete (
  StatCtxSP *contextP
)
{
  if (!*contextP)
    return;
  MqBufferDelete (&(*contextP)->name);
  MqSysFree (*contextP);
}

/// \brief log the data of a StatCtxS object to stderr
/// \sta_context
void
StatCtxPrint (
  StatCtxSP context
)
{
  MqDLogX (context->mqctx, "statistics",
           0, "%30s: %10.2f\n", context->name->data, context->val);
}

/// \brief update #StatCtxS statistical data using #StatTimerS timer object
/// \sta_context
/// \sta_timer
void
StatCtxCalc (
  StatCtxSP context,
  StatTimerSP const timer
)
{
  register MQ_INT dsec;
  register MQ_INT dusec;
  struct mq_timeval tv;
  struct mq_timezone tz;
  MQ_DBL count, count1;

  if (!timer->tv.tv_sec) return;

  SysGetTimeOfDay (NULL, &tv, &tz);

  dsec = tv.tv_sec - timer->tv.tv_sec;
  dusec = tv.tv_usec - timer->tv.tv_usec;

  if (dusec < 0) {
    dsec--;
    dusec += 1000000;
  }
  dusec += dsec * 1000000;

  count = context->count;
  count1 = count + 1;

  // 1. skip first value
  if (context->count == -1) {
    context->count = 0;
    return;
  }

  context->val = ((context->val * count + dusec) / count1);
  context->count++;
}

/// \brief log a statistic timer object StatTimerS to stderr
/// \prefix
/// \sta_timer
void
StatLog (
  const char *prefix,
  StatTimerSP const timer
)
{
  register struct MqS * const mqctx = timer->mqctx;
  MqDLogX (mqctx, prefix, 0, ">>> StatTimerS\n");
  MqDLogX (mqctx, prefix, 0, "tv_sec         = %li\n", (long) timer->tv.tv_sec);
  MqDLogX (mqctx, prefix, 0, "tv_usec        = %li\n", (long) timer->tv.tv_usec);
  MqDLogX (mqctx, prefix, 0, "tz_minuteswest = %i\n",  timer->tz.tz_minuteswest);
  MqDLogX (mqctx, prefix, 0, "tz_dsttime     = %i\n",  timer->tz.tz_dsttime);
  MqDLogX (mqctx, prefix, 0, "<<< StatTimerS\n");
}

/** \} stat_api */


