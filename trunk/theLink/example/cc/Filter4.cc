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

class Filter4 : public MqC, public IServerSetup, public IServerCleanup, public IEvent, public IService {
  private:
    struct FilterItmS {
      MQ_CBI  bdy;
      MQ_SIZE len;
    };
    queue<struct FilterItmS> itms;
    FILE *FH;

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
	// an item is available, try to send the data
	try {
	  // get the filter-context
	  MqC *ftr = ServiceGetFilter();
	  // reconnect to the server or do nothing if the server is already connected
	  ftr->LinkConnect();
	  // setup the BDY data from storage
	  ReadLOAD(it.bdy, it.len);
	  // send BDY data to the link-target
	  ReadForward(ftr);
	} catch (const exception& e) {
	  ErrorSet (e);
	  // on error, check if an "exit" happen
	  if (ErrorIsEXIT()) {
	    // on exit ignore the error but do !not! forget the data
	    ErrorReset();
	    return;
	  } else {
	    // on error write the error-text and "forget" the data
	    ErrorWrite();
	  }
	}
	// reset the item-storage
	MqSysFree(it.bdy);
	// delete the data, will contine with next item
	itms.pop();
      }
    }

    void Service (MqC * const ctx) {
      struct FilterItmS it;

      // read the body-item
      ReadDUMP(&it.bdy, &it.len);

      // fill the item data
      itms.push (it);

      SendRETURN();
    }

    void LOGF () {
      // get the "link-target"
      MqC *ftr = ServiceGetFilter();
      // check "Ident" from the "link-target"
      if (!strcmp(ftr->LinkGetTargetIdent (),"transFilter")) {
	// if "Ident" is "transFilter" send the data to the "link-target"
	ReadForward(ftr);
      } else {
	// if "Ident" is not "transFilter" use the data as file-name to open a file 
	FH = fopen (ReadC(), "a");
      }
      SendRETURN();
    }

    void WRIT () {
      // get the "master"
      Filter4 *ctx = static_cast<Filter4*>(ServiceGetFilter());
      fprintf (ctx->FH, "%s\n", ReadC());
      fflush (ctx->FH);
      SendRETURN();
    }

    void EXIT () {
      ErrorSetEXIT();
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

  public:
    Filter4(MqS * const tmpl) : MqC(tmpl) {
      ConfigSetIgnoreExit(true);
    }
};

/*****************************************************************************/
/*                                                                           */
/*                               M A I N                                     */
/*                                                                           */
/*****************************************************************************/

int MQ_CDECL main (int argc, MQ_CST argv[])
{
  MqFactoryC<Filter4>::Default("transFilter");

  static Filter4 filter(NULL);
  try {
    filter.LinkCreateVC (argc, argv);
    filter.ProcessEvent (MQ_WAIT_FOREVER);
  } catch (const exception& e) {
    filter.ErrorSet(e);
  }
  filter.Exit();
}

