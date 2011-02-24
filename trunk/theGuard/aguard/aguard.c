/**
 *  \file       theGuard/aguard/aguard.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include <stdlib.h>

#include "msgque.h"
#include "debug.h"
#include "key.h"

#define MQ_CONTEXT_S mqctx

/*****************************************************************************/
/*                                                                           */
/*                               Request Handler                             */
/*                                                                           */
/*****************************************************************************/

#define ENCRYPT MQ_YES
#define DECRYPT MQ_NO
static MQ_BIN guard_crypt (MQ_BIN data, MQ_SIZE size, MQ_BOL flag) {
  MQ_BIN key = KEY_DATA;
  const MQ_BIN keyend = KEY_DATA+KEY_LENGTH;
  const MQ_BIN dataend = data+size;
  for (;data<dataend; data++,key++) {
    if (key >= keyend) {
      key=KEY_DATA;
    }
    if (flag) {
      *data += *key;
    } else {
      *data -= *key;
    }
  }
  return data;
}

/// \brief display help using \b -h or \b --help command-line option
/// \param base the executable usually: <tt>basename(argv[0])</tt>
static void __attribute__ ((noreturn))
GuardHelp (const char * base)
{
  fprintf(stderr, "usage: %s [OPTION]... [ARGUMENT]...\n", base);
  fputs("\n", stderr);
  fputs("  This tool is the data protection server of NHI1. Read more at:\n", stderr);
  fputs("  http://openfacts2.berlios.de/wikien/index.php/BerliosProject:NHI1_-_TheGuard\n", stderr);
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

static enum MqErrorE
PkgToGuard (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  struct MqDumpS *dump;
  struct MqS *ftrctx;
  MQ_BIN bdy = NULL; MQ_SIZE len;
  enum MqHandShakeE hs = MqReadGetHandShake(mqctx);

  MqErrorCheck (MqServiceGetFilter (mqctx, 0, &ftrctx));

  MqErrorCheck (MqReadDUMP (mqctx, &dump));
  bdy = (MQ_BIN) dump;
  len = MqDumpSize(dump);
printLV("1. DUMP: sig<%x>, len<%i>\n", *(int*)bdy, len);
  guard_crypt (bdy, len, ENCRYPT);
printLV("2. DUMP: sig<%x>, len<%i>\n", *(int*)bdy, len);

  // build "+GRD" package and send as shortterm-transaction
  MqErrorCheck1 (MqSendSTART (ftrctx));
  MqErrorCheck1 (MqSendB (ftrctx, bdy, len));
  MqErrorCheck1 (MqSendEND_AND_WAIT (ftrctx, "+GRD", MQ_TIMEOUT_USER));
  MqDumpDelete (&dump);

  // continue with the original transaction
  // a "longterm-transaction client->server call" return 2 packages. A "_RET" and than a "+TRT",
  // the "_RET" have to be ignored
  if (hs == MQ_HANDSHAKE_TRANSACTION) {
    return MQ_OK;
  } else if (MqServiceIsTransaction (mqctx)) {
    MqErrorCheck1 (MqReadB (ftrctx, &bdy, &len));
    guard_crypt (bdy, len, DECRYPT);
    return MqReadForward (ftrctx, mqctx, (struct MqDumpS*) bdy);
  }

error:
  return MqSendRETURN(mqctx);

error1:
  MqDumpDelete (&dump);
  MqErrorCopy(mqctx, ftrctx);
  return MqErrorStack(mqctx);
}

static enum MqErrorE
GuardToPkg (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  struct MqDumpS *dump = NULL;
  struct MqS * ftrctx;
  MQ_BIN bdy; MQ_SIZE len;
  enum MqHandShakeE hs = MQ_HANDSHAKE_START;

  MqErrorCheck (MqServiceGetFilter (mqctx, 0, &ftrctx));

  MqErrorCheck (MqReadB (mqctx, &bdy, &len));
  MqErrorCheck (MqReadForward (mqctx, ftrctx, (struct MqDumpS*) guard_crypt (bdy, len, DECRYPT)));

//printH(hs)

  // a "longterm-transaction" return 2 packages "_RET" and "+TRT" -> ignore the "_RET"
  if (hs == MQ_HANDSHAKE_TRANSACTION) {
    return MQ_OK;
  } else {
    // check for a short-term-transaction and return the results
    // should be always != 0 because "PkgToGuard" use "MqSendEND_AND_WAIT"
    if (mqctx->link.transSId != 0) {
      // read all results from ftr
      MqErrorCheck1 (MqReadDUMP (ftrctx, &dump));
      bdy = (MQ_BIN)dump;
      len = MqDumpSize(dump);
      // encrypt
      guard_crypt  (bdy, len, ENCRYPT);
      // prepare binary package
      MqErrorCheck (MqSendSTART (mqctx));
      MqErrorCheck (MqSendB (mqctx, bdy, len));
    }
  }

error:
  MqDumpDelete(&dump);
  // aswer the "+GRD" service call
  return MqSendRETURN(mqctx);

error1:
  MqErrorCopy(mqctx, ftrctx);
  return MqErrorStack(mqctx);
}

/*****************************************************************************/
/*                                                                           */
/*                                context_init                               */
/*                                                                           */
/*****************************************************************************/

static enum MqErrorE
GuardCleanup (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  return MQ_OK;
}

static enum MqErrorE
GuardSetup (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  struct MqS * ftrctx;

  // "aguard" have to be a filter
  MqErrorCheck (MqServiceGetFilter (mqctx, 0, &ftrctx));

  // SERVER: every token (+ALL,+TRT) have to be "guard_encrypted"
  MqErrorCheck (MqServiceCreate (mqctx, "+ALL", PkgToGuard, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "+TRT", PkgToGuard, NULL, NULL));
  // SERVER: only the "+GRD" token is "guard_decrypted"
  MqErrorCheck (MqServiceCreate (mqctx, "+GRD", GuardToPkg, NULL, NULL));

  // CLIENT: every token (+ALL) have to be "guard_encrypted"
  MqErrorCheck (MqServiceCreate (ftrctx, "+ALL", PkgToGuard, NULL, NULL));
  MqErrorCheck (MqServiceCreate (ftrctx, "+TRT", PkgToGuard, NULL, NULL));
  // CLIENT: only the "+GRD" token is "guard_decrypted"
  MqErrorCheck (MqServiceCreate (ftrctx, "+GRD", GuardToPkg, NULL, NULL));

error:
  return MqErrorStack(mqctx);
}

/*****************************************************************************/
/*                                                                           */
/*                                  main                                     */
/*                                                                           */
/*****************************************************************************/

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
  // the parent-context
  struct MqS * const mqctx = MqContextCreate(0, NULL);

  // parse the command-line
  struct MqBufferLS * args = MqBufferLCreateArgs (argc, argv);

  // add config data
  mqctx->setup.Child.fCreate	    = MqLinkDefault;
  mqctx->setup.Parent.fCreate	    = MqLinkDefault;
  mqctx->setup.fHelp		    = GuardHelp;
  mqctx->setup.isServer		    = MQ_YES;
  mqctx->setup.ServerSetup.fCall    = GuardSetup;
  mqctx->setup.ServerCleanup.fCall  = GuardCleanup;

  // create the ServerCtxS
  MqErrorCheck(MqLinkCreate (mqctx, &args));

  // start event-loop and wait forever
  MqProcessEvent (mqctx, MQ_TIMEOUT, MQ_WAIT_FOREVER);

  // finish and exit
error:
  MqExit (mqctx);
}

