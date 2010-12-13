/**
 *  \file       theLink/libmsgque/link.c
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
#include "link.h"
#include "buffer.h"
#include "token.h"
#include "error.h"
#include "sys.h"
#include "bufferL.h"
#include "log.h"
#include "mq_io.h"
#include "send.h"
#include "read.h"
#include "trans.h"
#include "cache.h"
#include "slave.h"
#include "config.h"
#include "factory.h"

//#ifdef HAVE_STRINGS_H
//#  include <strings.h>
//#endif

#define MQ_CONTEXT_S context

#define MQ_TIME ((MQ_TIME_T)time(NULL))

BEGIN_C_DECLS

extern struct MqBufferLS * MqInitBuf;

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

static const MQ_STRB*
sInitGetFirst (void)
{
  return (MqInitBuf && MqInitBuf->cursize > 0 ? MqInitBuf->data[0]->cur.C : "unknown");
}

static MQ_STR
sHelp (void)
{
    MqBufferCreateStatic (buf, 2000);

    MqBufferAppendV (buf, "\n");
    MqBufferAppendV (buf, "'--help-msgque' summarize:\n");
    MqBufferAppendV (buf, "\n");
    MqBufferAppendV (buf, "These are the options for the 'libmsgque' library passed by\n");
    MqBufferAppendV (buf, "the second argument (argv) of 'MqLinkCreate'\n");
    MqBufferAppendV (buf, "\n");
    MqBufferAppendV (buf, " msgque wrapper [OPTION]:\n");
    MqBufferAppendV (buf, "  --name str         the display name of the msgque      (default: executable)\n");
    MqBufferAppendV (buf, "  --srvname str      the display name of the server      (default: executable)\n");
    MqBufferAppendV (buf, "  --string           use string based protocol           (default: no=binary)\n");
    MqBufferAppendV (buf, "  --silent           use silent mode                     (default: no)\n");
    MqBufferAppendV (buf, "  --debug #          use debug level (0-9)               (default: 0)\n");
    MqBufferAppendV (buf, "  --daemon pidfile   start as daemon and save the pid    (default: no)\n");
    MqBufferAppendV (buf, "\n");
    MqBufferAppendV (buf, " generic tcp/uds io [OPTION]:\n");
    MqBufferAppendV (buf, "  --timeout #        timeout for connect                 (default: %i sec)\n", MQ_TIMEOUT90);
    MqBufferAppendV (buf, "  --buffersize #     buffer size in bytes                (default: 4096)\n");
    MqBufferAppendV (buf, "\n");
    MqBufferAppendV (buf, " create a new entity as [OPTION]:\n");
    MqBufferAppendV (buf, "  --fork             new process using 'fork'            (default: no)\n");
    MqBufferAppendV (buf, "  --thread           new thread                          (default: no)\n");
    MqBufferAppendV (buf, "  --spawn            new process using 'spawn'           (default: no)\n");
    MqBufferAppendV (buf, "\n");
    MqBufferAppendV (buf, " tcp specific [OPTION]:\n");
    MqBufferAppendV (buf, "  --tcp              use a Tcp Sockets                   (default: no)\n");
    MqBufferAppendV (buf, "   --host str        use 'str' as remote host name       (default: empty)\n");
    MqBufferAppendV (buf, "   --port str        use 'str' as remote port name       (default: empty)\n");
    MqBufferAppendV (buf, "   --myhost str      use 'str' as local host name        (default: empty)\n");
    MqBufferAppendV (buf, "   --myport str      use 'str' as local port name        (default: empty)\n");
    MqBufferAppendV (buf, "\n");
    MqBufferAppendV (buf, " uds specific [OPTION]:\n");
    MqBufferAppendV (buf, "  --uds              use Unix Domain Sockets             (default: no)\n");
    MqBufferAppendV (buf, "   --file str        use 'str' as filename               (default: empty)\n");
    MqBufferAppendV (buf, "\n");
    MqBufferAppendV (buf, " pipe specific [OPTION]:\n");
    MqBufferAppendV (buf, "  --pipe             use Socket-Pair Sockets             (default: yes)\n");
    MqBufferAppendV (buf, "\n");

    return (MQ_STR) buf->data;
}

static enum MqErrorE
sWaitForToken (
  struct MqS * const context,
  MQ_TIME_T timeout,
  MQ_CST const str
)
{
  register MQ_TIME_T const startT = time (NULL);

  // 1. check if the token is already available
  if (pTokenCheck(context->link.srvT,str)) 
    return MQ_OK;

  // set the default for timeout
  if (timeout < 0) {
    timeout = (timeout == MQ_TIMEOUT_USER ? pIoGetTimeout(context->link.io) : MQ_TIMEOUT);
  }

  // 2. wait for the token
  do {
    MqDLogV(context,4,"wait for token<%s>\n",str);
    MqErrorSwitch (MqProcessEvent(context,timeout,MQ_WAIT_ONCE));
  } while (!pTokenCheck(context->link.srvT,str) && (MQ_TIME-startT) < timeout);

  MqDLogV(context,4,"got token<%s>\n",pTokenGetCurrent(context->link.srvT));

  if (!pTokenCheck(context->link.srvT,str)) {
    MqErrorDbV (MQ_ERROR_WAIT_FOR_TOKEN, (MQ_TIME-startT), str, pTokenGetCurrent(context->link.srvT));
  }

error:
  return MqErrorStack (context);
}

#define ArgBUF(V,T) \
  MqErrorCheck (MqBufferLDeleteItem (context, argv, idx, 1, MQ_YES)); \
  if (idx >= argv->cursize) return MqErrorDbV (MQ_ERROR_OPTION_ARG, T); \
  MqBufferDelete(&V); \
  V = argv->data[idx]; \
  MqBufferLDeleteItem (context, argv, idx, 1, MQ_NO); \
  idx--;

static enum MqErrorE
sMqCheckArg (
  register struct MqS * const context,
  register struct MqBufferLS * const argv
)
{
  // check to force SLAVE requirements
  if (MQ_IS_SLAVE(context)) {
    context->setup.isServer = MQ_NO;
  }

  // check to force CHILD requirements
  if (MQ_IS_CHILD(context)) {
    context->link.bits.endian = context->config.parent->link.bits.endian;
    MqSysFree (context->link.targetIdent);
    context->link.targetIdent = MqSysStrDup(MQ_ERROR_PANIC, context->config.parent->link.targetIdent);
  }
  // after the block from above 'setup.ident' is always set

  // parse "my" arguments
  if (argv != NULL && argv->cursize > 0) {
    struct MqBufferS *arg = NULL;
    MQ_CST strV, argC;
    MQ_INT intV;
    MQ_WID intW;
    MQ_SIZE idx;
    // allways read the first item from argv as executable-name
    arg = argv->data[0];
    // if "MqMainToolName" is empty -> fill it with a default value
    if (MqInitBuf == NULL) {
      struct MqBufferLS * initB = MqInitCreate();
      MqBufferLAppendC(initB, arg->cur.C);
    }
    // try to figure out a "good" name
    if (context->config.name == NULL) {
      if (arg != NULL) {
	pConfigSetName (context, MqSysBasename (arg->cur.C, MQ_NO));
      } else if (MqInitBuf != NULL) {
	pConfigSetName (context, MqSysBasename (MqInitBuf->data[0]->cur.C, MQ_NO));
      } else {
	MqConfigSetName (context, "unknown");
      }
    }
    // delete the "first" item
    MqErrorCheck (MqBufferLDeleteItem (context, argv, 0, 1, MQ_YES));
    // loop over the arguments until "end"
    for (idx=0; idx<argv->cursize; idx++) {
      arg = argv->data[idx];
      argC = arg->cur.C;

      // 0. test on "-h" prefix
      if ( arg->type == MQ_STRT && arg->cursize == 2 && 
	      argC[0] == '-' && argC[1] == 'h' && context->setup.fHelp) {
	(*context->setup.fHelp)(context->config.name);
	MqErrorCheck (MqBufferLDeleteItem (context, argv, idx, 1, MQ_YES));
	idx--;
	continue;
      // 1. test on "string" and on "--" prefix
      } else if ( arg->type != MQ_STRT || arg->cursize < 5 || argC[0] != '-' || argC[1] != '-') continue;
      argC += 2;

      switch (*argC) {
	case 's': {
	  if (!strncmp(argC, "string", 6)) {
	    MqConfigSetIsString (context, MQ_YES);
	  } else if (!strncmp(argC, "spawn", 5)) {
//printLC("spawn")
	    if (MqInitBuf == NULL) {
	      return MqErrorDb (MQ_ERROR_NO_INIT);
	    } else if (context->config.ignoreSpawn == MQ_YES) {
	      return MqErrorDbV (MQ_ERROR_OPTION_FORBIDDEN, "current", "--spawn");
	    } else {
	      MqConfigSetStartAs (context, MQ_START_SPAWN);
	      if (MQ_IS_SERVER(context) && context->statusIs & MQ_STATUS_IS_DUP) {
		context->statusIs = (enum MqStatusIsE) (context->statusIs | MQ_STATUS_IS_SPAWN);
	      }
	    }
	  } else if (!strncmp(argC, "silent", 5)) {
	    MqConfigSetIsSilent (context, MQ_YES);
	  } else if (!strncmp(argC, "srvname", 7)) {
	    MqErrorCheck (MqBufferLDeleteItem (context, argv, idx, 1, MQ_YES));
	    if (idx >= argv->cursize) return MqErrorDbV (MQ_ERROR_OPTION_ARG, "--srvname");
	    MqErrorCheck (MqBufferGetC(argv->data[idx], &strV));
	    MqConfigSetSrvName (context, strV);
	  } else if (!strncmp(argC, "socket", 6)) {
	    MqErrorCheck (MqBufferLDeleteItem (context, argv, idx, 1, MQ_YES));
	    if (idx >= argv->cursize) return MqErrorDbV (MQ_ERROR_OPTION_ARG, "--socket");
	    MqErrorCheck (MqBufferGetI(argv->data[idx], &context->config.io.pipe.socket[1]));
	  } else {
	    continue;
	  }
	  break;
	}
	case 'd': {
	  if (!strncmp(argC, "daemon", 6)) {
	    // close everything which makes me depend on the calling process
	    MqErrorCheck (MqBufferLDeleteItem (context, argv, idx, 1, MQ_YES));
	    if (idx >= argv->cursize) return MqErrorDbV (MQ_ERROR_OPTION_ARG, "--daemon");
	    MqErrorCheck (MqBufferGetC (argv->data[idx], &strV));
	    MqErrorCheck (MqConfigSetDaemon (context, strV));
	  } else if (!strncmp(argC, "debug", 5)) {
	    MqErrorCheck (MqBufferLDeleteItem (context, argv, idx, 1, MQ_YES));
	    if (idx >= argv->cursize) return MqErrorDbV (MQ_ERROR_OPTION_ARG, "--debug");
	    MqErrorCheck (MqBufferGetI(argv->data[idx], &intV));
	    MqConfigSetDebug (context, intV);
	  } else {
	    continue;
	  }
	  break;
	}
	case 't': {
	  if (!strncmp(argC, "thread", 6)) {
//printLC("thread")
	    if (context->config.ignoreThread == MQ_YES) {
	      return MqErrorDbV (MQ_ERROR_OPTION_FORBIDDEN, "current", "--thread");
	    } else {
	      MqConfigSetStartAs (context, MQ_START_THREAD);
	      if (MQ_IS_SERVER(context) && context->statusIs & MQ_STATUS_IS_DUP)
		context->statusIs = (enum MqStatusIsE) (context->statusIs | MQ_STATUS_IS_THREAD);
	    }
	  } else if (!strncmp(argC, "timeout", 7)) {
	    MqErrorCheck (MqBufferLDeleteItem (context, argv, idx, 1, MQ_YES));
	    if (idx >= argv->cursize) return MqErrorDbV (MQ_ERROR_OPTION_ARG, "--timeout");
	    MqErrorCheck (MqBufferGetW(argv->data[idx], &intW));
	    MqConfigSetTimeout (context, intW);
	  } else if (!strncmp(argC, "tcp", 3)) {
	    context->config.io.com = MQ_IO_TCP;
	  } else {
	    continue;
	  }
	  break;
	}
	case 'n': {
	  if (!strncmp(argC, "name", 4)) {
	    MqErrorCheck (MqBufferLDeleteItem (context, argv, idx, 1, MQ_YES));
	    if (idx >= argv->cursize) return MqErrorDbV (MQ_ERROR_OPTION_ARG, "--name");
	    MqErrorCheck (MqBufferGetC(argv->data[idx], &strV));
	    MqConfigSetName (context, strV);
	  } else {
	    continue;
	  }
	  break;
	}
	case 'b': {
	  if (!strncmp(argC, "buffersize", 10)) {
	    MqErrorCheck (MqBufferLDeleteItem (context, argv, idx, 1, MQ_YES));
	    if (idx >= argv->cursize) return MqErrorDbV (MQ_ERROR_OPTION_ARG, "--buffersize");
	    MqErrorCheck (MqBufferGetI(argv->data[idx], &intV));
	    MqConfigSetBuffersize (context, intV);
	  } else {
	    continue;
	  }
	  break;
	}
	case 'u': {
	  if (!strncmp(argC, "uds", 3)) {
#if defined(MQ_IS_WIN32)
	    return MqErrorDbV2(context,MQ_ERROR_OPTION_FORBIDDEN,"win32","--uds");
#else
	    context->config.io.com = MQ_IO_UDS;
#endif
	  } else {
	    continue;
	  }
	  break;
	}
	case 'p': {
	  if (!strncmp(argC, "pipe", 4)) {
	    context->config.io.com = MQ_IO_PIPE;
	  } else if (!strncmp(argC, "port", 4)) {
	    ArgBUF(context->config.io.tcp.port, "--port");
	    continue;
	  } else {
	    continue;
	  }
	  break;
	}
	case 'm': {
	  if (!strncmp(argC, "myhost", 6)) {
	    ArgBUF(context->config.io.tcp.myhost, "--myhost");
	    continue;
	  } else if (!strncmp(argC, "myport", 6)) {
	    ArgBUF(context->config.io.tcp.myport, "--myport");
	    continue;
	  } else {
	    continue;
	  }
	  break;
	}
	case 'f': {
	  if (!strncmp(argC, "fork", 4)) {
//printLC("fork")
	    if (context->config.ignoreFork == MQ_YES) {
	      return MqErrorDbV (MQ_ERROR_OPTION_FORBIDDEN, "current", "--fork");
	    } else {
	      MqConfigSetStartAs (context, MQ_START_FORK);
	      if (MQ_IS_SERVER(context) && context->statusIs & MQ_STATUS_IS_DUP)
		context->statusIs = (enum MqStatusIsE) (context->statusIs | MQ_STATUS_IS_FORK);
	    }
	  } else if (!strncmp(argC, "file", 4)) {
	    ArgBUF(context->config.io.uds.file, "--file");
	    continue;
	  } else {
	    continue;
	  }
	  break;
	}
	case '-': {
	  if (!strncmp(argC, "-duplicate", 10)) {
	    context->statusIs = (enum MqStatusIsE) (context->statusIs | MQ_STATUS_IS_DUP);
	    context->config.startAs = MQ_START_DEFAULT;
	    context->config.io.com = MQ_IO_PIPE;
	  } else if (!strncmp(argC, "-threadData", 11)) {
	    MQ_WID tmp;
	    MqErrorCheck (MqBufferLDeleteItem (context, argv, idx, 1, MQ_YES));
	    MqErrorCheck(MqBufferGetW(argv->data[idx], &tmp));
	    context->threadData = (MQ_PTR)tmp;
	  } else {
	    continue;
	  }
	  break;
	}
	case 'h': {
	  if (!strncmp(argC, "host", 4)) {
	    ArgBUF(context->config.io.tcp.host, "--host");
	    continue;
	  } else if (!strncmp(argC, "help-msgque", 11)) {
	    fputs(sHelp(),stderr);
	    MqSysExit(0,EXIT_SUCCESS);
	  } else if (!strncmp(argC, "help", 4)) {
	    if (context->setup.fHelp) {
	      (*context->setup.fHelp)(context->config.name);
	    }
	  } else {
	    continue;
	  }
	  break;
	}
	default:
	  continue;
      }
      MqErrorCheck (MqBufferLDeleteItem (context, argv, idx, 1, MQ_YES));
      idx--;
    }
  } else if (context->config.name == NULL) {
    // The "--name" is used in debugging/error messages as prefix and
    // should be available as soon as possible.
    // The first argument should be the name of the executable and
    // will be used to identify the socket if no '--name' option is available
    MqConfigSetName (context, "unknown");
  }

error:
  return MqErrorStack(context);
}

void
pMqShutdown (
  struct MqS * const context,
  MQ_CST prefix
)
{
  if (context->link.bits.onShutdown == MQ_YES) {
    return;
  } else {
    struct pChildS * child;

    MqDLogV(context,4,"START - called from '%s'\n",prefix);
    context->link.bits.onShutdown = MQ_YES;

    // shutdown all childs
    for (child = context->link.childs; child != NULL; child = child->right ) {
      // the next line is necessary because the item "context->link.childs->context" and
      // "context->link.childs->context->link.self->context" are identical. The last one is 
      // free'd during the following "MqLinkDelete" too and a double-free error
      // will happen
      pMqShutdown (child->context,__func__);
    }

    // shutdown all slaves
    if (context->setup.ignoreExit == MQ_NO) {
      pSlaveShutdown (context->link.slave);
    }

    // send message to shutdown the 'server' 
    // -> but only if the server is available -> pIoCheck
    if (pIoCheck (context->link.io)) {
      if (MQ_IS_CLIENT (context)) {
	MqDLogC(context,4,"send token<_SHD>\n");
	MqSendSTART (context);
	MqSendEND_AND_WAIT (context, "_SHD", MQ_TIMEOUT_USER);
      } else if (MQ_IS_SERVER (context) && context->link._trans != 0 &&
	  pTokenCheck(context->link.srvT,"_SHD")) {
	// return the "_SHD"
	MqSendSTART(context);
	MqSendRETURN(context);
      }
    }

    // shutdown the io
    pIoShutdown (context->link.io);

    // cleanup error
    pErrorReset(context);
    MqDLogC(context,4,"END shutdown\n");
    return;
  }
}

/*****************************************************************************/
/*                                                                           */
/*                             create / delete                               */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
MqLinkCreateChild (
  struct MqS * const context,
  struct MqS * const parent,
  struct MqBufferLS ** argvP
)
{
  // 'parent' not connected
  if (unlikely(parent == NULL)) {
    MqBufferLDelete (argvP);
    return MqErrorDbV(MQ_ERROR_CONNECTED, "parent", "not");
  }
  MqConfigDup(context, parent);
  pConfigSetParent(context, parent);
  return MqLinkCreate (context, argvP);
}

static enum MqErrorE
MqLinkPrepare (
  struct MqS * const context,
  struct MqBufferLS ** argvP
)
{
  // only prepare once
  if (context->link.bits.prepareDone == MQ_YES) {
    return MQ_OK;
  } else {
    struct MqBufferLS *argv = (argvP != NULL ? *argvP : NULL);
    context->link.bits.prepareDone = MQ_YES;

    // pointers are now owned by MqLinkS
    pBufferLSplitAlfa (&argv, &context->link.alfa);

    // 1. set the error object of the arguments
    pBufferLSetError(argv, context);
    pBufferLSetError(context->link.alfa, context);

    // 2. parse arguments
    if (MqErrorCheckI (sMqCheckArg (context, argv))) {
      MqBufferLDelete (&argv);
    }

    // 3. cleanup the pointer's
    if (argv != NULL && argv->cursize == 0) {
      MqBufferLDelete(&argv);
      if (argvP != NULL) *argvP = NULL;
    } else if (argv == NULL) {
      if (argvP != NULL) *argvP = NULL;
    }

    // 4. send message
    if (context->config.debug >= 2) {
      if (MQ_IS_SLAVE(context)) {
	MqLogV(context,__func__,2,"PREPARE-SLAVE: master<%p>\n", (void*) context->config.master);
      } else if (MQ_IS_PARENT(context)) {
	MqLogV(context,__func__,2,"PREPARE: io->com<%s>\n", pIoLogCom(context->config.io.com));
      } else {
	MqLogV(context,__func__,2,"PREPARE-CHILD: parent<%p>\n", (void*) context->config.parent);
      }
    }
    return MqErrorStack(context);
  }
}

enum MqErrorE
MqLinkConnect (
  struct MqS * const context
)
{
  MQ_BOL check = pIoCheck(context->link.io);
  if (check == MQ_YES && context->link.bits.isConnected == MQ_YES) {
    return MQ_OK;
  } else if (context->link.bits.onCreateStart == MQ_NO) {
    return MqErrorDb(MQ_ERROR_LINK_CREATE);
  } else {
    MQ_STR serverexec = NULL;
    struct pChildS *child;
    MQ_INT MqSetDebugLevel(context);

    // do a "re-connect" starting with the "initial-parent"
    if (context->link.io != NULL && check == MQ_NO && MQ_IS_CHILD (context)) {
      // if entry-point is the "child" and the "initial-parent"
      // (reponsible for the data communication) is not available
      // "connect" the parent and the child will be available too
      if (MqErrorCheckI (MqLinkConnect (context->link.ctxIdP))) {
	return MqErrorCopy (context, context->link.ctxIdP);
      }
      return MqErrorGetCodeI(context);
    }

    // create the "io"
    MqErrorCheck (pIoCreate (context, &context->link.io));
    MqDLogCL(context,4,"START\n");

    if (MQ_IS_CHILD (context)) {
    // this is a CHILD
      register struct MqS * const parent = context->config.parent;

      // PARENT, sending using the DATA of the CHILD as argument
      MqSendSTART (parent);
      MqSendI (parent, (context->config.debug == 0 ? -1 : context->config.debug));
      MqSendI (parent, (MQ_INT) context->config.isSilent);
      if (context->config.srvname != NULL) {
	MqSendC (parent, context->config.srvname);
      }

      // !!Attention wrong error (from the PARENT because the PARENT starts the CHILD on the SERVER)
      MqDLogCL(context,4,"send token<_OKS>\n");
      if (MqErrorCheckI (MqSendEND_AND_WAIT (parent, "_OKS", MQ_TIMEOUT_USER))) {
	MqErrorCopy (context, parent);
	pIoCloseSocket (context->link.io, __func__);
	goto error;
      }

    } else {
    // this is a PARENT

      MQ_CST ident;
      MQ_BOL mystring;
      MQ_BOL myendian;

      if (pIoIsRemote(context->link.io) == MQ_YES) {
	serverexec = MqSysStrDup(MQ_ERROR_PANIC, "remote-connect");
      } else {
	if (context->link.alfa != NULL)
	  serverexec = MqSysStrDup(MQ_ERROR_PANIC, context->link.alfa->data[0]->cur.C);
	if (serverexec == NULL)
	  serverexec = MqSysStrDup(MQ_ERROR_PANIC, sInitGetFirst());
      }

      // 0. Start the PIPE server (if necessary)
      MqErrorCheck (pIoStartServer(context->link.io, MQ_START_SERVER_AS_PIPE, NULL, NULL));

      // 1. connect to the server
      MqErrorCheck (pIoConnect (context->link.io));

      // 2. exchange startup messages
      //      - allways send as string

      // save my original binary-mode
      mystring = context->config.isString;

      // for '_IAA' we only use 'string' mode
      MqConfigSetIsString (context, MQ_YES);

      MqSendSTART (context);

      // send the binary mode
      MqSendO (context, mystring);

      // send my endian
#     if defined(WORDS_BIGENDIAN)
	MqSendO (context, MQ_YES);
#     else
	MqSendO (context, MQ_NO);
#     endif

      /// send my ident
      MqSendC (context, context->setup.ident);

      // send the server name
      MqSendC (context, context->config.srvname);

      // send package and wait for the answer
      MqDLogCL(context,4,"send token<_IAA>\n");
      if (MqErrorCheckI(MqSendEND_AND_WAIT (context, "_IAA", MQ_TIMEOUT_USER))) {
	MqErrorReset(context);
	MqErrorDbV2 (context,MQ_ERROR_CAN_NOT_START_SERVER, serverexec);
	goto error;
      }

      // read the other endian and set my context->link.bits.endian
      MqReadO(context, &myendian);

      // read the target ident
      MqReadC(context, &ident);
      MqSysFree(context->link.targetIdent);
      context->link.targetIdent = MqSysStrDup(MQ_ERROR_PANIC, ident);

#     if defined(WORDS_BIGENDIAN)
	context->link.bits.endian = (myendian ? MQ_NO : MQ_YES);
#     else
	context->link.bits.endian = (myendian ? MQ_YES : MQ_NO);
#     endif

      // restore the binary mode
      MqConfigSetIsString (context, mystring);

      // wait until the server is able to process events (and send _PEO)
      MqErrorCheck (sWaitForToken (context, MQ_TIMEOUT_USER, "_PEO"));
    }; // END PARENT

    // connect child's
    for (child = context->link.childs; child != NULL; child=child->right) {
      struct MqS *cldCtx = child->context;
      if (MqErrorCheckI (MqLinkConnect (cldCtx))) {
	MqErrorCopy(context, cldCtx);
	goto error;
      }
    }

    // change into "connected"
    MqDLogCL(context,4,"END\n");
    context->link.bits.isConnected = MQ_YES;
    goto end;

error:
    pIoDelete (&context->link.io);

end:
    MqSysFree (serverexec);
    return MqErrorStack(context);
  }
}

enum MqErrorE
MqLinkCreate (
  struct MqS * const context,
  struct MqBufferLS ** argvP
)
{
  // avoid double link create
  if (unlikely(context->link.read != NULL)) {
    MqBufferLDelete (argvP);
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "already");
  }

  // 'parent' not connected
  if (unlikely(context->config.parent != NULL && context->config.parent->link.read == NULL)) {
    MqBufferLDelete (argvP);
    return MqErrorDbV(MQ_ERROR_CONNECTED, "parent", "not");
  }

  // 'master' not connected, error if MQ_FACTORY_NEW_FILTER is not set
  if (unlikely(context->config.master != NULL && context->config.master->link.read == NULL)) {
    MqBufferLDelete (argvP);
    return MqErrorDbV(MQ_ERROR_CONNECTED, "master", "not");
  }

  // try to invoke the "CreateHandler" first
  if (context->link.bits.onCreateStart == MQ_NO) {
    context->link.bits.onCreateStart = MQ_YES;
    context->link.bits.onCreateEnd = MQ_NO;
    if (context->config.parent != NULL && context->setup.Child.fCreate != NULL) {
      enum MqErrorE ret;
      context->refCount++;
      ret = (*context->setup.Child.fCreate) (context, argvP);
      context->refCount--;
      return ret;
    } else if (context->config.parent == NULL && context->setup.Parent.fCreate != NULL) {
      enum MqErrorE ret;
      context->refCount++;
      ret = (*context->setup.Parent.fCreate) (context, argvP);
      context->refCount--;
      return ret;
    }
  }

  // just create the client/server link now.
  {
    MqErrorReset(context);

    // 2. parse the command-line arguments
    MqErrorCheck (MqLinkPrepare (context, argvP));
    if (context->config.debug >= 2) {
      if (MQ_IS_SLAVE(context)) {
	MqLogV(context,__func__,2,"START-SLAVE: master<%p>\n", (void*) context->config.master);
      } else if (MQ_IS_PARENT(context)) {
	MqLogV(context,__func__,2,"START: io->com<%s>\n", pIoLogCom(context->config.io.com));
      } else {
	MqLogV(context,__func__,2,"START-CHILD: parent<%p>\n", (void*) context->config.parent);
      }
    }

    // handle empty MqBufferLS list alfa
    // after this line "alfa" pointer is NULL (-> no data) or "alfa" ponter != NULL (-> has data)
    if (context->link.alfa != NULL) {
      // we have a "master", a "worker" have to be created
      if (context->link.alfa->cursize == 0) {
	MqBufferLDelete (&context->link.alfa);
      } else {
	if (!strncmp(context->link.alfa->data[0]->cur.C, "SELF", 4)) {
	  MqBufferLDeleteItem (MQ_ERROR_PANIC, context->link.alfa, 0, 1, MQ_YES);
	  if (	context->setup.factory != NULL &&  // "factory" available ?
		  context->setup.factory->Create.fCall != NULL  // constructor available ?
	      ) {
	    MqBufferLAppend (context->link.alfa,MqBufferCreateC (MQ_ERROR_PANIC, context->setup.factory->name), 0);
	  } else {
	    MqBufferLAppendL(context->link.alfa,MqInitBuf,0);
	  }
	} else if (!strncmp(context->link.alfa->data[0]->cur.C, "WORKER", 6)) {
	  context->link.bits.isWORKER = MQ_YES;
	}
      }
    }

    // create my ctxId's
    if (MQ_IS_PARENT (context)) {
      context->link.ctxIdA	= (struct MqS **) MqSysCalloc (MQ_ERROR_PANIC, 100, sizeof(context));
      context->link.ctxIdZ	= 100;
      context->link.ctxId	= 0;
      context->link.ctxIdA[0]	= context;
      context->link.ctxIdP	= context;
      context->link.ctxIdR	= 0;
    } else {
      // create and add my contextID to the initial 'parent' contextId storage
      register struct MqS * ctxIdP = context->link.ctxIdP = context->config.parent->link.ctxIdP;
      struct pChildS * link;
      struct pChildS * old;
      ctxIdP->link.ctxIdR++;
      context->link.ctxId = ctxIdP->link.ctxIdR;
      if (ctxIdP->link.ctxIdR >= ctxIdP->link.ctxIdZ) {
	ctxIdP->link.ctxIdA = (struct MqS **) MqSysRealloc (MQ_ERROR_PANIC, 
	  ctxIdP->link.ctxIdA, (ctxIdP->link.ctxIdZ + 100) * sizeof(context));
	memset (ctxIdP->link.ctxIdA + ctxIdP->link.ctxIdZ, '\0', 100 * sizeof(context));
	ctxIdP->link.ctxIdZ += 100;
      }
      ctxIdP->link.ctxIdA[context->link.ctxId] = context;

      // add myself as 'child' of parent
      link = (struct pChildS *) MqSysMalloc (MQ_ERROR_PANIC, sizeof (struct pChildS));
      old  = context->config.parent->link.childs;
      if (old != NULL) old->left = link;
      link->right  = old;
      link->left   = NULL;
      link->context = context;
      context->config.parent->link.childs = link;
      context->link.self = link;
    }

    // create private data
    MqErrorCheck (pSendCreate (context, &context->link.send));

    // pReadCreate have to fit into the 'CacheCF' function
    context->link.read = pReadCreate (context);
    if (MqErrorGetCodeI(context) == MQ_ERROR) goto error;

    context->link.srvT = pTokenCreate(context);
    MqErrorCheck (pTransCreate (context, &context->link.trans));

    // create the cache data
    pCacheCreate ((CacheCF) pReadCreate, (CacheDF) pReadDelete, context, &context->link.readCache);

    // ready to create the SLAVE objects
    MqErrorCheck (pSlaveCreate (context, &context->link.slave));

    // context specific initialization
    if (MQ_IS_CLIENT (context)) {   // CLIENT

      // client "connect" to the "server"
      MqErrorCheck (MqLinkConnect (context));

    } else {			    // SERVER

      // create the communication
      MqErrorCheck (pIoCreate   (context, &context->link.io));

      // we have to wait for the "_IAA" message of the "client" to set "context->config.debug" proper
      if (MQ_IS_PARENT (context)) {

	// the "server" allways default to "string", because the "client" set the
	// default in the "_IAA" message
	MqConfigSetIsString (context, MQ_YES);
	MqErrorCheck (sWaitForToken (context, MQ_TIMEOUT_USER, "_IAA"));

	// test on "filter" (alfa!=NULL) or a normal "server" (alfa==NULL)
	if (context->link.alfa == NULL) {
	  // a "server"
	  if (context->config.srvname != NULL && strncmp(context->config.srvname,"LOCK",4) != 0) 
	    MqConfigSetName(context, context->config.srvname);
	} else {
	  // a "filter", we have to start the right site (an Server)
	  struct MqS * myFilter;
	  struct MqBufferLS *alfa = MqBufferLCreate(context->link.alfa->cursize+2);
	  
	  // this is a typical situation for a server in a middel of an alfa syntax:
	  //	    acmd1 @ filter @ acmd3
	  // filter (this one) is server for acmd1 and client for acmd3

	  // step 1, append to the !beginning! of alfa the server name
	  MqBufferLAppend (alfa, MqBufferCreateC (MQ_ERROR_PANIC, context->config.name), -1);

	  // step 2, append "alfa" string on second position if the first item is !not! a option
	  if (context->link.alfa->data[0]->cur.C[0] != '-') {
	    MqBufferLAppend (alfa, MqBufferCreateC (MQ_ERROR_PANIC, MQ_ALFA_STR), -1);
	  }

	  // step 3, append the "context->link.alfa" data
	  MqBufferLAppendL (alfa, context->link.alfa, -1);

	  // step 4, create the new context and fill the myFilter
	  MqErrorCheck (pCallFactory (context, MQ_FACTORY_NEW_FILTER, context->setup.factory, &myFilter));

	  // step 5, configure "myFilter"
	  MqConfigSetServerSetup(myFilter, NULL, NULL, NULL, NULL);
	  MqConfigSetServerCleanup(myFilter, NULL, NULL, NULL, NULL);
	  MqConfigSetEvent(myFilter, NULL, NULL, NULL, NULL);
	  MqConfigSetIsServer(myFilter, MQ_NO);
	  MqConfigSetIgnoreExit(myFilter, MQ_NO);
	  pConfigSetMaster(myFilter, context, 0);
	  if (context->config.srvname != NULL && strncmp(context->config.srvname,"LOCK",4) != 0) {
	    MqConfigSetSrvName(myFilter, context->config.srvname);
	  }

	  // step 6, create the link
	  if (MqErrorCheckI (MqLinkCreate (myFilter, &alfa))) {
	    MqErrorCopy (context, myFilter);
	    MqContextDelete (&myFilter);
	    goto error;
	  }
	  MqBufferLDelete(&alfa);
	}; // END filter startup
      }

      // configure the new server
      if (context->setup.ServerSetup.fCall != NULL) {
	MqDLogC(context,8,"call: ServerSetup -> START\n");
	context->link.bits.flagServerSetup = MQ_YES;
	MqErrorCheck(MqCallbackCall(context, context->setup.ServerSetup));
	MqDLogC(context,8,"call: ServerSetup -> END\n");
      }

      // change into "connected"
      context->link.bits.isConnected = MQ_YES;

    }; // END SERVER

    MqDLogV(context,2,"END string<%s>, silent<%s>, debug<%i>, endian<%i>\n",
         (context->config.isString?"yes":"no"), (context->config.isSilent?"yes":"no"),
             context->config.debug, context->link.bits.endian);

    context->link.bits.onCreateEnd = MQ_YES;

  error:
    return MqErrorStack(context);
  }
}

enum MqErrorE
MqLinkDefault (
  struct MqS * const context,
  struct MqBufferLS ** argsP
)
{

  // create the Message-Queue
  MqErrorCheck (MqLinkCreate (context, argsP));

  // check for wrong arguments
  MqErrorCheck (MqCheckForLeftOverArguments(context, argsP));

error:
  MqBufferLDelete (argsP);
  return MqErrorStack(context);
}

void
MqLinkDelete (
  struct MqS * const context
)
{
  // report error if available
  pErrorReport(context);

  if (unlikely(context->link.bits.onCreateStart == MQ_NO)) {
    return; // nothing to do, never was connected
  } else  if (context->link.bits.onDelete == MQ_YES) {
    return; // nothing to do, onDelete was already performed
  } else if (MQ_IS_CHILD(context) && context->setup.Child.fDelete) {
    MqDeleteF func = context->setup.Child.fDelete;
    context->setup.Child.fDelete = NULL;  // no double call
    (*func) (context);
    return;
  } else if (MQ_IS_PARENT(context) && context->setup.Parent.fDelete) {
    MqDeleteF func = context->setup.Parent.fDelete;
    context->setup.Parent.fDelete = NULL;  // no double call
    (*func) (context);
    return;
  } else {
    // lock this function
    context->link.bits.onDelete = MQ_YES;

    // cleanup the server
    if (context->link.bits.flagServerSetup == MQ_YES && context->setup.ServerCleanup.fCall != NULL) {
      MqCallbackCall(context, context->setup.ServerCleanup);
    }

    // shutdown depending context
    pMqShutdown (context,__func__);

    // shutdown all childs
    while (context->link.childs) {
      // the next line is necessary because the item "context->link.childs->context" and
      // "context->link.childs->context->link.self->context" are identical. The last one is 
      // free'd during the following "MqLinkDelete" too and a double-free error
      // will happen
      //struct MqS * save = context->link.childs->context;
      MqLinkDelete (context->link.childs->context);
    }
    
    // delete my ctxId
    if (MQ_IS_PARENT (context)) {
      // delete the entire ctxIdA
      MqSysFree (context->link.ctxIdA);
      context->link.ctxIdZ = 0;
      context->link.ctxIdR = 0;
    } else if (context->link.self) {
      // delete my entry from the 'parent' context->link.childs entry
      struct pChildS * left  = context->link.self->left;
      struct pChildS * right = context->link.self->right;
      // delete the entry in the ctxIdA array
      context->link.ctxIdP->link.ctxIdA[context->link.ctxId] = NULL;
      // check the 'left' site of context->link.self
      if (left != NULL)  {
	// context->link.self is !not! the first child of parent
	left->right = right;
      } else {
	// context->link.self is the !most left! child of parent
	context->config.parent->link.childs = right;
      }
      if (right != NULL) right->left = left;
      MqSysFree (context->link.self);
    }

    // delete my cache
    pCacheDelete (&context->link.readCache);

    // delete my helper's
    pSlaveDelete (&context->link.slave);
    pTransDelete (&context->link.trans);
    pTokenDelete (&context->link.srvT);
    pIoDelete    (&context->link.io);
    pReadDelete  (&context->link.read);
    pSendDelete  (&context->link.send);

    // cleanup link-specific data
    context->link.bits.prepareDone = MQ_NO;
    MqBufferLDelete (&context->link.alfa);
    MqSysFree (context->link.targetIdent);

    // last chance to write a log message
    MqDLogV(context,2,"DELETE %s\n", (context->link.bits.doFactoryCleanup == MQ_YES ? "with FACTORY" : ""));

    // cleanup the factory object ?
    if (context->link.bits.doFactoryCleanup == MQ_YES) {
      // do not call "MqLinkDelete" again
      MqContextDelete((struct MqS **)&context);
    } else {
      // initialize "link" object to "NULL"
      memset(&context->link,0,sizeof(struct MqLinkS));
    }
    return;
  }
}

int
MqLinkIsConnected (
  struct MqS const * const context
)
{
  return pIoCheck (context->link.io) && context->link.bits.isConnected == MQ_YES;
}

struct MqS *
MqLinkGetParent (
  struct MqS const * const context
)
{
  return MqLinkGetParentI(context);
}

int
MqLinkIsParent (
  struct MqS const * const context
)
{
  return MqLinkIsParentI(context);
}

MQ_SIZE
MqLinkGetCtxId (
  struct MqS const * const context
)
{
  return MqLinkGetCtxIdI(context);
}

MQ_CST
MqLinkGetTargetIdent (
  struct MqS * const context
)
{
  return context->link.targetIdent;
}

void pLinkDisConnect (
  struct MqS * const context
) {
  struct pChildS *child;
  context->link.bits.isConnected = MQ_NO;
  for (child = context->link.childs; child != NULL; child=child->right) {
    pLinkDisConnect (child->context);
  }
}

void
pLinkMark (
  struct MqS * const context,
  MqMarkF const markF
)
{
  // MqLinkS
  struct pChildS * child = context->link.childs;
  struct MqS * cctx;
  if (child == NULL) return;
  for (; child != NULL; child=child->right) {
    cctx = child->context;
    if (cctx->self != NULL) (*markF)(cctx->self);
    MqMark (cctx, markF);
  }
}

/*****************************************************************************/
/*                                                                           */
/*                                debugging                                  */
/*                                                                           */
/*****************************************************************************/

#ifdef _DEBUG
void
MqLogChild (
  struct MqS const * const context,
  MQ_CST const prefix
)
{
  struct pChildS * childs;
  MqLogV(context,prefix,0,"MqLogMsgque >>>>>\n");
  MqLogV(context,prefix,0,"server=%s, parent=%p\n", (MQ_IS_SERVER(context)?"yes":"no"), (void*) context->config.parent);
  for(childs = context->link.childs; childs != NULL ; childs = childs->right) {
    MqLogV(context,prefix,0,"child: context=%p\n", (void*) childs->context);
  }
  MqLogV(context,prefix,0,"MqLogMsgque <<<<<\n");
}
#endif /* _DEBUG */

END_C_DECLS







