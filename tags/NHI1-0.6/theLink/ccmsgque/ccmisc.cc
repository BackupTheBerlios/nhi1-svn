/**
 *  \file       theLink/ccmsgque/ccmisc.cc
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

/*****************************************************************************/
/*                                                                           */
/*                                context                                    */
/*                                                                           */
/*****************************************************************************/

#include <iostream>
#include <exception>
#include <typeinfo>

#include "ccmsgque_private.h"

#define MQ_CONTEXT_S context

namespace ccmsgque {

  using namespace std;

  void MQ_DECL MqC::ProcFree (struct MqS const * const context, MQ_PTR *dataP)
  {
    MqSysFree(*dataP);
  }

  enum MqErrorE MQ_DECL MqC::ProcCall (
    struct MqS * const	context, 
    MQ_PTR const dataP
  )
  {
    struct ProcCallS const * const data = static_cast<struct ProcCallS const * const> (dataP);

    try {

      switch (data->type) {
	case ProcCallS::PC_CallbackF:
	  (GetThis(context)->*data->call.Callback)();
	  break;
	case ProcCallS::PC_IService:
	  (data->call.Service->Service)(GetThis(context));
	  break;
	case ProcCallS::PC_IEvent:
	  (data->call.Event->Event)();
	  break;
	case ProcCallS::PC_IBgError:
	  (data->call.BgError->BgError)();
	  break;
	case ProcCallS::PC_IServerSetup:
	  (data->call.ServerSetup->ServerSetup)();
	  break;
	case ProcCallS::PC_IServerCleanup:
	  (data->call.ServerCleanup->ServerCleanup)();
	  break;
      }

    } catch (exception& ex) {
      return GetThis(context)->ErrorSet (ex);
    } catch (...) {
      // http://groups.google.com/group/comp.programming.threads/browse_thread/thread/652bcf186fbbf697/f63757846514e5e5
      throw;
    }

    // everything is OK
    return MqErrorGetCodeI(context);
  }

} // END - namespace "ccmsgque"

