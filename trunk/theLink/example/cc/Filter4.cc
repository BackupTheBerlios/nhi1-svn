/**
 *  \file       theLink/example/cc/Filter4.cc
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#define MQ_CONTEXT_S &context

#include <vector>
#include <stdexcept>
#include "ccmsgque.h"
#include "debug.h"

using namespace std;
using namespace ccmsgque;

class Filter4 : public MqC, public IFactory, public IServerSetup,
		  public IServerCleanup, public IEvent, public IService {
  private:
    struct FilterItmS {
      MQ_STRB token[5];
      bool    isTransaction;
      MQ_BUF  data;
    } **itm;
    MQ_INT  rIdx;
    MQ_INT  wIdx;
    MQ_INT  size;

    MqC* Factory() const { 
      return new Filter4(); 
    }

    void Event () {
      // check if an item is available
      if (rIdx == wIdx) {
	// no transaction available
	ErrorSetCONTINUE();
      } else {
	MqC *ftr;
	register struct FilterItmS * it;

	// is an item available?
	try {
	  ftr = ServiceGetFilter();
	} catch (...) {
	  ErrorReset();
	  return;
	}

	// extract the first (oldest) item from the store
	it = itm[rIdx++];

	// send the ctxaction to the ctx, on error write message but do not stop processing
	ftr->SendSTART();
	ftr->SendBDY(it->data->cur.B, it->data->cursize);
	try {
	  if (ServiceIsTransaction()) {
	    ftr->SendEND_AND_WAIT(it->token);
	  } else {
	    ftr->SendEND(it->token);
	  }
	} catch (const exception& e) {
	  ftr->ErrorSet (e);
	  ftr->ErrorPrint();
	  ftr->ErrorReset();
	  ErrorReset();
	}

	// reset the item-storage
	MqBufferReset(it->data);
      }
    }

    void Service (MqC * const ctx) {
      MQ_BIN bdy;
      MQ_SIZE len;
      register struct FilterItmS * it;

      // read the body-item
      ReadBDY(&bdy, &len);

      // add space if space is empty
      if (rIdx-1==wIdx || (rIdx==0 && wIdx==size-1)) {
	MQ_SIZE i;
	MqSysRealloc(MQ_ERROR_PANIC, itm, sizeof(struct FilterItmS*)*size*2);
	for (i=0;i<rIdx;i++) {
	  itm[wIdx++] = itm[i];
	  itm[i] = NULL;
	}
	for (i=wIdx;i<size;i++) {
	  itm[i] = NULL;
	}
	size*=2;
      }

      it = itm[wIdx];

      // create storage if NULL
      if (it == NULL) {
	itm[wIdx] = it = (struct FilterItmS*)MqSysCalloc(MQ_ERROR_PANIC, 1, sizeof(struct FilterItmS));
      }
      if (it->data == NULL) {
	it->data = MqBufferCreate(MQ_ERROR_PANIC, len);
      }
      MqBufferSetB(it->data, bdy, len);
      strncpy(it->token, ServiceGetToken(), 5);
      it->isTransaction = ServiceIsTransaction();
      wIdx++;

      SendRETURN();
    }

    void ServerSetup() {
      // init the cache
      itm = (struct FilterItmS**)MqSysCalloc(MQ_ERROR_PANIC,100,sizeof(struct FilterItmS*));
      rIdx = 0;
      wIdx = 0;
      size = 100;

      // SERVER: listen on every token (+ALL)
      ServiceCreate ("+ALL", this);
    }

    void ServerCleanup() {
      MQ_SIZE i;
      for (i=0;i<size;i++) {
	if (itm[i] != NULL) {
	  MqBufferDelete (&itm[i]->data);
	  MqSysFree(itm[i]);
	}
      }
    }
};

/*****************************************************************************/
/*                                                                           */
/*                               M A I N                                     */
/*                                                                           */
/*****************************************************************************/

int MQ_CDECL main (int argc, MQ_CST argv[])
{
  Filter4 filter;
  try {
    filter.ConfigSetIgnoreExit(true);
    filter.LinkCreateVC (argc, argv);
    filter.ProcessEvent (MQ_WAIT_FOREVER);
  } catch (const exception& e) {
    filter.ErrorSet(e);
  }
  filter.Exit();
}



