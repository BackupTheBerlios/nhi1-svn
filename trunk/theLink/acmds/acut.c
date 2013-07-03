/**
 *  \file       theLink/acmds/acut.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

/** \addtogroup acut
 * \{
 */

#include "msgque.h"
#include "debug.h"
#include <stdlib.h>
#include <string.h>

#define CUTCTX ((struct CutCtxS * const)mqctx)

/// \brief the local \b context of the \ref acut tool
struct CutCtxS {

  struct MqS  mqctx;	    ///< \msgqueI
  MQ_INT      fields[256];  ///< fields used to cut, from the <tt>-f \b FIELDS</tt> option

};

/*****************************************************************************/
/*                                                                           */
/*                               Request Handler                             */
/*                                                                           */
/*****************************************************************************/

static void CutHelp ( const char * ) __attribute__ ((noreturn));

/// \brief display help using \b -h or \b --help command-line option
/// \param base the executable usually: <tt>basename(argv[0])</tt>
static void
CutHelp ( const char * base  )
{
    fprintf(stderr, "usage: %s [OPTION]... [ARGUMENT]...\n", base);
    fputs("\n", stderr);
    fprintf(stderr, "  The %s tool works as a filter between a msgque client and a msgque server\n", base);
    fputs("  using the option '-f' to extract columns.\n", stderr);
    fputs("\n", stderr);
    fputs(MqHelp (base), stderr);
    fputs("\n", stderr);
    fprintf(stderr, "  %s [OPTION]:\n", base);
    fputs("    -f NUM...        the column number(s)  (default: 'empty')\n", stderr);
    fputs("      NUM            a single column with '0' < NUM < '255'\n", stderr);
    fputs("      NUM1,NUM2      multiple columns, use ',' as delimiter\n", stderr);
    fputs("      NUM1-NUM2      multiple columns, from NUM1 up to NUM2\n", stderr);
    fputs("      NUM-           multiple columns, from NUM up to the end\n", stderr);
    fputs("      -NUM           multiple columns, from the beginn up to NUM\n", stderr);
    fputs("    -h, --help       print this help\n", stderr);
    fputs("\n", stderr);

    exit(EXIT_SUCCESS);
}

/// \tool_FTR
static enum MqErrorE
CutFTR (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  struct MqBufferS * buf;
  struct CutCtxS * const cutctx = CUTCTX;
  MQ_SIZE const end = MqReadGetNumItems(mqctx);
  MQ_INT i;
  struct MqS * ftr;
  MqErrorCheck (MqServiceGetFilter (mqctx, 0, &ftr));
  
  MqSendSTART (ftr);
  for (i=0; i<end ; i++) {
    MqErrorCheck (MqReadU (mqctx, &buf));
    if (cutctx->fields[i]) {
      MqSendU (ftr, buf);
    }
  }
  MqErrorCheck (MqSendEND_AND_WAIT (ftr, "+FTR", MQ_TIMEOUT_USER));
error:
  return MqSendRETURN (mqctx);
}

/*****************************************************************************/
/*                                                                           */
/*                                context_init                               */
/*                                                                           */
/*****************************************************************************/

/// \tool_Create
static enum MqErrorE
CutCreate (
  struct MqS  * mqctx,
  struct MqBufferLS ** argvP
)
{
  MQ_STR fieldsC=NULL, end, start;
  MQ_INT index, last=-1;
  struct CutCtxS * const cutctx = CUTCTX;

  // create the Message-Queue
  MqErrorCheck (MqLinkCreate (mqctx, argvP));

  // add services
  MqServiceCreate (mqctx, "+FTR", CutFTR, NULL, NULL);
  MqServiceProxy  (mqctx, "+EOF", 0);

  // get and parse the fields
  MqBufferLCheckOptionC (mqctx, *argvP, "-f", &fieldsC, MQ_NO);
  if (fieldsC == NULL) {
    MqErrorC (mqctx, __func__, 1, "option argument '-f' is required");
    goto error;
  }

  for (start=fieldsC; *start ; start=end) {
    index = (MQ_INT) strtol (start, &end, 10);
    if (index > 255) {
	MqErrorV (mqctx, __func__, 1, 
	    "field index '%i' is larger than the maximum allowed index '255'", index);
	goto error;
    }
    if (start == end) 
	index = 255;
    if (index < 0) {
	last = 0;
	index = (index * -1);
    }
    if (*end == ',' || *end == '\0') {
	if (*end == ',') end++;
	if (last == -1) {
	    cutctx->fields[index] = 1;
	} else {
	    for (; last <= index; last++) {
		cutctx->fields[last] = 1;
	    }
	    last = -1;
	}
    } else if (*end == '-') {
	if (last != -1) {
	  MqErrorC (mqctx, __func__, 1, 
	      "invalid '-' character found, use #-#, -# or #- instead");
	  goto error;
	}
	end++;
	if (*end) {
	    last = index;
	} else {
	    for (; index <= 255; index++) {
		cutctx->fields[index] = 1;
	    }
	}
    } else {
	MqErrorV (mqctx, __func__, 1, 
	    "invalid field character '%c' found, only 'NUMBER', ',' or '-' are allowed", *end);
	goto error;
    }
  }

  // check for wrong arguments
  MqErrorCheck (MqCheckForLeftOverArguments(mqctx, argvP));

  // everything is OK
  MqSysFree (fieldsC);
  return MQ_OK;

error:
  MqSysFree (fieldsC);
  MqErrorStack (mqctx);
  return MqErrorSAppendC(mqctx, "use '-h' or '--help' for usage");
}

enum MqErrorE
CutFactory (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryS * const item,
  struct MqS **contextP
)
{
  struct MqS * const mqctx = MqContextCreate(sizeof(struct CutCtxS),tmpl);
  mqctx->setup.fHelp = CutHelp;
  MqConfigSetName(mqctx, "cut");
  MqConfigSetIsServer(mqctx, MQ_YES);
  mqctx->setup.Parent.fCreate = CutCreate;
  *contextP = mqctx;
  return MQ_OK;
}

/** \} acut */


