/**
 *  \file       theLink/libmsgque/swap.c
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
#include "swap.h"
#include "buffer.h"
#include "log.h"

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                               swap_private                                */
/*                                                                           */
/*****************************************************************************/

static void sSwapRET (
  register MQ_BIN ist
);
static void sSwapDYN (
  register MQ_BIN ist
);

#   define SWAP_A(t,n,m,a) {(t)=(a)[(m)];(a)[(m)]=(a)[(n)];(a)[(n)]=(t);}

#   define SWAP_2(a)  {char t; SWAP_A(t,0,1,a)}
#   define SWAP_4(a)  {char t; SWAP_A(t,0,3,a); SWAP_A(t,1,2,a);}
#   define SWAP_8(a)  {char t; SWAP_A(t,0,7,a); SWAP_A(t,1,6,a); SWAP_A(t,2,5,a); SWAP_A(t,3,4,a);}

static void
sSwapRET (
  register MQ_BIN ist
)
{
  SWAP_4 (ist);

  // return values
  sSwapDYN (ist + HDR_INT_LEN + 1);
}

static void
sSwapDYN (
  register MQ_BIN ist
)
{
  MQ_INT numItems = 0, len = 0;
  MQ_BINB type;

  // BDY_NumItems_S
  SWAP_4 (ist);
  memcpy(&numItems, ist, sizeof(MQ_INT));

  // for all Element's
  ist += (HDR_INT_LEN + 1);
  while (numItems) {

    // type
    type = *ist;
    ist += 2;

    // len
    len = (type & MQ_TYPE_IS_NATIVE);
    if (len == 0) {
      SWAP_4 (ist);
      memcpy(&len, ist, sizeof(MQ_INT));
      ist += (HDR_INT_LEN + 1);
    }

    // val
    switch ((enum MqTypeE)type) {
      case MQ_SRTT: SWAP_2  (ist);  break;
      case MQ_INTT: SWAP_4  (ist);  break;
      case MQ_FLTT: SWAP_4  (ist);  break;
      case MQ_WIDT: SWAP_8  (ist);  break;
      case MQ_DBLT: SWAP_8  (ist);  break;
      case MQ_RETT: sSwapRET (ist); break;
      case MQ_STRT: case MQ_BINT: case MQ_BYTT: case MQ_BOLT: break;
      case MQ_LSTT: sSwapDYN (ist); break;
      case MQ_TRAT: sSwapDYN (ist); break;
    }
    ist += (len + 1);
    numItems--;
  }
}

/*****************************************************************************/
/*                                                                           */
/*                               swap_public                                 */
/*                                                                           */
/*****************************************************************************/

void
pSwap4 (
  register MQ_INT *i
)
{
  SWAP_4 ((MQ_BIN) i);
}

void
pSwapBDY (
  register MQ_BIN ist
)
{
  sSwapDYN (ist + BDY_NumItems_S);
}

END_C_DECLS

