/**
 *  \file       theFilter3/Filter3/Filter3.c
 *  \brief      \$Id: Filter3.c 81 2009-12-18 13:20:21Z aotto1968 $
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 81 $
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include <stdlib.h>

#include "msgque.h"
#include "debug.h"

/// link to the MqErrorS object
#define ARGS  struct MqS * const mqctx, void *data
#define GUARDCTX ((struct Filter3CtxS*const)mqctx)
#define MQCTX ((struct MqS*const)guard)
#define MQ_CONTEXT_S mqctx
#define SETUP_guard struct Filter3CtxS*const guard = GUARDCTX
#define CHECK_ARGS(s) \
  if (MqReadGetNumItems(mqctx))  { \
    return MqErrorV (mqctx, __func__, -1, "usage: %s (%s)\n", __func__, s); \
  }

/// \brief the local \b context of the \ref server tool
/// \mqctx
struct Filter3CtxS {
  struct MqS	mqctx;	///< \mqctxI
};

/*****************************************************************************/
/*                                                                           */
/*                               Request Handler                             */
/*                                                                           */
/*****************************************************************************/

static enum MqErrorE
Filter3 (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_BIN bdy; MQ_SIZE len;
  struct MqS * ftrctx;
  MqErrorCheck (MqConfigGetFilter (mqctx, 0, &ftrctx));

  MqErrorCheck (MqReadBDY (mqctx, &bdy, &len));
  MqErrorCheck1 (MqSendSTART (ftrctx));
  MqErrorCheck1 (MqSendBDY (ftrctx, bdy, len));

  // continue with the original transaction
  if (MqConfigGetIsTrans (mqctx)) {
    // use a transaction protection
    MqErrorCheck1 (MqSendEND_AND_WAIT (ftrctx, MqConfigGetToken(mqctx), MQ_TIMEOUT_USER));
    // read the answer
    MqSendSTART (mqctx);
    MqErrorCheck1 (MqReadBDY (ftrctx, &bdy, &len));
    MqErrorCheck  (MqSendBDY (mqctx, bdy, len));
  } else {
    // use a transaction protection
    MqErrorCheck1 (MqSendEND (ftrctx, MqConfigGetToken(mqctx)));
  }

error:
  return MqSendRETURN(mqctx);

error1:
  MqErrorCopy (mqctx, ftrctx);
  goto error;
}

/*****************************************************************************/
/*                                                                           */
/*                                context_init                               */
/*                                                                           */
/*****************************************************************************/

static enum MqErrorE
Filter3Cleanup (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  return MQ_OK;
}

static enum MqErrorE
ServerSetup (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  struct MqS * ftrctx;
  MqErrorCheck (MqConfigGetFilter (mqctx, 0, &ftrctx));

  // SERVER: listen on every token (+ALL)
  MqErrorCheck (MqServiceCreate (mqctx, "+ALL", Filter3, NULL, NULL));
  // CLIENT: listen on every token (+ALL)
  MqErrorCheck (MqServiceCreate (ftrctx, "+ALL", Filter3, NULL, NULL));

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
  struct MqS * const mqctx = MqContextCreate(sizeof(struct Filter3CtxS), NULL);

  // parse the command-line
  struct MqBufferLS * args = MqBufferLCreateArgs (argc, argv);

  // add config data
  mqctx->setup.Child.fCreate	    = MqDefaultLinkCreate;
  mqctx->setup.Parent.fCreate	    = MqDefaultLinkCreate;
  mqctx->setup.isServer		    = MQ_YES;
  mqctx->setup.ServerSetup.fFunc    = ServerSetup;
  mqctx->setup.ServerCleanup.fFunc  = Filter3Cleanup;
  MqConfigSetDefaultFactory (mqctx);

  // create the ServerCtxS
  MqErrorCheck(MqLinkCreate (mqctx, &args));

  // start event-loop and wait forever
  MqErrorCheck(MqProcessEvent (mqctx, MQ_TIMEOUT, MQ_WAIT_FOREVER));

  // finish and exit
error:
  MqExit (mqctx);
}
