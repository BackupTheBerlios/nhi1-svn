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
      MQ_CBI  bdy;
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
      // check if data is available
      if (itms.empty()) {
	// no data available, set error-code to CONTINUE
	ErrorSetCONTINUE();
      } else {
	// extract the first (oldest) item from the store
	struct FilterItmS it = itms.front();
	// get the filter-context
	Filter4 *ftr = static_cast<Filter4*>(ServiceGetFilter());
	// an item is available, try to send the data
	try {
	  // reconnect to the server or do nothing if the server is already connected
	  ftr->LinkConnect();
	  // send the data
	  ftr->SendBDY(it.bdy, it.len);
	// on error, check if an "exit" happen
	} catch (const exception& e) {
	  ftr->ErrorSet (e);
	  if (ftr->ErrorIsEXIT()) {
	    // on exit ignore the error but do !not! forget the data
	    ftr->ErrorReset();
	    return;
	  } else {
	    // on error write the error-text and "forget" the data
	    ftr->ErrorWrite();
	  }
	}
	// reset the item-storage
	MqSysFree(it.bdy);
	// delete the data, will contine with next item
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
      it.bdy = bdy;
      it.len = len;
      itms.push (it);

      SendRETURN();
    }

    void LOGF () {
      // get the "link-target"
      Filter4 *ftr = static_cast<Filter4*>(ServiceGetFilter());
      // check "Ident" from the "link-target"
      if (!strcmp(ftr->LinkGetTargetIdent (),"transFilter")) {
	// if "Ident" is "transFilter" send the data to the "link-target"
	ftr->SendSTART();
	ftr->SendC(ReadC());
	ftr->SendEND_AND_WAIT("LOGF");
      } else {
	// if "Ident" is not "transFilter" use the data as file-name to open a file 
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
      Exit();
    }

    void ServerCleanup() {
      Filter4 *ftr = static_cast<Filter4*>(ServiceGetFilter());
      if (ftr->FH != NULL)
	fclose (ftr->FH);
    }

    void ServerSetup() {
      Filter4 *ftr = static_cast<Filter4*>(ServiceGetFilter());
      // SERVER: listen on every token (+ALL)
      ServiceCreate ("LOGF", CallbackF(&Filter4::LOGF));
      ServiceCreate ("EXIT", CallbackF(&Filter4::EXIT));
      ServiceCreate ("+ALL", this);
      ftr->ServiceCreate ("WRIT", CallbackF(&Filter4::WRIT));
      ftr->FH = NULL;
    }
};

/*****************************************************************************/
/*                                                                           */
/*                               M A I N                                     */
/*                                                                           */
/*****************************************************************************/

int MQ_CDECL main (int argc, MQ_CST argv[])
{
  static Filter4 filter;
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

