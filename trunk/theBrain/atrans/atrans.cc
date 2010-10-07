/**
 *  \file       theLink/example/cc/atrans.cc
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

#if defined(_MSC_VER)
# define mq_mkdir _mkdir
# define mq_chdir _chdir
# define mq_pathsep "\\"
#else
# define mq_mkdir mkdir
# define mq_chdir chdir
# define mq_pathsep "/"

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#endif

using namespace std;
using namespace ccmsgque;

class atrans : public MqC, public IFactory, public IServerSetup,
		  public IServerCleanup, public IEvent {
  private:
    struct FilterItmS {
      MQ_CBI  bdy;
      MQ_SIZE len;
    };
    queue<struct FilterItmS> itms;

    MqC* Factory() const { 
      return new atrans(); 
    }

    void ErrorWrite () {
      fprintf(stderr, "ERROR: %s\n", ErrorGetText());
      fflush(stderr);
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
	atrans *ftr = static_cast<atrans*>(ServiceGetFilter());
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

    void ServiceALL (MqC * const ctx) {
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

    void ServiceTRT (MqC * const ctx) {
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

    void ServerCleanup() {
    }

    void ServerSetup() {
      //atrans *ftr = static_cast<atrans*>(ServiceGetFilter());
      // SERVER: listen on every token (+ALL)
      ServiceCreate ("+ALL", CallbackF(&atrans::ServiceALL));
      ServiceCreate ("+TRT", CallbackF(&atrans::ServiceTRT));
    }

  public:

    void SysMkDir (MQ_CST dirname, mode_t mode ) {
      if (mq_mkdir (dirname, mode) == -1) {
	ErrorV (__func__, errno, "can not create directory <%s> -> ERR<%s>", dirname, strerror (errno));
	ErrorRaise();
      }
    }

    void SysChDir (MQ_CST dirname) {
      if (mq_chdir (dirname) == -1) {
	ErrorV (__func__, errno, "can not change into the directory <%s> -> ERR<%s>", dirname, strerror (errno));
	ErrorRaise();
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
  static atrans filter;
  try {
    MQ_STR datadir = NULL;
    struct MqBufferLS *args = MqBufferLCreateArgs (argc, argv);

    // setup direktory
    MqBufferLCheckOptionC(MQ_ERROR_PANIC, args, "-datadir", &datadir);
    if (datadir != NULL) {
      filter.SysChDir (datadir);
      MqSysFree (datadir);
    }
    filter.SysMkDir ("atrans", 00700);
    filter.SysMkDir ("atrans" mq_pathsep "IN", 00700);
    filter.SysMkDir ("atrans" mq_pathsep "OUT", 00700);

    // setup link
    filter.ConfigSetIgnoreExit (true);
    filter.ConfigSetIdent ("transFilter");
    filter.LinkCreate (args);
    filter.ProcessEvent (MQ_WAIT_FOREVER);
  } catch (const exception& e) {
    filter.ErrorSet(e);
  }
  filter.Exit();
}

