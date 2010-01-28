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

#include <queue>
#include <stdexcept>
#include <stdlib.h>
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
      MQ_BIN  bdy;
      MQ_SIZE len;
    };
    queue<struct FilterItmS> itms;
    FILE *FH;

    MqC* Factory() const { 
      return new Filter4(); 
    }

    void ErrorWrite () {
      fprintf(FH, "ERROR: %s\n", ErrorGetText());
      fflush(FH);
      ErrorReset ();
    }

    void Event () {
      // check if an item is available
      if (itms.empty()) {
	// no transaction available
	ErrorSetCONTINUE();
      } else {
	// extract the first (oldest) item from the store
	struct FilterItmS it = itms.front();
	Filter4 *ftr = static_cast<Filter4*>(ServiceGetFilter());
	// is an item available?
	try {
	  // reconnect or do nothing if already connected
	  ftr->LinkConnect();
	  // send the ctxaction to the ctx, on error write message but do not stop processing
	  ftr->SendSTART();
	  ftr->SendBDY(it.bdy, it.len);
	  if (it.isTransaction) {
	    ftr->SendEND_AND_WAIT(it.token);
	  } else {
	    ftr->SendEND(it.token);
	  }
	} catch (const exception& e) {
	  ftr->ErrorSet (e);
	  if (ftr->ErrorIsEXIT()) {
	    ftr->ErrorReset();
	    return;
	  } else {
	    ftr->ErrorWrite();
	  }
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

    void LOGF () {
      Filter4 *ftr = static_cast<Filter4*>(ServiceGetFilter());
      if (!strcmp(ftr->LinkGetTargetIdent (),"transFilter")) {
	ftr->SendSTART();
	ftr->SendC(ReadC());
	ftr->SendEND_AND_WAIT("LOGF");
      } else {
	ftr->FH = fopen (ReadC(), "a");
      }
      SendRETURN();
    }

    void WRIT () {
      fprintf (FH, "%s\n", ReadC());
      fflush (FH);
      SendRETURN();
    }

    void EXIT () {
      abort();
    }

    void ServerCleanup() {
      Filter4 *ftr = static_cast<Filter4*>(ServiceGetFilter(0));
      if (ftr->FH != NULL)
	fclose (ftr->FH);
    }

    void ServerSetup() {
      MqC *ftr = ServiceGetFilter(0);
      // SERVER: listen on every token (+ALL)
      ServiceCreate ("LOGF", CallbackF(&Filter4::LOGF));
      ServiceCreate ("EXIT", CallbackF(&Filter4::EXIT));
      ServiceCreate ("+ALL", this);
      ftr->ServiceCreate ("WRIT", CallbackF(&Filter4::WRIT));
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
    filter.ConfigSetIdent("transFilter");
    filter.LinkCreateVC (argc, argv);
    filter.ProcessEvent (MQ_WAIT_FOREVER);
  } catch (const exception& e) {
    filter.ErrorSet(e);
  }
  filter.Exit();
}

