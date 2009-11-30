/**
 *  \file       libmsgque/swap.h
 *  \brief      \$Id: swap.h 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef SWAP_H
#define SWAP_H

BEGIN_C_DECLS

void pSwap4 (
  register MQ_INT *i
);

void pSwapBDY (
  register MQ_BIN ist
);

END_C_DECLS

#endif /* SWAP_H */
