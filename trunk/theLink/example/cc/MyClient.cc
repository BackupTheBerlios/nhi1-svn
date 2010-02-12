/**
 *  \file       theLink/example/cc/MyClient.cc
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include <iostream>
#include "ccmsgque.h"

using namespace std;
using namespace ccmsgque;

int MQ_CDECL main(int argc, MQ_CST argv[]) {
  static MqC c;
  try {
    c.LinkCreateVC(argc, argv);
    c.SendSTART();
    c.SendEND_AND_WAIT("HLWO");
    cout << c.ReadC() << endl;
  } catch (const exception& e) {
    c.ErrorSet(e);
  }
  c.Exit();
}
