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

    // call the function
    try {

      switch (data->type) {
	case ProcCallS::PC_CallbackF:
	  (GetThis(context)->*data->call.Callback)();
	  break;
	case ProcCallS::PC_IService:
	  (data->call.Service->Service)(GetThis(context));
	  break;
	case ProcCallS::PC_IFilterFTR:
	  (data->call.FilterFTR->fFTR)();
	  break;
	case ProcCallS::PC_IFilterEOF:
	  (data->call.FilterEOF->fEOF)();
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
      return MqErrorC(context, __func__, -1, "unknown MqCException");
    }

    // everything is OK
    return MQ_OK;
  }

} // END - namespace "ccmsgque"

