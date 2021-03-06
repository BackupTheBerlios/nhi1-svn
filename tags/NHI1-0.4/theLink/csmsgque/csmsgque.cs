/**
 *  \file       theLink/csmsgque/csmsgque.cs
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
using System.Text;
using System.Collections;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Security.Permissions;

[assembly:AssemblyTitleAttribute("csmsgque")]
[assembly:AssemblyDescriptionAttribute("LibMsgque language binding for C#")]
[assembly:AssemblyConfigurationAttribute("")]
[assembly:AssemblyCompanyAttribute("IPN")]
[assembly:AssemblyProductAttribute("csmsgque")]
[assembly:AssemblyCopyrightAttribute("Copyright (c) IPN 2009")]
[assembly:AssemblyTrademarkAttribute("")]
[assembly:AssemblyCultureAttribute("")]

[assembly:AssemblyVersionAttribute("4.2.*")]
[assembly:ComVisible(false)]
[assembly:CLSCompliantAttribute(true)]
[assembly:SecurityPermission(SecurityAction.RequestMinimum, UnmanagedCode = true)]

/// \defgroup Mq_Cs_API Mq_Cs_API
/// \brief \copybrief Mq_C_API
/// \details \copydetails Mq_C_API
/// \{
namespace csmsgque {

#if _DEBUG
  public class DEBUG {
    public static void SF(string id) {
      System.Diagnostics.Process thisProc = System.Diagnostics.Process.GetCurrentProcess();
      System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(2);
      Console.WriteLine(thisProc.Id + ":" + id + " -> " + sf.GetMethod() + " - " + System.Threading.Thread.CurrentThread.ManagedThreadId);
    }
    public static void P(string name, IntPtr id) {
      System.Diagnostics.Process thisProc = System.Diagnostics.Process.GetCurrentProcess();
      System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(1);
      Console.WriteLine(thisProc.Id + ":" + name + " = 0x{0:x}" + 
	  " -> " + sf.GetMethod().Name + " - " + System.Threading.Thread.CurrentThread.ManagedThreadId, (int) id);
    }
    public static void O(string name, object id) {
      System.Diagnostics.Process thisProc = System.Diagnostics.Process.GetCurrentProcess();
      System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(1);
      Console.WriteLine(thisProc.Id + ":" + name + " = {0}" + 
	  " -> " + sf.GetMethod().Name + " - " + System.Threading.Thread.CurrentThread.ManagedThreadId, id);
    }
    public static void M0() { SF("M0000000000000"); }
    public static void M1() { SF("M1111111111111"); }
    public static void M2() { SF("M2222222222222"); }
    public static void M3() { SF("M3333333333333"); }
    public static void M4() { SF("M4444444444444"); }
    public static void M5() { SF("M5555555555555"); }
    public static void M6() { SF("M6666666666666"); }
    public static void M7() { SF("M7777777777777"); }
    public static void M8() { SF("M8888888888888"); }
    public static void M9() { SF("M9999999999999"); }
    public static void ME() { SF("MEEEEEEEEEEEEE"); }
  }
#endif // _DEBUG

  internal enum MqErrorE :int {
    MQ_OK,
    MQ_CONTINUE,
    MQ_ERROR,
    MQ_EXIT
  };

  internal enum MqFactoryE :int {
    MQ_FACTORY_NEW_INIT,
    MQ_FACTORY_NEW_CHILD,
    MQ_FACTORY_NEW_SLAVE,
    MQ_FACTORY_NEW_FORK,
    MQ_FACTORY_NEW_THREAD,
    MQ_FACTORY_NEW_FILTER
  };

  internal enum MQ_BOL :byte {
    MQ_NO     = 0,
    MQ_YES    = 1
  };

  /// \ingroup Mq_Context_Cs_API
  /// \api #MqS
  public partial class MqS
  {

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqInitCreate")]
    private static extern IntPtr MqInitCreate();

    private const CallingConvention MSGQUE_CC = CallingConvention.Cdecl;
    private const CharSet MSGQUE_CS = CharSet.Ansi;
    private const string MSGQUE_DLL = "libmsgque";
    private static string APP = Assembly.GetEntryAssembly().Location;

    private IntPtr context;

    /// save delegate to protect again deleting
    static private MqExitF	    fProcessExit	= ProcessExit;
    static private MqExitF	    fThreadExit		= ThreadExit;
    static private MqTokenF	    fProcCall		= ProcCall;
    static private MqTokenDataFreeF fProcFree		= ProcFree;
    static private MqFactoryCreateF fFactoryCreate	= FactoryCreate;
    static private MqFactoryDeleteF fFactoryDelete	= FactoryDelete;

  /*****************************************************************************/
  /*                                                                           */
  /*                            context management                             */
  /*                                                                           */
  /*****************************************************************************/

    static private MqS GetSelf (IntPtr context) {
      return ((MqS)GCHandle.FromIntPtr(MqConfigGetSelf(context)).Target);
    }

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigDup")]
    private static extern MqErrorE MqConfigDup([In]IntPtr context, [In]IntPtr tmpl);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSetupDup")]
    private static extern MqErrorE MqSetupDup([In]IntPtr context, [In]IntPtr tmpl);

    static private MqErrorE FactoryCreate (IntPtr tmpl, MqFactoryE create, IntPtr data, ref IntPtr contextP) {
      try {
	contextP = ((IFactory)GCHandle.FromIntPtr(data).Target).Factory().context;
      } catch (Exception ex) {
	return MqErrorSet2 (tmpl, ex);
      }
      MqConfigDup (contextP, tmpl);
      MqSetupDup (contextP, tmpl);
      return MqErrorE.MQ_OK;
    }

    static private void FactoryDelete (IntPtr context, MQ_BOL doFactoryCleanup, IntPtr data) {
      IntPtr self = MqConfigGetSelf(context);
      MqContextDelete (ref GetSelf(context).context);
      GCHandle.FromIntPtr(self).Free();
    }
  } // END - class "MqS"
} // END - namespace "csmsgque"

/// \}


