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

/// link to the MqErrorS object
#define ARGS  struct MqS * const mqctx, void *data
#define GUARDCTX ((struct GuardCtxS*const)mqctx)
#define MQCTX ((struct MqS*const)guard)
#define MQ_CONTEXT_S mqctx
#define SETUP_guard struct GuardCtxS*const guard = GUARDCTX
#define CHECK_ARGS(s) \
  if (MqReadGetNumItems(mqctx))  { \
    return MqErrorV (mqctx, __func__, -1, "usage: %s (%s)\n", __func__, s); \
  }

/// \brief the local \b context of the \ref server tool
/// \mqctx
struct GuardCtxS {
  struct MqS	mqctx;	///< \mqctxI
};

/*****************************************************************************/
/*                                                                           */
/*                               Request Handler                             */
/*                                                                           */
/*****************************************************************************/

#define ENCRYPT MQ_YES
#define DECRYPT MQ_NO
static void guard_crypt (MQ_BIN data, MQ_SIZE size, MQ_BOL flag) {
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
  int isTrans = MqServiceIsTransaction (mqctx);
  MQ_BIN bdy; MQ_SIZE len;
  struct MqS * ftrctx;

  MqErrorCheck (MqServiceGetFilter (mqctx, 0, &ftrctx));

  MqErrorCheck (MqReadBDY (mqctx, &bdy, &len));

  guard_crypt (bdy, len, ENCRYPT);

  // build "+GRD" package
  MqErrorCheck1 (MqSendSTART (ftrctx));
  MqErrorCheck1 (MqSendC (ftrctx, MqServiceGetToken (mqctx)));
  MqErrorCheck1 (MqSendI (ftrctx, isTrans));
  MqErrorCheck1 (MqSendB (ftrctx, bdy, len));

  // use a transaction protection
  MqErrorCheck1 (MqSendEND_AND_WAIT (ftrctx, "+GRD", MQ_TIMEOUT_USER));

  // continue with the original transaction
  if (isTrans) {
    MqSendSTART (mqctx);
    MqErrorCheck1 (MqReadB (ftrctx, &bdy, &len));
    guard_crypt (bdy, len, DECRYPT);
    MqErrorCheck (MqSendBDY (mqctx, bdy, len));
  }

error:
  return MqSendRETURN(mqctx);

error1:
  return MqErrorCopy(mqctx, ftrctx);
}

static enum MqErrorE
GuardToPkg (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_INT isTrans;
  MQ_CST token;
  MQ_BIN bdy; MQ_SIZE len;
  struct MqS * ftrctx;

  MqErrorCheck (MqServiceGetFilter (mqctx, 0, &ftrctx));

  MqErrorCheck (MqReadC (mqctx, &token));
  MqErrorCheck (MqReadI (mqctx, &isTrans));
  MqErrorCheck (MqReadB (mqctx, &bdy, &len));
  guard_crypt (bdy, len, DECRYPT);
  MqErrorCheck1 (MqSendSTART (ftrctx));
  MqErrorCheck1 (MqSendBDY (ftrctx, bdy, len));
  if (isTrans) {
    // use a transaction protection
    MqErrorCheck1 (MqSendEND_AND_WAIT (ftrctx, token, MQ_TIMEOUT_USER));
    // send the "answer" back
    MqSendSTART (mqctx);
    MqErrorCheck1 (MqReadBDY (ftrctx, &bdy, &len));
    guard_crypt (bdy, len, ENCRYPT);
    MqErrorCheck (MqSendB (mqctx, bdy, len));
  } else {
    MqErrorCheck1 (MqSendEND (ftrctx, token));
  }

error:
  return MqSendRETURN(mqctx);

error1:
  return MqErrorCopy(mqctx, ftrctx);
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

  // SERVER: every token (+ALL) have to be "guard_encrypted"
  MqErrorCheck (MqServiceCreate (mqctx, "+ALL", PkgToGuard, NULL, NULL));
  // SERVER: only the "+GRD" token is "guard_decrypted"
  MqErrorCheck (MqServiceCreate (mqctx, "+GRD", GuardToPkg, NULL, NULL));

  // CLIENT: every token (+ALL) have to be "guard_encrypted"
  MqErrorCheck (MqServiceCreate (ftrctx, "+ALL", PkgToGuard, NULL, NULL));
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
  struct MqS * const mqctx = MqContextCreate(sizeof(struct GuardCtxS), NULL);

  // parse the command-line
  struct MqBufferLS * args = MqBufferLCreateArgs (argc, argv);

  // add config data
  mqctx->setup.Child.fCreate	    = MqDefaultLinkCreate;
  mqctx->setup.Parent.fCreate	    = MqDefaultLinkCreate;
  mqctx->setup.fHelp		    = GuardHelp;
  mqctx->setup.isServer		    = MQ_YES;
  mqctx->setup.ServerSetup.fFunc    = GuardSetup;
  mqctx->setup.ServerCleanup.fFunc  = GuardCleanup;
  MqConfigSetDefaultFactory (mqctx);

  // create the ServerCtxS
  MqErrorCheck(MqLinkCreate (mqctx, &args));

  // start event-loop and wait forever
  MqProcessEvent (mqctx, MQ_TIMEOUT, MQ_WAIT_FOREVER);

  // finish and exit
error:
  MqExit (mqctx);
}
