/**
 *  \file       example/java/MyClient.java
 *  \brief      \$Id: MyClient.java 507 2009-11-28 15:18:46Z dev1usr $
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
class MyClient extends MqS {
  public static void main(String[] argv) {
    MyClient c = new MyClient();
    try {
      c.LinkCreate(argv);
      c.SendSTART();
      c.SendEND_AND_WAIT("HLWO", 5);
      System.out.println(c.ReadC());
    } catch (Throwable e) {
      c.ErrorSet(e);
    }
    c.Exit();
  }
}
