/**
 *  \file       theLink/example/cc/Filter2.cc
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

class Filter2 : public MqC, public IFactory {
    MqC* Factory() const { 
      return new Filter2(); 
    }
  public:
    void fFTR () {
      throw runtime_error("my error");
    }
};

/*****************************************************************************/
/*                                                                           */
/*                               M A I N                                     */
/*                                                                           */
/*****************************************************************************/

int MQ_CDECL main (int argc, MQ_CST argv[])
{
  Filter2 filter;
  try {
    filter.ConfigSetName("filter");
    filter.ConfigSetIsServer(MQ_YES);
    filter.LinkCreateVC (argc, argv);
    filter.ServiceCreate ("+FTR", MqC::CallbackF(&Filter2::fFTR));
    filter.ServiceProxy  ("+EOF");
    filter.ProcessEvent ();
  } catch (const exception& e) {
    filter.ErrorSet(e);
  }
  filter.Exit ();
}
