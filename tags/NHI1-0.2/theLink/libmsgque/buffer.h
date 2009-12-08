/**
 *  \file       theLink/libmsgque/buffer.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef BUFFER_H
#   define BUFFER_H

BEGIN_C_DECLS

//MQ_INT pByte2INT (MQ_BINB const * const bin);
MQ_INT pByte2INT (MQ_CST const bin);

// can be ((*buf->cur.A).I)
static mq_inline MQ_INT iBufU2INT (
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

// can be (*in->cur.A)
static mq_inline MQ_ATO iBufU2ATO (
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

/// \brief create a reference to an existing \e MqBufferS
struct MqBufferS * pBufferCreateRef (
  struct MqBufferS const * const buf
);

/// \brief expand \e MqBufferS to a new \a newSize
/// \header
/// \buf
/// \param newSize the new requested size of the \e buf object
/// \attention only use #pBufferNewSize or #pBufferAddSize
///
/// the header is only used if debugging is on and an message is written
/// to stderr identified by \e header
void _pBufferNewSize (
  register struct MqBufferS * const buf,
  MQ_CST const header,
  MQ_SIZE const newSize
);

/// \brief the prototype for a pBufferGetA? function
typedef enum MqErrorE (
  *pBufferGetAF
) (
  struct MqBufferS * const buf,
  union MqBufferAtomU * const ato,
  enum MqTypeE const type
);

/// \brief generic function to read a #MqBufferAtomU union value from an #MQ_BUF object
/// \buf
/// \atom_out
/// \atom_type
/// \retMqErrorE
enum MqErrorE
pBufferGetA1 (
  struct MqBufferS * const buf,
  union MqBufferAtomU * const ato,
  enum MqTypeE const type
);

/// \copydoc pBufferGetA1
enum MqErrorE
pBufferGetA2(
  struct MqBufferS * const buf,
  union MqBufferAtomU * const ato,
  enum MqTypeE const type
);

/// \copydoc pBufferGetA1
enum MqErrorE
pBufferGetA4(
  struct MqBufferS * const buf,
  union MqBufferAtomU * const ato,
  enum MqTypeE const type
);

/// \copydoc pBufferGetA1
enum MqErrorE
pBufferGetA8(
  struct MqBufferS * const buf,
  union MqBufferAtomU * const ato,
  enum MqTypeE const type
);

/// \brief set \e MqBufferS to \a newSize
/// \buf
/// \param newSize the new requested size of the \e buf object
#define pBufferNewSize(buf, newSize) \
	if (unlikely(newSize > buf->size)) _pBufferNewSize(buf, __func__, newSize)

/// \brief add \a addSize to a \e MqBufferS
/// \buf
/// \param addSize add size to the current size
#define pBufferAddSize(buf, addSize) \
	{MQ_SIZE s=buf->cursize + addSize; pBufferNewSize(buf, s);}

#define MqBufferCreateStatic(NAME,SIZE) \
  static MQ_BINB data[SIZE+1]; \
  static struct MqBufferS tmp##NAME = { \
    MQ_MqBufferS_SIGNATURE,   /* signature */ \
    MQ_ERROR_PANIC,	      /* context   */ \
    data,		      /* data      */ \
    SIZE,		      /* size      */ \
    0,			      /* cursize   */ \
    0,			      /* numItems  */ \
    {NULL},		      /* cur       */ \
    MQ_ALLOC_STATIC,	      /* alloc     */ \
    MQ_STRT		      /* type      */ \
  }; \
  struct MqBufferS *NAME = &tmp##NAME; \
  tmp##NAME.cur.B = &data[0];

END_C_DECLS

#endif // BUFFER_H

