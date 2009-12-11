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
  //SETUP_guard;

  return MQ_OK;
}

static enum MqErrorE
GuardSetup (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  //SETUP_guard;

//error:
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
