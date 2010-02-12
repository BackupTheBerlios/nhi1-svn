/**
 *  \file       theLink/example/cc/Filter1.cc
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
#include <string>
#include "ccmsgque.h"
#include "debug.h"

using namespace std;
using namespace ccmsgque;

class Filter1 : public MqC, public IFactory {

    typedef vector<MQ_CST> MQ_ROW;
    typedef vector<MQ_ROW> MQ_TAB;
    MQ_TAB data;

    // factory  used to create server object instances
    MqC* Factory() const { 
      return new Filter1(); 
    }

  public:
    // service definition
    void fFTR () {
      MQ_ROW d;
      MQ_BUF buf = GetTempBuffer();
      while (ReadItemExists()) {
	d.push_back(mq_strdup(MqBufferSetV (buf, "<%s>", ReadC())->cur.C));
      }
      data.push_back(d);
      SendRETURN();
    }

    // service definition
    void fEOF () {
      MqC *ftr = ServiceGetFilter();
      MQ_TAB::iterator row;
      MQ_ROW::iterator itm;
      for (row = data.begin(); row < data.end(); row++ ) {
	ftr->SendSTART();
	for (itm = (*row).begin(); itm < (*row).end(); itm++) {
	  ftr->SendC(*itm);
	  free((void*)*itm);
	}
	ftr->SendEND_AND_WAIT("+FTR");
      }
      ftr->SendSTART();
      ftr->SendEND_AND_WAIT("+EOF");
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
  static Filter1 filter;
  filter.ConfigSetIsServer(MQ_YES);
  try {
    filter.LinkCreateVC (argc, argv);
    filter.ServiceCreate ("+FTR", MqC::CallbackF(&Filter1::fFTR));
    filter.ServiceCreate ("+EOF", MqC::CallbackF(&Filter1::fEOF));
    filter.ProcessEvent (MQ_WAIT_FOREVER);
  } catch (const exception& e) {
    filter.ErrorSet(e);
  }
  filter.Exit ();
}

