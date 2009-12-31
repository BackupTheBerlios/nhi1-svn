/**
 *  \file       theLink/libmsgque/msgque.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#define MQ_PRIVATE_IS_MAIN 1

#include "main.h"
#include "sys.h"

#include "error.h"
#include "buffer.h"
#include "bufferL.h"
#include "log.h"
#include "send.h"
#include "read.h"
#include "mq_io.h"
#include "token.h"
#include "swap.h"
#include "trans.h"
#include "cache.h"
#include "slave.h"

#ifdef HAVE_STRINGS_H
#  include <strings.h>
#endif

#define TOKEN_LEN (HDR_TOK_LEN+1)
#define MQ_CONTEXT_S context

#define MQ_TIME ((MQ_TIME_T)time(NULL))

BEGIN_C_DECLS

void sEventFree (void);

struct pChildS {
    struct pChildS * left     ;
    struct MqS     * context   ;
    struct pChildS * right    ;
};

// old: MQ_DDL_EXPORT
MQ_EXTERN struct MqBufferLS * MqInitBuf = NULL;
MQ_EXTERN MqFactorySelectorF MqFactorySelector = NULL;

static enum MqErrorE
pWaitForToken (
  struct MqS * const context,
  MQ_TIME_T const timeout,
  MQ_CST const str
);

static void pMqCleanup (
  void
) __attribute__ ((destructor));

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

void
pMqCleanup(void)
{
  if (MqInitBuf != NULL) MqBufferLDelete(&MqInitBuf);
}

MQ_STR
MqHelp ( MQ_CST  tool )
{
  MqBufferCreateStatic (buf, 1000);

  if (tool) {
      MqBufferAppendV (buf, "  %s [ARGUMENT]... syntax:\n", tool);
      MqBufferAppendV (buf, "    aclient [OPTION]... %c %s [OPTION]... %c [ARGUMENT]...\n", MQ_ALFA, tool, MQ_ALFA);
      MqBufferAppendV (buf, "\n");
  }
  MqBufferAppendV (buf, "  msgque [OPTION]:\n");
  MqBufferAppendV (buf, "    --help-msgque    print msgque specific help\n");

  return (MQ_STR) buf->data;
}

static MQ_STR
pMqHelp (void)
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
    context->link.endian = context->config.parent->link.endian;
  }

  // parse "my" arguments
  if (argv != NULL && argv->cursize > 0) {

    struct MqBufferS *arg = NULL;
    MQ_CST strV, argC;
    MQ_INT intV;
    MQ_WID intW;
    MQ_SIZE idx;
    // allways read the first item from argv
    arg = argv->data[0];
    // if "MqMainToolName" is empty -> fill it with a default value
    if (MqInitBuf == NULL) {
      struct MqBufferLS * initB = MqInitCreate();
      MqBufferLAppendC(initB, arg->cur.C);
    }
    // try to figure aout a "good" name
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
	    if (MqInitBuf == NULL) {
	      return MqErrorDb (MQ_ERROR_NO_INIT);
	    } else if (context->config.ignoreSpawn == MQ_YES) {
	      return MqErrorDbV (MQ_ERROR_OPTION_FORBIDDEN, "local", "--spawn");
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
	    MqErrorCheck (MqBufferGetI(argv->data[idx], &context->config.io.pipe.socks[1]));
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
	    if (context->config.ignoreThread == MQ_YES) {
	      return MqErrorDbV (MQ_ERROR_OPTION_FORBIDDEN, "local", "--thread");
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
	    if (context->config.ignoreFork == MQ_YES) {
	      return MqErrorDbV (MQ_ERROR_OPTION_FORBIDDEN, "local", "--fork");
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
	    fputs(pMqHelp(),stderr);
	    SysExit(0,EXIT_SUCCESS);
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

  // POST validation
  if (context->statusIs & MQ_STATUS_IS_DUP)
    context->config.io.com = MQ_IO_PIPE;

error:
  return MqErrorStack(context);
}

/*****************************************************************************/
/*                                                                           */
/*                             create / delete                               */
/*                                                                           */
/*****************************************************************************/

struct MqBufferLS*
MqInitCreate ()
{
  if (MqInitBuf != NULL) MqBufferLDelete(&MqInitBuf);
  MqInitBuf = MqBufferLCreate(2);
  return MqInitBuf;
}

const MQ_STRB*
MqInitGetFirst (void)
{
  return (MqInitBuf && MqInitBuf->cursize > 0 ? MqInitBuf->data[0]->cur.C : "unknown");
}

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
  MqConfigSetParent(context, parent);
  return MqLinkCreate (context, argvP);
}

enum MqErrorE
MqLinkCreate (
  struct MqS * const context,
  struct MqBufferLS ** argvP
)
{
  MqErrorReset(context);

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

  // 'master' not connected
  if (unlikely(context->config.master != NULL && context->config.master->link.read == NULL)) {
    MqBufferLDelete (argvP);
    return MqErrorDbV(MQ_ERROR_CONNECTED, "master", "not");
  }

  // Try to invoke the "CreateHandler" first
  if (context->link.onCreate == MQ_NO) {
    context->link.onCreate = MQ_YES;
    if (context->config.parent != NULL && context->setup.Child.fCreate != NULL) {
      return (*context->setup.Child.fCreate) (context, argvP);
    } else if (context->config.parent == NULL && context->setup.Parent.fCreate != NULL) {
      return (*context->setup.Parent.fCreate) (context, argvP);
    }
  }

  // just create the client/server link now.
  {
    struct MqBufferLS *argv, *alfa;
    MQ_STR serverexec = NULL;
    enum MqErrorE ret = MQ_OK;

    // pointers are now owned by MqLinkCreate
    pBufferLSplitAlfa (argvP, &alfa);
    argv = (argvP != NULL ? *argvP : NULL);

    // 1. set the error object of the arguments
    pBufferLSetError(argv, context);
    pBufferLSetError(alfa, context);

    // 2. parse arguments
    MqErrorCheck (sMqCheckArg (context, argv));
    if (context->config.debug >= 2) {
      if (MQ_IS_SLAVE(context))
	MqDLogX(context,__func__,2,"START-CREATE-SLAVE: master<%p>\n", (void*) context->config.master);
      else if (MQ_IS_PARENT(context))
	MqDLogX(context,__func__,2,"START-CREATE: io->com<%s>\n", pIoLogCom(context->config.io.com));
      else
	MqDLogX(context,__func__,2,"START-CREATE-CHILD: parent<%p>\n", (void*) context->config.parent);
    }

    // handle empty MqBufferLS list alfa
    // after this line "alfa" pointer is NULL (-> no data) or "alfa" ponter != NULL (-> has data)
    if (alfa != NULL) {
      // we have a "master", a "worker" have to be created
      if (alfa->cursize == 0) {
	MqBufferLDelete (&alfa);
      } else {
	if (!strncmp(alfa->data[0]->cur.C, "SELF", 4)) {
	  MqBufferLDeleteItem (MQ_ERROR_PANIC, alfa, 0, 1, MQ_YES);
	  MqBufferLAppendL(alfa,MqInitBuf,0);
	} else if (!strncmp(alfa->data[0]->cur.C, "WORKER", 6)) {
	  context->link.isWORKER = MQ_YES;
	}
	serverexec = mq_strdup(alfa->data[0]->cur.C);
      }
    }
    if (serverexec == NULL) serverexec = mq_strdup(MqInitGetFirst());

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
    if (MqErrorGetCode(context) == MQ_ERROR) goto error;

    MqErrorCheck (pIoCreate   (context, alfa, &context->link.io));
    context->link.srvT = pTokenCreate(context);
    MqErrorCheck (pTransCreate (context, &context->link.trans));

    // create the cache data
    pCacheCreate ((CacheCF) pReadCreate, (CacheDF) pReadDelete, context, &context->link.readCache);

    // ready to create the SLAVE objects
    MqErrorCheck (pSlaveCreate (context, &context->link.slave));

    // context specific initialization
    if (MQ_IS_CLIENT (context)) {
    // this is a CLIENT

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
	if (MqErrorCheckI (MqSendEND_AND_WAIT (parent, "_OKS", MQ_TIMEOUT_USER))) {
	  MqErrorCopy (context, parent);
	  pIoCloseSocket (context->link.io, __func__);
	  goto error;
	}

      } else {
      // this is a PARENT

	MQ_BOL mystring;
	MQ_BOL myendian;

	// 0. Start the PIPE server (if necessary)
	MqErrorCheck (pIoStartServer(context->link.io, MQ_START_SERVER_AS_PIPE, NULL, &alfa, NULL, NULL));

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

	// send the server name
	if (context->config.srvname != NULL)
	  MqSendC (context, (context->config.srvname));

	// send package and wait for the answer
	MqDLogV(context,4,"send token<%s>\n","_IAA");
	if (MqErrorCheckI(MqSendEND_AND_WAIT (context, "_IAA", MQ_TIMEOUT_USER))) {
	  ret = MqErrorDbV2 (context,MQ_ERROR_CAN_NOT_START_SERVER, 
	    (pIoIsRemote(context->link.io) == MQ_NO ? serverexec : "remote-connect"));
	  goto error;
	}

	// read the other endian and set my context->link.endian
	MqReadO(context, &myendian);

#     if defined(WORDS_BIGENDIAN)
	  context->link.endian = (myendian ? MQ_NO : MQ_YES);
#     else
	  context->link.endian = (myendian ? MQ_YES : MQ_NO);
#     endif

	// restore the binary mode
	MqConfigSetIsString (context, mystring);

	// wait until the server is able to process events (and send _PEO)
	switch (ret = pWaitForToken (context, MQ_TIMEOUT_USER, "_PEO")) {
	  case MQ_OK:	    break; 
	  case MQ_ERROR:    goto error; 
	  case MQ_EXIT:	    goto cleanup; 
	  case MQ_CONTINUE: goto cleanup;
	}

      }; // END PARENT

    } else { // this is a SERVER

      // we have to wait for the "_IAA" message of the "client" to set "context->config.debug" proper
      if (MQ_IS_PARENT (context)) {

	// the "server" allways default to "string", because the "client" set the
	// default in the "_IAA" message
	MqConfigSetIsString (context, MQ_YES);

	switch (ret = pWaitForToken (context, MQ_TIMEOUT_USER, "_IAA")) {
	  case MQ_OK:	    break; 
	  case MQ_ERROR:    goto error; 
	  case MQ_EXIT:	    goto cleanup; 
	  case MQ_CONTINUE: goto cleanup;
	}

	// test on "filter" (alfa!=NULL) or a normal "server" (alfa==NULL)
	if (alfa == NULL) {
	  // a final "server"
	  if (context->config.srvname != NULL) 
	    MqConfigSetName(context, context->config.srvname);
	} else {
	  // a "filter", we have to start the right site (an Server)
	  struct MqS * myFilter;
	  
	  // this is a typical situation for a server in a middel of an alfa syntax:
	  //	    acmd1 @ filter @ acmd3
	  // filter (this one) is server for acmd1 and client for acmd3

	  // step 1, append to the !beginning! of alfa the server name
	  MqBufferLAppend (alfa, MqBufferCreateC (MQ_ERROR_PANIC, context->config.name), 0);

	  // step 2, append "alfa" string on second position if the first item is !not! a option
	  if (alfa->data[1]->cur.C[0] != '-') {
	    MqBufferLAppend (alfa, MqBufferCreateC (MQ_ERROR_PANIC, MQ_ALFA_STR), 1);
	  }

	  // step 3, create the new context and fill the myFilter
	  MqErrorCheck (pCallFactory (context, MQ_FACTORY_NEW_FILTER, context->setup.Factory, &myFilter));

	  // step 3, link between "myFilter" and "context"
	  MqConfigSetServerSetup(myFilter, NULL, NULL, NULL, NULL);
	  MqConfigSetServerCleanup(myFilter, NULL, NULL, NULL, NULL);
	  MqConfigSetIsServer(myFilter, MQ_NO);
	  MqConfigSetMaster  (myFilter, context, 0);

	  if (MqErrorCheckI (MqLinkCreate (myFilter, &alfa))) {
	    MqErrorCopy (context, myFilter);
	    MqContextDelete (&myFilter);
	    goto error;
	  }
	}; // END filter startup
      }

      // configure the new server
      if (context->setup.ServerSetup.fFunc != NULL) {
	context->link.flagServerSetup = MQ_YES;
	MqErrorCheck((*context->setup.ServerSetup.fFunc) (context, context->setup.ServerSetup.data));
      }

    }; // END SERVER

    MqDLogV(context,2,"END-CREATE string<%s>, silent<%s>, debug<%i>, endian<%i>\n",
	  (context->config.isString?"yes":"no"), (context->config.isSilent?"yes":"no"), 
	      context->config.debug, context->link.endian);


    goto cleanup;

  error:
    MqBufferLDelete (argvP);
    ret = MqErrorStack (context);

  cleanup:
    // alfa is allways owned by MqLinkCreate
    if (alfa != NULL) MqBufferLDelete(&alfa);
    MqSysFree(serverexec);
    return ret;
  }
}

enum MqErrorE
MqDefaultLinkCreate (
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
pDeleteProtectionCreate (
  struct MqS * const context
)
{
  context->link.deleteProtection = MQ_YES;
}

void
MqLinkDelete (
  struct MqS * const context
)
{
  // report error if available
  pErrorReport(context);

  if (unlikely(context->link.onCreate == MQ_NO)) {
    return;
  } else {
    // check on "deleteProtection"
    if (context->link.ctxIdP != NULL && context->link.ctxIdP->link.deleteProtection == MQ_YES) {
      MqDLogC(context,3,"DELETE protection\n");
      pErrorSetEXIT (context, __func__);
      return;
    }

    // Try to invoke the "DeleteHandler" first
    if (context->link.MqLinkDelete_LOCK == MQ_NO) {
      context->link.MqLinkDelete_LOCK = MQ_YES;
      if (MQ_IS_CHILD(context) && context->setup.Child.fDelete) {
	(*context->setup.Child.fDelete) (context);
	return;
      } else if (MQ_IS_PARENT(context) && context->setup.Parent.fDelete) {
	(*context->setup.Parent.fDelete) (context);
	return;
      }
    }

    // cleanup the server
    if (context->link.flagServerSetup == MQ_YES && context->setup.ServerCleanup.fFunc != NULL) {
      (*context->setup.ServerCleanup.fFunc) (context, context->setup.ServerCleanup.data);
    }

    // shutdown depending context
    pMqShutdown (context);

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

    // last chance to write a log message
    {
      MqDLogV(context,2,"DELETE %s\n", (context->link.doFactoryCleanup == MQ_YES ? "with FACTORY" : ""));

      // cleanup the factory object ?
      if (context->link.doFactoryCleanup == MQ_YES) {
	context->link.onCreate = MQ_NO;
	MqContextDelete((struct MqS **)&context);
      } else {
	// initialize "msgque" object to "NULL"
	memset(&context->link,0,sizeof(struct MqLinkS));
      }
    }
    return;
  }
}

void
pMqShutdown (
  struct MqS * const context
)
{
  struct pChildS * child;
  if (!MQ_ERROR_IS_POINTER(context) || context->link.onShutdown == MQ_YES) return;
  context->link.onShutdown = MQ_YES;

  MqDLogC(context,4,"START shutdown\n");

  // shutdown all childs
  for (child = context->link.childs; child != NULL; child = child->right ) {
    // the next line is necessary because the item "context->link.childs->context" and
    // "context->link.childs->context->link.self->context" are identical. The last one is 
    // free'd during the following "MqLinkDelete" too and a double-free error
    // will happen
    pMqShutdown (child->context);
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
      MqSendC(context, "dummy");
      MqSendRETURN(context);
    }
  }

  // shutdown the io
  pIoShutdown (context->link.io);

  // cleanup error
  MqErrorReset(context);

  MqDLogC(context,4,"END shutdown\n");
  return;
}

/*****************************************************************************/
/*                                                                           */
/*                                context/event                               */
/*                                                                           */
/*****************************************************************************/

static enum MqErrorE
sMqEventStart (
  struct MqS * const context
)
{
  struct MqS * a_context = context;
  const int MqSetDebugLevel(context);

  MqDLogCL(context,6,"START\n");

  // ##################### READ HEADER #####################
  switch (pReadHDR (context, &a_context)) {
    case MQ_OK:	      break;
    case MQ_CONTINUE: return MQ_OK;
    case MQ_ERROR:    MqDLogCL(context,7,"pReadHDR: report an error\n"); goto error;
    case MQ_EXIT:     return MQ_EXIT;
  }

  // ##################### TOKEN Handler #####################
  // The following code will "only" run on the "server" site.
  // An error in a service-handler will !not! shutdown the server
  if (MqErrorCheckI(pTokenInvoke (a_context->link.srvT))) {
    enum MqErrorE err = MqErrorGetCode(a_context);
    if (err == MQ_ERROR) {
      if (a_context->link._trans != 0) {
	// in a transaction, "MqSendRETURN" will convert the context error 
	// into an "error" package and send this package back to the client
	MqErrorCheck (MqSendRETURN (a_context));
      } else if (pIoCheck (a_context->link.io)) {
	// outsite of a transaction we have to send a "real" error,
	// but "only" if the connection "pIoCheck" is still available
	MqErrorCheck (MqSendERROR (a_context));
      } else {
	// report thr error to the top-level
	goto error;
      }
    } else {
      // MQ_EXIT, MQ_CONTINUE, ...
      goto error;
    }
  }

  MqDLogCL(a_context,6,"END-OK\n");
  return MQ_OK;

error:
  // the errors are reported into a_context->link.error
  MqErrorStack (a_context);
  return MqErrorCopy (context, a_context);
}

static enum MqErrorE
sCallEventProc (
  struct MqS * const context,
  MqEventF const proc
)
{
  struct MqS * cldCtx;
  MqEventF cldProc;
  MQ_INT NUM=1, CONTINUE=0;
  struct pChildS * child;
  switch ((*proc) (context)) {
    case MQ_OK:
      break;
    case MQ_CONTINUE:
      if (context->link.onShutdown == MQ_YES) {
	CONTINUE++;
      }
      break;
    case MQ_ERROR:
    case MQ_EXIT:
      goto error;
  }
  for (child = context->link.childs; child != NULL; child=child->right) {
    NUM++;
    cldCtx = child->context;
    cldProc = cldCtx->setup.fEvent;
    if (cldProc != NULL && cldCtx != NULL) {
      switch (sCallEventProc (cldCtx, cldProc)) {
	case MQ_OK:
	  break;
	case MQ_CONTINUE:
	  if (cldCtx->link.onShutdown == MQ_YES) {
	    CONTINUE++;
	  }
	  break;
	case MQ_ERROR:
	case MQ_EXIT:
	  MqErrorCopy(context, cldCtx);
	  goto error;
	  break;
      }
    }
  }
  // with "ignoreExit" all "context" have to be on "CONTINUE" to trigger an exit
  if (context->setup.ignoreExit != MQ_YES || NUM != CONTINUE) {
    MqErrorReset(context);
  }
error:
  return MqErrorStack(context);
}

enum MqErrorE
pWaitOnEvent (
  struct MqS * const context,
  const enum MqIoSelectE what,
  const MQ_TIME_T timeout
)
{
  const MQ_TIME_T startT = time (NULL);
  const MqEventF proc = context->setup.fEvent;
  register MQ_TIME_T nowT;

  struct timeval tv;
  const int MqSetDebugLevel(context);

  // set initial timeout
  if (proc) {
    // we have an extern event-handlig procedure (proc) ... give this code the chance 
    // to do somethig usefull
    tv.tv_sec = 0L;
    // why 10000 uSec, to let the select a chance to process the direct response
    // on a previously submitted 'MqSendEND_AND_WAIT' packages (e.g. special _ERR)
    tv.tv_usec = 10000L;
  } else {
    tv.tv_sec = (long)timeout;
    tv.tv_usec = 0L;
  }

  MqDLogCL(context,6,"START loop\n");
  do {

    // 1. test on 'select'
    switch (pIoSelectAll (context->link.io, what, &tv)) {
      case MQ_CONTINUE:	break;		  // nothing found wait for next event
      case MQ_OK:	goto end;	  // found event
      case MQ_ERROR:	goto error;	  // something wrong happen
      case MQ_EXIT:	return MQ_EXIT;	  // something unexpected happen
    }

    // 2. call external event-proc
    if (proc != NULL && context != NULL) {
      // this guarding with "____" is important to detect and break-out-of "nested" event calls
      // pWaitOnEvent
      //   -> guard 
      //   -> *proc 
      //     ... -> pWaitOnEvent -> found-an-event
      //   -> check-guard
      pTokenSetCurrent(context->link.srvT,"____");

      //MqDLogC(context,7,"call fEvent in<%p>\n", msgque);
      switch (sCallEventProc (context, proc)) {
	case MQ_OK:
	  break;
	case MQ_CONTINUE: {
	    struct pChildS * child;
	    context->setup.ignoreExit = MQ_NO;
	    for (child = context->link.childs; child != NULL; child=child->right) {
	      child->context->setup.ignoreExit = MQ_NO;
	    }
	    return pErrorSetEXIT (context, __func__);
	  }
	  break;
	case MQ_ERROR:
	case MQ_EXIT:
	  goto error;
      }
      //MqDLogC(context,7,"finish fEvent in<%p>\n", msgque);

      // the fEvent found an event belonging to "context" but does not know
      // if the event belongs to the "parent" self or to one of his "childs".
      // -> let upper code decide what to do
      if (!pTokenCheck(context->link.srvT,"____")) {
        //pDLog(context,7,"fEvent in<%p>, token<%s>\n", msgque, pTokenGetCurrent(context->link.srvT));
        return MQ_CONTINUE;
      }
    }

    // 3. how long does it takes until now
    nowT = time (NULL) - startT;
  }
  while (nowT < timeout);

  MqDLogCL(context,6,"END-ERROR-TIMEOUT\n");
  return MqErrorDbV(MQ_ERROR_TIMEOUT, timeout);

end:
  MqDLogCL(context,6,"END-OK\n");
  return MQ_OK;

error:
  return MqErrorStack (context);
}

/// \attention this code should call all available event-loop functions
enum MqErrorE
pUSleep (
  struct MqS * const context,
  long const usec
)
{
  if (context->setup.fEvent != NULL && context != NULL) {
    const MqEventF proc = context->setup.fEvent;
    struct mq_timeval start;
    struct mq_timeval now;
    MqErrorCheck (SysGetTimeOfDay (context, &start, NULL));
    do {
      (*proc) (context);
      MqErrorCheck (MqSysUSleep (context, 99999L));
      MqErrorCheck (SysGetTimeOfDay (context, &now, NULL));
    }
    while (((now.tv_sec - start.tv_sec) * 1000000L + (now.tv_usec - start.tv_usec)) < usec);
  } else {
    MqErrorCheck (MqSysUSleep (context, usec));
  }

error:
  return MqErrorStack (context);
}

enum MqErrorE
MqProcessEvent (
  struct MqS * const context,
  MQ_TIME_T timeout,
  enum MqWaitOnEventE const wait
)
{
  enum MqErrorE ret = MQ_OK;
  const int forever = (wait >= MQ_WAIT_FOREVER);
  const int once = (wait >= MQ_WAIT_ONCE);
  struct mq_timeval tv = {0L, 0L};
  int debugLevel;
  struct MqS * const master = context->config.master;

  // save master transaction
  MQ_HDL trans = (master != NULL ? master->link._trans : 0);

  // protection code
  MqSetDebugLevel(context);

  // first announce that the event-handling is active
  if (forever) {
    MqDLogC(context,4,"send token<_PEO>\n");
    MqSendSTART (context);
    MqErrorCheck (MqSendEND (context, "_PEO"));
  }

  // set the default for timeout
  if (timeout < 0) {
    timeout = (timeout == MQ_TIMEOUT_USER ? pIoGetTimeout(context->link.io) : MQ_TIMEOUT);
  }

  // check for an event
  MqDLogCL(context,6,"START\n");
  do {
    // ################ CHECK TO BE READABLE ##################
    if (once) {
      ret = pWaitOnEvent (context, MQ_SELECT_RECV, timeout);
      switch (ret) {
        case MQ_OK:	  break;
        case MQ_CONTINUE: ret = MQ_OK; continue;
        case MQ_ERROR:	  goto error;
        case MQ_EXIT:	  goto end;
      }
    }

    // ##################### Process Events #####################
    MqErrorCheck (ret=pIoSelectStart(context->link.io, &tv, sMqEventStart));

    // !! ATTENTION -> context is gone after an server "SHD"
  }
  while (forever && ret == MQ_OK);

end:
  // restore master transaction
  if (master != NULL) master->link._trans = trans;
  MqDLogVL(context,6,"END-%s\n", MqLogErrorCode(ret));
  return ret;

error:
  // restore master transaction
  if (master != NULL) master->link._trans = trans;
  return MqErrorStack (context);
}

static enum MqErrorE
pWaitForToken (
  struct MqS * const context,
  MQ_TIME_T const timeout,
  MQ_CST const str
)
{
  register MQ_TIME_T const startT = time (NULL);

  // 1. check if the token is already available
  if (pTokenCheck(context->link.srvT,str)) 
    return MQ_OK;

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

/*****************************************************************************/
/*                                                                           */
/*                                misc                                       */
/*                                                                           */
/*****************************************************************************/

MQ_BOL
MqCurrentTokenIs(
  struct MqS const * const context,
  char const * const str
)
{
  return pTokenCheck(context->link.srvT,str);
}

enum MqErrorE
MqCheckForLeftOverArguments (
  struct MqS * const context,
  struct MqBufferLS ** argvP
)
{
  MQ_SIZE i;
  MQ_CST str;
  struct MqBufferLS * argv;

  if (unlikely(argvP == NULL || *argvP == NULL)) {
    return MQ_OK;
  }
  argv = *argvP;

  if (argv->cursize == 0) {
    MqBufferLDelete(argvP);
    return MQ_OK;
  }

  MqErrorDb2 (context,MQ_ERROR_INVALID_OPTION);
  for (i = 0; i < argv->cursize; i++) {
    if ( MqBufferGetC (argv->data[i], &str) != MQ_ERROR)
       MqErrorSAppendV (context, "=> %s", str);
  }
  MqBufferLDelete(argvP);
  return MqErrorGetCode(context);
}

/*****************************************************************************/
/*                                                                           */
/*                              service                                      */
/*                                                                           */
/*****************************************************************************/

static enum MqErrorE 
sServiceProxy (
  struct MqS * const context,
  MQ_PTR const data
) {
  MQ_BIN bdy; MQ_SIZE len;
  struct MqS * ftrctx;

  MqErrorCheck (MqConfigGetFilter (context, (int) (long) data, &ftrctx));

  MqErrorCheck  (MqReadBDY (context, &bdy, &len));
  MqErrorCheck1 (MqSendSTART (ftrctx));
  MqErrorCheck1 (MqSendBDY (ftrctx, bdy, len));

  // continue with the original transaction
  if (MqConfigGetIsTransaction (context)) {
    // use a transaction protection
    MqErrorCheck1 (MqSendEND_AND_WAIT (ftrctx, MqConfigGetToken(context), MQ_TIMEOUT_USER));
    // read the answer
    MqSendSTART (context);
    MqErrorCheck1 (MqReadBDY (ftrctx, &bdy, &len));
    MqErrorCheck  (MqSendBDY (context, bdy, len));
  } else {
    // use a transaction protection
    MqErrorCheck1 (MqSendEND (ftrctx, MqConfigGetToken(context)));
  }

error:
  return MqSendRETURN(context);

error1:
  MqErrorCopy (context, ftrctx);
  goto error;
}

enum MqErrorE 
MqServiceProxy(
  struct MqS * const context, 
  MQ_CST const token,
  MQ_SIZE const id
)
{
  return MqServiceCreate (context, token, sServiceProxy, (void*)(long)id,  NULL);
}

enum MqErrorE 
MqServiceCreate(
  struct MqS * const context, 
  MQ_CST const token,
  MqTokenF const proc,
  MQ_PTR data,
  MqTokenDataFreeF datafreeF
)
{
//MqDLogX(context,__func__,0,"data<%p>\n", data);
  struct MqCallbackS cb = {proc, data, datafreeF, NULL};
  if (strlen(token) != HDR_TOK_LEN) {
    return MqErrorDbV (MQ_ERROR_TOKEN_LENGTH, token);
  }
  return (pTokenAddHdl (context->link.srvT, token, cb));
}

enum MqErrorE 
MqServiceDelete(
  struct MqS const * const context, 
  MQ_CST const token
)
{
  return (pTokenDelHdl (context->link.srvT, token));
}

/*****************************************************************************/
/*                                                                           */
/*                                misc                                       */
/*                                                                           */
/*****************************************************************************/

struct MqS *
pMqGetFirstParent(
  struct MqS * const context
)
{
  return context->link.ctxIdP;
}

void
MqExit (
  struct MqS * context
)
{
  int num;
  MqExitF exitF;
  MQ_BOL isThread;

  // exit on empty context
  if (context == NULL) SysExit (0,0);
  // no double invovation of MqExit
  if (context->link.onExit == MQ_YES) MqPanicSYS(context);
  context->link.onExit = MQ_YES;
  MqDLogC(context,3,"EXIT\n");

  // switch to parent
  if (MQ_IS_CHILD(context)) {
    struct MqS * parent = pMqGetFirstParent(context);
    MqErrorCopy (parent, context);
    context = parent;
    context->link.onExit = MQ_YES;
    MqDLogC(context,3,"switch to PARENT\n");
  }

  // save context data because the next section will delete it
  num = MqErrorGetNum(context);

  // MQ_ERROR_EXIT is a "normal" exit 
  if (num == (200+MQ_ERROR_EXIT)) num = EXIT_SUCCESS;

  // if context was started as thread?
  isThread = ((context->statusIs & MQ_STATUS_IS_THREAD) != 0);

  // get the right application specific exit function
  exitF = (isThread ? context->setup.fThreadExit : context->setup.fProcessExit);

  // 1. delete the main Link and all depending links also
  MqLinkDelete (context);

  // 2. delete all other parent context from current thread or process
  sEventFree();

  // 3. call application specific exit function
  if (exitF) (*exitF) (num);

  // 4. finally call libmsgque exit function
  SysExit(isThread, num);
}



/*****************************************************************************/
/*                                                                           */
/*                                debugging                                  */
/*                                                                           */
/*****************************************************************************/

#ifdef _DEBUG
void
MqLogData (
  struct MqS const * const context,
  MQ_CST const prefix
)
{
  MqDLogX (context, prefix, 0, ">>>> MqS \n");

  MqDLogX (context, prefix, 0, "debug     = " MQ_FORMAT_I "\n", context->config.debug);
  MqDLogX (context, prefix, 0, "silent    = " MQ_FORMAT_C "\n", (context->config.isSilent ? "yes" : "no"));
  MqDLogX (context, prefix, 0, "string    = " MQ_FORMAT_C "\n", (context->config.isString ? "yes" : "no"));
  MqDLogX (context, prefix, 0, "ctxId     = " MQ_FORMAT_I "\n", context->link.ctxId);
  MqDLogX (context, prefix, 0, "sOc       = " MQ_FORMAT_C "\n", MqLogServerOrClient (context));
  MqDLogX (context, prefix, 0, "pOc       = " MQ_FORMAT_C "\n", MqLogParentOrChild (context));

  MqDLogX (context, prefix, 0, "<<<< MqS\n");
}

void
MqLogChild (
  struct MqS const * const context,
  char const * const prefix
)
{
  struct pChildS * childs;
  MqDLogX(context,prefix,0,"MqLogMsgque >>>>>\n");
  MqDLogX(context,prefix,0,"server=%s, parent=%p\n", (MQ_IS_SERVER(context)?"yes":"no"), (void*) context->config.parent);
  for(childs = context->link.childs; childs != NULL ; childs = childs->right) {
    MqDLogX(context,prefix,0,"child: context=%p\n", (void*) childs->context);
  }
  MqDLogX(context,prefix,0,"MqLogMsgque <<<<<\n");
}
#endif /* _DEBUG */

  void sGenericCreate (void);
  void sGenericDelete (void);
  void sEventCreate (void);
  void sEventDelete (void);

END_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                                DLL                                        */
/*                                                                           */
/*****************************************************************************/

#if defined(_MSC_VER)

BOOL WINAPI DllMain( 
    HINSTANCE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
# ifdef MQ_COMPILE_AS_CC
    using namespace libmsgque;
# endif
  
  switch (ul_reason_for_call)
  {
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
      return FALSE;
    case DLL_PROCESS_ATTACH:
      sEventCreate();
      sGenericCreate();
      break;
    case DLL_PROCESS_DETACH:
      sEventDelete();
      sGenericDelete();
      break;
  }
  return TRUE;
}
#endif
