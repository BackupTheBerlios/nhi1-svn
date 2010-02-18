/**
 *  \file       theLink/example/cc/Filter3.cc
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include <vector>
#include <stdexcept>
#include "ccmsgque.h"

using namespace std;
using namespace ccmsgque;

class Filter3 : public MqC, public IFactory, public IServerSetup {
  private:
    MqC* Factory() const { 
      return new Filter3(); 
    }
    void ServerSetup() {
      MqC *ftr = ServiceGetFilter();
      ServiceCreate ("+ALL", CallbackF(&Filter3::Filter));
      ftr->ServiceCreate ("+ALL", CallbackF(&Filter3::Filter));
    }
    void Filter () {
      MQ_BIN bdy;
      MQ_SIZE len;
      MqC *ftr = ServiceGetFilter();
      ReadBDY(&bdy, &len);
      ftr->SendSTART();
      ftr->SendBDY(bdy, len);
      if (ServiceIsTransaction()) {
	ftr->SendEND_AND_WAIT(ServiceGetToken());
	SendSTART();
	ftr->ReadBDY(&bdy, &len);
	SendBDY(bdy, len);
      } else {
	ftr->SendEND(ServiceGetToken());
      }
      SendRETURN();
    }
};

/*****************************************************************************/
/*                                                                           */
/*                               M A I N                                     */
/*                                                                           */
/*****************************************************************************/

int MQ_CDECL main (int argc, MQ_CST argv[])
{
  static Filter3 filter;
  try {
    filter.ConfigSetIsServer(MQ_YES);
    filter.ConfigSetName("Filter3");
    filter.LinkCreateVC (argc, argv);
    filter.ProcessEvent (MQ_WAIT_FOREVER);
  } catch (const exception& e) {
    filter.ErrorSet(e);
  }
  filter.Exit();
}
