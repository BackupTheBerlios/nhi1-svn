/**
 *  \file       example/csharp/MyClient.cs
 *  \brief      \$Id: MyClient.cs 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
using System;
using csmsgque;

namespace example {

  sealed class MyClient : MqS {

    static void Main(string[] argv) {
      MyClient c = new MyClient();
      try {
	c.LinkCreate(argv);
	c.SendSTART();
	c.SendEND_AND_WAIT("HLWO", 5);
	Console.WriteLine(c.ReadC());
      } catch (Exception ex) {
        c.ErrorSet (ex);
      }
      c.Exit();
    }
  }
}
