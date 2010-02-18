/**
 *  \file       theLink/libmsgque/bufferL.c
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
#include "bufferL.h"
#include "buffer.h"
#include "error.h"
#include "sys.h"
#include "log.h"

BEGIN_C_DECLS

#define MQ_CONTEXT_S context

/*****************************************************************************/
/*                                                                           */
/*                              buffer_init                                  */
/*                                                                           */
/*****************************************************************************/

struct MqBufferLS *
MqBufferLCreate (
  MQ_SIZE const size
)
{
  register struct MqBufferLS * const bufL = (struct MqBufferLS * const) 
      MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof (*bufL));

  bufL->data = (struct MqBufferS **) MqSysCalloc (MQ_ERROR_PANIC, size, sizeof (*bufL->data));
  bufL->size = size;
  bufL->cursize = 0;
  bufL->cur = bufL->data;

  return bufL;
}

struct MqBufferLS *
MqBufferLCreateArgs (
  int const argc,
  MQ_CST argv[]
)
{
  if (argc == 0) {
    return NULL;
  } else {
    struct MqBufferLS * const args = MqBufferLCreate (argc);
    MQ_CST *start = argv;
    MQ_CST *end = argv+argc;
    for (; start < end; start++) {
      MqBufferLAppendC (args, *start);
    }
    return args;
  }
}

struct MqBufferLS *
MqBufferLCreateArgsVA (
  struct MqS * const context,
  va_list ap
)
{
  if (ap == NULL) {
    return NULL;
  } else {
    MQ_CST str;
    struct MqBufferLS * const argv = MqBufferLCreate (5);
    while ( (str=(MQ_CST)va_arg(ap,MQ_CST)) != NULL) {
      MqBufferLAppend (argv, MqBufferCreateC (context, str), -1);
    }
    return argv;
  }
}

struct MqBufferLS *
MqBufferLCreateArgsV (
  struct MqS * const context,
  ...
)
{
  va_list ap;
  struct MqBufferLS * argv;
  va_start(ap, context);
  argv = MqBufferLCreateArgsVA (context, ap);
  va_end(ap);
  return argv;
}

struct MqBufferLS *
MqBufferLCreateArgsVC (
  struct MqS * const context,
  int const   argc,
  MQ_CST      argv[]
)
{
  if (argc == 0 || argv == NULL) {
    return NULL;
  } else {
    struct MqBufferLS * const args = MqBufferLCreate (argc);
    MQ_CST str;
    MQ_CST *end = argv+argc;

    for (; argv < end; argv++) {
      str = *argv;
      MqBufferLAppendC (args, str);
    }
    return args;
  }
}

struct MqBufferLS * 
MqBufferLDup (
  struct MqBufferLS const * const in
)
{
  struct MqBufferLS * ret;
  struct MqBufferS ** retP, ** inP;
  int n;

  if (unlikely(in == NULL)) return NULL;

  ret = MqBufferLCreate(in->cursize);
  retP = ret->data;
  inP  = in->data;
  
  for (n=0; n<in->cursize; n++,inP++,retP++) {
    *retP = MqBufferDup(*inP);
  }  
  ret->cursize = in->cursize;
  ret->cur = ret->data + ret->cursize;
  return ret;
}

enum MqErrorE
pBufferLSplitAlfa (
  struct MqBufferLS ** alfa1P,		///< [in,out] source of split (all arguments after MQ_ALFA will go to alfa2P
  struct MqBufferLS ** alfa2P 		///< [out] the new alfa1 object
)
{
  register struct MqBufferS ** argP, ** endP;
  register struct MqBufferLS * in;
  MQ_SIZE idx;
  struct MqBufferLS * alfa2;
  struct MqBufferS ** inP;

  // initialize
  *alfa2P = NULL;

  // check if "work" is available
  if (!alfa1P || !(*alfa1P)) {
    return MQ_OK;
  }

  if ((*alfa1P)->cursize == 0) {
    MqBufferLDelete (alfa1P);
    return MQ_OK;
  }

  in = *alfa1P;

  // 1. find the MQ_ALFA
  idx = -1;
  {
    argP = in->data;
    endP = in->data + in->cursize;
    while (argP < endP) {
      if ((*argP)->type == MQ_STRT && *((*argP)->cur.C) == MQ_ALFA) {
        struct MqBufferS ** tmpP = argP;
	idx = in->cursize - (endP - argP);
	MqBufferDelete(tmpP);
	break;
      }
      argP++;
    }
  }

  if (idx == -1) return MQ_OK;		// nothing to do *alfa1P == in

  // 2. create alfa1/2
  alfa2 = *alfa2P = MqBufferLCreate (in->cursize-idx-1);
  inP = in->data+idx+1;

  // fill alfa2
  argP = alfa2->data;
  endP = alfa2->data + alfa2->size;
  for (;argP < endP; argP++, inP++) {
    *argP = *inP;
  }  
  alfa2->cursize = alfa2->size;
  alfa2->cur = alfa2->data + alfa2->cursize;

  // clean not used array data from "in" (e.g. *alfa1P)
  in->cur = in->data+idx;
  memset(in->cur, '\0', in->cursize-idx);
  in->cursize = idx;

  return MQ_OK;
}

void
pBufferLDeleteStatic (
  struct MqBufferLS * const bufL
)
{
  register struct MqBufferS * const *arg, * const *end;

  if (unlikely (bufL == NULL))
    return;

  arg = bufL->data;
  end = bufL->data + bufL->cursize;
  
  for (; arg < end; arg++) {
    MqBufferDelete ((struct MqBufferS * *) arg);
  }
  MqSysFree (bufL->data);
}

void
MqBufferLDelete (
  struct MqBufferLS ** bufP
)
{
  if (unlikely(bufP == NULL || *bufP == NULL)) return;
  pBufferLDeleteStatic (*bufP);
  MqSysFree (*bufP);
}

/*****************************************************************************/
/*                                                                           */
/*                              buffer_atom                                  */
/*                                                                           */
/*****************************************************************************/

/// \brief extend \e bufL to \e newSize size
/// \bufL
/// \param newSize expand \e bufL able to hold \e newsize data
static void
sBufferLNewSize (
  register struct MqBufferLS * const bufL,
  MQ_SIZE const newSize
)
{
  bufL->size = newSize + (newSize / 3);  // + 1/3 reserve
  bufL->data = (struct MqBufferS **) MqSysRealloc (MQ_ERROR_PANIC, (MQ_PTR) bufL->data, bufL->size * sizeof (*bufL->data));
  bufL->cur = bufL->data + bufL->cursize;
}

/// \brief wrapper for \e sBufferLNewSize
#define sBufferLAddSize(bufL, addSize) \
    if (unlikely((bufL->cursize + addSize) > bufL->size)) \
        sBufferLNewSize(bufL, (bufL->cursize + addSize))

/*****************************************************************************/
/*                                                                           */
/*                              buffer_append                                */
/*                                                                           */
/*****************************************************************************/

/// \attention after append the \e in belongs to \e bufL
void
MqBufferLAppend (
  register struct MqBufferLS * const bufL,
  struct MqBufferS * const in,
  MQ_SIZE const position
)
{
  sBufferLAddSize (bufL, 1);
  if (position == -1 || position >= bufL->cursize) {
    // append to the end
    *bufL->cur = in;
  } else {
    // append before the end
    struct MqBufferS ** end = bufL->data + bufL->cursize;
    struct MqBufferS ** const arg = bufL->data + position;
    for ( ; arg < end ; end--) {
	*end = *(end-1);
    }
    *arg = in;
  }
  bufL->cursize++;
  bufL->cur++;
}

void
MqBufferLAppendL (
  register struct MqBufferLS * const bufL,
  struct MqBufferLS * const in,
  MQ_SIZE position
)
{
  int shift = in->cursize;
  struct MqBufferS ** end;
  struct MqBufferS ** arg, **argin;

  sBufferLAddSize (bufL, shift);
  if (position == -1 || position >= bufL->cursize) {
    // append to the end
    position = bufL->cursize;
  } else {
    // shift everything after "position" to the right
    end = bufL->data + bufL->cursize - 1 + shift;
    arg = bufL->data + position + shift;
    for ( ; arg <= end ; end--) {
       *end = *(end-shift);
    }
  }
  // copy in data starting at position
  arg = bufL->data + position;
  end = arg + shift;
  argin = in->data;
  for ( ; arg < end ; arg++, argin++) {
      *(arg) = MqBufferDup(*argin);
  }
  bufL->cursize+=shift;
  bufL->cur = bufL->data + bufL->cursize;
}

void
MqBufferLAppendY (
  struct MqBufferLS * const bufL,
  MQ_BYT const val
)
{
  MqBufferLAppend (bufL, MqBufferCreateY (MQ_ERROR_PANIC, val), -1);
}

void
MqBufferLAppendO (
  struct MqBufferLS * const bufL,
  MQ_BOL const val
)
{
  MqBufferLAppend (bufL, MqBufferCreateO (MQ_ERROR_PANIC, val), -1);
}

void
MqBufferLAppendS (
  struct MqBufferLS * const bufL,
  MQ_SRT const val
)
{
  MqBufferLAppend (bufL, MqBufferCreateS (MQ_ERROR_PANIC, val), -1);
}

void
MqBufferLAppendI (
  struct MqBufferLS * const bufL,
  MQ_INT const val
)
{
  MqBufferLAppend (bufL, MqBufferCreateI (MQ_ERROR_PANIC, val), -1);
}

void
MqBufferLAppendF (
  struct MqBufferLS * const bufL,
  MQ_FLT const val
)
{
  MqBufferLAppend (bufL, MqBufferCreateF (MQ_ERROR_PANIC, val), -1);
}

void
MqBufferLAppendW (
  struct MqBufferLS * const bufL,
  MQ_WID const val
)
{
  MqBufferLAppend (bufL, MqBufferCreateW (MQ_ERROR_PANIC, val), -1);
}

void
MqBufferLAppendD (
  struct MqBufferLS * const bufL,
  MQ_DBL const val
)
{
  MqBufferLAppend (bufL, MqBufferCreateD (MQ_ERROR_PANIC, val), -1);
}

void
MqBufferLAppendC (
  struct MqBufferLS * const bufL,
  MQ_CST const val
)
{
  MqBufferLAppend (bufL, MqBufferCreateC (MQ_ERROR_PANIC, val), -1);
}

void
MqBufferLAppendU (
  struct MqBufferLS * const bufL,
  MQ_BUF const val
)
{
  MqBufferLAppend (bufL, val, -1);
}

/*****************************************************************************/
/*                                                                           */
/*                            bufferL_option                                 */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
MqBufferLCheckOptionO (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_CST const opt,
  MQ_BOL * const var
)
{
  MQ_SIZE index;
  MQ_SIZE len;
  *var = MQ_NO;
  if (unlikely(bufL == NULL || bufL->cursize == 0)) return MQ_OK;
  index = 0;
  len = strlen(opt);
  while ((index = MqBufferLSearchC (bufL, opt, len, index)) != -1) {
    *var = MQ_YES;
    MqBufferLDeleteItem (context, bufL, index, 1, MQ_YES);
  }
  return MQ_OK;
}

/// \brief search for an \e option with an #MqBufferAtomU argument in an #MqBufferLS object
/// \context
/// \bufL0
/// \optionL
/// \atom_out
/// \atom_type
/// \param[in] func value extraction function pointer
/// \retMqErrorE
/// \attL
static enum MqErrorE
sBufferLCheckOptionA (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_CST const opt,
  union MqBufferAtomU * const ato,
  enum MqTypeE const type,
  const pBufferGetAF func
)
{
  MQ_SIZE index;
  MQ_SIZE len;
  if (unlikely(bufL == NULL || bufL->cursize == 0)) return MQ_OK;
  index = 0;
  len = strlen(opt);
  while ((index = MqBufferLSearchC (bufL, opt, len, index)) != -1) {
    MqBufferLDeleteItem (context, bufL, index, 1, MQ_YES);
    if (likely (bufL->cursize) && likely (index < bufL->cursize)) {
      MqErrorCheck ((*func) (bufL->data[index], ato, type));
      MqBufferLDeleteItem (context, bufL, index, 1, MQ_YES);
    } else {
      return MqErrorDbV (MQ_ERROR_OPTION_ARG, opt);
    }
  }

  return MQ_OK;

error:
  return MqErrorStack (context);
}

enum MqErrorE 
MqBufferLCheckOptionY (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_CST const opt,
  MQ_BYT * const valP
)
{
  return sBufferLCheckOptionA (context, bufL, opt, (union MqBufferAtomU * const)valP , MQ_BYTT, pBufferGetA1);
}

enum MqErrorE 
MqBufferLCheckOptionS (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_CST const opt,
  MQ_SRT * const valP
)
{
  return sBufferLCheckOptionA (context, bufL, opt, (union MqBufferAtomU * const)valP , MQ_SRTT, pBufferGetA2);
}

enum MqErrorE 
MqBufferLCheckOptionI (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_CST const opt,
  MQ_INT * const valP
)
{
  return sBufferLCheckOptionA (context, bufL, opt, (union MqBufferAtomU * const)valP , MQ_INTT, pBufferGetA4);
}

enum MqErrorE 
MqBufferLCheckOptionF (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_CST const opt,
  MQ_FLT * const valP
)
{
  return sBufferLCheckOptionA (context, bufL, opt, (union MqBufferAtomU * const)valP , MQ_FLTT, pBufferGetA4);
}

enum MqErrorE 
MqBufferLCheckOptionW (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_CST const opt,
  MQ_WID * const valP
)
{
  return sBufferLCheckOptionA (context, bufL, opt, (union MqBufferAtomU * const)valP , MQ_WIDT, pBufferGetA8);
}

enum MqErrorE 
MqBufferLCheckOptionD (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_CST const opt,
  MQ_DBL * const valP
)
{
  return sBufferLCheckOptionA (context, bufL, opt, (union MqBufferAtomU * const)valP , MQ_DBLT, pBufferGetA8);
}

enum MqErrorE
MqBufferLCheckOptionC (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_CST const opt,
  MQ_STR * const var
)
{
  MQ_SIZE index;
  MQ_SIZE len;

  if (!bufL) return MQ_OK;

  index = 0;
  len = strlen(opt);

  while ((index = MqBufferLSearchC (bufL, opt, len, index)) != -1) {
    MqBufferLDeleteItem (context, bufL, index, 1, MQ_YES);
    if (likely (bufL->cursize) && likely (index < bufL->cursize)) {
      MQ_CST tmp;
      MqErrorCheck (MqBufferGetC (bufL->data[index], &tmp));
      MqSysFree(*var);
      *var = mq_strdup(tmp);
      MqBufferLDeleteItem (context, bufL, index, 1, MQ_YES);
    } else {
      return MqErrorDbV (MQ_ERROR_OPTION_ARG, opt);
    }
  }

  return MQ_OK;

error:
  return MqErrorStack (context);
}

enum MqErrorE
MqBufferLCheckOptionU (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_CST const opt,
  MQ_BUF * const var
)
{
  struct MqBufferS * lbuf;
  MQ_SIZE index;
  MQ_SIZE len;

	if (!bufL) return MQ_OK;

  index = 0;
  len = strlen(opt);

  while ((index = MqBufferLSearchC (bufL, opt, len, index)) != -1) {
    MqBufferLDeleteItem (context, bufL, index, 1, MQ_YES);
    if (likely (bufL->cursize) && likely (index < bufL->cursize)) {
      lbuf = bufL->data[index];
      if (*var) MqBufferDelete(var);
      *var = bufL->data[index];
      MqBufferLDeleteItem (context, bufL, index, 1, MQ_NO);
    } else {
      return MqErrorDbV (MQ_ERROR_OPTION_ARG, opt);
    }
  }

  return MQ_OK;
}

/*****************************************************************************/
/*                                                                           */
/*                             bufferL_misc                                  */
/*                                                                           */
/*****************************************************************************/

void
MqBufferLMove (
  struct MqBufferLS * const to,
  struct MqBufferLS ** fromP
)
{
  if (unlikely (to == NULL || fromP == NULL || *fromP == NULL)) {
    return;
  } else {
    struct MqBufferLS * const from = *fromP;
    sBufferLAddSize (to, from->cursize);
    memcpy(to->cur, from->data, from->cursize * sizeof(*to->cur));
    to->cur	    +=  from->cursize;
    to->cursize	    +=  from->cursize;
    from->cur	    =   from->data;
    from->cursize   =   0;
    MqBufferLDelete(fromP);
  }
}

enum MqErrorE
MqBufferLDeleteItem (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  const MQ_SIZE index,
  const MQ_SIZE numitems,
  const MQ_BOL doDelete
)
{
  MQ_SIZE const cursize = bufL->cursize;
  struct MqBufferS * *arg = bufL->data + index;

  if (index < 0 || index+numitems > cursize) {
    return MqErrorV (context, __func__, -1,
	 "wrong index '" MQ_FORMAT_Z "', allowed 0 <= index <= " MQ_FORMAT_Z, 
	    index, cursize - numitems);
  }

  if (doDelete) {
    struct MqBufferS **argSTART = arg, **argEND = arg + numitems;
    for (; argSTART < argEND; argSTART++) {
      MqBufferDelete (argSTART);
    }
  }

  memmove (arg, (arg+numitems), sizeof(struct MqBufferS *)*(cursize-index-numitems));
  bufL->cursize-=numitems;
  bufL->cur-=numitems;

  return MQ_OK;
}

MQ_SIZE
MqBufferLSearchC (
  struct MqBufferLS const * const bufL,
  MQ_CST const str,
  MQ_SIZE const len,
  MQ_SIZE const startindex
)
{
  register struct MqBufferS ** arg = bufL->data + startindex;
  register struct MqBufferS ** const end = bufL->data + bufL->cursize;

  while (arg < end) {
    register struct MqBufferS const * const val = *arg;
    if (val->cursize == len && val->type == MQ_STRT) {
      if (!strcmp (val->cur.C, str)) {
        return bufL->cursize - (end - arg);
      }
    }
    arg++;
  }

  return -1;
}

enum MqErrorE
MqBufferLGetU (
  struct MqS * const context,
  struct MqBufferLS * const bufL,
  MQ_SIZE const index,
  struct MqBufferS ** const var
)
{
  if (likely (bufL->cursize) && likely (index < bufL->cursize)) {
    if (*var) 
      MqBufferDelete(var);
    *var = bufL->data[index];
    MqBufferLDeleteItem (context, bufL, index, 1, MQ_NO);
  } else {
    return MqErrorV (context, __func__, -1, "index '" MQ_FORMAT_Z "' is not available", index);
  }

  return MQ_OK;
}

void
pBufferLSetError (
  struct MqBufferLS * const bufL,
  struct MqS * const context
)
{
  if (unlikely(bufL == NULL)) {
    return;
  } else {
    MQ_SIZE i;
    for (i=0; i<bufL->cursize; i++) {
      bufL->data[i]->context = context;
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*                                debugging                                  */
/*                                                                           */
/*****************************************************************************/

#if defined _DEBUG
void
MqBufferLLog (
  struct MqS const * const context,
  struct MqBufferLS const * const bufL,
  MQ_CST const func
)
{
  char tmp[20];
  int i;

  if (unlikely(bufL == NULL)) {
    MqLogV (context, func, 0, "%s\n", "++++ MqBufferLS (NULL)");
    return;
  }

  MqLogV (context, func, 0, ">>>> MqBufferLS (%p) (" MQ_FORMAT_Z " items)\n", (void*) bufL, bufL->cursize);
  for (i = 0; i < bufL->cursize; i++) {
    sprintf (tmp, "# %i ", i);
    MqBufferLog (context, bufL->data[i], tmp);
    if (i < bufL->cursize - 1)
      MqLogV (context, func, 0, "-----------------------------\n");
  }
  MqLogV (context, func, 0, "<<<< MqBufferLS <<<<\n");
}

void
MqBufferLLogS (
  struct MqS const * const context,
  struct MqBufferLS const * const bufL,
  MQ_CST const func,
  MQ_CST const varname
)
{
  if (unlikely(bufL == NULL)) {
    MqLogV (context, func, 0, "++++ MqBufferLS (%s:NULL)", varname);
    return;
  } else {
    MQ_BUF buf = (context ? context->temp : MqBufferCreate (MQ_ERROR_PANIC, 100));
    int i;
    int relsize = bufL->cur - bufL->data;
    MqLogV (context, func, 0, ">>>> MqBufferLS (%s:%p) (dataP=%p|curP=%p|relsize=%i|cursize=%i|check=%s)\n", varname, (void*) bufL, 
      (void*) bufL->data, (void*) bufL->cur, relsize, bufL->cursize, (relsize == bufL->cursize ? "OK" : "CUR-ERROR"));
    for (i = 0; i < bufL->cursize; i++) {
      MqBufferSetV(buf, "# %i (%p)", i, bufL->data[i]);
      MqBufferLogS (context, bufL->data[i], buf->cur.C);
    }
    MqLogV (context, func, 0, "<<<< MqBufferLS <<<<\n");
    if (!context) MqBufferDelete(&buf);
  }
}

#endif /* _DEBUG */

END_C_DECLS




