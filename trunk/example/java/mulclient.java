/**
 *  \file       example/java/mulclient.java
 *  \brief      \$Id: mulclient.java 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
package example;
import javamsgque.*;
class mulclient extends MqS {
  public static void main(String[] argv) {
    mulclient ctx = new mulclient();
    try {
      ctx.ConfigSetName("MyMul");
      ctx.LinkCreate(argv);
      ctx.SendSTART();
      ctx.SendD(3.67);
      ctx.SendD(22.3);
      ctx.SendEND_AND_WAIT("MMUL", 5);
      System.out.println(ctx.ReadD());
    } catch (Throwable e) {
      ctx.ErrorSet(e);
    }
    ctx.Exit();
  }
}
