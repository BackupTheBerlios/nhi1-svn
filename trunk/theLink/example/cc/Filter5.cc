/**
 *  \file       theLink/example/cc/Filter5.cc
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
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

// F1 **********************************************************************

class F1 : public MqC, public IServerSetup {
  public:
    F1(MqS *tmpl) : MqC(tmpl) {}
  private:
    void FTR() {
      MqC *ftr = static_cast<MqC*>(ServiceGetFilter());
      ftr->SendSTART();
      ftr->SendC("F1");
      ftr->SendC(ftr->ConfigGetName());
      ftr->SendI(ftr->ConfigGetStartAs());
      ftr->SendI(ConfigGetStatusIs());
      while (ReadGetNumItems() != 0) {
	ftr->SendC(ReadC());
      }
      ftr->SendEND_AND_WAIT("+FTR");
      SendRETURN();
    }
    void ServerSetup() {
      ServiceCreate ("+FTR", CallbackF(&F1::FTR));
      ServiceProxy ("+EOF");
    }
};

// F2 **********************************************************************

class F2 : public MqC, public IServerSetup {
  public:
    F2(MqS *tmpl) : MqC(tmpl) {}
  private:
    void FTR() {
      MqC *ftr = static_cast<MqC*>(ServiceGetFilter());
      ftr->SendSTART();
      ftr->SendC("F2");
      ftr->SendC(ftr->ConfigGetName());
      ftr->SendI(ftr->ConfigGetStartAs());
      ftr->SendI(ConfigGetStatusIs());
      while (ReadGetNumItems() != 0) {
	ftr->SendC(ReadC());
      }
      ftr->SendEND_AND_WAIT("+FTR");
      SendRETURN();
    }
    void ServerSetup() {
      ServiceCreate ("+FTR", CallbackF(&F2::FTR));
      ServiceProxy ("+EOF");
    }
};

// F3 **********************************************************************

class F3 : public MqC, public IServerSetup {
  public:
    F3(MqS *tmpl) : MqC(tmpl) {}
  private:
    void FTR() {
      MqC *ftr = static_cast<MqC*>(ServiceGetFilter());
      ftr->SendSTART();
      ftr->SendC("F3");
      ftr->SendC(ftr->ConfigGetName());
      ftr->SendI(ftr->ConfigGetStartAs());
      ftr->SendI(ConfigGetStatusIs());
      while (ReadGetNumItems() != 0) {
	ftr->SendC(ReadC());
      }
      ftr->SendEND_AND_WAIT("+FTR");
      SendRETURN();
    }
    void ServerSetup() {
      ServiceCreate ("+FTR", CallbackF(&F3::FTR));
      ServiceProxy ("+EOF");
    }
};

/*****************************************************************************/
/*                                                                           */
/*                               M A I N                                     */
/*                                                                           */
/*****************************************************************************/

int MQ_CDECL main (int argc, MQ_CST argv[])
{
  // init libmsgque global data
  MqInitArg0(argv[0],NULL);

  // define factories
  MqFactoryC<F1>::Add("F1");
  MqFactoryC<F2>::Add("F2");
  MqFactoryC<F3>::Add("F3");

  // call factory using the !second! argument,
  // first argument is the executable self
  MqC *filter = MqFactoryC<MqC>::GetCalled(argv[1]).New();
  try {
    filter->LinkCreateVC (argc-1, argv+1);
    filter->ProcessEvent (MQ_WAIT_FOREVER);
  } catch (const exception& e) {
    filter->ErrorSet(e);
  }
  filter->Exit();
}

