/**
 *  \file       theLink/example/c/Filter3.c
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

/// link to the MqErrorS object
#define ARGS  struct MqS * const mqctx, void *data
#define MQ_CONTEXT_S mqctx

/*****************************************************************************/
/*                                                                           */
/*                                context_init                               */
/*                                                                           */
/*****************************************************************************/

static enum MqErrorE
ServerSetup (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  struct MqS * ftrctx = MqServiceGetFilter (mqctx, 0);
  if (ftrctx == NULL) goto error;

  // SERVER: listen on every token (+ALL)
  MqErrorCheck (MqServiceProxy  (mqctx, "+ALL", 0));
  MqErrorCheck (MqServiceProxy  (mqctx, "+TRT", 0));
  // CLIENT: listen on every token (+ALL)
  MqErrorCheck (MqServiceProxy  (ftrctx, "+ALL", 0));
  MqErrorCheck (MqServiceProxy  (ftrctx, "+TRT", 0));

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
  mqctx->setup.isServer		    = MQ_YES;
  mqctx->setup.ServerSetup.fCall    = ServerSetup;
  MqConfigSetDefaultFactory (mqctx);
  MqConfigSetName (mqctx, "Filter3");

  // create the ServerCtxS
  MqErrorCheck(MqLinkCreate (mqctx, &args));

  // start event-loop and wait forever
  MqErrorCheck(MqProcessEvent (mqctx, MQ_TIMEOUT, MQ_WAIT_FOREVER));

  // finish and exit
error:
  MqExit (mqctx);
}

