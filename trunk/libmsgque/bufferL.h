/**
 *  \file       libmsgque/bufferL.h
 *  \brief      \$Id: bufferL.h 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef BUFFERL_H
#   define BUFFERL_H

BEGIN_C_DECLS

enum MqErrorE pBufferLSplitAlfa (struct MqBufferLS **, struct MqBufferLS **);
void pBufferLSetError (struct MqBufferLS * const, struct MqS * const);

END_C_DECLS

#endif // BUFFERL_H


