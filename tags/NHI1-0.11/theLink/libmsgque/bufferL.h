/**
 *  \file       theLink/libmsgque/bufferL.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef BUFFERL_H
#   define BUFFERL_H

BEGIN_C_DECLS

enum MqErrorE pBufferLSplitAlfa (struct MqBufferLS **, struct MqBufferLS **);
void pBufferLSetError (struct MqBufferLS * const, struct MqS * const);
struct MqBufferLS * pBufferLExtractOptions (struct MqBufferLS * const);
END_C_DECLS

#endif // BUFFERL_H


