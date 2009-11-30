/**
 *  \file       example/java/Filter2.java
 *  \brief      \$Id: Filter2.java 507 2009-11-28 15:18:46Z dev1usr $
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

class Filter2 extends MqS implements IFilterFTR {

  // service definition from IFilter
  public void FTR () {
    throw new RuntimeException("my error");
  }

  public static void main(String[] argv) {
    Filter2 srv = new Filter2();
    try {
      srv.ConfigSetName("filter");
      srv.LinkCreate(argv);
      srv.ProcessEvent(MqS.WAIT.FOREVER);
    } catch (Throwable e) {
      srv.ErrorSet(e);
    } finally {
      srv.Exit();
    }
  }
}
