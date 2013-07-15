/**
 *  \file       theLink/libmsgque/buffer.c
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

#include <errno.h>
#include <math.h>
#include <limits.h>
#include <float.h>

#define MQ_ERROR_S buf->context
#define MQ_CONTEXT_S context

BEGIN_C_DECLS

static struct MqBufferS *
sBufferSetA1 ( struct MqBufferS * const, union MqBufferAtomU const, enum MqTypeE const);
static struct MqBufferS *
sBufferSetA2 ( struct MqBufferS * const, union MqBufferAtomU const, enum MqTypeE const);
static struct MqBufferS *
sBufferSetA4 ( struct MqBufferS * const, union MqBufferAtomU const, enum MqTypeE const);
static struct MqBufferS *
sBufferSetA8 ( struct MqBufferS * const, union MqBufferAtomU const, enum MqTypeE const);

// can be ((*buf->cur.A).Y)
MQ_BYT MqBufU2BYT (
  union MqBufferU buf
) {
#if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
  MQ_ATO a;
  a.B[0] = (*buf.A).B[0];
  return a.Y;
#else
  return (*buf.A).Y;
#endif
}

// can be ((*buf->cur.A).O)
MQ_BOL MqBufU2BOL (
  union MqBufferU buf
) {
#if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
  MQ_ATO a;
  a.B[0] = (*buf.A).B[0];
  return a.O;
#else
  return (*buf.A).O;
#endif
}

// can be ((*buf->cur.A).S)
MQ_SRT MqBufU2SRT (
  union MqBufferU buf
) {
#if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
  MQ_ATO a;
  a.B[0] = (*buf.A).B[0];
  a.B[1] = (*buf.A).B[1];
  return a.S;
#else
  return (*buf.A).S;
#endif
}

// can be ((*buf->cur.A).I)
MQ_INT MqBufU2INT (
  union MqBufferU buf
) {
#if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
  MQ_ATO a;
  a.B[0] = (*buf.A).B[0];
  a.B[1] = (*buf.A).B[1];
  a.B[2] = (*buf.A).B[2];
  a.B[3] = (*buf.A).B[3];
  return a.S;
#else
  return (*buf.A).I;
#endif
}

// can be ((*buf->cur.A).F)
MQ_FLT MqBufU2FLT (
  union MqBufferU buf
) {
#if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
  MQ_ATO a;
  a.B[0] = (*buf.A).B[0];
  a.B[1] = (*buf.A).B[1];
  a.B[2] = (*buf.A).B[2];
  a.B[3] = (*buf.A).B[3];
  return a.F;
#else
  return (*buf.A).F;
#endif
}

// can be ((*buf->cur.A).W)
MQ_WID MqBufU2WID (
  union MqBufferU buf
) {
#if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
  MQ_WID w;
  memcpy(&w,buf.B,sizeof(MQ_WID));
  return w;
#else
  return (*buf.A).W;
#endif
}

// can be ((*buf->cur.A).W)
MQ_TRA pBufU2TRA (
  union MqBufferU buf
) {
#if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
  MQ_TRA w;
  memcpy(&w,buf.B,sizeof(MQ_TRA));
  return w;
#else
  return (*buf.A).T;
#endif
}

// can be (*((MQ_DBL const *)(bin)))
MQ_DBL MqBufU2DBL (
  union MqBufferU buf
) {
#if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
  MQ_DBL d;
  memcpy(&d,buf.B,sizeof(MQ_DBL));
  return d;
#else
  return (*buf.A).D;
#endif
}

// can be (*in->cur.A)
MQ_ATO MqBufU2ATO (
  union MqBufferU buf
) {
#if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
  MQ_ATO p;
  memcpy(&p,buf.B,sizeof(MQ_ATO));
  return p;
#else
  return *buf.A;
#endif
}

/*****************************************************************************/
/*                                                                           */
/*                              buffer_init                                  */
/*                                                                           */
/*****************************************************************************/

struct MqBufferS *
MqBufferCreate (
  struct MqS * const context,
  MQ_SIZE size
)
{
  register struct MqBufferS * const buf = (struct MqBufferS * const) MqSysMalloc(MQ_ERROR_PANIC, sizeof (*buf));

  // +1 to allow strings with len=strlen(str) fit into the buffer with an additional
  // '\0' at the end
  if (size > MQ_BLS_SIZE) {
    buf->data = (MQ_BIN) MqSysCalloc(MQ_ERROR_PANIC, (size+1), sizeof (*buf->data));
    buf->size = size;
    buf->bits.alloc = MQ_ALLOC_DYNAMIC;
  } else {
    buf->data = (MQ_BIN) memset(buf->bls,'\0',MQ_BLS_SIZE);
    buf->size = MQ_BLS_SIZE;
    buf->bits.alloc = MQ_ALLOC_STATIC;
  }
  buf->context = context;
  buf->cursize = 0;
  buf->numItems = 0;
  buf->cur.B = buf->data;
  buf->type = MQ_STRT;
  buf->signature = MQ_MqBufferS_SIGNATURE;
  buf->bits.ref = MQ_REF_GLOBAL;

  return buf;
}

/*
/// \brief create a new \e MqBufferS with \a size and a external memory \b not managed by \libmsgque
/// \context
/// \param data the external memory
/// \param size the size of the buffer to create
/// \attention be aware of the following restrictions:
///   - the user is responsible for creating and deleting the external memory, \libmsgque
///     is responsibilty for for creating and deleting the #MqBufferS object
///   - the external memory is \b read \b only, every modification of the object
///     (appending or setting) will convert the object into an dynamic memory object and
///     the original memory will \b not be used anymore
///   - the #MqBufferS object have to be deleted with #MqBufferDelete
///   - type type of the new object is #MQ_BINT
///   .
struct MqBufferS * MqBufferCreateBinary (
  struct MqS * const context,
  MQ_BIN data,
  const MQ_SIZE size
);
struct MqBufferS *
MqBufferCreateBinary (
  register struct MqS * const context,
  MQ_BIN data,
  const MQ_SIZE size
)
{
  struct MqBufferS * buf = MqSysMalloc (context, sizeof (*buf));

  buf->context = context;
  buf->data = data;
  buf->size = size;
  buf->cursize = 0;
  buf->numItems = 0;
  buf->cur.B = data;
  buf->alloc = MQ_ALLOC_STATIC;
  buf->type = MQ_BINT;

  return buf;
}
*/

struct MqBufferS *
pBufferCreateRef (
  struct MqBufferS const * const buf
)
{
  struct MqBufferS * const ret = (struct MqBufferS *) MqSysMalloc (MQ_ERROR_PANIC, sizeof (*ret));
  memcpy (ret, buf, sizeof (*ret));
  ret->bits.alloc = MQ_ALLOC_STATIC;
  ret->bits.ref = MQ_REF_LOCAL;
  return ret;
}

void
pBufferDeleteRef (
  struct MqBufferS ** const bufP
)
{
  if (unlikely (bufP == NULL || *bufP == NULL)) return;
  (*bufP)->signature = 0;
  MqSysFree (*bufP);
}

void
MqBufferDelete (
  struct MqBufferS ** const bufP
)
{
  struct MqBufferS *buf;
  // do NOT detete an invalid pointer
  if (unlikely (bufP == NULL || (buf=*bufP) == NULL)) return;
  // do NOT delete an invalid buffer object
  if (buf->signature != MQ_MqBufferS_SIGNATURE) return;
  // do not delete a local refernence
  if (buf->bits.ref == MQ_REF_LOCAL) return;
  // do NOT delete STATIC storage 
  if (buf->data && buf->data != buf->bls && buf->bits.alloc == MQ_ALLOC_DYNAMIC ) {
    MqSysFree (buf->data);
  }
  // NOW delete
  buf->signature = 0;
  MqSysFree (*bufP);
}

void
MqBufferReset (
  struct MqBufferS * const buf
)
{
  buf->cur.B = buf->data;
  buf->cursize = 0;
  buf->numItems = 0;
}

/*****************************************************************************/
/*                                                                           */
/*                           buffer_create_type                              */
/*                                                                           */
/*****************************************************************************/

struct MqBufferS * 
MqBufferCreateY (
  struct MqS * const context,
  MQ_BYT const val
)
{
  union MqBufferAtomU ato;
  ato.Y = val;
  return sBufferSetA1(MqBufferCreate (context, 0), ato, MQ_BYTT);
}

struct MqBufferS * 
MqBufferCreateO (
  struct MqS * const context,
  MQ_BOL const val
)
{
  union MqBufferAtomU ato;
  ato.O = val;
  return sBufferSetA1(MqBufferCreate (context,0), ato, MQ_BOLT);
}

struct MqBufferS * 
MqBufferCreateS (
  struct MqS * const context,
  MQ_SRT const val
)
{
  union MqBufferAtomU ato;
  ato.S = val;
  return sBufferSetA2(MqBufferCreate (context,0), ato, MQ_SRTT);
}

struct MqBufferS * 
MqBufferCreateI (
  struct MqS * const context,
  MQ_INT const val
)
{
  union MqBufferAtomU ato;
  ato.I = val;
  return sBufferSetA4(MqBufferCreate (context,0), ato, MQ_INTT);
}

struct MqBufferS * 
MqBufferCreateF (
  struct MqS * const context,
  MQ_FLT const val
)
{
  union MqBufferAtomU ato;
  ato.F = val;
  return sBufferSetA4(MqBufferCreate (context,0), ato, MQ_FLTT);
}

struct MqBufferS * 
MqBufferCreateW (
  struct MqS * const context,
  MQ_WID const val
)
{
  union MqBufferAtomU ato;
  ato.W = val;
  return sBufferSetA8(MqBufferCreate (context,0), ato, MQ_WIDT);
}

struct MqBufferS * 
MqBufferCreateD (
  struct MqS * const context,
  MQ_DBL const val
)
{
  union MqBufferAtomU ato;
  ato.D = val;
  return sBufferSetA8(MqBufferCreate (context,0), ato, MQ_DBLT);
}

struct MqBufferS * 
MqBufferCreateC (
  struct MqS * const context,
  MQ_CST const val
)
{
  return MqBufferSetC(MqBufferCreate (context,0), val);
}

struct MqBufferS * 
MqBufferCreateB (
  struct MqS * const context,
  MQ_CBI  const val,
  MQ_SIZE const len
)
{
  return MqBufferSetB(MqBufferCreate (context, 0), val, len);
}

struct MqBufferS * 
MqBufferCreateU (
  struct MqS * const context,
  struct MqBufferS const * const val
)
{
  return MqBufferSetU(MqBufferCreate (context,0), val);
}

/*****************************************************************************/
/*                                                                           */
/*                              buffer_atom                                  */
/*                                                                           */
/*****************************************************************************/

void
_pBufferNewSize (
  register struct MqBufferS * const buf,
  MQ_CST const header,
  MQ_SIZE const newSize
)
{
//if (newSize == buf->size+1) {
//  printXLV (MQ_ERROR_PRINT, "buffer SIZE warning: newSize<%i> = buf->size<%i> + 1 !!\n", newSize, buf->size);
//}

  MQ_SIZE size = newSize + (newSize / 3);  // + 1/3 reserve

  if (unlikely (buf->bits.alloc == MQ_ALLOC_DYNAMIC)) {
    // this is "MQ_ALLOC_DYNAMIC"
    // the buf->size+1 is used to fit a string with '\0' and strlen() size into the buffer
    buf->data = (MQ_BIN) MqSysRealloc (MQ_ERROR_PANIC, buf->data, size+1);
  } else {
    // this is "MQ_ALLOC_STATIC"
    buf->data = (MQ_BIN)MqSysMalloc (MQ_ERROR_PANIC, size+1);
    buf->bits.alloc = MQ_ALLOC_DYNAMIC;
  }
  buf->size = size;
  buf->cur.B = buf->data + buf->cursize;
  memset (buf->cur.B, '\0', (buf->size - buf->cursize));
  // additional debugging output
  //printXLV (buf->context, "increase buffer size to " MQ_FORMAT_Z " bytes\n", buf->size);
}

/*****************************************************************************/
/*                                                                           */
/*                              buffer_get                                   */
/*                                                                           */
/*****************************************************************************/

#define RANGE_E(txt,type) \
    MqErrorSGenV(MQ_ERROR_S, __func__, MQ_ERROR, -1, \
        #txt " ERROR, can not convert '%s' into an '" #type "' object", buf->cur.C)

#define TYPE_E(ttype,stype) \
    MqErrorSGenV(MQ_ERROR_S, __func__, MQ_ERROR, -1, \
        "type ERROR, it is not allowed to convert an '%s' object into an '%s' object", \
	MqLogTypeName(stype), MqLogTypeName(ttype))


/// \brief MQ_BYT maximum
#define MQ_BYT_MAX      SCHAR_MAX
/// \brief MQ_BYT minimum
#define MQ_BYT_MIN      SCHAR_MIN

/// \brief MQ_SRT maximum
#define MQ_SRT_MAX      SHRT_MAX
/// \brief MQ_SRT minimum
#define MQ_SRT_MIN      SHRT_MIN

/// \brief MQ_INT maximum
#define MQ_INT_MAX      INT_MAX
/// \brief MQ_INT minimum
#define MQ_INT_MIN      INT_MIN

/// \brief MQ_WID maximum
#define MQ_WID_MAX      LLONG_MAX
/// \brief MQ_WID minimum
#define MQ_WID_MIN      LLONG_MIN

enum MqErrorE
pBufferGetA1 (
  struct MqBufferS * const buf,
  union MqBufferAtomU * const ato,
  enum MqTypeE const type 
)
{
  buf->cur.B = buf->data;
  if (likely(type == buf->type)) {
    (*ato).Y = (*buf->cur.A).Y;
  } else if ( buf->type == MQ_STRT) {
    if (type == MQ_BYTT) {
      long ret;
      MQ_STR end;
      errno = 0;
      ret = str2int (buf->cur.C, &end, 0);
      (*ato).Y = (MQ_BYT) ret;
    // on 32bit: long=int
      if (unlikely (errno == ERANGE)) {
	if (unlikely (ret == LONG_MIN)) {
	  return RANGE_E (underflow, BYT);
	} else if (unlikely (ret == LONG_MAX)) {
	  return RANGE_E (overflow, BYT);
	}
      } else {
	if (unlikely (ret < (long) MQ_BYT_MIN)) {
	  return RANGE_E (underflow, BYT);
	} else if (unlikely (ret > (long) MQ_BYT_MAX)) {
	  return RANGE_E (overflow, BYT);
	} else if (unlikely ((end - buf->cur.C) != buf->cursize)) {
	  return RANGE_E (size, BYT);
	}
      }
    } else {
      // MQ_BOLT
      (*ato).O = ((*buf->cur.C)-48);   // 48 is the ascii value from '0'
      if (unlikely ((*ato).O > 1)) {
	return RANGE_E (overflow, BOL);
      } else if (unlikely (buf->cursize != 1)) {
	return RANGE_E (size, BOL);
      }
    }
  } else {
    return TYPE_E(type, buf->type);
  }
  return MQ_OK;
}

enum MqErrorE
pBufferGetA2 (
  struct MqBufferS * const buf,
  union MqBufferAtomU * const ato,
  enum MqTypeE const type 
)
{
  buf->cur.B = buf->data;
  if (likely(type == buf->type)) {
#if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
    memcpy(ato,buf->cur.B,2);
#else
    (*ato).S = (*buf->cur.A).S;
#endif
  } else if ( buf->type == MQ_STRT ) {
    // MQ_SRTT
    MQ_STR end;
    long ret;
    errno = 0;
    ret = str2int (buf->cur.C, &end, 0);
    (*ato).S = (MQ_SRT) ret;
    if (unlikely (errno == ERANGE)) {
      if (unlikely (ret == LONG_MIN)) {
	return RANGE_E (underflow, SRT);
      } else if (unlikely (ret == LONG_MAX)) {
	return RANGE_E (overflow, SRT);
      }
    } else {
      if (unlikely (ret < (long) MQ_SRT_MIN)) {
	return RANGE_E (underflow, SRT);
      } else if (unlikely (ret > (long) MQ_SRT_MAX)) {
	return RANGE_E (overflow, SRT);
      } else if (unlikely ((end - buf->cur.C) != buf->cursize)) {
	return RANGE_E (size, SRT);
      }
    }
  } else {
    return TYPE_E(type, buf->type);
  }
  return MQ_OK;
}

enum MqErrorE
pBufferGetA4 (
  struct MqBufferS * const buf,
  union MqBufferAtomU * const ato,
  enum MqTypeE const type 
)
{
  buf->cur.B = buf->data;
  if (likely(type == buf->type)) {
#if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
    memcpy(ato,buf->cur.B,4);
#else
    (*ato).I = (*buf->cur.A).I;
#endif
  } else if ( buf->type == MQ_STRT ) {
    MQ_STR end;
    errno = 0;
    if (type == MQ_INTT) {
      // MQ_INTT
      long ret = str2int (buf->cur.C, &end, 0);
      (*ato).I = (MQ_INT) ret;
      // on 32bit: long=int
      if (unlikely (errno == ERANGE)) {
        if (unlikely (ret == LONG_MIN)) {
          return RANGE_E (underflow, INT);
        } else if (unlikely (ret == LONG_MAX)) {
          return RANGE_E (overflow, INT);
        }
      } else {
        if (unlikely (ret < (long) MQ_INT_MIN)) {
          return RANGE_E (underflow, INT);
        } else if (unlikely (ret > (long) MQ_INT_MAX)) {
          return RANGE_E (overflow, INT);
        } else if (unlikely ((end - buf->cur.C) != buf->cursize)) {
          return RANGE_E (size, INT);
        }
      }
    } else {
#if defined(HAVE_STRTOF)
      // MQ_FLTT
      (*ato).F = str2flt (buf->cur.C, &end);
      if (unlikely (errno == ERANGE)) {
        if (unlikely ((*ato).F == 0.0)) {
          return RANGE_E (underflow, FLT);
        } else if (unlikely ((*ato).F == +MQ_HUGE_VALF || (*ato).F == -MQ_HUGE_VALF)) {
          return RANGE_E (overflow, FLT);
        }
      } else if (unlikely ((end - buf->cur.C) != buf->cursize)) {
        return RANGE_E (size, FLT);
      }
#else /* ! HAVE_STRTOF */
      // MQ_DBLT
      double d = str2dbl (buf->cur.C, &end);
      if (unlikely (errno == ERANGE)) {
        if (unlikely (d == 0.0L)) {
          return RANGE_E (underflow, FLT);
        } else if (unlikely (d == +MQ_HUGE_VAL || d == -MQ_HUGE_VAL)) {
          return RANGE_E (overflow, FLT);
        }
      } else if (unlikely ((end - buf->cur.C) != buf->cursize)) {
        return RANGE_E (size, FLT);
      }
      (*ato).F = (float) d;
#endif /* !HAVE_STRTOF */
    }
  } else {
    return TYPE_E(type, buf->type);
  }
  return MQ_OK;
}

enum MqErrorE
pBufferGetA8 (
  struct MqBufferS * const buf,
  union MqBufferAtomU * const ato,
  enum MqTypeE const type 
)
{
  buf->cur.B = buf->data;
  if (likely(type == buf->type)) {
#if defined(HAVE_ALIGNED_ACCESS_REQUIRED)
    memcpy(ato,buf->cur.B,8);
#else
    (*ato).W = (*buf->cur.A).W;
#endif
  } else if ( buf->type == MQ_STRT ) {
    MQ_STR end;
    errno = 0;
    if (type == MQ_WIDT) {
      // MQ_WIDT
      (*ato).W = str2wid (buf->cur.C, &end, 0);
      if (unlikely (errno == ERANGE)) {
	if (unlikely ((*ato).W == MQ_WID_MIN)) {
	  return RANGE_E (underflow, WID);
	} else if (unlikely ((*ato).W == MQ_WID_MAX)) {
	  return RANGE_E (overflow, WID);
	}
      } else if (unlikely ((end - buf->cur.C) != buf->cursize)) {
	return RANGE_E (size, WID);
      }
    } else {
      // MQ_DBLT
      (*ato).D = str2dbl (buf->cur.C, &end);
      if (unlikely (errno == ERANGE)) {
	if (unlikely ((*ato).D == 0.0L)) {
	  return RANGE_E (underflow, DBL);
	} else if (unlikely ((*ato).D == +MQ_HUGE_VAL || (*ato).D == -MQ_HUGE_VAL)) {
	  return RANGE_E (overflow, DBL);
	}
      } else if (unlikely ((end - buf->cur.C) != buf->cursize)) {
	return RANGE_E (size, DBL);
      }
    }
  } else {
    return TYPE_E(type, buf->type);
  }
  return MQ_OK;
}

enum MqErrorE
MqBufferGetY (
  struct MqBufferS * const buf,
  MQ_BYT * const valP
)
{
  return pBufferGetA1(buf,(MQ_ATO*)valP,MQ_BYTT);
}

enum MqErrorE
MqBufferGetO (
  struct MqBufferS * const buf,
  MQ_BOL * const valP
)
{
  return pBufferGetA1(buf,(MQ_ATO*)valP,MQ_BOLT);
}

enum MqErrorE
MqBufferGetS (
  struct MqBufferS * const buf,
  MQ_SRT * const valP
)
{
  return pBufferGetA2(buf,(MQ_ATO*)valP,MQ_SRTT);
}

enum MqErrorE
MqBufferGetI (
  struct MqBufferS * const buf,
  MQ_INT * const valP
)
{
  return pBufferGetA4(buf,(MQ_ATO*)valP,MQ_INTT);
}

enum MqErrorE
MqBufferGetF (
  struct MqBufferS * const buf,
  MQ_FLT * const valP
)
{
  return pBufferGetA4(buf,(MQ_ATO*)valP,MQ_FLTT);
}

enum MqErrorE
MqBufferGetW (
  struct MqBufferS * const buf,
  MQ_WID * const valP
)
{
  return pBufferGetA8(buf,(MQ_ATO*)valP,MQ_WIDT);
}

enum MqErrorE
MqBufferGetD (
  struct MqBufferS * const buf,
  MQ_DBL * const valP
)
{
  return pBufferGetA8(buf,(MQ_ATO*)valP,MQ_DBLT);
}

enum MqErrorE
MqBufferGetB (
  struct MqBufferS * const buf,
  MQ_BIN * const out,
  MQ_SIZE * const size
)
{
  *out = buf->data;
  *size = buf->cursize;
  return MQ_OK;
}

enum MqErrorE
MqBufferGetC (
  struct MqBufferS * const buf,
  MQ_CST * const out
)
{
  /// the first MQ_ATO bytes of the \e bls are reserved for the native data
  MQ_STR stringP = (MQ_STR) (buf->bls+sizeof(MQ_ATO));

  buf->cur.B = buf->data;
  switch (buf->type) {
    case MQ_STRT:   stringP = buf->cur.C; break;
    case MQ_BYTT:   sprintf (stringP, MQ_FORMAT_Y, MqBufU2BYT(buf->cur)); break;
    case MQ_BOLT:   sprintf (stringP, MQ_FORMAT_O, MqBufU2BOL(buf->cur)); break;
    case MQ_SRTT:   sprintf (stringP, MQ_FORMAT_S, MqBufU2SRT(buf->cur)); break;
    case MQ_INTT:   sprintf (stringP, MQ_FORMAT_I, MqBufU2INT(buf->cur)); break;
    case MQ_FLTT:   sprintf (stringP, MQ_FORMAT_F, MqBufU2FLT(buf->cur)); break;
    case MQ_WIDT:   sprintf (stringP, MQ_FORMAT_W, MqBufU2WID(buf->cur)); break;
    case MQ_DBLT:   sprintf (stringP, MQ_FORMAT_D, MqBufU2DBL(buf->cur)); break;
    default:	    return TYPE_E(MQ_STRT, buf->type);
  }
  *out = stringP;
  return MQ_OK;
}

char
MqBufferGetType (
  struct MqBufferS * const buf
)
{
  switch (buf->type) {
    case MQ_STRT: return 'C';
    case MQ_INTT: return 'I';
    case MQ_DBLT: return 'D';
    case MQ_WIDT: return 'W';
    case MQ_BINT: return 'B';
    case MQ_BYTT: return 'Y';
    case MQ_BOLT: return 'O';
    case MQ_SRTT: return 'S';
    case MQ_FLTT: return 'F';
    case MQ_LSTT: return 'L';
    case MQ_TRAT: return 'T';
  }
  return '*';
}

MQ_CST
MqBufferGetType3 (
  struct MqBufferS * const buf
)
{
  switch (buf->type) {
    case MQ_STRT: return "C";
    case MQ_INTT: return "I";
    case MQ_DBLT: return "D";
    case MQ_WIDT: return "W";
    case MQ_BINT: return "B";
    case MQ_BYTT: return "Y";
    case MQ_BOLT: return "O";
    case MQ_SRTT: return "S";
    case MQ_FLTT: return "F";
    case MQ_LSTT: return "L";
    case MQ_TRAT: return "T";
  }
  return "*";
}

struct MqS *
MqBufferGetContext (
  struct MqBufferS * const buf
)
{
  return buf->context;
}

/*****************************************************************************/
/*                                                                           */
/*                              buffer_set                                   */
/*                                                                           */
/*****************************************************************************/

static struct MqBufferS *
sBufferSetA1 (
  struct MqBufferS * const buf,
  union MqBufferAtomU const in,
  enum MqTypeE const type
)
{
  // reset the cur pointer to the start of the buffer
  buf->cur.B = buf->data;
  // the following line is !!not!! necessary because the initial
  // size of a new created buffer is bigger than sizeof(in)
  // pBufferNewSize (buf, sizeof(in));
  (*buf->cur.A).Y = in.Y;
  buf->cursize = 1;
  buf->type = type;
  return buf;
}

static struct MqBufferS *
sBufferSetA2 (
  struct MqBufferS * const buf,
  union MqBufferAtomU const in,
  enum MqTypeE const type
)
{
  // reset the cur pointer to the start of the buffer
  buf->cur.B = buf->data;
  // the following line is !!not!! necessary because the initial
  // size of a new created buffer is bigger than sizeof(in)
  // pBufferNewSize (buf, sizeof(in));
  (*buf->cur.A).S = in.S;
  buf->cursize = 2;
  buf->type = type;
  return buf;
}

static struct MqBufferS *
sBufferSetA4 (
  struct MqBufferS * const buf,
  union MqBufferAtomU const in,
  enum MqTypeE const type
)
{
  // reset the cur pointer to the start of the buffer
  buf->cur.B = buf->data;
  // the following line is !!not!! necessary because the initial
  // size of a new created buffer is bigger than sizeof(in)
  // pBufferNewSize (buf, sizeof(in));
  (*buf->cur.A).I = in.I;
  buf->cursize = 4;
  buf->type = type;
  return buf;
}

static struct MqBufferS *
sBufferSetA8 (
  struct MqBufferS * const buf,
  union MqBufferAtomU const in,
  enum MqTypeE const type
)
{
  // reset the cur pointer to the start of the buffer
  buf->cur.B = buf->data;
  // the following line is !!not!! necessary because the initial
  // size of a new created buffer is bigger than sizeof(in)
  // pBufferNewSize (buf, sizeof(in));
  (*buf->cur.A).W = in.W;
  buf->cursize = 8;
  buf->type = type;
  return buf;
}

struct MqBufferS *
MqBufferSetY (
  struct MqBufferS * const buf,
  MQ_BYT const val
)
{
  union MqBufferAtomU ato;
  ato.Y = val;
  return sBufferSetA1(buf,ato,MQ_BYTT);
}

struct MqBufferS *
MqBufferSetO (
  struct MqBufferS * const buf,
  MQ_BOL const val
)
{
  union MqBufferAtomU ato;
	ato.O = val;
  return sBufferSetA1(buf,ato,MQ_BOLT);
}

struct MqBufferS *
MqBufferSetS (
  struct MqBufferS * const buf,
  MQ_SRT const val
)
{
  union MqBufferAtomU ato;
	ato.S = val;
  return sBufferSetA2(buf,ato,MQ_SRTT);
}

struct MqBufferS *
MqBufferSetI (
  struct MqBufferS * const buf,
  MQ_INT const val
)
{
  union MqBufferAtomU ato;
	ato.I = val;
  return sBufferSetA4(buf,ato,MQ_INTT);
}

struct MqBufferS *
MqBufferSetF (
  struct MqBufferS * const buf,
  MQ_FLT const val
)
{
  union MqBufferAtomU ato;
	ato.F = val;
  return sBufferSetA4(buf,ato,MQ_FLTT);
}

struct MqBufferS *
MqBufferSetW (
  struct MqBufferS * const buf,
  MQ_WID const val
)
{
  union MqBufferAtomU ato;
	ato.W = val;
  return sBufferSetA8(buf,ato,MQ_WIDT);
}

struct MqBufferS *
MqBufferSetD (
  struct MqBufferS * const buf,
  MQ_DBL const val
)
{
  union MqBufferAtomU ato;
	ato.D = val;
  return sBufferSetA8(buf,ato,MQ_DBLT);
}

struct MqBufferS *
MqBufferSetC (
  struct MqBufferS * const buf,
  MQ_CST const in
)
{
  MQ_SIZE len = (MQ_SIZE) strlen(in);
  pBufferNewSize (buf, len);
  // reset the cur pointer to the start of the buffer
  buf->cur.B = buf->data;
  strcpy (buf->cur.C, in);
  buf->cursize = len;
  buf->type = MQ_STRT;
  return buf;
}

struct MqBufferS *
MqBufferSetB (
  struct MqBufferS * const buf,
  MQ_CBI  const in,
  MQ_SIZE const len
)
{
  pBufferNewSize (buf, len);
  // reset the cur pointer to the start of the buffer
  buf->cur.B = buf->data;
  memcpy (buf->cur.B, in, len);
  buf->cursize = len;
  buf->type = MQ_BINT;
  return buf;
}

struct MqBufferS *
MqBufferSetU (
  struct MqBufferS * const buf,
  struct MqBufferS const * const in
)
{
  pBufferNewSize (buf, in->cursize);
  // reset the cur pointer to the start of the buffer
  buf->cur.B = buf->data;
  // the +1 is for the string \0 byte
  memcpy (buf->cur.B, in->data, (in->cursize+1));
  buf->cursize = in->cursize;
  buf->type = in->type;
  return buf;
}

struct MqBufferS *
MqBufferSetV (
  struct MqBufferS * const buf,
  MQ_CST const fmt,
  ...
)
{
  va_list ap;
  MqBufferReset (buf);
  va_start (ap, fmt);
  MqBufferAppendVL (buf, fmt, ap);
  buf->cur.B = buf->data;
  va_end (ap);
  return buf;
}

struct MqBufferS *
MqBufferCastTo (
  struct MqBufferS * const buf,
  enum MqTypeE const type
)
{
  MQ_ATO val;

  if (buf->type == type)
    return buf;

  switch (type & MQ_TYPE_IS_NATIVE) {
    case 8:
      MqErrorCheck (pBufferGetA8 (buf, &val, type));
      sBufferSetA8 (buf, val, type);
      break;
    case 4:
      MqErrorCheck (pBufferGetA4 (buf, &val, type));
      sBufferSetA4 (buf, val, type);
      break;
    case 2:
      MqErrorCheck (pBufferGetA2 (buf, &val, type));
      sBufferSetA2 (buf, val, type);
      break;
    case 1:
      MqErrorCheck (pBufferGetA1 (buf, &val, type));
      sBufferSetA1 (buf, val, type);
      break;
    default:
      MqErrorDb2(buf->context,MQ_ERROR_CAST_TO_NON_ATOM);
  }

  return buf;

error:
  MqErrorStack (MQ_ERROR_S);
  return buf;
}

/*****************************************************************************/
/*                                                                           */
/*                              buffer_append                                */
/*                                                                           */
/*****************************************************************************/

struct MqBufferS *
MqBufferCopy (
  register struct MqBufferS * const dest,
  register struct MqBufferS const * const srce
)
{
  dest->cursize = srce->cursize;
  dest->numItems = srce->numItems;
  dest->type = srce->type;
  pBufferNewSize (dest, srce->cursize);
  memcpy (dest->data, srce->data, srce->cursize+1);
  dest->cur.B = dest->data + (srce->cur.B - srce->data);
  return dest;
}

struct MqBufferS *
MqBufferDup (
  struct MqBufferS const * const srce
)
{
  return MqBufferCopy (MqBufferCreate (srce->context, srce->cursize), srce);
}

MQ_SIZE
MqBufferAppendC (
  register struct MqBufferS * const buf,
  MQ_CST const str
)
{
  MQ_SIZE len = (MQ_SIZE) strlen (str);
  pBufferAddSize (buf, len);
  buf->cur.B = buf->data + buf->cursize;
  strcpy (buf->cur.C, str);
  buf->cursize += len;
  buf->type = MQ_STRT;
  return len;
}

MQ_SIZE
MqBufferAppendVL (
  register struct MqBufferS * const buf,
  MQ_CST const fmt,
  va_list ap
)
{
  MQ_SIZE left;
  MQ_SIZE len;
  va_list ap_save;

  va_copy(ap_save,ap);
  buf->cur.B = buf->data + buf->cursize;
  //     MaxSize     CurrSize    
  left = buf->size - buf->cursize;
  // test the len
  len = vsnprintf (buf->cur.C, left, fmt, ap_save);
  // free arg-list
  va_end(ap_save);

  // glibc 2.0.6 return -1 when the output was truncated
  if (len == -1) {
    va_copy(ap_save,ap);
    len = vsnprintf (NULL, 0, fmt, ap_save);
    va_end(ap_save);
  }

  if (len >= left) {
    //                               CurSize      + len + \0
    _pBufferNewSize (buf, __func__, buf->cursize + len + 1);
    //       MaxSize     CurrSize     
    left = buf->size - buf->cursize;
    len = vsnprintf ((buf->cur.C), left, fmt, ap);
    if (len >= left)
      MqPanicV (MQ_ERROR_PANIC, __func__, -1, "length mismatch: len<" 
	MQ_FORMAT_Z "> > left<" MQ_FORMAT_Z ">", len, left);
  }
  buf->cursize += len;
  buf->cur.B += len;
  buf->type = MQ_STRT;
  return len;
}

MQ_SIZE
MqBufferAppendV (
  struct MqBufferS * const buf,
  MQ_CST const fmt,
  ...
)
{
  MQ_SIZE ret;
  va_list ap;
  va_start (ap, fmt);
  ret = MqBufferAppendVL (buf, fmt, ap);
  va_end (ap);
  return ret;
}

/*****************************************************************************/
/*                                                                           */
/*                              buffer_push                                  */
/*                                                                           */
/*****************************************************************************/

MQ_SIZE
MqBufferPush (
  register struct MqBufferS * const buf,
  MQ_CST const str
)
{
  const MQ_SIZE len = (MQ_SIZE) strlen (str);
  pBufferAddSize (buf, len);
  strcpy (buf->cur.C, str);
  buf->cursize += len;
  buf->cur.B += len;
  return len;
}

MQ_SIZE
MqBufferPop (
  register struct MqBufferS * const buf,
  MQ_CST const str
)
{
  const MQ_SIZE len = (MQ_SIZE) strlen (str);
  buf->cursize -= len;
  buf->cur.B -= len;
  *buf->cur.B = '\0';
  return len;
}

/*****************************************************************************/
/*                                                                           */
/*                              buffer_log                                   */
/*                                                                           */
/*****************************************************************************/

void
MqBufferLogS (
  struct MqS const * const context,
  struct MqBufferS const * const buf,
  MQ_CST const prefix
)
{
#define MY_MAX 300
  MQ_STR cur = (buf->cursize > MY_MAX ? (MQ_STR) "" : buf->cur.C);
  switch (buf->type) {
    case MQ_STRT:
      MqLogV (context, prefix, 0, "cur      = <" MQ_FORMAT_C ">\n", cur);
      break;
    case MQ_BINT:
      MqLogV (context, prefix, 0, "cur      = <" MQ_FORMAT_B ">\n", buf->cur.B);
      break;
    case MQ_BYTT:
      MqLogV (context, prefix, 0, "cur      = <" MQ_FORMAT_Y ">\n", MqBufU2BYT(buf->cur));
      break;
    case MQ_BOLT:
      MqLogV (context, prefix, 0, "cur      = <" MQ_FORMAT_O ">\n", MqBufU2BOL(buf->cur));
      break;
    case MQ_SRTT:
      MqLogV (context, prefix, 0, "cur      = <" MQ_FORMAT_S ">\n", MqBufU2SRT(buf->cur));
      break;
    case MQ_INTT:
      MqLogV (context, prefix, 0, "cur      = <" MQ_FORMAT_I ">\n", MqBufU2INT(buf->cur));
      break;
    case MQ_FLTT:
      MqLogV (context, prefix, 0, "cur      = <" MQ_FORMAT_F ">\n", MqBufU2FLT(buf->cur));
      break;
    case MQ_WIDT:
      MqLogV (context, prefix, 0, "cur      = <" MQ_FORMAT_W ">\n", MqBufU2WID(buf->cur));
      break;
    case MQ_TRAT:
      MqLogV (context, prefix, 0, "cur      = <" MQ_FORMAT_T ">\n", pBufU2TRA(buf->cur));
      break;
    case MQ_DBLT:
      MqLogV (context, prefix, 0, "cur      = <" MQ_FORMAT_D ">\n", MqBufU2DBL(buf->cur));
      break;
    case MQ_LSTT:
      MqLogV (context, prefix, 0, "cur      = <" MQ_FORMAT_L ">\n", buf->cur.L);
      break;
  }
#undef MY_MAX
}

/*****************************************************************************/
/*                                                                           */
/*                                debugging                                  */
/*                                                                           */
/*****************************************************************************/

//#if defined _DEBUG
void
MqBufferLog (
  struct MqS const * const context,
  struct MqBufferS const * const buf,
  MQ_CST const prefix
)
{
  MQ_STR tmp = (MQ_STR) MqSysMalloc(MQ_ERROR_PANIC, (buf->cursize+1));
  // no memory -> no work
  if (!buf) return;

  MqLogV (context, prefix, 0, ">>>> MqBufferS (%p)\n", (void*) buf);

  MqLogV (context, prefix, 0, "data     = <%s>\n", pLogAscii (tmp,(MQ_STR)buf->data, buf->cursize));
  MqLogV (context, prefix, 0, "size     = <" MQ_FORMAT_Z ">\n", buf->size);
  MqLogV (context, prefix, 0, "cursize  = <" MQ_FORMAT_Z ">\n", buf->cursize);
  MqLogV (context, prefix, 0, "numItems = <" MQ_FORMAT_Z ">\n", buf->numItems);
  MqLogV (context, prefix, 0, "curpos   = <" MQ_FORMAT_Z ">\n", (MQ_SIZE) (buf->cur.B - buf->data));
  MqLogV (context, prefix, 0, "type     = <%s:%i>\n", MqLogTypeName(buf->type), buf->type);
  MqLogV (context, prefix, 0, "pointer  = data<%p> buf->cur.B<%p>\n", buf->data, buf->cur.B);

  MqBufferLogS(context, buf, prefix);

  MqLogV (context, prefix, 0, "alloc    = <%s:%s>\n",
   (buf->bits.alloc == MQ_ALLOC_DYNAMIC ? "DYNAMIC"   : "STATIC"),
   (buf->bits.ref   == MQ_REF_LOCAL     ? "REF_LOCAL" : "REF_GLOBAL")
  );
  MqLogV (context, prefix, 0, "<<<< MqBufferS\n");

  MqSysFree(tmp);
}

//#endif

END_C_DECLS

