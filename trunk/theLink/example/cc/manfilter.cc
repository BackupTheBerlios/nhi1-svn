/**
 *  \file       example/cc/manfilter.cc
 *  \brief      \$Id: manfilter.cc 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "ccmsgque.h"

using namespace std;
using namespace ccmsgque;

class manfilter : public MqC, public IFilterFTR {
  public:
    void fFTR () {
      MQ_BUF temp = GetTempBuffer();
      SendSTART();
      while (ReadItemExists()) {
	SendU(MqBufferSetV(temp, "<%s>", ReadC()));
      }
      SendFTR();
    }
};

int MQ_CDECL main (int argc, MQ_CST argv[])
{
  manfilter filter;
  try {
    filter.ConfigSetName ("manfilter");
    filter.LinkCreateVC(argc, argv);
    filter.ProcessEvent ();
  } catch (const exception& e) {
    filter.ErrorSet(e);
  }
  filter.Exit();
}
