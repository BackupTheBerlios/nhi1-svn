/**
 *  \file       theBrain/tests/client.c
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
    fputs("  --brain-perf\n",stderr);
    fputs("    Creation of '--num' (default: " MQ_CPPXSTR(NUM_TRANS) ") transactions.\n", stderr);
    fputs("    The data is send from the client to the brain server using four different\n", stderr);
    fputs("    kind of packages:\n", stderr);
    fputs("\n", stderr);
    fputs("      1. an MQ_INT integer between INT_MIN and INT_MAX\n", stderr);
    fputs("      2. an MQ_WID integer between WID_MIN and WID_MAX\n", stderr);
    fputs("      3. a  MQ_DBL double  between DBL_MIN and DBL_MAX\n", stderr);
    fputs("      4. a  MQ_BIN binary  of size between 1 and 1000 bytes\n", stderr);
    fputs("    for saving and finally extract from the database.\n", stderr);
    fputs("\n", stderr);
    fprintf(stderr, "  %s [ARGUMENT]... syntax:\n", base);
    fprintf(stderr, "    %s [OPTION]... %c theBrain [OPTION]... [ARGUMENT]\n", base, MQ_ALFA);
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

  // what should be tested ?
  MQ_BOL brainB;	// test brain PUT/GET time
  MQ_BOL all;		// test all
  MQ_INT num = -1;
#define SIZE 1000
  MQ_BIN data = MqSysMalloc (MQ_ERROR_PANIC, (SIZE));
  MQ_CST db[]	= { "*"		  , "+"		  , "tmp.tch"	, "tmp.tcb"    };
  MQ_CST dbS[]	= { "memory hash" , "memory btree", "file hash"	, "file btree" };

  // make a backup of largv and lalfa for every test

  // create the client context and start the server
  struct MqS * mqctx = MqContextCreate(0, NULL);

  mqctx->setup.fHelp = ClientHelp;

  MqErrorCheck (MqLinkCreate (mqctx, &largv));

  // read application specific arguments
  MqBufferLCheckOptionO (mqctx, largv, "--brain-perf", &brainB);
  MqBufferLCheckOptionO (mqctx, largv, "--all", &all);
  if (all) {
    brainB = MQ_YES;
  } else if (brainB == MQ_NO) {
    brainB = MQ_YES;
  }

  // the user can supply --num to change the number of iterations
  MqBufferLCheckOptionI(mqctx, largv,"--num",&num);

  // check for wrong arguments
  MqErrorCheck (MqCheckForLeftOverArguments(mqctx, &largv));

  // initialize memory, just run one test-case to initialize dynamic data
  memset (data, 'A', SIZE);
  MqSendSTART (mqctx);
  MqSendB (mqctx, data, SIZE);
  MqErrorCheck (MqSendEND_AND_WAIT (mqctx, "_PIN", MQ_TIMEOUT_USER));
  MqErrorCheck (MqReadU (mqctx, &buf));

  // start the brein PUT/GET transaction-performance test
  MqDLogC (mqctx, 0, "start: --------------------------------------\n");

  if (brainB) {

    // if necessary apply the default number of transactions
    MQ_INT lnum = (num == -1 ? NUM_TRANS : num);

    StatTimerSP itemT = StatCreate (mqctx);

    {
      StatCtxSP stat;
      const MQ_BYT stepY = 1;
      const MQ_SRT stepS = ((SHRT_MAX/lnum)*2);
      const MQ_INT stepI = ((INT_MAX/lnum)*2);
      const MQ_WID stepW = ((LLONG_MAX/lnum)*2);
      MQ_BYT valY = SCHAR_MIN;
      MQ_SRT valS = SHRT_MIN;
      MQ_INT valI = INT_MIN;
      MQ_WID valW = LLONG_MIN;
      int i, j; 
      // loop over all databases
      for (j=0; j<4; j++) {
	// open the database
	MqSendSTART (mqctx);
	MqSendV (mqctx, "%s#mode=wct", db[j]);
	MqErrorCheck (MqSendEND_AND_WAIT (mqctx, "AOPN", MQ_TIMEOUT_USER));
	// init the test
	valY = SCHAR_MIN;
	valS = SHRT_MIN;
	valI = INT_MIN;
	valW = LLONG_MIN;
	stat = StatCtxCreate (mqctx, MqBufferSetV(mqctx->temp,"%s - PUT", dbS[j])->cur.C, 0);
	StatInit (itemT);
	// save data uset APUT
	  for (i=0; i<lnum; i++) {
	    MqSendSTART (mqctx);
	    MqSendI (mqctx, i);
	    MqSendY (mqctx, valY);
	    MqSendI (mqctx, i+lnum);
	    MqSendS (mqctx, valS);
	    MqSendI (mqctx, i+2*lnum);
	    MqSendI (mqctx, valI);
	    MqSendI (mqctx, i+3*lnum);
	    MqSendW (mqctx, valW);
	    MqSendI (mqctx, i+4*lnum);
	    MqSendB (mqctx, data, ((i%SIZE)+1));
	    MqErrorCheck (MqSendEND_AND_WAIT (mqctx, "APUT", MQ_TIMEOUT_USER));
	    valY += stepY;
	    valS += stepS;
	    valI += stepI;
	    valW += stepW;
	  };
	StatCtxCalc (stat, itemT);
	stat->val /= lnum;
	StatCtxPrint (stat);
	// init the test
	valY = SCHAR_MIN;
	valS = SHRT_MIN;
	valI = INT_MIN;
	valW = LLONG_MIN;
	stat = StatCtxCreate (mqctx, MqBufferSetV(mqctx->temp,"%s - GET", dbS[j])->cur.C, 0);
	StatInit (itemT);
	// read data uset AGET
	  for (i=0; i<lnum; i++) {
	    MqSendSTART (mqctx);
	    MqSendI (mqctx, i);
	    MqSendI (mqctx, i+lnum);
	    MqSendI (mqctx, i+2*lnum);
	    MqSendI (mqctx, i+3*lnum);
	    MqSendI (mqctx, i+4*lnum);
	    MqErrorCheck (MqSendEND_AND_WAIT (mqctx, "AGET", MQ_TIMEOUT_USER));
	    MqErrorCheck (MqReadY (mqctx, &valY));
	    MqErrorCheck (MqReadS (mqctx, &valS));
	    MqErrorCheck (MqReadI (mqctx, &valI));
	    MqErrorCheck (MqReadW (mqctx, &valW));
	    MqErrorCheck (MqReadU (mqctx, &buf));
	  };
	StatCtxCalc (stat, itemT);
	stat->val /= lnum;
	StatCtxPrint (stat);
	StatCtxDelete (&stat);
	// close the database
	MqSendSTART (mqctx);
	MqErrorCheck (MqSendEND_AND_WAIT (mqctx, "ACLO", MQ_TIMEOUT_USER));
      }
    }

    // cleanup
    StatDelete (&itemT);

  } /* finish the brain PUT/GET test */

// cleanup
  MqSysFree(data);
  MqDLogC (mqctx, 0, "end: ----------------------------------------\n");

// START-C-CLIENT-4
  goto exit;

error:
  MqErrorSAppendC(mqctx, "use '-h' or '--help' for usage");

exit:
  MqBufferLDelete(&largv);
  MqExit (mqctx);
}
// END-C-CLIENT-4

/** \} client */



