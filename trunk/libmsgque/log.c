/**
 *  \file       libmsgque/log.c
 *  \brief      \$Id: log.c 510 2009-11-29 11:24:06Z dev1usr $
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 510 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "main.h"
#include "log.h"
#include "buffer.h"
#include "read.h"
#include "error.h"
#include "sys.h"
#include "token.h"

#if defined(_MSC_VER)
// need for "getpid"
#  include <process.h>
#endif

#define SEEK(ptr,pos) ptr->cur.B = (ptr->data + pos); *(ptr->cur.C-1) = BUFFER_CHAR;

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                               log_basic                                   */
/*                                                                           */
/*****************************************************************************/

void
pLogHEX (
  struct MqS const * const context,
  MQ_CST const func,
  MQ_BINB const * data,
  MQ_SIZE const len
)
{  
# define COL 8
  MQ_BINB const * const dataE = (data + len);
  MQ_BINB const * dataL;
  MQ_INT pos=0;
  MQ_BINB as[COL+1], *asP;

  MqDLogX(context,func,0,"START hex log >>>>>>>>>>>>>\n");
  for (;data<dataE;) {
    // init "as"
    dataL = as + COL;
    for (asP=as;asP<dataL;asP++) *asP='.';
    *asP = '\0';
    // print data
    dataL = data + COL;
    if (dataL > dataE) dataL = dataE;
    for (asP=as;data<dataL;data++,pos++,asP++) {
      fprintf(stderr,"[%3d:%2x] ", pos, *data);
      *asP = (isprint(*data) ? *data : (*data == 0 ? '+' : '.'));
    }
    fprintf(stderr,"= [%s]\n", as);
  }
  MqDLogX(context,func,0,"END hex log >>>>>>>>>>>>>\n");
# undef COL
}

#define MQ_USE_DATE_TIME_LOGGING

#if defined(MQ_USE_DATE_TIME_LOGGING)
static MQ_CST 
sLogTime (
  char * TIME
)
{
  struct tm *t;
  struct mq_timeval tv;
  time_t time;

  SysGetTimeOfDay (NULL, &tv, NULL);
  time = tv.tv_sec;
  t = localtime (&time);
  sprintf (TIME, "[%4d-%02d-%02d:%02d-%02d-%02d]", t->tm_year + 1900,
           t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
  TIME[21] = '\0';

  return TIME;
}
#else
#   define sLogTime(dummy) ""
#endif

void
MqLog (
  FILE * channel,
  MQ_CST const fmt,
  ...
)
{
  va_list ap;
  if (channel == NULL) return;
  va_start (ap, fmt);
  vfprintf (channel, fmt, ap);
  fflush (channel);
  va_end (ap);
}

static void
sLogVL (
  struct MqS const * const context,
  MQ_CST const proc,
  MQ_INT level,
  MQ_CST const fmt,
  va_list ap,
  FILE *channel
) {
  char time_buf[50];

  if (channel == NULL) return;
  if (MQ_ERROR_IS_POINTER(context)) {
    MQ_STR name = context->config.name;
    if (context->config.isSilent) return;
    if (MQ_IS_SERVER (context))
      fprintf (channel, "%c> (%s:%i) ", (MQ_IS_CHILD (context) ? 's' : 'S'), name ,mq_getpid());
    else
      fprintf (channel, "%c> (%s:%i) ", (MQ_IS_CHILD (context) ? 'c' : 'C'), name ,mq_getpid());
    if (proc) fprintf (channel, "%s [%i-%i-%p-%s]: ",
             sLogTime (time_buf), level, context->link.ctxId, context, proc);
  } else {
    if (proc) {
      fprintf (channel, "X> %s [%i-%i-%p-%s]: ",
		 sLogTime (time_buf), level, 0, (void*)NULL, proc);
    }
  }

  vfprintf (channel, fmt, ap);
  fflush (channel);
}

void
MqDLogX (
  struct MqS const * const context, 
  MQ_CST const proc,
  MQ_INT level,
  MQ_CST const fmt,
  ...
)
{
  va_list ap;
  if (MQ_ERROR_IS_POINTER(context) && level > context->config.debug) return;
  va_start (ap, fmt);
  sLogVL (context, proc, level, fmt, ap, stderr);
  va_end (ap);
}

void
MqDLogEVL (
  struct MqS const * const context,
  MQ_CST const proc,
  MQ_CST const fmt,
  va_list ap
)
{
  sLogVL (context, proc, 0, fmt, ap, stderr);
}

void
MqDLogEV (
  struct MqS const * const context,
  MQ_CST const proc,
  MQ_CST const fmt,
  ...
)
{
  va_list ap;
  va_start (ap, fmt);
  MqDLogEVL (context, proc, fmt, ap);
  va_end (ap);
}

// ##########################################################################

static void
sLogDynItem (
  struct MqS * const context,
  struct MqBufferS * const dyn,
  MQ_CST prefix,
  const MQ_INT level,
  struct MqBufferS * space
);

static void
sLogDynList (
  struct MqS * const context,
  struct MqBufferS * const dyn,
  MQ_CST prefix,
  const MQ_INT level,
  struct MqBufferS * space
) {
  // init
  MqBufferPush (space, "   ");

  // read numItems
  dyn->type = MQ_STRING_TYPE(context->config.isString);
  dyn->cur.B = dyn->data;
  dyn->numItems = (dyn->type==MQ_BINT ? MqBufU2INT(dyn->cur) : str2int(dyn->cur.C,NULL,16));
  dyn->cur.B += (HDR_INT_LEN + 1);

  // start-msg
  if (likely (!context->config.isSilent))
    MqLog (stderr, "numItems<" MQ_FORMAT_Z ">\n", dyn->numItems);

  // print item
  sLogDynItem (context, dyn, prefix, level, space);

  // cleanup readRef
  MqBufferPop (space, "   ");

  return;
}

static void
sLogRET (
  struct MqS * const context,
  struct MqBufferS * const dyn,
  MQ_CST prefix,
  const MQ_INT level,
  struct MqBufferS * space
)
{
  MQ_INT retNum;

  // set binary or string
  dyn->type = MQ_STRING_TYPE(context->config.isString);

  // read numItems
  SEEK (dyn, 0);
  retNum = (dyn->type==MQ_BINT ? MqBufU2INT(dyn->cur) : str2int(dyn->cur.C,NULL,16));
  SEEK (dyn, RET_NumItems_S);
  dyn->numItems = (dyn->type==MQ_BINT ? MqBufU2INT(dyn->cur) : str2int(dyn->cur.C,NULL,16));
  SEEK (dyn, RET_SIZE);

  // Message
  if (likely (!context->config.isSilent))
      MqLog (stderr, "code<%c> retNum<%i> numItems<" MQ_FORMAT_Z ">\n", 
	  pReadGetReturnCode(context), retNum, dyn->numItems);

  // print item's
  MqBufferPush (space, "   ");
  sLogDynItem (context, dyn, prefix, level, space);
  MqBufferPop (space, "   ");

  return;
}

static void
sLogDynItem (
  struct MqS * const context,
  struct MqBufferS * const dyn,
  MQ_CST prefix,
  const MQ_INT level,
  struct MqBufferS * space
)
{
  struct MqBufferS * hd = pBufferCreateRef (dyn);
  register enum MqTypeE type;

  while (dyn->numItems) {
    MqErrorCheck (pReadWord (context, dyn, hd));
    type = hd->type;
    MqDLogX (context, prefix, level, "%s%7i : %s : ", space->data, 
	hd->cursize, MqLogTypeName(hd->type));
    if (context->config.isSilent)
      continue;
    switch (type) {
      case MQ_STRT:
        MqLog (stderr, "%s\n", hd->data);
        break;
      case MQ_BYTT:
        MqLog (stderr, MQ_FORMAT_Y "\n", MqBufU2BYT(hd->cur));
        break;
      case MQ_BOLT:
        MqLog (stderr, MQ_FORMAT_O "\n", MqBufU2BOL(hd->cur));
        break;
      case MQ_SRTT:
        MqLog (stderr, MQ_FORMAT_S "\n", MqBufU2SRT(hd->cur));
        break;
      case MQ_INTT:
        MqLog (stderr, MQ_FORMAT_I "\n", MqBufU2INT(hd->cur));
        break;
      case MQ_FLTT:
        MqLog (stderr, MQ_FORMAT_F "\n", MqBufU2FLT(hd->cur));
        break;
      case MQ_WIDT:
        MqLog (stderr, MQ_FORMAT_W "\n", MqBufU2WID(hd->cur));
        break;
      case MQ_DBLT:
        MqLog (stderr, MQ_FORMAT_D "\n", MqBufU2DBL(hd->cur));
        break;
      case MQ_LSTT:
        MqLog (stderr, ">>>> ");
        sLogDynList (context, hd, prefix, level, space);
        break;
      case MQ_RETT:
        MqLog (stderr, ">>>> ");
        sLogRET (context, hd, prefix, level, space);
        break;
      case MQ_BINT: 
        MqLog (stderr, "?binary?\n");
        break;
    }
  }

  MqBufferDelete (&hd);

  return;

error:
  MqPanicC (context, __func__, -1, "logging not possible");
}

// ##########################################################################

void
pLogHDR (
  struct MqS * const context,
  MQ_CST prefix,
  const MQ_INT level,
  struct MqBufferS * const buf
)
{
  struct MqBufferS * hd = MqBufferDup (buf);
  register MQ_STR t;
  register MQ_STR e;

  if (likely(context->config.isString == MQ_NO)) {
    SEEK (hd, HDR_CtxId_S);
    sprintf (hd->cur.C, MQ_FORMAT_XI (HDR_INT_LEN), MqBufU2INT(hd->cur));
    SEEK (hd, HDR_BdySize_S);
    sprintf (hd->cur.C, MQ_FORMAT_XI (HDR_INT_LEN), MqBufU2INT(hd->cur));
    SEEK (hd, HDR_Token_S);
  }

  SEEK (hd, HDR_Trans_S);
  mq_snprintf (hd->cur.C, 4, "%d", hd->cur.A->I);
  hd->data[HDR_Code_S-1] = BUFFER_CHAR;
  hd->data[HDR_SIZE] = '\0';

  // fix \0
  t = (MQ_STR) hd->data;
  e = (MQ_STR) (hd->data + buf->cursize);
  while (t < e) {
    if (!isprint (*t)) *t = (*t == 0 ? '+' : '?');
    t++;
  }

  MqDLogX (context, prefix, level, "HEADER <%s> ...\n", hd->data);

  MqBufferDelete (&hd);
}

/*****************************************************************************/
/*                                                                           */
/*                               log_body                                    */
/*                                                                           */
/*****************************************************************************/

void
pLogBDY (
  struct MqS * const context,
  MQ_CST prefix,
  const MQ_INT level,
  struct MqBufferS * const buf
)
{
  struct MqBufferS * hd;
  struct MqBufferS * space;

	// no SYSTEM RETR body
  if (!strcmp (pTokenGetCurrent (context->link.srvT), "_SRT"))
    return;

  // init
  hd = MqBufferDup (buf);
  space = MqBufferCreate (MQ_ERROR_PANIC, 100);
  hd->cur.B = hd->data + BDY_SIZE;
  hd->type = MQ_STRING_TYPE(context->config.isString);

  // start-msg
  MqDLogX (context, prefix, level, ">>>>> BDY size<" MQ_FORMAT_Z "> numItems<" MQ_FORMAT_Z ">\n",
           hd->cursize - BDY_SIZE, hd->numItems);

  // items
  sLogDynItem (context, hd, prefix, level, space);

  // end-msg
  MqDLogX (context, prefix, level, "<<<<< BDY\n");

  // cleanup readRef
  MqBufferDelete (&hd);
  MqBufferDelete (&space);

  return;
}

/*****************************************************************************/
/*                                                                           */
/*                               log_advanced                                */
/*                                                                           */
/*****************************************************************************/

#if defined _DEBUG
void
pLogMqBuffer (
  struct MqS * const context,
  MQ_CST header,
  MQ_INT level,
  struct MqBufferS * buf
)
{
  MqDLogX (context, header, level,
      "pointer<%p> type<%c>, alloc<%s>, size<" MQ_FORMAT_Z ">, cursize<" MQ_FORMAT_Z ">, data<%s>\n",
      buf, buf->type,
      (buf->alloc == MQ_ALLOC_DYNAMIC	? "DYNAMIC"   : "STATIC"), 
      buf->size, buf->cursize, buf->data);
}
#endif

MQ_CST 
MqLogErrorCode (
  enum MqErrorE code
)
{
  switch (code) {
    case MQ_OK:		return "OK";
    case MQ_ERROR:	return "ERROR";
    case MQ_CONTINUE:	return "CONTINUE";
    case MQ_EXIT:	return "EXIT";
  }
  return "unknown";
}

MQ_CST 
MqLogTypeName (
  const enum MqTypeE type
)
{
  switch (type) {
    case MQ_STRT: return "STR";
    case MQ_BINT: return "BIN";
    case MQ_BYTT: return "BYT";
    case MQ_BOLT: return "BOL";
    case MQ_SRTT: return "SRT";
    case MQ_INTT: return "INT";
    case MQ_FLTT: return "FLT";
    case MQ_WIDT: return "WID";
    case MQ_DBLT: return "DBL";
    case MQ_LSTT: return "LST";
    case MQ_RETT: return "RET";
  }
  return "UNKNOWN";
}

MQ_CST
MqLogServerOrClient (
  struct MqS const * const context
)
{
  return (MQ_IS_SERVER (context) ? "SERVER" : "CLIENT");
}

MQ_CST
MqLogParentOrChild (
  struct MqS const * const context
)
{
  return (MQ_IS_PARENT (context) ? "PARENT" : "CHILD");
}

MQ_STR
MqLogC (
  MQ_STR buf,
  MQ_STR str,
  MQ_SIZE size
)
{
  register MQ_STR g = str;
  register MQ_STR t = buf;
  register MQ_STR e = buf + size;
  for (;t < e; t++, g++) {
    *t = (isprint (*g) ? *g : (*g == 0 ? '+' :'?'));
  }
  *t = '\0';

  return buf;
}

END_C_DECLS

