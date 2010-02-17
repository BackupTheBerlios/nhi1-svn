/**
 *  \file       theLink/libmsgque/error.c
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

#include "error.h"
#include "buffer.h"
#include "send.h"
#include "log.h"
#include "sys.h"
#include "mq_io.h"

#define MQ_CONTEXT_S context

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                              error_init                                   */
/*                                                                           */
/*****************************************************************************/

void
pErrorReset (
  struct MqS * const context
)
{
  context->error.code = MQ_OK;
  context->error.num = EXIT_SUCCESS;
  if (context->error.text == NULL) {
    context->error.text = MqBufferCreate (MQ_ERROR_PANIC, 1000);
  } else {
    MqBufferReset (context->error.text);
  }
  context->error.append = MQ_YES;
  context->error.errctx = context;
}

void
pErrorCleanup (
  struct MqS * const context
)
{
  MqBufferDelete (&context->error.text);
}

/*****************************************************************************/
/*                                                                           */
/*                              error_api                                    */
/*                                                                           */
/*****************************************************************************/

void
MqPanicVL (
  struct MqS * const context,
  MQ_CST const prefix,
  MQ_INT const errnum,
  MQ_CST const fmt,
  va_list ap
)
{
  // try to report a 'panic' to link-target
  if (MQ_ERROR_IS_POINTER(context)) {
    struct MqS * const parent = pMqGetFirstParent(context);
    MqErrorSGenVL (context, "PANIC", MQ_ERROR, errnum, fmt, ap);
    if (prefix) MqErrorSAppendV (context, "found in function \"%s\"", prefix);
    MqErrorCopy(parent, context);
    pErrorReport(parent);
  }
  // always report a 'panic' local
  {
    struct MqBufferS * buf = MqBufferCreate (MQ_ERROR_PANIC, 255);
    MqBufferSetV (buf, "PANIC: %s\n", fmt);
    MqLogVL (MQ_ERROR_IS_POINTER(context) ? context : NULL, prefix, 0, buf->cur.C, ap);
    MqBufferDelete (&buf);
  }
  SysAbort();
}

void
MqPanicV (
  struct MqS * const context,
  MQ_CST const prefix,
  MQ_INT const errnum,
  MQ_CST const fmt,
  ...
)
{
  va_list ap;
  va_start (ap, fmt);
  MqPanicVL (context, prefix, errnum, fmt, ap);
  va_end (ap);
}

/*****************************************************************************/
/*                                                                           */
/*                            error_create                                   */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
pErrorSet (
  struct MqS * const context,
  MQ_INT const errnum,
  MQ_CST const errtext,
  MQ_BOL const append
)
{
  context->error.code = MQ_ERROR;
  context->error.num = (int) (errnum > 0 ? errnum : EXIT_FAILURE);
  context->error.append = append;
  MqBufferReset (context->error.text);
  MqBufferAppendC (context->error.text, errtext);
  if (context->config.master != NULL)
    pErrorSync(context->config.master, context);
  return MQ_ERROR;
}

enum MqErrorE
MqErrorSGenVL (
  struct MqS * const context,
  MQ_CST const prefix,
  enum MqErrorE const errcode,
  MQ_INT const errnum,
  MQ_CST const fmt,
  va_list ap
)
{
  if (context == MQ_ERROR_IGNORE) {
    // do nothing
  } else if (context == MQ_ERROR_PANIC) {
    struct MqBufferS * buf = MqBufferCreate (MQ_ERROR_PANIC, 255);
    MqBufferSetV (buf, "#(" MQ_FORMAT_I ") -> %s\n", errnum, fmt);
    if (errcode == MQ_ERROR) {
      MqPanicVL (MQ_ERROR_PANIC, prefix, errnum, (MQ_STR) buf->data, ap);
    } else {
      MqLogVL (MQ_ERROR_PANIC, prefix, 0, (MQ_STR) buf->data, ap);
    }
    MqBufferDelete (&buf);
  } else if (context == MQ_ERROR_PRINT) {
    struct MqBufferS * buf = MqBufferCreate (MQ_ERROR_PANIC, 255);
    MqBufferSetV (buf, "#(" MQ_FORMAT_I ") -> %s\n", errnum, fmt);
    MqLogVL (MQ_ERROR_PANIC, prefix, 0, (MQ_STR) buf->data, ap);
    MqBufferDelete (&buf);
  } else {
    struct MqBufferS * const text = context->error.text;
    context->error.code = errcode;
    context->error.num = (errnum > 0 ? errnum : EXIT_FAILURE);
    context->error.errctx = context;
    MqBufferReset (text);
    if (prefix) {
      if (context) {
	MqBufferAppendV (text, "%c> (%s) ", ( MQ_IS_SERVER (context) ?
	  (MQ_IS_CHILD (context) ? 's' : 'S') : (MQ_IS_CHILD (context) ? 'c' : 'C')),
	  context->config.name ? context->config.name : "unknown"); 
      }
      MqBufferAppendV (text, "[%s] ", prefix);
    }
    MqBufferAppendVL (text, fmt, ap);
    if (context->config.master != NULL) {
      pErrorSync(context->config.master, context);
    }
  }
  return errcode;
}

enum MqErrorE
MqErrorSGenV (
  struct MqS * const context,
  MQ_CST const prefix,
  enum MqErrorE const errcode,
  MQ_INT const errnum,
  MQ_CST const fmt,
  ...
)
{
  va_list ap;
  if (context != MQ_ERROR_IGNORE) {
    va_start (ap, fmt);
    MqErrorSGenVL (context, prefix, errcode, errnum, fmt, ap);
    va_end (ap);
  }
  return errcode;
}

enum MqErrorE
MqErrorC (
  struct MqS * const context,
  MQ_CST const prefix,
  MQ_INT const errnum,
  MQ_CST const message
)
{
  return MqErrorSGenV(context, prefix, MQ_ERROR, errnum, "%s", message);
}

// no because "context" - code is allways slow
enum MqErrorE
MqErrorStackP(
  struct MqS * const context,
  MQ_CST const func,
  MQ_CST const file
)
{
  if (MQ_ERROR_IS_POINTER(context)) {
    if (MqErrorCheckI(context->error.code)) {
      MQ_STR basename = MqSysBasename(file, MQ_YES);
      MQ_CST type = context->error.num == MqMessageNum(MQ_ERROR_EXIT) ? "EXIT" : "ERROR";
      if (unlikely(context->config.debug >= 5)) 
	MqLogV (context, func, 5, "detect %s in file '%s'\n", type, basename);
      MqErrorSAppendV(context, "found in function \"%s\" at file \"%s\"", func, basename);
      free(basename);
    }
    return context->error.code;
  } else {
    return MQ_ERROR;
  }
}

enum MqErrorE
MqErrorSAppendV (
  struct MqS const * const context,
  MQ_CST const fmt,
  ...
)
{
  va_list ap;
  if (context == MQ_ERROR_IGNORE) return MQ_ERROR;
  va_start (ap, fmt);
  if (context) {
    struct MqBufferS * text = context->error.text;
    struct MqS * const master = context->config.master;
    MQ_CST name = context->config.name ? context->config.name : "unknown";
    if (!context->error.append) return context->error.code;
    MqBufferAppendV (text, "\n%c> (%s) ", ( MQ_IS_SERVER (context) ?
			(MQ_IS_CHILD (context) ? 's' : 'S') : (MQ_IS_CHILD (context) ? 'c' : 'C')), name); 
    if (master != NULL)
      MqBufferAppendV (master->error.text, "\n%c> (%s) ", ( MQ_IS_SERVER (context) ?
			  (MQ_IS_CHILD (context) ? 's' : 'S') : (MQ_IS_CHILD (context) ? 'c' : 'C')), name); 
    MqBufferAppendVL (text, fmt, ap);
    if (master != NULL)
      MqBufferAppendVL (master->error.text, fmt, ap);
    return context->error.code;
  } else {
    MqLogVL (MQ_ERROR_PANIC, __func__, 0, fmt, ap);
    pLog (stderr, "\n");
  }
  va_end (ap);
  return MQ_ERROR;
}

void
pErrorAppendC (
  struct MqS * const context,
  MQ_CST const msg
)
{
  struct MqS * const master = context->config.master;
  MqBufferAppendC (context->error.text, msg);
  if (master != NULL)
    MqBufferAppendC (master->error.text, msg);
}

/*****************************************************************************/
/*                                                                           */
/*                              context_misc                                   */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
MqErrorGetCode (
  struct MqS const * const context
)
{
  return MqErrorGetCodeI(context);
}

enum MqErrorE
MqErrorSetCode (
  struct MqS * const context,
  enum MqErrorE code
)
{
  return (context->error.code = code);
}

MQ_CST
MqErrorGetText (
  struct MqS const * const context
)
{
  return (MQ_STR) context->error.text->data;
}

/// error-reset with master cleanup
enum MqErrorE
MqErrorReset (
  struct MqS * const context
)
{
  switch (context->error.code) {
    case MQ_OK: 
      return MQ_OK;
    case MQ_CONTINUE: 
      return context->error.code = MQ_OK;
    default:
      pErrorReset(context);
      if (context->config.master != NULL)
	MqErrorReset(context->config.master);
      return MQ_OK;
  }
}

MQ_INT
MqErrorGetNum (
  struct MqS const * const context
)
{
  return MqErrorGetNumI(context);
}

void
MqErrorPrint (
  struct MqS * const context
)
{
  fprintf(stderr, "BACKGROUND ERROR: %s\n", (MQ_STR) context->error.text->data);
  fflush(stderr);
  MqErrorReset(context);
}

enum MqErrorE
MqErrorSet (
  struct MqS * const context,
  MQ_INT num,
  enum MqErrorE code,
  MQ_CST const message,
  struct MqS *const errctx
)
{
  context->error.num = num;
  context->error.code = code;
  context->error.append = MQ_YES;
  context->error.errctx = errctx == NULL ? context : errctx;
  MqBufferSetC(context->error.text, message);
  if (context->config.master != NULL)
    pErrorSync(context->config.master, context);
  return code;
}

enum MqErrorE
MqErrorSetCONTINUE (
  struct MqS * const context
)
{
  MqDLogC(context, 5, "done\n");
  return context->error.code = MQ_CONTINUE;
}

enum MqErrorE
MqErrorCreateEXITP (
  struct MqS * const context,
  MQ_CST const prefix
)
{
  if (context->setup.ignoreExit == MQ_YES) {
    MqDLogV(context, 3, "%s - ignore EXIT\n", prefix);
    return MQ_CONTINUE;
  } else {
    MqDLogV(context, 3, "called from %s\n", prefix);
    return MqErrorSGenV(context,prefix,MQ_ERROR,MqMessageNum(MQ_ERROR_EXIT),MqMessageText[MQ_ERROR_EXIT]);
  }
}

int
MqErrorIsEXIT (
  struct MqS * const context
)
{
  return (MqMessageNum(MQ_ERROR_EXIT) == context->error.num);
};

enum MqErrorE MqErrorDeleteEXIT (
  struct MqS * const context
)
{
  return MqErrorReset(context);
}

void
pErrorSync (
  struct MqS * const out,
  struct MqS * const in
)
{
  if (unlikely(in == out)) return;
  MqBufferCopy (out->error.text, in->error.text);
  out->error.code = in->error.code;
  out->error.num = in->error.num;
  out->error.append = in->error.append;
  out->error.errctx = in->error.errctx;
}

enum MqErrorE
MqErrorCopy (
  struct MqS * const out,
  struct MqS * const in
)
{
  if (out != in) {
    if (in->error.code == MQ_OK) {
      MqErrorReset(out);
      return MQ_OK;
    } else if (out != in->config.master) {
      pErrorSync (out, in);
      if (out->config.master != NULL)
	pErrorSync(out->config.master, out);
      MqErrorReset(in);
    }
  }

  return out->error.code;
}

// report parent context
void
pErrorReport(
  struct MqS * const context
)
{
  if (!MQ_ERROR_IS_POINTER(context)) return;
  if (MQ_IS_PARENT (context) && 
	context->error.code == MQ_ERROR && 
	  context->error.num != MqMessageNum(MQ_ERROR_EXIT)) {
    if (MQ_IS_SERVER (context) && pIoCheck (context->link.io)) {
      // save the original context
      MQ_BUF err = MqBufferDup (context->error.text);
      // send the context to the client
      if (MqSendERROR (context) == MQ_ERROR) {
	if (context->config.isSilent == MQ_NO && context->config.master == NULL) {
	  pLog (stderr, "%s\n", (MQ_STR) err->data);
	}
      }
      MqBufferDelete (&err);
    } else if (context->config.isSilent == MQ_NO && context->config.master == NULL) {
      pLog (stderr, "%s\n", (MQ_STR) context->error.text->data);
    }
  }
  pErrorReset(context);
}

/*****************************************************************************/
/*                                                                           */
/*                                debugging                                  */
/*                                                                           */
/*****************************************************************************/

#ifdef _DEBUG
void
MqErrorLog (
  struct MqS const * const context,
  MQ_CST const prefix
)
{
  MqLogV (context, prefix, 0, ">>>> MqErrorS (%p)\n", (void*) context);
  MqLogV (context, prefix, 0, "status   = <%s>\n", MqLogErrorCode (context->error.code));
  MqLogV (context, prefix, 0, "num      = <%i>\n", context->error.num);
  MqBufferLog (context, context->error.text, "context->error.text");
  MqLogV (context, prefix, 0, "<<<< MqErrorS\n");
}
#endif /* _DEBUG */

END_C_DECLS





