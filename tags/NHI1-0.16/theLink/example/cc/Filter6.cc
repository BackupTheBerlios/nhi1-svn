/**
 *  \file       theLink/example/cc/Filter6.cc
 *  \brief      \$Id$
 *  
 *  (C) 2011 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#define MQ_CONTEXT_S &context

#include <stdexcept>
#include <stdlib.h>
#include "ccmsgque.h"
#include "debug.h"

using namespace std;
using namespace ccmsgque;

class Filter6 : public MqC, public IServerSetup, public IServerCleanup, public IEvent {
  private:
    FILE *FH;

    void ErrorWrite () {
      fprintf(FH, "ERROR: %s\n", ErrorGetText());
      fflush(FH);
      ErrorReset ();
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

    void EXIT () {
      Exit();
    }

    void WRIT () {
      // get the "master"
      Filter6 *ctx = static_cast<Filter6*>(ServiceGetFilter());
      fprintf (ctx->FH, "%s\n", ReadC());
      fflush (ctx->FH);
      SendRETURN();
    }

    void FilterIn () {
      StorageInsert();
      SendRETURN();
    }

    void Event () {
      // check if data is available
      if (StorageCount() == 0LL) {
	// no data available, set error-code to CONTINUE
	ErrorSetCONTINUE();
      } else {
	MQ_TRA Id = 0LL;
	// an item is available, try to send the data
	try {
	  // get the filter-context
	  MqC *ftr = ServiceGetFilter();
	  // reconnect to the server or do nothing if the server is already connected
	  ftr->LinkConnect();
	  // read package from storage
	  Id = StorageSelect();
	  // forward the entire BDY data to the ftr-target
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
	// on "success" or on "error" delete item from storage
	StorageDelete(Id);
      }
    }

    void ServerCleanup() {
      if (FH != NULL)
	fclose (FH);
    }

    void ServerSetup() {
      MqC *ftr = ServiceGetFilter();
      FH = NULL;
      // SERVER: listen on every token (+ALL)
      ServiceCreate ("LOGF", CallbackF(&Filter6::LOGF));
      ServiceCreate ("EXIT", CallbackF(&Filter6::EXIT));
      ServiceCreate ("+ALL", CallbackF(&Filter6::FilterIn));
      ServiceStorage ("PRNT");
      ftr->ServiceCreate ("WRIT", CallbackF(&Filter6::WRIT));
      ftr->ServiceProxy ("+TRT");
    }

  public:
    Filter6(MqS * const tmpl) : MqC(tmpl) {
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
  Filter6 *filter = MqFactoryC<Filter6>::Add("transFilter").New();
  try {
    filter->LinkCreateVC (argc, argv);
    filter->ProcessEvent (MQ_WAIT_FOREVER);
  } catch (const exception& e) {
    filter->ErrorSet(e);
  }
  filter->Exit();
}

