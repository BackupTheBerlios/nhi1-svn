/**
 *  \file       theLink/example/java/testclient.java
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
package example;

import javamsgque.*;
import java.util.List;
import java.util.ArrayList;
import java.util.Collections;

class testclient extends MqS {

  // my helper
  private StringBuffer Get() throws MqSException {
    StringBuffer RET = new StringBuffer();
    SendSTART();
    SendEND_AND_WAIT("GTCX", -2);
    RET.append(ConfigGetName());
    RET.append("+");
    RET.append(ReadC());
    RET.append(ReadC());
    RET.append(ReadC());
    RET.append(ReadC());
    RET.append(ReadC());
    RET.append(ReadC());
    return RET;
  }

  public static void main(String[] argv) {
    // setup the clients
    List<String> LIST = new ArrayList<String>();
    LIST.add("--name");
    LIST.add("c1");
    LIST.add("--srvname");
    LIST.add("s1");
    Collections.addAll(LIST, argv);
    // create the objects
    testclient c0 = new testclient();
    testclient c00 = new testclient();
    testclient c01 = new testclient();
    testclient c000 = new testclient();
    testclient c1 = new testclient();
    testclient c10 = new testclient();
    testclient c100 = new testclient();
    testclient c101 = new testclient();
    try {
      // create the Link
      c0.LinkCreate("--name", "c0", "--debug", System.getenv("TS_DEBUG"), "@", "java", "example.testserver", "--name", "s0");
      c00.LinkCreateChild(c0, "--name", "c00", "--srvname", "s00");
      c01.LinkCreateChild(c0, "--name", "c01", "--srvname", "s01");
      c000.LinkCreateChild(c00, "--name", "c000", "--srvname", "s000");
      c1.LinkCreate(LIST.toArray(new String[0]));
      c10.LinkCreateChild(c1, "--name", "c10", "--srvname", "s10");
      c100.LinkCreateChild(c10, "--name", "c100", "--srvname", "s100");
      c101.LinkCreateChild(c10, "--name", "c101", "--srvname", "s101");

      // do the tests
      System.out.println(c0.Get());
      System.out.println(c00.Get());
      System.out.println(c01.Get());
      System.out.println(c000.Get());
      System.out.println(c1.Get());
      System.out.println(c10.Get());
      System.out.println(c100.Get());
      System.out.println(c101.Get());
    } catch (Throwable e) {
      c0.ErrorSet(e);
    } finally {
      // do the cleanup
      c0.LinkDelete();
      c1.LinkDelete();
    }
  }
}
