/**
 *  \file       theLink/tests/server.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "msgque.h"
#include "debug.h"

/** \ingroup validation
 *  \defgroup server server
 *  \{
 *  \brief server \server_desc
 *
 *  \verbinclude server.help
 */

/// link to the MqErrorS object
#define SRVCTX ((struct ServerCtxS*const)mqctx)
#define MQCTX(ctx) ((struct MqS*const)ctx)
#define CLIENTCTX(ctx) ((struct ClientCtxS*const)ctx)
#define MQ_CONTEXT_S mqctx

/// \brief the local \b context of the \ref server tool
/// \mqctx
struct ServerCtxS {
  struct MqS	mqctx;	///< \mqctxI
  MQ_BUF	val;	///< context specific data
  struct MqS	*cl[3];	///< \e MqSend* test client
  MQ_INT	i;	///< callback data
  MQ_INT	j;	///< callback data
};

/// \brief the local child \b context used in the Callback test
/// \mqctx
struct ClientCtxS {
  struct MqS  mqctx;		///< \mqctxI
  MQ_INT      i;		///< Callback server context
};

/*****************************************************************************/
/*                                                                           */
/*                               Request Handler                             */
/*                                                                           */
/*****************************************************************************/

/// \brief display help using \b -h or \b --help command-line option
/// \param base the executable usually: <tt>basename(argv[0])</tt>
static void __attribute__ ((noreturn))
ServerHelp (const char * base)
{
    fprintf(stderr, "usage: %s [OPTION]... [ARGUMENT]...\n", base);
    fputs("\n", stderr);
    fputs("  This tool is the server part of the performance test toolkit and expect\n", stderr);
    fputs("  packages from the libmsgque performance test and validation client.\n", stderr);
    fputs("\n", stderr);
    fprintf(stderr, "  %s [ARGUMENT]... syntax:\n", base);
    fprintf(stderr, "    aclient [OPTION]... %c %s [OPTION]... [ARGUMENT]\n", MQ_ALFA, base);
    fputs("\n", stderr);
    fputs(MqHelp (NULL), stderr);
    fputs("\n", stderr);
    fprintf(stderr,"  %s [OPTION]:\n", base);
    fputs("    -h, --help       print this help\n", stderr);
    fputs("\n", stderr);

    exit(EXIT_SUCCESS);
}

/// \brief test a non recoverable mqctx (panic)
/// \service
static enum MqErrorE
Ot_ERR1 (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MqPanicC (mqctx, __func__, -1, "this is a non recoverable error");

  // this line will never reached
  return MQ_ERROR;
}

/// \brief test a recoverable error using an error return package
/// \service
static enum MqErrorE
Ot_ERR2 (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MqSendSTART (mqctx);
  MqSendC (mqctx, "some data");
  MqErrorC (mqctx, __func__, 10, "some error");

  return MqSendRETURN (mqctx);
}

/// \brief double return message
/// \service
static enum MqErrorE
Ot_ERR3 (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MqSendSTART (mqctx);
  // double MqSendRETURN
  MqSendRETURN (mqctx);

  return MqSendRETURN (mqctx);
}

/// \brief exit server
/// \service
static enum MqErrorE
Ot_ERR4 (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  exit (1);
}

/// \brief test MqSendERROR
/// \service
static enum MqErrorE
Ot_ERRT (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_CST err;
  MqReadC (mqctx, &err);
  MqErrorC (mqctx, "MYERR", 9 , err);
  return MqSendERROR (mqctx);
}

/// \brief echo a MqBufferS object
/// \service
static enum MqErrorE
Ot_ECOU (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  struct MqBufferS *buf;
  MqSendSTART (mqctx);
  MqErrorCheck (MqReadU (mqctx, &buf));

  MqSendU (mqctx, buf);

error:
  return MqSendRETURN (mqctx);
}

/// \brief echo the server-name
/// \service
static enum MqErrorE
Ot_ECON (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_CST str;
  MqSendSTART (mqctx);
  MqErrorCheck (MqReadC (mqctx, &str));
  MqBufferSetV(mqctx->temp,"%s-%s", str, mqctx->config.name);
  MqSendC (mqctx, mqctx->temp->cur.C);

error:
  return MqSendRETURN (mqctx);
}

// START-C-SERVICE-PROC
/// \brief echo a #MQ_INT object
/// \service
static enum MqErrorE
Ot_ECOI (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_INT l;
  MqSendSTART (mqctx);
  MqErrorCheck (MqReadI (mqctx, &l));
  MqSendI (mqctx, l);
error:
  return MqSendRETURN (mqctx);
}
// END-C-SERVICE-PROC

/// \brief echo a #MQ_SRT object
/// \service
static enum MqErrorE
Ot_ECOS (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_SRT l;
  MqSendSTART (mqctx);
  MqErrorCheck (MqReadS (mqctx, &l));
  MqSendS (mqctx, l);

error:
  return MqSendRETURN (mqctx);
}

/// \brief echo a #MQ_BYT object
/// \service
static enum MqErrorE
Ot_ECOY (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_BYT l;
  MqSendSTART (mqctx);
  MqErrorCheck (MqReadY (mqctx, &l));
  MqSendY (mqctx, l);

error:
  return MqSendRETURN (mqctx);
}

/// \brief echo a #MQ_BOL object
/// \service
static enum MqErrorE
Ot_ECOO (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_BOL l;
  MqSendSTART (mqctx);
  MqErrorCheck (MqReadO (mqctx, &l));
  MqSendO (mqctx, l);

error:
  return MqSendRETURN (mqctx);
}

/// \brief echo a #MQ_WID object
/// \service
static enum MqErrorE
Ot_ECOW (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_WID w;
  MqSendSTART (mqctx);
  MqErrorCheck (MqReadW (mqctx, &w));
  MqSendW (mqctx, w);

error:
  return MqSendRETURN (mqctx);
}

/// \brief echo a #MQ_DBL object
/// \service
static enum MqErrorE
Ot_ECOD (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_DBL d;
  MqSendSTART (mqctx);
  MqErrorCheck (MqReadD (mqctx, &d));
  MqSendD (mqctx, d);

error:
  return MqSendRETURN (mqctx);
}

/// \brief echo a #MQ_FLT object
/// \service
static enum MqErrorE
Ot_ECOF (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_FLT d;
  MqSendSTART (mqctx);
  MqErrorCheck (MqReadF (mqctx, &d));
  MqSendF (mqctx, d);

error:
  return MqSendRETURN (mqctx);
}

/// \brief echo a #MQ_STR object
/// \service
static enum MqErrorE
Ot_ECOC (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_CST str;
  MqSendSTART (mqctx);
  MqErrorCheck (MqReadC (mqctx, &str));
  MqSendC (mqctx, str);

error:
  return MqSendRETURN (mqctx);
}

/// \brief echo a #MQ_BIN object
/// \service
static enum MqErrorE
Ot_ECOB (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_BIN bin;
  MQ_SIZE len;
  MqSendSTART (mqctx);
  MqErrorCheck (MqReadB (mqctx, &bin, &len));
  MqSendB (mqctx, bin, len);

error:
  return MqSendRETURN (mqctx);
}

/// \brief helper to increment/echo data objects
/// \mqctx
/// \param incr bool for increment testing 0 = no else yes
/// \retMqErrorE
static enum MqErrorE
_ECOL_Item (
  struct MqS * const mqctx,
  const int incr
)
{
  struct MqBufferS * buf;
  MQ_INT i;
  while (MqReadGetNumItems (mqctx)) {
    MqErrorCheck (MqReadU (mqctx, &buf));
    if (buf->type == MQ_LSTT) {
      MqReadL_START (mqctx, buf);
      MqSendL_START (mqctx);
      MqErrorCheck (_ECOL_Item (mqctx, incr));
      MqSendL_END (mqctx);
      MqReadL_END (mqctx);
    } else if (incr) {
      MqErrorCheck (MqBufferGetI (buf, &i));
      MqSendI (mqctx, ++i);
    } else {
      MqSendU (mqctx, buf);
    }
  }
  return MQ_OK;

error:
  return MqErrorStack(mqctx);
}

/// \brief echo a #MQ_LST object
/// \service
static enum MqErrorE
Ot_ECLI (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_BUF buf;
  MQ_BOL doincr;

  MqErrorCheck( MqReadU(mqctx,&buf));

  doincr = (buf->type == MQ_STRT && strncmp(buf->cur.C,"--incr",6) == 0);
  if (!doincr) MqReadUndo(mqctx);
  
  MqSendSTART (mqctx);
  MqErrorCheck (_ECOL_Item (mqctx, doincr));

error:
  return MqSendRETURN (mqctx);
}

/// \brief force echo a #MQ_LST object
/// \service
static enum MqErrorE
Ot_ECOL (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MqSendSTART (mqctx);
  MqErrorCheck (MqReadL_START (mqctx, NULL));
  MqSendL_START (mqctx);
  MqErrorCheck (_ECOL_Item (mqctx, 0));
  MqSendL_END (mqctx);
  MqReadL_END (mqctx);

error:
  return MqSendRETURN (mqctx);
}

/// \brief echo a #MqBufferLS object
/// \service
static enum MqErrorE
Ot_ECUL (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_BYT y;
  MQ_SRT s;
  MQ_INT i;
  MQ_WID w;
  MQ_BUF b;

  MqErrorCheck(MqReadY(mqctx, &y));
  MqErrorCheck(MqReadS(mqctx, &s));
  MqErrorCheck(MqReadI(mqctx, &i));
  MqErrorCheck(MqReadW(mqctx, &w));
  MqErrorCheck(MqReadU(mqctx, &b));

  MqSendSTART (mqctx);

  MqSendY (mqctx, y);
  MqSendS (mqctx, s);
  MqSendI (mqctx, i);
  MqSendW (mqctx, w);
  MqSendU (mqctx, b);

error:
  return MqSendRETURN (mqctx);
}

/// \brief mqctx a #MqBufferLS object
/// \service
static enum MqErrorE
Ot_RDUL (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_BYT y;
  MQ_SRT s;
  MQ_INT i;
  MQ_WID w;
  MQ_BUF b;

  MqErrorCheck(MqReadY(mqctx, &y));
  MqErrorCheck(MqReadS(mqctx, &s));
  MqErrorCheck(MqReadI(mqctx, &i));
  MqErrorCheck(MqReadW(mqctx, &w));
  MqErrorCheck(MqReadU(mqctx, &b));

error:
  return MQ_OK;
}

/// \brief echo a variable length buffer object
/// \service
static enum MqErrorE
Ot_ECOM (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MqSendSTART(mqctx);
  while (MqReadItemExists(mqctx))
    MqReadProxy(mqctx,mqctx);
  return MqSendRETURN (mqctx);
}

/// \brief return debug, binary, and silent value
/// \service
static enum MqErrorE
Ot_MSQT (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  // Send
  MqSendSTART (mqctx);
  if (mqctx->config.debug) {
    MqSendC (mqctx, "debug");
    MqSendI (mqctx, mqctx->config.debug);
  }
  if (!mqctx->config.isString)
    MqSendC (mqctx, "binary");
  if (mqctx->config.isSilent)
    MqSendC (mqctx, "silent");
  MqSendC (mqctx, "sOc");
  MqSendC (mqctx, MqLogServerOrClient (mqctx));
  MqSendC (mqctx, "pOc");
  MqSendC (mqctx, MqLogParentOrChild (mqctx));

  return MqSendRETURN (mqctx);
}

/// \brief server sleeps n seconds
/// \service
static enum MqErrorE
Ot_SLEP (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_INT time;

  MqSendSTART (mqctx);
  MqErrorCheck (MqReadI (mqctx, &time));
  MqErrorCheck ((*MqLal.SysSleep)(mqctx, time));
  MqSendI (mqctx, time);

error:
  return MqSendRETURN (mqctx);
}

/// \brief server sleeps n microseconds
/// \service
static enum MqErrorE
Ot_USLP (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_INT time;

  MqSendSTART (mqctx);
  MqErrorCheck (MqReadI (mqctx, &time));
  MqErrorCheck (MqSysUSleep (mqctx, time));
  MqSendI (mqctx, time);

error:
  return MqSendRETURN (mqctx);
}

/// \brief test client/server interaction
/// \service
static enum MqErrorE
Ot_CSV1 (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_INT num;

  // call an other service
  //    I don't need an transactionPtr because retTok = NULL
  MqSendSTART (mqctx);
  MqErrorCheck (MqReadI (mqctx, &num));
  num++;
  MqSendI (mqctx, num);
  MqErrorCheck(MqSendEND_AND_WAIT (mqctx, "CSV2", MQ_TIMEOUT));

  // mqctx the answer and mqctx the result back
  MqSendSTART (mqctx);
  MqErrorCheck (MqReadI (mqctx, &num));
  num++;
  MqSendI (mqctx, num);

error:
  return MqSendRETURN (mqctx);
}

/// \brief test mass startup of childs
/// \service
static enum MqErrorE
Ot_GTCX (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MqSendSTART (mqctx);
  MqSendI (mqctx, MqLinkGetCtxId(mqctx));
  return MqSendRETURN (mqctx);
}

/// \brief get the current token
/// \service
static enum MqErrorE
Ot_GTTO (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MqSendSTART (mqctx);
  MqSendC (mqctx, MqServiceGetToken(mqctx));
  return MqSendRETURN (mqctx);
}

/// \brief get the MqConfigS::statusIs field value
/// \service
static enum MqErrorE
Ot_GTSI (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MqBufferReset(mqctx->temp);

  MqSendSTART (mqctx);
  if (mqctx->statusIs & MQ_STATUS_IS_DUP)
    MqBufferAppendC(mqctx->temp, "DUP-");
  if (mqctx->statusIs & MQ_STATUS_IS_THREAD)
    MqBufferAppendC(mqctx->temp, "THREAD-");
  if (mqctx->statusIs & MQ_STATUS_IS_FORK)
    MqBufferAppendC(mqctx->temp, "FORK-");
  if (mqctx->statusIs & MQ_STATUS_IS_SPAWN)
    MqBufferAppendC(mqctx->temp, "SPAWN-");
  MqSendU(mqctx, mqctx->temp);
  return MqSendRETURN (mqctx);
}

/// \brief set context specific data
/// \service
static enum MqErrorE
Ot_SETU (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  return MqReadU(mqctx, &SRVCTX->val);
}

/// \brief get context specific data
/// \service
static enum MqErrorE
Ot_GETU (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MqSendSTART (mqctx);
  MqSendU (mqctx, SRVCTX->val);
  return MqSendRETURN (mqctx);
}

/// \brief "callback" used in #Ot_SND1
/// \service
static enum MqErrorE
Callback1 (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  return MqReadI(mqctx, &CLIENTCTX(mqctx)->i);
}

/// \brief "callback" used in #Ot_SND2
/// \service
static enum MqErrorE
Callback2 (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  return MqReadI(mqctx, &CLIENTCTX(mqctx)->i);
}

/// \brief just mqctx an MQ_INT
/// \service
static enum MqErrorE
Ot_REDI (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_INT i;
  return MqReadI(mqctx, &i);
}

/// \brief test buffer #1
/// \service
static enum MqErrorE
Ot_BUF1 (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_BUF buf;
  char str[2] = { '\0', '\0' };
  MqErrorCheck(MqReadU(mqctx, &buf));
  MqSendSTART(mqctx);
  str[0] = MqBufferGetType(buf);
  MqSendC(mqctx, str);
  switch (buf->type) {
    case MQ_BYTT:
      MqSendY(mqctx, buf->cur.A->Y); break;
    case MQ_BOLT:
      MqSendO(mqctx, buf->cur.A->O); break;
    case MQ_SRTT:
      MqSendS(mqctx, buf->cur.A->S); break;
    case MQ_INTT:
      MqSendI(mqctx, buf->cur.A->I); break;
    case MQ_FLTT:
      MqSendF(mqctx, buf->cur.A->F); break;
    case MQ_WIDT:
      MqSendW(mqctx, buf->cur.A->W); break;
    case MQ_DBLT:
      MqSendD(mqctx, buf->cur.A->D); break;
    case MQ_BINT:
      MqSendB(mqctx, buf->cur.B, buf->cursize); break;
    case MQ_STRT:
      MqSendC(mqctx, buf->cur.C); break;
    case MQ_LSTT:
    case MQ_TRAT:
      break;
  }
  return MqSendRETURN(mqctx);
error:
  return MqErrorStack(mqctx);
}

/// \brief test buffer #2
/// \service
static enum MqErrorE
Ot_BUF2 (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_BUF buf;
  int i;
  char str[2] = {'\0', '\0'};
  MqSendSTART(mqctx);
  for (i=0; i<3; i++) {
    MqErrorCheck(MqReadU(mqctx, &buf));
    str[0] = MqBufferGetType(buf);
    MqSendC(mqctx, str);
    MqSendU(mqctx, buf);
  }
  return MqSendRETURN(mqctx);
error:
  return MqErrorStack(mqctx);
}

/// \brief test buffer #3
/// \service
static enum MqErrorE
Ot_BUF3 (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_BUF buf;
  MQ_INT i;
  char str[2] = {'\0', '\0'};
  MqSendSTART(mqctx);
  MqErrorCheck(MqReadU(mqctx, &buf));
  str[0] = MqBufferGetType(buf);
  MqSendC(mqctx, str);
  MqSendU(mqctx, buf);
  MqErrorCheck(MqReadI(mqctx, &i));
  MqSendI(mqctx, i);
  MqSendU(mqctx, buf);
  return MqSendRETURN(mqctx);
error:
  return MqErrorStack(mqctx);
}

/// \brief test list #1
/// \service
static enum MqErrorE
Ot_LST1 (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MqSendSTART(mqctx);
  MqSendL_END(mqctx);
  return MqSendRETURN(mqctx);
}

/// \brief test list #2
/// \service
static enum MqErrorE
Ot_LST2 (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MqSendSTART(mqctx);
  MqReadL_END(mqctx);
  return MqSendRETURN(mqctx);
}

static enum MqErrorE
BgError (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  struct MqS * const master = MqSlaveGetMaster(mqctx);
  if (master) {
    MqErrorC (master, "BGERROR", MqErrorGetNumI(mqctx), MqErrorGetText(mqctx));
    MqSendERROR (master);
  }
  return MqErrorGetCode(mqctx);
}

static enum MqErrorE
ClientCreateParent (
  struct MqS * const errorctx,
  struct MqS * const mqctx,
  MQ_INT debug
)
{
  struct MqBufferLS * args = MqBufferLCreateArgsV(mqctx, "test-client", "@", "server", "--name", "test-server", NULL);
  MqConfigSetDebug(mqctx, debug);
  MqConfigSetDefaultFactory(mqctx, "client");
  MqConfigSetBgError(mqctx,BgError, NULL, NULL, NULL);
  MqErrorCheck(MqLinkCreate(mqctx,&args));
  MqErrorCheck(MqCheckForLeftOverArguments(mqctx,&args));
  return MQ_OK;
error:
  MqBufferLDelete(&args);
  return MqErrorCopy (errorctx, mqctx);
}

static enum MqErrorE
ClientERRCreateParent (
  struct MqS * const errorctx,
  struct MqS * const mqctx,
  MQ_INT debug
)
{
  struct MqBufferLS * args = MqBufferLCreateArgsV(mqctx, "test-client", "@", "server", "--name", "test-server", NULL);
  MqConfigSetDebug(mqctx, debug);
  MqErrorCheck(MqLinkCreate(mqctx,&args));
  MqErrorCheck(MqCheckForLeftOverArguments(mqctx,&args));
  return MQ_OK;
error:
  MqBufferLDelete(&args);
  return MqErrorCopy (errorctx, mqctx);
}

static enum MqErrorE
ClientCreateChild (
  struct MqS * const errorctx,
  struct MqS * const mqctx,
  struct MqS * const parent
)
{
  MqConfigDup(mqctx, parent);
  MqLinkCreateChild(mqctx, parent, NULL);
  return MqErrorCopy (errorctx, mqctx);
}

static enum MqErrorE
CreateWorker (
  struct MqS * const errorctx,
  struct MqS * const master,
  MQ_INT master_id
)
{
  struct MqBufferLS * argv = MqBufferLCreate(5+MqReadGetNumItems(master));
  MqErrorCheck (MqReadL (master, &argv));
  MqBufferLAppendC(argv, "--name");
  MqBufferLAppendU(argv, MqBufferSetV(MqBufferCreate(MQ_ERROR_PANIC,20), "wk-cl-%d", master_id));
  MqBufferLAppendC(argv, "@");
  MqBufferLAppendC(argv, "--name");
  MqBufferLAppendU(argv, MqBufferSetV(MqBufferCreate(MQ_ERROR_PANIC,20), "wk-sv-%d", master_id));
  MqSlaveWorker (master, master_id, &argv);
error:
  MqBufferLDelete(&argv);
  return MqErrorCopy(errorctx,master);
}

/// \brief test the "MqSend*" feature
/// \service
static enum MqErrorE
Ot_SND1 (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
#define MqErrorCheckAndCopy(PROC,SRCERR,TGTERR) if (MqErrorCheckI(PROC)) {\
  MqErrorCopy(TGTERR, SRCERR); \
  goto error; \
}
#define LocalErrorCheckAndCopy(PROC) \
  MqErrorCheckAndCopy(PROC, clmqctx, mqctx)

  struct ServerCtxS * const srvctx = SRVCTX;
  MQ_CST s;
  MQ_BUF buf;
  MQ_INT clid;
  MqErrorCheck(MqReadC(mqctx, &s));
  MqErrorCheck(MqReadI(mqctx, &clid));
  MqSendSTART(mqctx);
    if (!strcmp(s,"START")) {
      struct MqS * const parent = mqctx->config.parent;
      struct ServerCtxS * parentCtx = (struct ServerCtxS *) parent;
      if (parentCtx != NULL && MqLinkIsConnected(parentCtx->cl[clid])) {
	MqErrorCheck (ClientCreateChild(mqctx, srvctx->cl[clid], parentCtx->cl[clid]));
      } else {
	MqErrorCheck (ClientCreateParent(mqctx, srvctx->cl[clid], mqctx->config.debug));
      }
    } else if (!strcmp(s,"START2")) {
      MqErrorCheck (ClientCreateParent(mqctx, srvctx->cl[clid], mqctx->config.debug));
      MqErrorCheck (ClientCreateParent(mqctx, srvctx->cl[clid], mqctx->config.debug));
    } else if (!strcmp(s,"START3")) {
      enum MqErrorE ret;
      struct MqS * parent = MqContextCreate(0, NULL);
      ret = ClientCreateChild(mqctx, srvctx->cl[clid], parent);
      MqContextDelete(&parent);
      MqErrorCheck(ret);
    } else if (!strcmp(s,"START4")) {
      if (MqErrorCheckI(MqSlaveWorker(srvctx->cl[clid], 0, NULL))) {
	MqErrorCopy(mqctx, srvctx->cl[clid]);
      }
    } else if (!strcmp(s,"START5")) {
      MqErrorCheck (CreateWorker(mqctx, mqctx, clid));
    } else if (!strcmp(s,"STOP")) {
      MqLinkDelete(srvctx->cl[clid]);
    } else if (!strcmp(s,"SEND")) {
      MQ_CST TOK;
      struct MqS * const clmqctx = srvctx->cl[clid];
      MqSendSTART(clmqctx);
      MqErrorCheck(MqReadC(mqctx, &TOK));
      MqErrorCheck(MqReadProxy(mqctx, clmqctx));
      LocalErrorCheckAndCopy(MqSendEND(clmqctx, TOK));
    } else if (!strcmp(s,"WAIT")) {
      struct MqS * const clmqctx = srvctx->cl[clid];
      MqSendSTART(clmqctx);
      MqErrorCheck(MqReadProxy(mqctx, clmqctx));
      LocalErrorCheckAndCopy(MqSendEND_AND_WAIT(clmqctx, "ECOI", 5));
      LocalErrorCheckAndCopy(MqReadI(clmqctx, &srvctx->i));
      MqSendI(mqctx, srvctx->i+1);
    } else if (!strcmp(s,"CALLBACK")) {
      struct MqS * const clmqctx = srvctx->cl[clid];
      srvctx->i = -1;
      MqSendSTART(clmqctx);
      MqErrorCheck(MqReadU(mqctx, &buf));
      MqSendU(clmqctx, buf);
      LocalErrorCheckAndCopy(MqSendEND_AND_CALLBACK(clmqctx, "ECOI", Callback1, NULL, NULL));
      LocalErrorCheckAndCopy(MqProcessEvent(clmqctx, 10, MQ_WAIT_ONCE));
      MqSendI(mqctx, CLIENTCTX(clmqctx)->i+1);
    } else if (!strcmp(s,"ERR-1")) {
      struct MqS * const clmqctx = srvctx->cl[clid];
      MqSendSTART(clmqctx);
      MqErrorCheck(MqReadProxy(mqctx, clmqctx));
      if (MqErrorCheckI(MqSendEND_AND_WAIT(clmqctx, "ECOI", 5))) {
	MqErrorCopy(mqctx, clmqctx); \
	MqSendI(mqctx, MqErrorGetNumI(mqctx));
	MqSendC(mqctx, MqErrorGetText(mqctx));
	MqErrorReset(mqctx);
      }
    }

error:
  return MqSendRETURN (mqctx);
}

/// \brief test the "MqSend*" feature
/// \service
static enum MqErrorE
Ot_SND2 (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  struct ServerCtxS * srvctx = SRVCTX;
  MQ_CST s;
  MQ_BUF buf;
  MQ_INT clid;
  struct MqS * clmqctx;
  MqErrorCheck(MqReadC(mqctx, &s));
  MqErrorCheck(MqReadI(mqctx, &clid));
  clmqctx = MqSlaveGet(mqctx, clid);
  MqSendSTART(mqctx);
    if (!strcmp(s,"CREATE")) {
      MqErrorCheck (CreateWorker (mqctx, mqctx, clid));
    } else if (!strcmp(s,"CREATE2")) {
      struct MqS * clmqctx = MqContextCreate (sizeof (struct ClientCtxS), NULL);
      MqErrorCheck (ClientCreateParent(mqctx, clmqctx, mqctx->config.debug));
      MqErrorCheck (MqSlaveCreate (mqctx, clid, clmqctx));
    } else if (!strcmp(s,"CREATE3")) {
      struct MqS * clmqctx = MqContextCreate (sizeof (struct ClientCtxS), NULL);
      MqErrorCheck (ClientERRCreateParent(mqctx, clmqctx, mqctx->config.debug));
      MqErrorCheck (MqSlaveCreate (mqctx, clid, clmqctx));
    } else if (!strcmp(s,"DELETE")) {
      MqErrorCheck(MqSlaveDelete(mqctx, clid));
      MqSendC (mqctx, MqSlaveGet(mqctx, clid) == NULL ? "OK" : "ERROR");
    } else if (!strcmp(s,"SEND")) {
      MQ_CST TOK;
      MqErrorCheck(MqSendSTART(clmqctx));
      MqErrorCheck(MqReadC(mqctx, &TOK));
      MqErrorCheck(MqReadU(mqctx, &buf));
      MqErrorCheck(MqSendU(clmqctx, buf));
      MqErrorCheck(MqSendEND(clmqctx, TOK));
    } else if (!strcmp(s,"WAIT")) {
      MQ_CBI itm;
      MQ_SIZE len;
      MqErrorCheck(MqSendSTART(clmqctx));
      MqErrorCheck(MqReadN(mqctx, &itm, &len));
      MqErrorCheck(MqSendN(clmqctx, itm, len));
      MqErrorCheck(MqSendEND_AND_WAIT(clmqctx, "ECOI", 5));
      MqErrorCheck(MqReadI(clmqctx, &srvctx->i));
      MqSendI(mqctx, srvctx->i+1);
    } else if (!strcmp(s,"MqSendEND_AND_WAIT")) {
      MQ_CST token;
      MqErrorCheck(MqReadC(mqctx, &token));
      MqErrorCheck(MqSendSTART(clmqctx));
      while (MqReadItemExists(mqctx)) {
	MqErrorCheck(MqReadU(mqctx, &buf));
	MqErrorCheck(MqSendU(clmqctx, buf));
      }
      MqErrorCheck(MqSendEND_AND_WAIT(clmqctx, token, 5));
      while (MqReadItemExists(clmqctx)) {
	MqErrorCheck(MqReadU(clmqctx, &buf));
	MqSendU(mqctx, buf);
      }
    } else if (!strcmp(s,"MqSendEND")) {
      MQ_CST token;
      MqErrorCheck(MqReadC(mqctx, &token));
      MqErrorCheck(MqSendSTART(clmqctx));
      while (MqReadItemExists(mqctx)) {
	MqErrorCheck(MqReadU(mqctx, &buf));
	MqErrorCheck(MqSendU(clmqctx, buf));
      }
      MqErrorCheck(MqSendEND(clmqctx, token));
      return MQ_OK;
    } else if (!strcmp(s,"CALLBACK")) {
      srvctx->i = -1;
      MqErrorCheck(MqSendSTART(clmqctx));
      MqErrorCheck(MqReadU(mqctx, &buf));
      MqErrorCheck(MqSendU(clmqctx, buf));
      MqErrorCheck(MqSendEND_AND_CALLBACK(clmqctx, "ECOI", Callback2, NULL, NULL));
      MqErrorCheck(MqProcessEvent(clmqctx, 10, MQ_WAIT_ONCE));
      MqSendI(mqctx, CLIENTCTX(clmqctx)->i+1);
    } else if (!strcmp(s,"ERR-1")) {
      struct MqBufferLS * args = MqBufferLCreateArgsV(clmqctx, "cl-err-1", "@", "DUMMY", NULL);
      clmqctx = MqContextCreate (sizeof(struct ClientCtxS), NULL);
      MqConfigSetDebug(clmqctx, MqConfigGetDebug(mqctx));
      if (MqErrorCheckI(MqLinkCreate(clmqctx, &args))) {
	MqErrorCopy(mqctx, clmqctx);
	MqContextDelete (&clmqctx);
      }
      MqBufferLDelete(&args);
    } else if (!strcmp(s,"isSlave")) {
      MqSendO(mqctx, MqSlaveIs(clmqctx));
    }
error:
  return MqSendRETURN (mqctx);
}

/// \brief test the "Init" feature
/// \service
static enum MqErrorE
Ot_INIT (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  struct MqBufferLS * initB = MqInitCreate();
  MqReadL(mqctx, &initB);
  return MqSendRETURN (mqctx);
}

/// \brief test the "Init" feature
/// \service
static enum MqErrorE
Ot_CNFG (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MqSendSTART(mqctx);

  MqSendO(mqctx,mqctx->setup.isServer);
  MqSendO(mqctx,MqLinkIsParent(mqctx));
  MqSendO(mqctx,MqSlaveIs(mqctx));
  MqSendO(mqctx,mqctx->config.isString);
  MqSendO(mqctx,mqctx->config.isSilent);
  MqSendO(mqctx,MQ_YES);

  MqSendC(mqctx,mqctx->config.name);
  MqSendI(mqctx,mqctx->config.debug);
  MqSendI(mqctx,MqLinkGetCtxId(mqctx));
  MqSendC(mqctx,MqServiceGetToken(mqctx));

  return MqSendRETURN (mqctx);
}

/// \brief list in list read error
/// \service
static enum MqErrorE
Ot_ERLR (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MqSendSTART(mqctx);

  MqReadL_START(mqctx, NULL);
  MqReadL_START(mqctx, NULL);

  return MqSendRETURN (mqctx);
}

/// \brief list in list read error
/// \service
static enum MqErrorE
Ot_ERLS (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_BUF buf;
  MqSendSTART(mqctx);

  MqSendL_START(mqctx);
  MqReadU(mqctx, &buf);
  MqSendU(mqctx, buf);
  MqSendL_START(mqctx);
  MqReadU(mqctx, &buf);
  MqSendU(mqctx, buf);

  return MqSendRETURN (mqctx);
}

#define CFGTest(A,I) \
    I ## O = MqConfigGet ## A (mqctx); \
    MqErrorCheck (MqRead ## I (mqctx, & I ## V)); \
    MqConfigSet ## A (mqctx, I ## V); \
    MqErrorCheck (MqSend ## I (mqctx, MqConfigGet ## A (mqctx))); \
    MqConfigSet ## A (mqctx, I ## O);

#define CFGTestC(A) \
    CO = MqConfigGet ## A (mqctx); \
    if (CO) CO = mq_strdup(CO); \
    MqErrorCheck (MqReadC (mqctx, &CV)); \
    MqConfigSet ## A (mqctx, CV); \
    MqErrorCheck (MqSendC (mqctx, MqConfigGet ## A (mqctx))); \
    MqConfigSet ## A (mqctx, CO); \
    MqSysFree (CO);

/// \brief test the configuration
/// \service
static enum MqErrorE
Ot_CFG1 (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_CST cmd;
  MQ_INT IV, IO;
  MQ_WID WV, WO;
  MQ_CST CV, CO;
  MQ_BOL OV, OO;

  MqErrorCheck (MqReadC (mqctx, &cmd));

  MqSendSTART(mqctx);

  if (!strncmp (cmd, "Buffersize", 10)) {
    CFGTest(Buffersize, I)
  } else if (!strncmp (cmd, "Debug", 5)) {
    CFGTest(Debug, I)
  } else if (!strncmp (cmd, "Timeout", 7)) {
    CFGTest(Timeout, W)
  } else if (!strncmp (cmd, "Name", 4)) {
    CFGTestC(Name)
  } else if (!strncmp (cmd, "SrvName", 7)) {
    CFGTestC(SrvName)
  } else if (!strncmp (cmd, "Ident", 5)) {
    MQ_BOL check;
    CO = MqConfigGetIdent (mqctx);
    if (CO) CO = mq_strdup(CO);
    MqErrorCheck (MqReadC (mqctx, &CV));
    MqConfigSetIdent (mqctx, CV);
    MqErrorCheck (MqReadC (mqctx, &CV));
    check = !strcmp(MqLinkGetTargetIdent (mqctx),CV);
    // send
    MqSendSTART(mqctx);
    MqErrorCheck (MqSendC (mqctx, MqConfigGetIdent (mqctx)));
    MqErrorCheck (MqSendO (mqctx, check));
    // clenup
    MqConfigSetIdent (mqctx, CO);
    MqSysFree (CO);
  } else if (!strncmp (cmd, "IsSilent", 8)) {
    CFGTest(IsSilent,O)
  } else if (!strncmp (cmd, "IsString", 8)) {
    CFGTest(IsString,O)
  } else if (!strncmp (cmd, "IoUds", 5)) {
    CFGTestC(IoUdsFile)
  } else if (!strncmp (cmd, "IoTcp", 5)) {
    MQ_CST h, p, mh, mp;
    MQ_CST hV, pV, mhV, mpV;
    h = MqSysStrDup(MQ_ERROR_PANIC, MqConfigGetIoTcpHost(mqctx));
    p = MqSysStrDup(MQ_ERROR_PANIC, MqConfigGetIoTcpPort(mqctx));
    mh = MqSysStrDup(MQ_ERROR_PANIC, MqConfigGetIoTcpMyHost(mqctx));
    mp = MqSysStrDup(MQ_ERROR_PANIC, MqConfigGetIoTcpMyPort(mqctx));
    MqErrorCheck (MqReadC (mqctx, &hV));
    MqErrorCheck (MqReadC (mqctx, &pV));
    MqErrorCheck (MqReadC (mqctx, &mhV));
    MqErrorCheck (MqReadC (mqctx, &mpV));
    MqErrorCheck(MqConfigSetIoTcp (mqctx, hV, pV, mhV, mpV));
    MqErrorCheck (MqSendC (mqctx, MqConfigGetIoTcpHost(mqctx)));
    MqErrorCheck (MqSendC (mqctx, MqConfigGetIoTcpPort(mqctx)));
    MqErrorCheck (MqSendC (mqctx, MqConfigGetIoTcpMyHost(mqctx)));
    MqErrorCheck (MqSendC (mqctx, MqConfigGetIoTcpMyPort(mqctx)));
    MqConfigSetIoTcp (mqctx, h, p, mh, mp);
    MqSysFree (h);
    MqSysFree (p);
    MqSysFree (mh);
    MqSysFree (mp);
  } else if (!strncmp (cmd, "IoPipe", 6)) {
    CFGTest(IoPipeSocket, I)
  } else if (!strncmp (cmd, "StartAs", 7)) {
    CFGTest(StartAs, I)
  } else {
    MqErrorV(mqctx, __func__, 1, "invalid command: %s", cmd);
  }

error:
  return MqSendRETURN (mqctx);
}

/// \brief print data to file
/// \service
static enum MqErrorE
Ot_PRNT (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_CST str;
  MqErrorCheck (MqSendSTART(mqctx));
  MqErrorCheck (MqReadC(mqctx, &str));
  MqErrorCheck (MqSendV(mqctx, "%d - %s", MqLinkGetCtxId(mqctx), str));
  MqErrorCheck (MqSendEND_AND_WAIT(mqctx, "WRIT", MQ_TIMEOUT_USER));
error:
  return MqSendRETURN (mqctx);
}

/// \brief print data to file
/// \service
static enum MqErrorE Ot_TRN2 (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  struct ServerCtxS *srvctx = (struct ServerCtxS*) mqctx;
  MqErrorCheck (MqReadT_START (mqctx, NULL));
  MqErrorCheck (MqReadI (mqctx, &srvctx->i));
  MqErrorCheck (MqReadT_END (mqctx));
  MqErrorCheck (MqReadI (mqctx, &srvctx->j));
error:
  return MqErrorStack (mqctx);
}

/// \brief print data to file
/// \service
static enum MqErrorE
Ot_TRNS (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  struct ServerCtxS *srvctx = (struct ServerCtxS*) mqctx;
  MQ_INT i;
  MqErrorCheck (MqSendSTART (mqctx));
  MqErrorCheck (MqSendT_START (mqctx, "TRN2"));
  MqErrorCheck (MqSendI (mqctx, 9876));
  MqErrorCheck (MqSendT_END (mqctx));
  MqErrorCheck (MqReadI (mqctx, &i));
  MqErrorCheck (MqSendI (mqctx, i));
  MqErrorCheck (MqSendEND_AND_WAIT (mqctx, "ECOI", MQ_TIMEOUT_USER));
  MqErrorCheck (MqProcessEvent (mqctx, MQ_TIMEOUT_USER, MQ_WAIT_ONCE));
  MqErrorCheck (MqSendSTART (mqctx));
  MqErrorCheck (MqSendI (mqctx, srvctx->i));
  MqErrorCheck (MqSendI (mqctx, srvctx->j));
error:
  return MqSendRETURN (mqctx);
}

/*****************************************************************************/
/*                                                                           */
/*                                context_init                               */
/*                                                                           */
/*****************************************************************************/

static enum MqErrorE
ServerCleanup (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  // cleanup the context data
  int i;
  struct ServerCtxS * srvctx = SRVCTX;
  for (i=0; i<3; i++) {
    MqContextDelete((struct MqS**) &srvctx->cl[i]);
  }
  return MQ_OK;
}

static enum MqErrorE
ServerSetup (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  if (MqSlaveIs(mqctx)) {
    // add "slave" services here
  } else {
    // create the context data
    int i;
    MQ_BUF buf = mqctx->temp;
    struct ServerCtxS * srvctx = SRVCTX;
    for (i=0; i<3; i++) {
      srvctx->cl[i] = MqContextCreate (sizeof (struct ClientCtxS), NULL);
      MqBufferSetV(buf, "cl-%i", i);
      MqConfigSetName(srvctx->cl[i], buf->cur.C);
      MqBufferSetV(buf, "sv-%i", i);
      MqConfigSetSrvName(srvctx->cl[i], buf->cur.C);
    }
    // add "master" services here
    MqErrorCheck (MqServiceCreate (mqctx, "CSV1", Ot_CSV1, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ERR1", Ot_ERR1, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ERR2", Ot_ERR2, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ERR3", Ot_ERR3, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ERR4", Ot_ERR4, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ERRT", Ot_ERRT, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ECOU", Ot_ECOU, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ECON", Ot_ECON, NULL, NULL));
    // START-C-SERVICE-DEFINITION
    MqErrorCheck (MqServiceCreate (mqctx, "ECOI", Ot_ECOI, NULL, NULL));
    // END-C-SERVICE-DEFINITION
    MqErrorCheck (MqServiceCreate (mqctx, "ECOY", Ot_ECOY, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ECOO", Ot_ECOO, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ECOS", Ot_ECOS, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ECOW", Ot_ECOW, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ECOD", Ot_ECOD, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ECOF", Ot_ECOF, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ECOC", Ot_ECOC, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ECOB", Ot_ECOB, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ECLI", Ot_ECLI, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ECOL", Ot_ECOL, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ECOM", Ot_ECOM, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ECUL", Ot_ECUL, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "GETU", Ot_GETU, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "GTCX", Ot_GTCX, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "GTTO", Ot_GTTO, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "GTSI", Ot_GTSI, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "MSQT", Ot_MSQT, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "RDUL", Ot_RDUL, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "REDI", Ot_REDI, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "SETU", Ot_SETU, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "SLEP", Ot_SLEP, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "SND1", Ot_SND1, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "SND2", Ot_SND2, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "USLP", Ot_USLP, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "BUF1", Ot_BUF1, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "BUF2", Ot_BUF2, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "BUF3", Ot_BUF3, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "LST1", Ot_LST1, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "LST2", Ot_LST2, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "INIT", Ot_INIT, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "CNFG", Ot_CNFG, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ERLR", Ot_ERLR, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "ERLS", Ot_ERLS, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "CFG1", Ot_CFG1, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "PRNT", Ot_PRNT, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "TRNS", Ot_TRNS, NULL, NULL));
    MqErrorCheck (MqServiceCreate (mqctx, "TRN2", Ot_TRN2, NULL, NULL));
  }

error:
  return MqErrorStack(mqctx);
}

/*****************************************************************************/
/*                                                                           */
/*                                  main                                     */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
ServerFactory (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryItemS * const item,
  struct MqS **contextP
)
{ 
  struct MqS * const mqctx = MqContextCreate(sizeof(struct ServerCtxS),tmpl);
  // we do not copy the "setup" because the "caller" could be something
  // !NOT! necessary an other "server" object
  mqctx->setup.Child.fCreate	    = MqLinkDefault;
  mqctx->setup.Parent.fCreate	    = MqLinkDefault;
  mqctx->setup.fHelp		    = ServerHelp;
  mqctx->setup.isServer		    = MQ_YES;
  mqctx->setup.ServerSetup.fCall    = ServerSetup;
  mqctx->setup.ServerCleanup.fCall  = ServerCleanup;
  MqConfigSetFactoryItem (mqctx, item);
  *contextP = mqctx;
  return MQ_OK;
}

/// \brief main entry-point for the tool
/// \param argc the number of command-line arguments
/// \param argv the command-line arguments as an array of strings
/// \return the exit number
int
main (
  const int argc,
  MQ_CST argv[]
)
{
  struct MqS *mqctx = NULL;

  // parse the command-line
  struct MqBufferLS * args = MqBufferLCreateArgs (argc, argv);

  // add and all Factory 
  mqctx = MqFactoryNew("server", ServerFactory, NULL, NULL, NULL, NULL, NULL);

  // context not available -> an error happen
  if (mqctx == NULL) {
    ServerHelp(MqSysBasename("server", MQ_NO));
  }

  // create the ServerCtxS
  MqErrorCheck(MqLinkCreate (mqctx, &args));

  // test debug output
  MqLogC(mqctx, "test", 1, "this is the log test\n");

  // start event-loop and wait forever
  MqErrorCheck(MqProcessEvent (mqctx, MQ_TIMEOUT_DEFAULT, MQ_WAIT_FOREVER));

  // finish and exit
error:
  MqExit (mqctx);
}

/** \} server */

