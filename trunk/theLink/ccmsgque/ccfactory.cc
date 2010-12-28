
/*****************************************************************************/
/*                                                                           */
/*                                factory                                    */
/*                                                                           */
/*****************************************************************************/

#include "ccmsgque_private.h"

namespace ccmsgque {
  
  template <class T>
  enum MqErrorE MqFactoryC<T>::FactoryCreate (
    struct MqS * const tmpl,
    enum MqFactoryE create,
    struct MqFactoryItemS * item,
    struct MqS  ** contextP
  )
  {
    try { 
      struct MqS * const mqctx = static_cast<MqC*const>(new T(tmpl))->context;
      if (MqErrorCheckI(MqErrorGetCode(mqctx))) {
	*contextP = NULL;
	if (create != MQ_FACTORY_NEW_INIT) {
	  MqErrorCopy (tmpl, mqctx);
	  MqContextDelete (&mqctx);
	  return MqErrorStack(tmpl);
	} else {
	  return MQ_ERROR;
	}
      }
      if (create != MQ_FACTORY_NEW_INIT) {
	MqSetupDup(mqctx, tmpl);
      }
      *contextP = mqctx;
    } catch (exception& ex) {
      *contextP = NULL;
      if (create != MQ_FACTORY_NEW_INIT) {
	return GetThis(tmpl)->ErrorSet(ex);
      } else {
	return MQ_ERROR;
      }
    } catch (...) {
      *contextP = NULL;
      if (create != MQ_FACTORY_NEW_INIT) {
	return MqErrorC(tmpl, __func__, -1, "Factory return no MqS type");
      } else {
	return MQ_ERROR;
      }
    }
    return MQ_OK;
  }

  template <class T>
  void MqFactoryC<T>::FactoryDelete (
    struct MqS * context,
    MQ_BOL doFactoryCleanup,
    struct MqFactoryItemS * const item
  )
  {
    if (doFactoryCleanup == MQ_YES) {
      delete GetThis(context);
    } else {
      MqContextFree(context);
    }
  }
} // END - namespace "ccmsgque"

