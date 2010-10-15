/**
 *  \file       theLink/acmds/ajoin.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

/** \addtogroup ajoin
 * \{
 */

#include <stdlib.h>
#include <string.h>
#include "msgque.h"
#include "debug.h"
#include "optionL.h"

#define JOINCTX ((struct JoinCtxS *const)mqctx)
#define MQCTX ((struct MqS *const)joinctx)
#define MQ_CONTEXT_S mqctx

/// \brief the local \b context of the \ref ajoin tool
struct JoinCtxS {
  struct MqS 	mqctx;		///< \msgqueI
  MQ_STR		delimiter;	///< string used to join columns together (from: -d DELIMITER)
  MQ_STR		format[256];    ///< array of format items for the first 256 columns
  enum MqTypeE		type[256];	///< the type of the format (from: -NUM FORMAT)
  OptLSP		formatlist;	///< the output from the #OptLCreate function used as storage for
					///  #format strings
  MQ_STR		free[256];      ///< array of temporary formats ready to be freed
};

/*****************************************************************************/
/*                                                                           */
/*                               Request Handler                             */
/*                                                                           */
/*****************************************************************************/

static void JoinHelp ( const char * ) __attribute__ ((noreturn));

/// \brief display help using \b -h or \b --help command-line option
/// \param base the executable usually: <tt>basename(argv[0])</tt>
static void
JoinHelp ( const char * base  )
{
    fprintf(stderr, "usage: %s [OPTION]... [ARGUMENT]...\n", base);
    fputs("\n", stderr);
    fprintf(stderr, "  The %s tool expect data from a msgque client as input and create for\n", base);
    fputs("  every input package an stdout output string by joining the objects of the\n", stderr);
    fputs("  input package together using the delimiter '-d'.\n", stderr);
    fputs("\n", stderr);
    fprintf(stderr, "  %s [ARGUMENT]... syntax:\n", base);
    fprintf(stderr, "    aclient [OPTIONS]... @ %s [OPTIONS]... | ...\n", base);
    fputs("\n", stderr);
    fputs(MqHelp (NULL), stderr);
    fputs("\n", stderr);
    fprintf(stderr,"  %s [OPTION]:\n", base);
    fputs("    -d CHAR          the delimiter         (default: ' ')\n", stderr);
    fputs("    -NUM FORTMAT     the column number(s)  (default: 'empty'\n", stderr);
    fputs("      NUM            a single column with '0' < NUM < '255'\n", stderr);
    fputs("      FORMAT         printf like format    (default: '%s'\n", stderr);
    fputs("    -h, --help       print this help\n", stderr);
    fputs("\n", stderr);

    exit(EXIT_SUCCESS);
}

/// \tool_FTR
static enum MqErrorE
JoinFTR (
  struct MqS * const mqctx,
  MQ_CST prefix,
  MQ_PTR data
)
{
  MQ_CST readStr;
  MQ_INT readInt;
  MQ_WID readWid;
  MQ_DBL readDbl;
  MQ_STR delimiter = JOINCTX->delimiter;
  MQ_STR *format = JOINCTX->format;
  enum MqTypeE *type = JOINCTX->type;
  MQ_SIZE const end = MqReadGetNumItems(mqctx);

  MQ_INT i;
  if (!end) goto error;
  for (i=0; i<end ; i++, format++, type++) {
    if (*format) {
	switch (*type) {
	  case MQ_STRT:
	    MqErrorCheck (MqReadC (mqctx, &readStr));
	    printf (*format, readStr);
	    break;
	  case MQ_INTT:
	    MqErrorCheck (MqReadI (mqctx, &readInt));
	    printf (*format, readInt);
	    break;
	  case MQ_WIDT:
	    MqErrorCheck (MqReadW (mqctx, &readWid));
	    printf (*format, readWid);
	    break;
	  case MQ_DBLT:
	    MqErrorCheck (MqReadD (mqctx, &readDbl));
	    printf (*format, readDbl);
	    break;
	  default:
	    break;
	}
    } else {
	MqErrorCheck (MqReadC (mqctx, &readStr));
	fputs (readStr,stdout);
    }
    fputs ((i < (end-1) ? delimiter : "\n"),stdout);
    
  }
  fflush(stdout);
error:
  return MqSendRETURN (mqctx);
}

/*****************************************************************************/
/*                                                                           */
/*                                context_init                               */
/*                                                                           */
/*****************************************************************************/

/// \tool_Delete
static void
JoinDelete (
  struct MqS *mqctx
)
{
  struct JoinCtxS * joinctx = JOINCTX;
  MQ_STR *freeP = joinctx->free;

  while (*freeP) {
    free(*freeP++);
  }

  OptLDelete (&joinctx->formatlist);
  MqSysFree (joinctx->delimiter);
  MqLinkDelete (mqctx);
}

/// \tool_Create
enum MqErrorE
JoinCreate (
  struct MqS *  mqctx,
  struct MqBufferLS ** argvP
)
{
  struct JoinCtxS * const joinctx = JOINCTX;
  OptLSP format;

  // create the Message-Queue
  MqErrorCheck (MqLinkCreate (mqctx, argvP));

  MqErrorCheck (MqServiceCreate (mqctx, "+FTR", JoinFTR, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "+EOF", NULL,    NULL, NULL));
  
  // initial values
  joinctx->delimiter = mq_strdup(" ");

  // get the delimiter
  MqErrorCheck (MqBufferLCheckOptionC (mqctx, *argvP, "-d", &joinctx->delimiter));

  // check for the format key
  MqErrorCheck (OptLCreate (mqctx, *argvP, &joinctx->formatlist));
  format = joinctx->formatlist;

  // format found
  if (format->cursize) {

    MQ_SIZE i,idx;
    MQ_STR cur,fmt;
#if defined(MQ_IS_WIN32)
    MQ_STR modfmt = NULL;
    MQ_INT freeIndex = 0;
#endif
    int found, myshort, mylonglong;
    enum MqTypeE type;
    char id[128];
    memset(id,0,128);
    
    // init the search array
    id['h'] = 1;	    // short
    id['l'] = 2;	    // long or longlong
    id['q'] = 3;	    // old: longlong
    id['s'] = 4;	    // string
    id['e'] = id['E'] = id['f'] = id['F'] = id['a'] = id['A'] = id['g'] = id['G'] = 5;	// double
    id['d'] = id['i'] = id['o'] = id['u'] = id['x'] = id['X'] = 6;    // integer

    for (i=0; i<format->cursize; i++) {
      idx = format->data[i].idx;
      joinctx->format[idx] = cur = fmt = (MQ_STR) format->data[i].val->data;;
      found=myshort=mylonglong=0;
      type=-1;
      while (*cur != '\0') {
	if (*cur != '%') {	// skip != %
	  cur++; continue;
	}
	if (*(cur+1) == '%') {	// skip %%
	  cur+=2; continue;
	}
	if (found) {
	  MqErrorV(mqctx,__func__,-1,
	  "column '" MQ_FORMAT_Z "' has multiple '%%' formats '%s' but only one is allowed", 
	      idx, fmt);
	  goto error;
	}
	found=1;
	cur++;
	while (*cur != '\0' && *cur != '%') {
	  if (*cur > 0 && id[(unsigned char)*cur]) {
	    switch (id[(unsigned char)*cur]) {
	      case 1:
		myshort=1; break;
	      case 2:
		if (*(cur+1) == 'l') {
#if defined(MQ_IS_WIN32)
		  modfmt=cur;
#endif
		  mylonglong=1; cur+=2; continue;
		}
		break;
	      case 3:
		mylonglong=1; break;
	      case 4:
		type = MQ_STRT; goto end;
	      case 5:
		type = MQ_DBLT; goto end;
	      case 6:
		if (mylonglong) {
		  type = MQ_WIDT; goto end;
		} else {
		  type = MQ_INTT; goto end;
		}
	    }
	  }
	  cur++;
	}
end:
	cur++;
      }

#  if defined(MQ_IS_WIN32)
    // if the system is WIN32 we have to transform the %ll(x) format to %I64(n) format -> one extra char
      if (type == MQ_WIDT && modfmt != NULL) {
	  MQ_STR tmp, tfmt=fmt;
	  tmp = cur = MqSysMalloc(MQ_ERROR_PANIC, strlen(joinctx->format[idx])+1+1);
	  // 1. copy everything up to modfmt
	  while (tfmt < modfmt) {
	      *cur++ = *tfmt++;
	  }
	  // 2. add I64
	  *cur++ = 'I';
	  *cur++ = '6';
	  *cur++ = '4';
	  modfmt+=2;
	  // 3. add everything after  the %ll format
	  while (*modfmt != '\0') *cur++ = *modfmt++;
	  // 4. add final '\0'
	  *cur++ = '\0';
	  // 5. activate the new format
	  joinctx->free[freeIndex] = tmp;
	  joinctx->format[idx] = tmp;
	  freeIndex++;
      }
#  endif

  //printLog(context->link, "idx<%i>, type<%s>, format<%s>\n", idx, MqLogTypeName(type), fmt);
      if (type == -1) {
	MqErrorV(mqctx,__func__,-1,
	  "column '" MQ_FORMAT_Z "' has no valid format '%s'", idx, fmt);
	goto error;
      }
      joinctx->type[idx] = type;
    }
  }

  // check for wrong arguments
  MqErrorCheck (MqCheckForLeftOverArguments(mqctx, argvP));

  // everything is OK
  return MQ_OK;

error:
  MqBufferLDelete (argvP);
  MqErrorStack (mqctx);
  return MqErrorSAppendC(mqctx, "use '-h' or '--help' for usage");
}

enum MqErrorE
JoinFactory (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  MQ_PTR data, 
  struct MqS **contextP
)
{
  struct MqS * const mqctx = MqContextCreate(sizeof(struct JoinCtxS), tmpl);
  mqctx->setup.fHelp = JoinHelp;
  MqConfigSetName(mqctx, "join");
  MqConfigSetIsServer(mqctx, MQ_YES);
  mqctx->setup.Parent.fCreate = JoinCreate;
  mqctx->setup.Parent.fDelete = JoinDelete;
  mqctx->setup.Factory.Create.fCall = JoinFactory;
  *contextP = mqctx;
  return MQ_OK;
}

/** \} server */

