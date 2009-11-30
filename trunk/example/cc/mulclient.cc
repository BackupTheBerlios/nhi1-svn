/**
 *  \file       example/cc/mulclient.cc
 *  \brief      \$Id: mulclient.cc 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include <iostream>
#include "ccmsgque.h"

using namespace std;
using namespace ccmsgque;

int MQ_CDECL main (int argc, MQ_CST argv[])
{
  MqC ctx;
  try {
    ctx.ConfigSetName ("MyMulClient");
    ctx.LinkCreateVC (argc, argv);
    ctx.SendSTART();
    ctx.SendD(3.67);
    ctx.SendD(22.3);
    ctx.SendEND_AND_WAIT("MMUL", 5);
    cout << ctx.ReadD() << endl;
  } catch (const exception& e) {
    ctx.ErrorSet(e);
  }
  ctx.Exit();
}
