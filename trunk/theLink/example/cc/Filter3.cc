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

class Filter3 : public MqC, public IServerSetup {
  public:
    Filter3(MqS *tmpl) : MqC(tmpl) {}
  private:
    void ServerSetup() {
      MqC *ftr = ServiceGetFilter();
      ServiceProxy ("+ALL");
      ServiceProxy ("+TRT");
      ftr->ServiceProxy ("+ALL");
      ftr->ServiceProxy ("+TRT");
    }
};

/*****************************************************************************/
/*                                                                           */
/*                               M A I N                                     */
/*                                                                           */
/*****************************************************************************/

int MQ_CDECL main (int argc, MQ_CST argv[])
{
  Filter3 *filter = MqFactoryC<Filter3>::New("Filter3");
  try {
    filter->LinkCreateVC (argc, argv);
    filter->ProcessEvent (MQ_WAIT_FOREVER);
  } catch (const exception& e) {
    filter->ErrorSet(e);
  }
  filter->Exit();
}
