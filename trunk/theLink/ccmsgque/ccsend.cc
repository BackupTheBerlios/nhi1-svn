/**
 *  \file       theLink/ccmsgque/ccsend.cc
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

/*****************************************************************************/
/*                                                                           */
/*                                context                                    */
/*                                                                           */
/*****************************************************************************/

#include "ccmsgque_private.h"

namespace ccmsgque {

  void MqC::SendEND_AND_CALLBACK (
    MQ_CST const      token,
    CallbackF const   callback
  ) throw(MqCException)
  {
    struct ProcCallS * ptr = (struct ProcCallS *) MqSysMalloc(MQ_ERROR_PANIC, sizeof(*ptr));
    ptr->type = ptr->PC_CallbackF;
    ptr->call.Callback = callback;
    ErrorCheck (MqSendEND_AND_CALLBACK(&context, token, ProcCall, static_cast<MQ_PTR>(ptr), ProcFree));
  }

  void MqC::SendEND_AND_CALLBACK (
    MQ_CST const      token,
    IService * const  service
  ) throw(MqCException)
  {
    struct ProcCallS * ptr = (struct ProcCallS *) MqSysMalloc(MQ_ERROR_PANIC, sizeof(*ptr));
    ptr->type = ptr->PC_IService;
    ptr->call.Service = service;
    ErrorCheck (MqSendEND_AND_CALLBACK(&context, token, ProcCall, static_cast<MQ_PTR>(ptr), ProcFree));
  }

} // END - namespace "ccmsgque"



