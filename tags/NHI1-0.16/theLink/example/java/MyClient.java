/**
 *  \file       theLink/example/java/MyClient.java
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
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
