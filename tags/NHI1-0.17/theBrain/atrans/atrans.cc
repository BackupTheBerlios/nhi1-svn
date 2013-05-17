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
#include <unistd.h>
#include <errno.h>
#endif

using namespace std;
using namespace ccmsgque;

class atrans : public MqC, public IServerSetup,
		  public IServerCleanup, public IEvent {

  public:
    atrans(MqS *tmpl) : MqC(tmpl) {
      ConfigSetIgnoreExit (true);
    }

  private:
    queue<MqDumpC*> itms;

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
	MqDumpC* it = itms.front();
	// get the filter-context
	MqC *ftr = ServiceGetFilter();
	// an item is available, try to send the data
	try {
	  // reconnect to the server or do nothing if the server is already connected
	  ftr->LinkConnect();
	  // load dump package
	  ReadLOAD(it);
	  // send the data
	  ReadForward(ftr);
	// on error, check if an "exit" happen
	} catch (const exception& e) {
	  ErrorSet (e);
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
	delete it;
	// delete the data, will contine with next item
	itms.pop();
      }
    }

    void ServiceALL (MqC * const ctx) {
      itms.push (ReadDUMP());
      SendRETURN();
    }

    void ServiceTRT (MqC * const ctx) {
      itms.push (ReadDUMP());
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
      if(access(dirname, F_OK) == -1){
	if (mq_mkdir (dirname, mode) == -1) {
	  ErrorV (__func__, errno, "can not create directory <%s> -> ERR<%s>", dirname, strerror (errno));
	  ErrorRaise();
	}
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
  atrans *filter = MqFactoryC<atrans>::Add("transFilter").New();
  try {
    MQ_STR datadir = NULL;
    struct MqBufferLS *args = MqBufferLCreateArgs (argc, argv);

    // setup direktory
    MqBufferLCheckOptionC(MQ_ERROR_PANIC, args, "--datadir", &datadir);
    if (datadir != NULL) {
      filter->SysChDir (datadir);
      MqSysFree (datadir);
    }
    filter->SysMkDir ("atrans", 00700);
    filter->SysMkDir ("atrans" mq_pathsep "IN", 00700);
    filter->SysMkDir ("atrans" mq_pathsep "OUT", 00700);

    // setup link
    filter->LinkCreate (args);
    filter->ProcessEvent (MQ_WAIT_FOREVER);
  } catch (const exception& e) {
    filter->ErrorSet(e);
  }
  filter->Exit();
}

