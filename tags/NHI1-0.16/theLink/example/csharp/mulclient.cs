/**
 *  \file       theLink/example/csharp/mulclient.cs
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
using System;
using csmsgque;
namespace example {
  sealed class mulclient : MqS {
    static void Main(string[] argv) {
      mulclient ctx = new mulclient();
      try {
	ctx.ConfigSetName("MyMul");
	ctx.LinkCreate(argv);
	ctx.SendSTART();
	ctx.SendD(3.67);
	ctx.SendD(22.3);
	ctx.SendEND_AND_WAIT("MMUL", 5);
	Console.WriteLine(ctx.ReadD());
      } catch (Exception ex) {
        ctx.ErrorSet (ex);
      }
      ctx.Exit();
    }
  }
}
