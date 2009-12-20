/**
 *  \file       theLink/example/cc/manfilter.cc
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "ccmsgque.h"

using namespace std;
using namespace ccmsgque;

class manfilter : public MqC, public IFactory {
    MqC* Factory() const { 
      return new manfilter(); 
    }
  public:
    void fFTR () {
      MqC *ftr = ConfigGetFilter();
      MQ_BUF temp = GetTempBuffer();
      ftr->SendSTART();
      while (ReadItemExists()) {
	ftr->SendU(MqBufferSetV(temp, "<%s>", ReadC()));
      }
      ftr->SendEND_AND_WAIT("+FTR");
      return SendRETURN();
    }
};

int MQ_CDECL main (int argc, MQ_CST argv[])
{
  manfilter filter;
  try {
    filter.ConfigSetName ("manfilter");
    filter.ConfigSetIsServer (MQ_YES);
    filter.LinkCreateVC(argc, argv);
    filter.ServiceCreate("+FTR", MqC::CallbackF(&manfilter::fFTR));
    filter.ServiceProxy ("+EOF");
    filter.ProcessEvent ();
  } catch (const exception& e) {
    filter.ErrorSet(e);
  }
  filter.Exit();
}
