/**
 *  \file       theLink/example/csharp/testclient.cs
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
using System;
using System.IO;
using System.Text;
using System.Collections.Generic;
using System.Reflection;
using csmsgque;

namespace example {

  sealed class testclient : MqS {

    private StringBuilder Get() {
      StringBuilder RET = new StringBuilder();
      SendSTART();
      SendEND_AND_WAIT("GTCX", -1);
      RET.Append(ConfigGetName());
      RET.Append("+");
      RET.Append(ReadC());
      RET.Append(ReadC());
      RET.Append(ReadC());
      RET.Append(ReadC());
      RET.Append(ReadC());
      RET.Append(ReadC());
      return RET;
    }

    static void Main(string[] argv) {

      // setup the clients
      string dirname = new FileInfo(Assembly.GetEntryAssembly().Location).DirectoryName;
      string server = Path.Combine(dirname, "testserver.exe");
      List<String> LIST = new List<String>() {"--name", "c1", "--srvname", "s1"};
      LIST.AddRange(argv);
      // create the object
      testclient c0 = new testclient();
      testclient c00 = new testclient();
      testclient c01 = new testclient();
      testclient c000 = new testclient();
      testclient c1 = new testclient();
      testclient c10 = new testclient();
      testclient c100 = new testclient();
      testclient c101 = new testclient();
      try {
	// create the link
	if(Type.GetType ("Mono.Runtime") != null)
	  c0.LinkCreate("--name", "c0", "--debug", System.Environment.GetEnvironmentVariable("TS_DEBUG"), 
					    "@", "mono", server, "--name", "s0");
	else
	  c0.LinkCreate("--name", "c0", "--debug", System.Environment.GetEnvironmentVariable("TS_DEBUG"), 
					    "@", server, "--name", "s0");

	c00.LinkCreateChild(c0, "--name", "c00", "--srvname", "s00");
	c01.LinkCreateChild(c0, "--name", "c01", "--srvname", "s01");
	c000.LinkCreateChild(c00, "--name", "c000", "--srvname", "s000");
	c1.LinkCreate(LIST.ToArray());
	c10.LinkCreateChild(c1, "--name", "c10", "--srvname", "s10");
	c100.LinkCreateChild(c10, "--name", "c100", "--srvname", "s100");
	c101.LinkCreateChild(c10, "--name", "c101", "--srvname", "s101");
	// do the tests
	Console.WriteLine(c0.Get());
	Console.WriteLine(c00.Get());
	Console.WriteLine(c01.Get());
	Console.WriteLine(c000.Get());
	Console.WriteLine(c1.Get());
	Console.WriteLine(c10.Get());
	Console.WriteLine(c100.Get());
	Console.WriteLine(c101.Get());
      } catch {
      } finally {
	// do the cleanup
	c0.LinkDelete();
	c1.LinkDelete();
      }
    }
  }
}
