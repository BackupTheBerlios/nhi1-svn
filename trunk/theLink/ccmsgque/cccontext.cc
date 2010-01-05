/**
 *  \file       theLink/ccmsgque/cccontext.cc
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

#include "ccmsgque_private.h"

namespace ccmsgque {
  
  enum MqErrorE MqC::FactoryCreate (
    struct MqS * const tmpl,
    enum MqFactoryE create,
    MQ_PTR data,
    struct MqS  ** contextP
  )
  {
    try { 
      struct MqS * const context = *contextP = &(static_cast<IFactory*const>(data))->Factory()->context;
      MqConfigDup(context, tmpl);
      GetThis(context)->Init();
      MqSetupDup(context, tmpl);
    } catch (exception& ex) {
      return GetThis(tmpl)->ErrorSet(ex);
    } catch (...) {
      return MqErrorC (tmpl, __func__, -1, "factory exception");
    }
    return MQ_OK;
  }

  void MqC::FactoryDelete (
    struct MqS * context,
    MQ_BOL doFactoryCleanup,
    MQ_PTR data
  )
  {
    if (doFactoryCleanup == MQ_YES) {
      delete GetThis(context);
    } else {
      MqContextFree(context);
    }
  }

  enum MqErrorE MqC::ErrorSet (const exception& e) {
    MqCException const * const  mqex = dynamic_cast<MqCException const * const>(&e);
    if (mqex != NULL) {
      return MqErrorSet (&context, mqex->num(), mqex->code(), mqex->what());
    } else {
      return MqErrorC (&context, __func__, -1, e.what());
    }
  }

} // END - namespace "ccmsgque"

