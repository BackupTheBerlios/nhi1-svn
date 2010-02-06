/**
 *  \file       theLink/libmsgque/log.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
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

  MqSysGetTimeOfDay (NULL, &tv, NULL);
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
  MQ_CST proc,
  MQ_INT level,
  MQ_CST const fmt,
  va_list ap,
  FILE *channel
) {
  char	time_buf[50];
  char	header[400];

  if (channel == NULL) return;
  if (proc == NULL) proc = "UNKNOWN";
  if (MQ_ERROR_IS_POINTER(context)) {
    char t;
    MQ_STR name = context->config.name;
    if (context->config.isSilent) return;
    t = (MQ_IS_SERVER (context) ? (MQ_IS_CHILD (context) ? 's' : 'S') : (MQ_IS_CHILD (context) ? 'c' : 'C'));
    snprintf (header, 400, "%c> (%s:%i) %s [%i-%i-%p-%s]: %s", t, name, mq_getpid(),
	sLogTime (time_buf), level, context->link.ctxId, (void*) context, proc, fmt);
  } else {
    snprintf (header, 400, "X> %s [%i-%i-%p-%s]: %s", sLogTime (time_buf), level, 0, (void*)NULL, proc, fmt);
  }

  vfprintf (channel, header, ap);
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
)
{
  if (context->config.isSilent) {
    return;
  } else {
    struct MqBufferS * hd = pBufferCreateRef (dyn);
    int num, size=400;
    char buf[400];
    char *ptr;


    while (dyn->numItems) {
      ptr = buf;
      MqErrorCheck (pReadWord (context, dyn, hd));

      num  =  snprintf(ptr, size, "%s%7i : %s : ", space->data, hd->cursize, MqLogTypeName(hd->type));
      ptr  += num;
      size -= num;

      switch (hd->type) {
	case MQ_STRT:
	  snprintf(ptr, size, MQ_FORMAT_C, hd->cur.C);
	  break;
	case MQ_BYTT:
	  snprintf(ptr, size, MQ_FORMAT_Y, MqBufU2BYT(hd->cur));
	  break;
	case MQ_BOLT:
	  snprintf(ptr, size, MQ_FORMAT_O, MqBufU2BOL(hd->cur));
	  break;
	case MQ_SRTT:
	  snprintf(ptr, size, MQ_FORMAT_S, MqBufU2SRT(hd->cur));
	  break;
	case MQ_INTT:
	  snprintf(ptr, size, MQ_FORMAT_I, MqBufU2INT(hd->cur));
	  break;
	case MQ_FLTT:
	  snprintf(ptr, size, MQ_FORMAT_F, MqBufU2FLT(hd->cur));
	  break;
	case MQ_WIDT:
	  snprintf(ptr, size, MQ_FORMAT_W, MqBufU2WID(hd->cur));
	  break;
	case MQ_DBLT:
	  snprintf(ptr, size, MQ_FORMAT_D, MqBufU2DBL(hd->cur));
	  break;
	case MQ_LSTT:
	  snprintf(ptr, size, ">>>> ");
	  break;
	case MQ_RETT:
	  snprintf(ptr, size, ">>>> code<%c>", pReadGetReturnCode(context));
	  break;
	case MQ_BINT: 
	  snprintf(ptr, size, "%s", "?binary?");
	  break;
      }
      
      switch (hd->type) {
	case MQ_LSTT:
	case MQ_RETT:
	  // set binary or string
	  hd->type = MQ_STRING_TYPE(context->config.isString);

	  // read numItems
	  hd->cur.B = hd->data;
	  hd->numItems = (hd->type==MQ_BINT ? MqBufU2INT(hd->cur) : str2int(hd->cur.C,NULL,16));
	  hd->cur.B += (HDR_INT_LEN + 1);

	  // start-msg
	  MqDLogX (context, prefix, level, "%s, numItems<" MQ_FORMAT_Z ">\n", buf, hd->numItems);

	  // print item-list
	  MqBufferPush	(space, "   ");
	  sLogDynItem	(context, hd, prefix, level, space);
	  MqBufferPop	(space, "   ");
	  break;
	default:
	  MqDLogX (context, prefix, level, "%s\n", buf);
	  break;
      }
    }

    MqBufferDelete (&hd);

    return;

error:
    MqPanicC (context, __func__, -1, "logging not possible");
  }
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
      (void*) buf, buf->type,
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

