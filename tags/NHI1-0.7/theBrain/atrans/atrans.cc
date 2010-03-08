/**
 *  \file       theBrain/atrans/atrans.cc
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

#include <queue>
#include <stdexcept>
#include "ccmsgque.h"
#include "debug.h"

using namespace std;
using namespace ccmsgque;

class atrans : public MqC, public IFactory, public IServerSetup,
		  public IEvent, public IService {
  private:
    struct FilterItmS {
      MQ_STRB token[5];
      bool    isTransaction;
      MQ_BIN  bdy;
      MQ_SIZE len;
    };
    queue<struct FilterItmS> itms;

    MqC* Factory() const { 
      return new atrans(); 
    }

    void Event () {
      // check if an item is available
      if (itms.empty()) {
	// no transaction available
	ErrorSetCONTINUE();
      } else {
	MqC *ftr;
	register struct FilterItmS it;

	// is an item available?
	try {
	  ftr = ServiceGetFilter();
	} catch (...) {
	  ErrorReset();
	  return;
	}

	// extract the first (oldest) item from the store
	it = itms.front();

	// send the transaction to the ctx, on error write message but do not stop processing
	ftr->SendSTART();
	ftr->SendBDY(it.bdy, it.len);
	try {
	  if (ServiceIsTransaction()) {
	    ftr->SendEND_AND_WAIT(it.token);
	  } else {
	    ftr->SendEND(it.token);
	  }
	} catch (const exception& e) {
	  ftr->ErrorSet (e);
	  ftr->ErrorPrint();
	  ftr->ErrorReset();
	  // do not delete item without successful send
	  return;
	}

	// reset the item-storage
	MqSysFree(it.bdy);
	itms.pop();
      }
    }

    void Service (MqC * const ctx) {
      MQ_BIN bdy;
      MQ_SIZE len;
      register struct FilterItmS it;

      // read the body-item
      ReadBDY(&bdy, &len);

      // fill the item data
      it.bdy = (MQ_BIN)MqSysMalloc(MQ_ERROR_PANIC, len * sizeof(MQ_BINB));
      memcpy(it.bdy, bdy, len);
      it.len = len;
      strncpy(it.token, ServiceGetToken(), 5);
      it.isTransaction = ServiceIsTransaction();
      itms.push (it);

      SendRETURN();
    }

    void ServerSetup() {
      ServiceCreate ("+ALL", this);
    }
};

/*****************************************************************************/
/*                                                                           */
/*                               M A I N                                     */
/*                                                                           */
/*****************************************************************************/

int MQ_CDECL main (int argc, MQ_CST argv[])
{
  atrans filter;
  try {
    filter.ConfigSetIgnoreExit(true);
    filter.LinkCreateVC (argc, argv);
    filter.ProcessEvent (MQ_WAIT_FOREVER);
  } catch (const exception& e) {
    filter.ErrorSet(e);
  }
  filter.Exit();
}



