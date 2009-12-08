/**
 *  \file       theLink/tclmsgque/msgque_tcl.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_tcl.h"

#include <limits.h>
#include <float.h>

#ifdef MQ_SHARED_IS_DLL
#    define TCLMQ_EXTERN __declspec(dllexport)
#else
#    define TCLMQ_EXTERN __attribute__ ((visibility("default")))
#endif

typedef int (
  *LookupKeywordF
) (
  Tcl_Interp		  *interp,
  int			  objc,
  struct Tcl_Obj *const * objv
);

struct LookupKeyword {
  const char	  *key;
  LookupKeywordF  keyF;
};

/*****************************************************************************/
/*                                                                           */
/*                                helper                                     */
/*                                                                           */
/*****************************************************************************/


/// \brief helper to print the name of a \e Tcl_Obj
MQ_STR NS(printName) (
  Tcl_Obj * Obj
)
{
  return (Obj->typePtr ? Obj->typePtr->name : "unknown");
}

/// \brief helper to print a \e Tcl_Obj
MQ_STR NS(printObj) (
  const char *header,
  Tcl_Obj * Obj
)
{
  char *buf = ckalloc (206);
  int len;
  char *name = NS(printName) (Obj);

  // fill the buf and get the len back
  len = mq_snprintf (buf, 206, "%s: obj<%p>, refCount<%i>, type<%s>, string<%s>",
                  header, Obj, Obj->refCount, name, Tcl_GetString (Obj));

  // add additional ...
  if (len > 206)
#if defined(_MSC_VER)
    strcat_s (buf + 200, 6, " ...");
#else
    strcat (buf + 200, " ...");
#endif

  return buf;
}

/*****************************************************************************/
/*                                                                           */
/*                               subcommands                                 */
/*                                                                           */
/*****************************************************************************/

/** \brief create the <B>msgque get</B> subcommand
 *
 *  \tclmsgque_man
 *
 * \param[in] interp current Tcl interpreter
 * \param[in] objc number of objects in \e objv
 * \param[in] objv array of \e Tcl_Obj objects
 * \return Tcl error-code
 */
static int NS(Const) (
  Tcl_Interp * interp,
  int objc,
  struct Tcl_Obj *const *objv
)
{
  int index;

  Tcl_Obj *Obj = NULL;

  static const char *constant[] = {
    "maxY", "minY", "maxS", "minS", "maxI", "minI", "maxF", "minF", "maxW", "minW", "maxD", "minD", NULL
  };
  enum constants {
    MAXY, MINY, MAXS, MINS, MAXI, MINI, MAXF, MINF, MAXW, MINW, MAXD, MIND, 
  };

  // read the index
  if (objc != 3) {
    Tcl_WrongNumArgs (interp, 2, objv, "constant");
    return TCL_ERROR;
  }
  // get the Index
  TclErrorCheck (Tcl_GetIndexFromObj (interp, objv[2], constant, "constant", 0, &index));

  // do the work
  switch ((enum constants) index) {
    case MAXY:
      Obj = Tcl_NewIntObj (SCHAR_MAX);
      break;
    case MINY:
      Obj = Tcl_NewIntObj (SCHAR_MIN);
      break;
    case MAXS:
      Obj = Tcl_NewIntObj (SHRT_MAX);
      break;
    case MINS:
      Obj = Tcl_NewIntObj (SHRT_MIN);
      break;
    case MAXI:
      Obj = Tcl_NewLongObj (INT_MAX);
      break;
    case MINI:
      Obj = Tcl_NewLongObj (INT_MIN);
      break;
    case MAXF:
      Obj = Tcl_NewDoubleObj (FLT_MAX);
      break;
    case MINF:
      Obj = Tcl_NewDoubleObj (FLT_MIN);
      break;
    case MAXW:
      Obj = Tcl_NewWideIntObj (LLONG_MAX);
      break;
    case MINW:
      Obj = Tcl_NewWideIntObj (LLONG_MIN);
      break;
    case MAXD:
      Obj = Tcl_NewDoubleObj (DBL_MAX);
      break;
    case MIND:
      Obj = Tcl_NewDoubleObj (DBL_MIN);
      break;
  }

  Tcl_SetObjResult (interp, Obj);
  return TCL_OK;
}

/** \brief create the <B>msgque support</B> subcommand
 *
 *  \tclmsgque_man
 *
 * \param[in] interp current Tcl interpreter
 * \param[in] objc number of objects in \e objv
 * \param[in] objv array of \e Tcl_Obj objects
 * \return Tcl error-code
 */
static int NS(Support) (
  Tcl_Interp * interp,
  int objc,
  struct Tcl_Obj *const *objv
)
{
  int index;

  Tcl_Obj *Obj = NULL;

  static const char *constant[] = {
    "thread", "fork", NULL
  };
  enum constants {
    THREAD, FORK, 
  };

  // read the index
  if (objc != 3) {
    Tcl_WrongNumArgs (interp, 2, objv, "configuration");
    return TCL_ERROR;
  }
  // get the Index
  TclErrorCheck (Tcl_GetIndexFromObj (interp, objv[2], constant, "configuration", 0, &index));

  // do the work
  switch ((enum constants) index) {
    case THREAD:
#if defined(MQ_HAS_THREAD)
      Obj = Tcl_NewBooleanObj (1);
#else
      Obj = Tcl_NewBooleanObj (0);
#endif
      break;
    case FORK:
#if defined(HAVE_FORK)
      Obj = Tcl_NewBooleanObj (1);
#else
      Obj = Tcl_NewBooleanObj (0);
#endif
      break;
  }

  Tcl_SetObjResult (interp, Obj);
  return TCL_OK;
}

/** \brief create the <B>tclmsgque Init</B> subcommand
 *
 *  \tclmsgque_man
 *
 * \param[in] interp current Tcl interpreter
 * \param[in] objc number of objects in \e objv
 * \param[in] objv array of \e Tcl_Obj objects
 * \return Tcl error-code
 */
static int NS(InitCmd) (
  Tcl_Interp * interp,
  int objc,
  struct Tcl_Obj *const *objv
)
{
  struct MqBufferLS * initB = MqInitCreate();
  int i;
  for (i=2; i<objc; i++) {
    MqBufferLAppendC(initB, Tcl_GetString(objv[i]));
  }
  Tcl_ResetResult(interp);
  return TCL_OK;
}

/** \brief create the <B>msgque print</B> subcommand
 *
 *  \tclmsgque_man
 *
 * \param[in] interp current Tcl interpreter
 * \param[in] objc number of objects in \e objv
 * \param[in] objv array of \e Tcl_Obj objects
 * \return Tcl error-code
 */
static int NS(Print) (
  Tcl_Interp * interp,
  int objc,
  struct Tcl_Obj *const *objv
)
{
  int index;

  static const char *option[] = {
    "object", "objtype", NULL
  };
  enum options {
    OBJECT, OBJECT_TYPE
  };

  // read the index
  if (objc < 3) {
    Tcl_WrongNumArgs (interp, 2, objv, "option");
    return TCL_ERROR;
  }
  // get the Index
  TclErrorCheck (Tcl_GetIndexFromObj (interp, objv[2], option, "option", 0, &index));

  // do the work
  switch ((enum options) index) {
    case OBJECT:
      if (objc != 4) {
        Tcl_WrongNumArgs (interp, 4, objv, "tclObj");
        return TCL_ERROR;
      }
      Tcl_SetResult (interp, NS(printObj) ("print", objv[3]), TCL_DYNAMIC);
      break;
    case OBJECT_TYPE:
      if (objc != 4) {
        Tcl_WrongNumArgs (interp, 4, objv, "tclObj");
        return TCL_ERROR;
      }
      Tcl_SetResult (interp, NS(printName) (objv[3]), TCL_VOLATILE);
  }

  return TCL_OK;
}

/** \brief create the <B>msgque help</B> subcommand
 *
 *  \tclmsgque_man
 *
 * \param[in] interp current Tcl interpreter
 * \param[in] objc number of objects in \e objv
 * \param[in] objv array of \e Tcl_Obj objects
 * \return Tcl error-code
 */
static int NS(Help) (
  Tcl_Interp * interp,
  int objc,
  struct Tcl_Obj *const *objv
)
{
  int only_options=0;

  // read the index
  if (objc != 2 && objc != 3) {
    goto wrong_args;
  }

  if (objc == 3) {
    if (strncmp(Tcl_GetStringFromObj (objv[2],NULL),"-options",8)) {
	goto wrong_args;
    } else {
	only_options=1;
    }
  }

  Tcl_ResetResult (interp);

  if (only_options) {
    Tcl_AppendResult (interp,
	MqHelp (NULL),
	"    --help-tclmsgque print tclmsgque specific help\n",
	NULL);
  } else {
    Tcl_AppendResult (interp,
	"usage: tclmsgque MqS [OPTION]... [ARGUMENT]...\n",
	"\n",
	"  tclmsgque MqS [ARGUMENT]... syntax:\n",
	"    tclmsgque MqS [OPTION]... @ server [OPTION]... [ARGUMENT]\n",
	"\n",
	"  Create and configure a new msgque using the options from below.\n",
	"  The command is part of the 'TclMsgque' package available with:\n",
	"      > package require TclMsgque\n",
	"  and returns a handle to the new created msgque\n",
	"      > set cmd [tclmsgque MqS ...]\n",
	"  To delete the msgque and shutdown the communication use:\n",
	"      > rename $cmd \"\"\n",
	"\n",
	MqHelp (NULL),
	"\n",
	"  tclmsgque MqS [OPTION]:\n",
	"    --parent CMD     create new msgque as child of parent CMD\n",
	"    --server         act as a server\n",
	"      --config proc    use proc to configure the server\n",
	"    --filter         act as a filter (requires --server too)\n"
	"      --FTR proc       use proc to serve the filter FTR service\n",
	"      --EOF proc       use proc to serve the filter EOF service\n",
	NULL);
  }

  return TCL_OK;

wrong_args:
    Tcl_WrongNumArgs (interp, 2, objv, "[-options]");
    return TCL_ERROR;
}

static int NS(Main) (
  Tcl_Interp * interp,
  int objc,
  Tcl_Obj * const objv[]
)
{
  if (3 != objc) {
    Tcl_WrongNumArgs(interp, 2, objv, "code");
    return TCL_ERROR;
  }
  if (Tcl_UnsetVar (interp, "MQ_STARTUP_IS_THREAD", TCL_GLOBAL_ONLY) == TCL_ERROR) {
    TclErrorCheck (Tcl_EvalObjEx (interp, objv[2], TCL_EVAL_GLOBAL));
  }
  return TCL_OK;
}

/*****************************************************************************/
/*                                                                           */
/*                                 public                                    */
/*                                                                           */
/*****************************************************************************/

/** \brief handle the \b msgque tcl command
 *
 * \param[in] clientData Tcl mandatory field, not used
 * \param[in] interp the current interpreter
 * \param[in] objc number of arguments in \e objv
 * \param[in] objv argument list as \e Tcl_Obj array
 * \return Tcl error-code
 */
static int NS(MsgqueCmd) (
  ClientData clientData,
  Tcl_Interp * interp,
  int objc,
  struct Tcl_Obj *const *objv
)
{
  int index;

  struct LookupKeyword keys[] = {
    { "MqS",	    NS(MqS_Init)	},  
    { "print",      NS(Print)		},  { "Main",       NS(Main)            },
    { "const",	    NS(Const)		},  { "support",    NS(Support)		},
    { "help",	    NS(Help)		},  { "Init",	    NS(InitCmd)		},
    { NULL,	    NULL		}
  };

  // read the index
  if (objc < 2) {
    Tcl_WrongNumArgs (interp, 1, objv, "subcommand ...");
    return TCL_ERROR;
  }

  TclErrorCheck (Tcl_GetIndexFromObjStruct (interp, objv[1], &keys, 
      sizeof(struct LookupKeyword), "subcommand", 0, &index));

  return (*keys[index].keyF) (interp, objc, objv);
}

/** \brief initialize the tclmsgque package
 *
 * The tclmsgque package is created and one new command "msgque"
 * is added to the Tcl interpreter.
 * \param[in] interp the current interpreter
 * \return Tcl error-code
 */
TCLMQ_EXTERN int
Tclmsgque_Init (
  Tcl_Interp * interp
)
{
  // check for the reight tcl
  if (Tcl_InitStubs (interp, "8.5", 0) == NULL) {
    return TCL_ERROR;
  }

  // announce my package
  TclErrorCheck (Tcl_PkgProvide (interp, "TclMsgque", LIBMSGQUE_VERSION));

  // provide "msgque" as only public cammand of the package
  Tcl_CreateObjCommand (interp, "tclmsgque", NS(MsgqueCmd), (ClientData) NULL,
                        (Tcl_CmdDeleteProc *) NULL);

  // init libmsgque global data
  if (MqInitBuf == NULL && Tcl_GetNameOfExecutable() != NULL) {
    struct MqBufferLS * initB = MqInitCreate();

    if (Tcl_Eval(interp, "info script") == TCL_ERROR)
      return TCL_ERROR;

    MqBufferLAppendC(initB, Tcl_GetNameOfExecutable());
    MqBufferLAppendC(initB, Tcl_GetStringResult(interp));
  }

  return TCL_OK;
}


