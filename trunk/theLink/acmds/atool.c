/**
 *  \file       theLink/acmds/atool.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

/** \ingroup tools
 *  \defgroup atool atool
 *  \{
 *  \brief atool \atool_desc
 *
 *  \verbinclude atool.help
 */

#include "mqconfig.h"
#include "msgque.h"
#include "debug.h"

#include <stdlib.h>
#include <string.h>

#define MSGQUE msgque

#ifndef DOXYGEN_SHOULD_SKIP_THIS
enum MqErrorE SplitFactory (struct MqS * const, enum MqFactoryE, struct MqFactoryItemS * const , struct MqS **);
enum MqErrorE CutFactory   (struct MqS * const, enum MqFactoryE, struct MqFactoryItemS * const , struct MqS **);
enum MqErrorE SortFactory  (struct MqS * const, enum MqFactoryE, struct MqFactoryItemS * const , struct MqS **);
enum MqErrorE JoinFactory  (struct MqS * const, enum MqFactoryE, struct MqFactoryItemS * const , struct MqS **);
#endif

/*****************************************************************************/
/*                                                                           */
/*                                     main                                  */
/*                                                                           */
/*****************************************************************************/

/// \brief display help using \b -h or \b --help command-line option
/// \param base the executable usually: <tt>basename(argv[0])</tt>
static void
ToolHelp ( const char * base  )
{
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: %s [OPTION]... [ARGUMENT]...\n", base);
    fprintf(stderr, "\n");
    fprintf(stderr, "  %s was designed to act like a filter for command-line\n", base);
    fprintf(stderr, "  tasks with the basic sub-commands:\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  split - a Filter to split the stdin into libmsgque objects\n");
    fprintf(stderr, "  cut   - a Filter to cut libmsgque objects into columns\n");
    fprintf(stderr, "  sort  - a Filter to sort columns\n");
    fprintf(stderr, "  join  - a Filter to join columns into a string and send it to stdout\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  please use \"sub-command -h\" or \"sub-command --help\" for help\n");
    fprintf(stderr, "\n");

    exit(EXIT_SUCCESS);
}

/// \brief main entry-point for the tool
/// \param argc the number of command-line arguments
/// \param argv the command-line arguments as an array of strings
/// \return the exit number
int
main (
  int const argc,
  MQ_CST argv[]
)
{
  // check the command-line argument
  if (argc < 2) {
    ToolHelp(MqSysBasename(argv[0], MQ_NO));
  } else {
    // the parent-context
    struct MqBufferLS * largv = MqBufferLCreateArgs ((argc-1), (argv+1));
    struct MqBufferLS * initB = MqInitCreate();
    struct MqS *ctx = NULL;

    // init libmsgque global data
    MqBufferLAppendC(initB, argv[0]);

    // add Factory 
    MqFactoryPanic (MqFactoryAdd("split", SplitFactory, NULL, NULL, NULL, NULL, NULL));
    MqFactoryPanic (MqFactoryAdd("cut",   CutFactory,   NULL, NULL, NULL, NULL, NULL));
    MqFactoryPanic (MqFactoryAdd("sort",  SortFactory,  NULL, NULL, NULL, NULL, NULL));
    MqFactoryPanic (MqFactoryAdd("join",  JoinFactory,  NULL, NULL, NULL, NULL, NULL));

    // call the initial factory to initialize the "config"
    if (MqFactoryCheckI (MqFactoryCall(argv[1], NULL, &ctx))) {
      ToolHelp(MqSysBasename(argv[0], MQ_NO));
    }

    // call entry point
    MqErrorCheck(MqLinkCreate(ctx, &largv));

    // start event-loop and wait forever
    if (ctx->setup.isServer == MQ_YES)
      MqProcessEvent (ctx, MQ_TIMEOUT, MQ_WAIT_FOREVER);

    // finish
error:
    MqExit(ctx);
  }
}

/** \} atool */
