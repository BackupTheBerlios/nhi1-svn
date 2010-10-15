/**
 *  \file       theLink/tests/client.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

/** \ingroup validation
 *  \defgroup client client
 *  \{
 *  \brief client \client_desc
 *
 *  \verbinclude client.help
 */

#include "mqconfig.h"
#include "msgque.h"
#include "stat.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>

#define NUM_TRANS   50000
#define NUM_PARENT  100
#define NUM_CHILD   1000

/*****************************************************************************/
/*                                                                           */
/*                                     main                                  */
/*                                                                           */
/*****************************************************************************/

static void ClientHelp ( const char * ) __attribute__ ((noreturn));

static MQ_INT callnum = 0;

static enum MqErrorE 
RET_ECUL
(
  struct MqS * const mqctx,
  MQ_CST prefix,
  MQ_PTR data
) {
  MQ_BYT valY;
  MQ_SRT valS;
  MQ_INT valI;
  MQ_WID valW;
  MQ_BUF buf;
  callnum++;
  MqErrorCheck (MqReadY (mqctx, &valY));
  MqErrorCheck (MqReadS (mqctx, &valS));
  MqErrorCheck (MqReadI (mqctx, &valI));
  MqErrorCheck (MqReadW (mqctx, &valW));
  MqErrorCheck (MqReadU (mqctx, &buf));
  return MQ_OK;

error:
  return MqErrorStack(mqctx);
}

/// \brief display help using \b -h or \b --help command-line option
/// \param base the executable usually: <tt>basename(argv[0])</tt>
static void
ClientHelp ( const char * base  )
{
    fprintf(stderr, "usage: %s [OPTION]... [ARGUMENT]...\n", base);
    fputs("\n", stderr);
    fputs("  This tool is the client part of the performance test toolkit and expect\n", stderr);
    fputs("  the libmsgque performance test and validation 'server' as argument.\n", stderr);
    fputs("  The following test are defined:\n",stderr);
    fputs("\n",stderr);
    fputs("  --all\n",stderr);
    fputs("    Do all the following tests.\n", stderr);
    fputs("\n",stderr);
    fputs("  --send-perf\n",stderr);
    fputs("    Creation of '--num' (default: " MQ_CPPXSTR(NUM_TRANS) ") transactions.\n", stderr);
    fputs("    The data is send from the client to the server using four different\n", stderr);
    fputs("    kind of packages:\n", stderr);
    fputs("\n", stderr);
    fputs("      1. an MQ_INT integer between INT_MIN and INT_MAX\n", stderr);
    fputs("      2. an MQ_WID integer between WID_MIN and WID_MAX\n", stderr);
    fputs("      3. a  MQ_DBL double  between DBL_MIN and DBL_MAX\n", stderr);
    fputs("      4. a  MQ_BIN binary  of size between 1 and 1000 bytes\n", stderr);
    fputs("\n", stderr);
    fputs("  --send-and-wait-perf  (this is the DEFAULT)\n",stderr);
    fputs("    Same as '--send-perf' but use 'MqSendEND_AND_WAIT'\n", stderr);
    fputs("\n", stderr);
    fputs("  --send-and-callback-perf\n",stderr);
    fputs("    Same as '--send-perf' but use 'MqSendEND_AND_CALLBACK'\n", stderr);
    fputs("\n", stderr);
    fputs("  --parent-perf\n",stderr);
    fputs("    Creation of '--num' (default: " MQ_CPPXSTR(NUM_PARENT) ") parent-context\n", stderr);
    fputs("\n", stderr);
    fputs("  --child-perf\n",stderr);
    fputs("    Creation of '--num' (default: " MQ_CPPXSTR(NUM_CHILD) ") child-context\n", stderr);
    fputs("\n", stderr);
    fprintf(stderr, "  %s [ARGUMENT]... syntax:\n", base);
    fprintf(stderr, "    %s [OPTION]... %c server [OPTION]... [ARGUMENT]\n", base, MQ_ALFA);
    fputs("\n", stderr);
    fputs(MqHelp (NULL), stderr);
    fputs("\n", stderr);
    fprintf(stderr,"  %s [OPTION]:\n", base);
    fputs("    --num NUMBER     number of transactions\n", stderr);
    fputs("    -h, --help       print this help\n", stderr);
    fputs("\n", stderr);

    exit(EXIT_SUCCESS);
}

// help doxygen to build external references to the "main" proc of a tool
#define ClientMain main

// START-C-CLIENT-1
/// \brief main entry-point for the tool
/// \param argc the number of command-line arguments
/// \param argv the command-line arguments as an array of strings
/// \return the exit number
int
ClientMain (
  int argc,
  MQ_CST argv[]
)
{
  struct MqBufferS * buf;			// a object for return values
  // the commandline-arguments (before and after the first MQ_ALFA)
  struct MqBufferLS * largv = MqBufferLCreateArgs(argc, argv);
  struct MqBufferLS * parentArgv = MqBufferLDup(largv);

  // what should be tested ?
  MQ_BOL sendB;		// test MqSendEND time?
  MQ_BOL sendAndWait;	// test MqSendEND_AND_WAIT round-trip time?
  MQ_BOL sendAndCall;	// test MqSendEND_AND_CALLBACK round-trip time?
  MQ_BOL parent;	// test parent-context creation time?
  MQ_BOL child;		// test child-context creation time?
  MQ_BOL all;		// test all
  MQ_INT num = -1;
#define SIZE 1000
  MQ_BIN data = MqSysMalloc (MQ_ERROR_PANIC, (SIZE));

  // make a backup of largv and lalfa for every test

  // create the client context and start the server
  struct MqS * mqctx = MqContextCreate(0, NULL);

  mqctx->setup.fHelp = ClientHelp;

  MqErrorCheck (MqLinkCreate (mqctx, &largv));

  // read application specific arguments
  MqBufferLCheckOptionO (mqctx, largv, "--send-perf", &sendB);
  MqBufferLCheckOptionO (mqctx, largv, "--send-and-wait-perf", &sendAndWait);
  MqBufferLCheckOptionO (mqctx, largv, "--send-and-callback-perf", &sendAndCall);
  MqBufferLCheckOptionO (mqctx, largv, "--parent-perf", &parent);
  MqBufferLCheckOptionO (mqctx, largv, "--child-perf", &child);
  MqBufferLCheckOptionO (mqctx, largv, "--all", &all);
  if (all) {
    sendB = sendAndWait = sendAndCall = parent = child = MQ_YES;
  } else if (sendB == MQ_NO && sendAndWait == MQ_NO && sendAndCall == MQ_NO
		&& parent == MQ_NO && child == MQ_NO ) {
    sendAndWait = MQ_YES;
  }

  // the user can supply --num to change the number of iterations
  MqBufferLCheckOptionI(mqctx, largv,"--num",&num);

  // check for wrong arguments
  MqErrorCheck (MqCheckForLeftOverArguments(mqctx, &largv));

  // initialize memory, just run one test-case to initialize dynamic data
  memset (data, 'A', SIZE);
  MqSendSTART (mqctx);
  MqSendB (mqctx, data, SIZE);
  MqErrorCheck (MqSendEND_AND_WAIT (mqctx, "ECOU", MQ_TIMEOUT10));
  MqErrorCheck (MqReadU (mqctx, &buf));

  // start the MqSendEND_AND_WAIT transaction-performance test
  MqDLogC (mqctx, 0, "start: --------------------------------------\n");

  if (sendB) {
    // if necessary apply the default number of transactions
    MQ_INT lnum = (num == -1 ? NUM_TRANS : num);

    StatTimerSP itemT = StatCreate (mqctx);

    {
      StatCtxSP stat = StatCtxCreate (mqctx, "MqSendEND", 0);
      const MQ_BYT stepY = 1;
      const MQ_SRT stepS = ((SHRT_MAX/lnum)*2);
      const MQ_INT stepI = ((INT_MAX/lnum)*2);
      const MQ_WID stepW = ((LLONG_MAX/lnum)*2);
      MQ_BYT valY = SCHAR_MIN;
      MQ_SRT valS = SHRT_MIN;
      MQ_INT valI = INT_MIN;
      MQ_WID valW = LLONG_MIN;
      int i; 
      StatInit (itemT);
	for (i=0; i<lnum; i++) {
	  MqSendSTART (mqctx);
	  MqSendY (mqctx, valY);
	  MqSendS (mqctx, valS);
	  MqSendI (mqctx, valI);
	  MqSendW (mqctx, valW);
	  MqSendB (mqctx, data, ((i%SIZE)+1));
	  MqErrorCheck (MqSendEND (mqctx, "RDUL"));
	  valY += stepY;
	  valS += stepS;
	  valI += stepI;
	  valW += stepW;
	};
	// just sync with the server
	MqSendSTART (mqctx);
	MqSendI(mqctx, valI);
	MqErrorCheck (MqSendEND_AND_WAIT (mqctx, "ECOI", 10));
	MqErrorCheck (MqReadI(mqctx,&valI));
      StatCtxCalc (stat, itemT);
      stat->val /= lnum;
      StatCtxPrint (stat);
      StatCtxDelete (&stat);
    }

    // cleanup
    StatDelete (&itemT);

  } /* finish the MqSendEND test */

  if (sendAndCall) {
    // if necessary apply the default number of transactions
    MQ_INT lnum = (num == -1 ? NUM_TRANS : num);

    StatTimerSP itemT = StatCreate (mqctx);

    {
      StatCtxSP stat = StatCtxCreate (mqctx, "MqSendEND_AND_CALLBACK", 0);
      const MQ_BYT stepY = 1;
      const MQ_SRT stepS = ((SHRT_MAX/lnum)*2);
      const MQ_INT stepI = ((INT_MAX/lnum)*2);
      const MQ_WID stepW = ((LLONG_MAX/lnum)*2);
      MQ_BYT valY = SCHAR_MIN;
      MQ_SRT valS = SHRT_MIN;
      MQ_INT valI = INT_MIN;
      MQ_WID valW = LLONG_MIN;
      int i; 
      StatInit (itemT);
	for (i=0; i<lnum; i++) {
	  MqSendSTART (mqctx);
	  MqSendY (mqctx, valY);
	  MqSendS (mqctx, valS);
	  MqSendI (mqctx, valI);
	  MqSendW (mqctx, valW);
	  MqSendB (mqctx, data, ((i%SIZE)+1));
	  MqErrorCheck (MqSendEND_AND_CALLBACK (mqctx, "ECUL", RET_ECUL, NULL, NULL));
	  valY += stepY;
	  valS += stepS;
	  valI += stepI;
	  valW += stepW;
	  // don't flood the socket buffer with unread messages
	  if ((i % 7) == 0) {
	    while (MqProcessEvent(mqctx, 3, MQ_WAIT_NO) == MQ_OK);
	  }
	  MqErrorCheck(MqErrorGetCode(mqctx));
	};

      // wait untill all callbacks are processed
      while (callnum != lnum)
	MqErrorCheck(MqProcessEvent(mqctx, 3, MQ_WAIT_ONCE));

      StatCtxCalc (stat, itemT);
      stat->val /= lnum;
      StatCtxPrint (stat);
      StatCtxDelete (&stat);
    }

    // cleanup
    StatDelete (&itemT);

    MqSysSleep (MQ_ERROR_IGNORE, 2);

  } /* finish the MqSendEND_AND_CALLBACK test */

  if (sendAndWait) {

    // if necessary apply the default number of transactions
    MQ_INT lnum = (num == -1 ? NUM_TRANS : num);

    StatTimerSP itemT = StatCreate (mqctx);

    {
      StatCtxSP stat = StatCtxCreate (mqctx, "MqSendEND_AND_WAIT", 0);
      const MQ_BYT stepY = 1;
      const MQ_SRT stepS = ((SHRT_MAX/lnum)*2);
      const MQ_INT stepI = ((INT_MAX/lnum)*2);
      const MQ_WID stepW = ((LLONG_MAX/lnum)*2);
      MQ_BYT valY = SCHAR_MIN;
      MQ_SRT valS = SHRT_MIN;
      MQ_INT valI = INT_MIN;
      MQ_WID valW = LLONG_MIN;
      MQ_BUF buf;
      int i; 
      StatInit (itemT);
	for (i=0; i<lnum; i++) {
	  MqSendSTART (mqctx);
	  MqSendY (mqctx, valY);
	  MqSendS (mqctx, valS);
	  MqSendI (mqctx, valI);
	  MqSendW (mqctx, valW);
	  MqSendB (mqctx, data, ((i%SIZE)+1));
	  MqErrorCheck (MqSendEND_AND_WAIT (mqctx, "ECUL", MQ_TIMEOUT10));
	  MqErrorCheck (MqReadY (mqctx, &valY));
	  MqErrorCheck (MqReadS (mqctx, &valS));
	  MqErrorCheck (MqReadI (mqctx, &valI));
	  MqErrorCheck (MqReadW (mqctx, &valW));
	  MqErrorCheck (MqReadU (mqctx, &buf));
	  valY += stepY;
	  valS += stepS;
	  valI += stepI;
	  valW += stepW;
	};
      StatCtxCalc (stat, itemT);
      stat->val /= lnum;
      StatCtxPrint (stat);
      StatCtxDelete (&stat);
    }

    // cleanup
    StatDelete (&itemT);

  } /* finish the transaction-performance test */

  // start the parent-context creation test
  if (parent) {
    int n;
    StatTimerSP itemT;

    // if necessary apply the default number of transactions
    MQ_INT lnum = (num == -1 ? NUM_PARENT : num);
    
    struct MqS** msgqueA = (struct MqS**) MqSysMalloc(MQ_ERROR_PANIC, lnum * sizeof(struct MqS*));
    struct MqBufferLS** largvA = (struct MqBufferLS**) MqSysMalloc(MQ_ERROR_PANIC, lnum * sizeof(struct MqBufferLS*));

    // copy the argument vector to create 'num' parent context
    for (n=0; n<lnum; n++) {
      largvA[n] = MqBufferLDup(parentArgv);
    }

    itemT = StatCreate (mqctx);

    {
      // loop to create the parent context
      StatCtxSP stat = StatCtxCreate (mqctx, "parent create", 0);
      StatInit (itemT);
      for (n=0; n<lnum; n++) {
	msgqueA[n] = MqContextCreate(0, mqctx);
	if (MqLinkCreate (msgqueA[n], &largvA[n]) == MQ_ERROR) {
	  MqContextDelete (&mqctx);
	  mqctx = msgqueA[n];
	  lnum = n-1;
	  for (n=0; n < lnum; n++) {
	    MqContextDelete (&msgqueA[n]);
	  }
	  goto error;
	}
	MqBufferLDelete(&largvA[n]);
      }
      StatCtxCalc (stat, itemT);
      stat->val /= lnum;
      StatCtxPrint (stat);
      StatCtxDelete (&stat);

      // loop to delete the parent context
      stat = StatCtxCreate (mqctx, "parent delete", 0);
      StatInit (itemT);
      for (n=0; n<lnum; n++) {
	MqContextDelete (&msgqueA[n]);
      }
      StatCtxCalc (stat, itemT);
      stat->val /= lnum;
      StatCtxPrint (stat);
      StatCtxDelete (&stat);
    }

    // cleanup
    MqSysFree(largvA);
    MqSysFree(msgqueA);
    StatDelete (&itemT);
  }

  // start the child-context creation test
  if (child) {
    int n;
    StatTimerSP itemT;

    // if necessary apply the default number of transactions
    MQ_INT lnum = (num == -1 ? NUM_CHILD : num);
    
    struct MqS** contextA = (struct MqS**) MqSysMalloc(MQ_ERROR_PANIC, lnum * sizeof(struct MqS*));

    // fill template configuration
    struct MqS *template = MqContextCreate(0, NULL);
    MqConfigSetName (template, "child");
    MqConfigSetDebug (template, mqctx->config.debug);

    // copy the argument vector to create 'num' parent context
    itemT = StatCreate (mqctx);

    {
      // loop to create the parent context
      StatCtxSP stat = StatCtxCreate (mqctx, "child create", 0);
      StatInit (itemT);
      for (n=0; n<lnum; n++) {
	contextA[n] = MqContextCreate(0,template);
	if (MqLinkCreateChild (contextA[n], mqctx, NULL) == MQ_ERROR) {
	  MqErrorCopy(mqctx,contextA[n]);
	  goto error;
	}
      }
      StatCtxCalc (stat, itemT);
      stat->val /= lnum;
      StatCtxPrint (stat);
      StatCtxDelete (&stat);

      // loop to delete the parent context
      stat = StatCtxCreate (mqctx, "child delete", 0);
      StatInit (itemT);
      for (n=0; n<lnum; n++) {
	MqContextDelete (&contextA[n]);
      }
      StatCtxCalc (stat, itemT);
      stat->val /= lnum;
      StatCtxPrint (stat);
      StatCtxDelete (&stat);
    }

    // cleanup
    MqContextDelete(&template);
    MqSysFree(contextA);
    StatDelete (&itemT);
  }

// cleanup
  MqSysFree(data);
  MqDLogC (mqctx, 0, "end: ----------------------------------------\n");

// START-C-CLIENT-4
  goto exit;

error:
  MqErrorSAppendC(mqctx, "use '-h' or '--help' for usage");

exit:
  MqBufferLDelete(&largv);
  MqBufferLDelete(&parentArgv);
  MqExit (mqctx);
}
// END-C-CLIENT-4

/** \} client */



