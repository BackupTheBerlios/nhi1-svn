/**
 *  \file       theLink/acmds/asort.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

/** \addtogroup asort
 * \{
 */

#ifdef MQ_IS_POSIX
# include <unistd.h>
#endif

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "msgque.h"
#include "debug.h"
#include "optionL.h"

/// link to the MqErrorS object
#define SORTCTX ((struct SortCtxS * const)mqctx)

/// \brief storage for the relation between \e key and the whole input \e line
struct SortKeyS {
    struct MqBufferS *	key;		///< the sort key as \b -NUM index from the input \e line
    struct MqBufferLS * line;		///< the whole input line
};

/// \brief the local \b context of the \ref asort tool
struct SortCtxS {
  struct MqS mqctx;		///< \msgqueI
  MQ_SIZE	    idx;		///< the column index from: <tt>\b -NUM TYPE</tt>
  enum MqTypeE	    type;		///< type of the column index from: <tt>-NUM \b TYPE</tt>
  struct SortKeyS   *sort;		///< the array of sort keys filled in #SortFTR
  MQ_SIZE	    sort_size;		///< the total amount of the allocated SortKeyS objects in the #sort array
  MQ_SIZE	    sort_cursize;	///< the used number of SortKeyS objects in the #sort array
};

/*****************************************************************************/
/*                                                                           */
/*                               Request Handler                             */
/*                                                                           */
/*****************************************************************************/

static void SortHelp ( const char * ) __attribute__ ((noreturn));

/// \brief display help using \b -h or \b --help command-line option
/// \param base the exesortable usually: <tt>basename(argv[0])</tt>
static void
SortHelp ( const char * base  )
{
    fprintf(stderr, "usage: %s [OPTION]... [ARGUMENT]...\n", base);
    fputs("\n", stderr);
    fprintf(stderr, "  The %s tool works as a filter between a msgque client and a msgque server\n", base);
    fputs("  using the option '-# TYPE' to identify a column as sort index.\n", stderr);
    fputs("\n", stderr);
    fputs(MqHelp (base), stderr);
    fputs("\n", stderr);
    fprintf(stderr, "  %s [OPTION]:\n", base);
    fputs("    -NUM TYPE        the column number(s)  (default: 'empty'\n", stderr);
    fputs("      NUM            a single column with '0' < NUM < '255'\n", stderr);
    fputs("      TYPE           the sort type of the column C,I,W,D\n", stderr);
    fputs("       C             sort as string\n", stderr);
    fputs("       I,W           sort as I)nt or W)ide integer\n", stderr);
    fputs("       D             sort as double precision decimal\n", stderr);
    fputs("    -h, --help       print this help\n", stderr);
    fputs("\n", stderr);

    exit(EXIT_SUCCESS);
}

/// \brief compare function for qsort (man qsort)
/// \param left left site of the comparison
/// \param right right site of the comparison
/// \return \b <0 for smaller, \b 0 for equal or >0 for larger
static int
SortComp (
    const void *left,
    const void *right
)
{
  struct MqBufferS * l = ((struct SortKeyS *) left)->key;
  struct MqBufferS * r = ((struct SortKeyS *) right)->key;

  // get the right 'compare' function

  switch (l->type) {
    case MQ_STRT: return strcmp (l->cur.C, r->cur.C);
    case MQ_INTT: return (MqBufU2INT(l->cur) > MqBufU2INT(r->cur) ? 1 : MqBufU2INT(l->cur) < MqBufU2INT(r->cur) ? -1 : 0);
    case MQ_WIDT: return (MqBufU2WID(l->cur) > MqBufU2WID(r->cur) ? 1 : MqBufU2WID(l->cur) < MqBufU2WID(r->cur) ? -1 : 0);
    case MQ_DBLT: return (MqBufU2DBL(l->cur) > MqBufU2DBL(r->cur) ? 1 : MqBufU2DBL(l->cur) < MqBufU2DBL(r->cur) ? -1 : 0);
    default: return 0;
  }
  return 0;
}

/// \brief service-handler for the system token \b _EOF used in filters after
///   the last line received
/// \msgque
/// \param data not used, allways \b NULL
/// \retMqErrorE
static enum MqErrorE
SortEOF (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  struct SortCtxS * const sortctx = SORTCTX;
  struct SortKeyS * start = sortctx->sort;
  struct SortKeyS * end   = sortctx->sort + sortctx->sort_cursize;
  struct MqS * ftr;

  MqErrorCheck (MqServiceGetFilter (mqctx, 0, &ftr));

  // sort the data
  qsort(sortctx->sort, sortctx->sort_cursize, sizeof(struct SortKeyS), SortComp);

  for (; start < end; start++) {
    MqSendSTART (ftr);
    MqSendL (ftr, start->line);
    MqErrorCheck (MqSendEND_AND_WAIT (ftr, "+FTR", MQ_TIMEOUT_USER));
    MqBufferLDelete (&start->line);
    start->key = NULL;
  }

  // cleanup after sending
  sortctx->sort_cursize = 0;

  MqSendSTART(ftr);
  MqErrorCheck (MqSendEND_AND_WAIT (ftr, "+EOF", MQ_TIMEOUT_USER));
error: 
  return MqSendRETURN (mqctx);
}

/// \tool_FTR
static enum MqErrorE
SortFTR (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  struct SortCtxS * const sortctx = SORTCTX;
  struct MqBufferLS * line = NULL;
  MQ_SIZE sort_size = sortctx->sort_size;
  MQ_SIZE sort_cursize = sortctx->sort_cursize;
  struct SortKeyS * sortkey;
  struct MqBufferS * key;

  // create the list of all items in MqReadS
  MqErrorCheck (MqReadL (mqctx, &line));

  // check if sort key is large enough
  if (sort_cursize >= sort_size) {
    sort_size = (sort_size+sort_size*1/3);
    sortctx->sort = MqSysRealloc (MQ_ERROR_PANIC, sortctx->sort, sort_size * sizeof (struct SortKeyS));
    sortctx->sort_size = sort_size;
  }

  // add the idx'th column to the search key
  if (sortctx->idx > line->cursize) {
    MqBufferLDelete (&line);
    return MqErrorV (mqctx, __func__, 1, 
	"can not get column '" MQ_FORMAT_Z "' from input line-number '" 
	    MQ_FORMAT_Z "'", sortctx->idx, sort_cursize );
  }

  // save the current line into the sort key
  sortkey = &sortctx->sort[sort_cursize];
  key = line->data[sortctx->idx];

  // save and cast the key into the given type
  sortkey->key = key;
  sortkey->line = line;
  sortctx->sort_cursize++;
  if (key->type != sortctx->type) {
    MqErrorCheck( MqBufferCastTo (key, sortctx->type));
  }
error:
  return MqSendRETURN (mqctx);
}

/*****************************************************************************/
/*                                                                           */
/*                                sortctx_init                               */
/*                                                                           */
/*****************************************************************************/

/// \tool_Delete
static void
SortDelete (
  struct MqS * const mqctx
)
{
  struct SortCtxS *const sortctx = SORTCTX;
  struct SortKeyS *start = sortctx->sort;
  struct SortKeyS *const end = start + sortctx->sort_cursize;

  // cleanup array items
  for (; start < end; start++) {
    MqBufferLDelete (&start->line);
    start->key = NULL;
  }

  // cleanup entire array
  MqSysFree (sortctx->sort);
  MqLinkDelete (mqctx);
}

/// \tool_Create
static enum MqErrorE
SortCreate (
  struct MqS * mqctx,
  struct MqBufferLS ** argvP
)
{
  struct SortCtxS * const sortctx = SORTCTX;
  OptLSP sortcols;
  MQ_CST key;
  char type;

  // create the Message-Queue
  MqErrorCheck (MqLinkCreate (mqctx, argvP));

  // add service
  MqErrorCheck (MqServiceCreate (mqctx, "+FTR", SortFTR, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "+EOF", SortEOF, NULL, NULL));

  // initialize the sort index
  sortctx->sort = (struct SortKeyS *) MqSysMalloc (MQ_ERROR_PANIC, 10 * sizeof(struct SortKeyS));
  sortctx->sort_size = 10;

  // check for the sort key
  MqErrorCheck ( OptLCreate (mqctx, *argvP, &sortcols));
  if (sortcols->cursize != 1) {
    OptLDelete (&sortcols);
    MqErrorC (mqctx, __func__, -1,
		"expect exact 'one' sort-column of format '-NUM TYPE'");
    goto error;
  }

  MqBufferGetC(sortcols->data[0].val,&key);
  type = *key;

  if (strlen (key) > 1 || ( type != 'C' && type != 'I' && type != 'W' && type != 'D')) {
    MqErrorV (mqctx, __func__, 1, 
		"invalid type '%s', expect 'C,I,W or D'", key);
    // !attention! the !key! frum upper code is part of the "sortcols" memory
    OptLDelete (&sortcols);
    goto error;
  }

  sortctx->idx = sortcols->data[0].idx;

  switch (type) {
    case 'C': sortctx->type = MQ_STRT;	break;
    case 'I': sortctx->type = MQ_INTT;	break;
    case 'W': sortctx->type = MQ_WIDT;	break;
    case 'D': sortctx->type = MQ_DBLT;	break;
  }

  // cleanup
  OptLDelete (&sortcols);

  // check for wrong arguments
  MqErrorCheck (MqCheckForLeftOverArguments(mqctx, argvP));

  return MQ_OK;

error:
  MqErrorStack (mqctx);
  return MqErrorSAppendC(mqctx, "use '-h' or '--help' for usage");
}

enum MqErrorE
SortFactory (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  MQ_PTR data, 
  struct MqS **contextP
)
{
  struct MqS * const mqctx = MqContextCreate(sizeof(struct SortCtxS),tmpl);
  mqctx->setup.fHelp = SortHelp;
  MqConfigSetName(mqctx, "sort");
  if (create != MQ_FACTORY_NEW_FILTER) {
    MqConfigSetIsServer(mqctx, MQ_YES);
    mqctx->setup.Parent.fCreate = SortCreate;
    mqctx->setup.Parent.fDelete = SortDelete;
    mqctx->setup.Factory.Create.fCall = SortFactory;
  }
  *contextP = mqctx;
  return MQ_OK;
}

/** \} asort */


