/**
 *  \file       theLink/acmds/optionL.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "optionL.h"
#include "debug.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/*****************************************************************************/
/*                                                                           */
/*                    reading options from a MqBufferLS                      */
/*                                                                           */
/*****************************************************************************/

/** \defgroup optionL_api struct OptLS - parsing an MqBufferLS object for options
 *  \ingroup support
 *  \{
 *  \brief parsing options with arguments from an MqBufferLS object
 */

/// \brief parse the \e argv list and find options of syntay \b -NUM \b ARG with 0<=NUM<=255
/// \mqctx
/// \argv
/// \retval out an array of OptLSP as search result
/// \retMqErrorE
enum MqErrorE
OptLCreate (
  struct MqS * const mqctx,
  struct MqBufferLS * const argv,
  struct OptLS ** const out
)
{
  *out = NULL;
  if (argv == NULL) {
    return MQ_OK;
  } else {
    MQ_SIZE  size    = 10;
    MQ_SIZE  cursize = 0;
    OptLSP option  = MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof(OptLS));
    OptLASP data   = MqSysCalloc (MQ_ERROR_PANIC, size, sizeof(OptLAS));

    // check for the sort key
    MQ_STR key, cur, end;
    long idx;

    while (argv->cursize) {
      key = cur = (MQ_STR) (*argv->data)->data;
      if (*cur++ != '-') {
	MqErrorC (mqctx, __func__, 1, "expect column option of form '-NUM' with 0 <= NUM <= 255");
	goto error;
      }
      errno=idx=0;
      idx = strtol (cur, &end, 10);
      if (idx < 0 || idx > 255 || (idx == 0 && errno != 0) || (end-cur) != strlen (cur)) {
	MqErrorV (mqctx, __func__, errno, "number out of range or invalid, unable to convert option '%s' into a column number", key);
	goto error;
      }
      if (argv->cursize==1) {
	MqErrorV (mqctx, __func__, 1, "expect argument for option '%s'", key);
	goto error;
      }
      MqBufferLDeleteItem (mqctx, argv, 0, 1, MQ_YES);

      // check if array has enough space
      if (cursize >= size) {
	  size = (size+size/3);
	  data = MqSysRealloc (MQ_ERROR_PANIC, data, size * sizeof(OptLAS));
      }
    
      // save the data in the array  
      data[cursize].idx = idx;
      data[cursize].val = *argv->data;
      cursize++;

      // cleanup
      MqBufferLDeleteItem (mqctx, argv, 0, 1, MQ_NO);
    }

    // the data back
    option->data = data;
    option->cursize = cursize;
    option->size = size;
    *out = option;

    // everything is OK
    return MQ_OK;

error:
    option->data = data;
    option->cursize = cursize;
    option->size = size;
    OptLDelete (&option);
    return MQ_ERROR;
  }
}

/// \brief delete an OptLSP array created by #OptLCreate
/// \param out a pointer to an OptLSP object
/// \attDelete
void
OptLDelete (
  OptLSP *out
)
{
  OptLSP opt = *out;
  if (opt == NULL) {
    return;
  } else {
    OptLASP start;
    OptLASP end;
    start = opt->data;
    end = opt->data + opt->cursize ;
    for (; start < end; start++) {
      MqBufferDelete (&(start->val));
    }
    free (opt->data);
    MqSysFree (*out);
  }
}

/** \} optionL_api */

