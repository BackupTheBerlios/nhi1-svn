/**
 *  \file       theLink/csmsgque/MqS.cs
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
using System.Collections.Generic;
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

[assembly:AssemblyVersionAttribute("5.5.*")]
[assembly:ComVisible(false)]
[assembly:CLSCompliantAttribute(true)]

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
      Console.Error.WriteLine(thisProc.Id + ":" + id + " -> " + sf.GetMethod() + " - " + System.Threading.Thread.CurrentThread.ManagedThreadId);
    }
    public static void P(string name, IntPtr id) {
      System.Diagnostics.Process thisProc = System.Diagnostics.Process.GetCurrentProcess();
      System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(1);
      Console.Error.WriteLine(thisProc.Id + ":" + name + " = 0x{0:x}" + 
	  " -> " + sf.GetMethod().Name + " - " + System.Threading.Thread.CurrentThread.ManagedThreadId, (int) id);
    }
    public static void O(string name, object id) {
      System.Diagnostics.Process thisProc = System.Diagnostics.Process.GetCurrentProcess();
      System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(1);
      Console.Error.WriteLine(thisProc.Id + ":" + name + " = {0}" + 
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
    MQ_ERROR
  };

  internal enum MQ_BOL :byte {
    MQ_NO     = 0,
    MQ_YES    = 1
  };

  /// \ingroup Mq_Context_Cs_API
  /// \api #MqS
  public partial class MqS
  {

    static MqS() {

      // init the application "spawn" starter
      IntPtr initB = MqInitArg0();
      if(Type.GetType ("Mono.Runtime") != null) MqBufferLAppendC(initB, "mono");
      MqBufferLAppendC(initB, APP);
    }

    private const CallingConvention MSGQUE_CC = CallingConvention.Cdecl;
    private const CharSet MSGQUE_CS = CharSet.Ansi;
    internal const string MSGQUE_DLL = "msgque";
    private static string APP = Assembly.GetEntryAssembly().Location;

    internal IntPtr context;

    /// save delegate to protect again deleting
    static private MqExitF	      fProcessExit  = ProcessExit;
    static private MqExitF	      fThreadExit   = ThreadExit;
    static private MqTokenF	      fProcCall	    = ProcCall;
    static internal MqDataFreeF	      fProcFree	    = ProcFree;

  /*****************************************************************************/
  /*                                                                           */
  /*                            context management                             */
  /*                                                                           */
  /*****************************************************************************/

    internal static MqS GetSelf (IntPtr context) {
      return ((MqS)GCHandle.FromIntPtr(MqConfigGetSelf(context)).Target);
    }

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigDup")]
    private static extern MqErrorE MqConfigDup([In]IntPtr context, [In]IntPtr tmpl);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqDLogX")]
    private static extern void MqDLogX([In]IntPtr context, [In]string prefix, [In]int level, [In]string fmt, [In]string val );

    public void DLogC (int level, string val) {
      System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(1);
      MqDLogX (context, sf.GetMethod().Name, level, "%s", val);
    }

  /*****************************************************************************/
  /*                                                                           */
  /*                                static                                     */
  /*                                                                           */
  /*****************************************************************************/

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqInitArg0")]
    private static extern IntPtr MqInitArg0(
      [In,MarshalAs(UnmanagedType.LPStr)]string arg0=null, 
      [In,MarshalAs(UnmanagedType.LPStr)]string arg2=null,
      [In,MarshalAs(UnmanagedType.LPStr)]string arg1=null
    );

    /// \api #MqInitArg0
    protected static void Init(params string[] argv) {
      IntPtr initB = MqInitArg0();
      foreach (string s in argv) {
	MqBufferLAppendC(initB, s);
      }
    }

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqResolve")]
    private static extern IntPtr MqResolve([In,MarshalAs(UnmanagedType.LPStr)]string ident, [In,Out]ref Int32 size);

    /// \api #MqResolve
    protected static MqS[] Resolve(string ident) {
      Int32 size = 0;
      int elementSize = Marshal.SizeOf(typeof(IntPtr));
      IntPtr ctxL = MqResolve(ident,ref size);
      MqS[] ret = new MqS[size];
      for(int i=0; i<size; i++) {
	ret[i] = GetSelf(Marshal.ReadIntPtr(ctxL,i*elementSize));
      }
      return ret;
    }

  /*****************************************************************************/
  /*                                                                           */
  /*                                static                                     */
  /*                                                                           */
  /*****************************************************************************/

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqFactoryAdd")]
    internal static extern IntPtr MqFactoryAdd([In]IntPtr error, [In]string ident,
      [In]FactoryCreateF FactoryCreate, [In]IntPtr CreateData, [In]FactoryDataFreeF CreateFree, [In]FactoryDataCopyF CreateCopy,
      [In]FactoryDeleteF FactoryDelete, [In]IntPtr DeleteData, [In]FactoryDataFreeF DeleteFree, [In]FactoryDataCopyF DeleteCopy
    );

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqFactoryDefault")]
    internal static extern IntPtr MqFactoryDefault([In]IntPtr error, [In]string ident,
      [In]FactoryCreateF FactoryCreate, [In]IntPtr CreateData, [In]FactoryDataFreeF CreateFree, [In]FactoryDataCopyF CreateCopy,
      [In]FactoryDeleteF FactoryDelete, [In]IntPtr DeleteData, [In]FactoryDataFreeF DeleteFree, [In]FactoryDataCopyF DeleteCopy
    );

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqFactoryDefaultIdent")]
    internal static extern IntPtr MqFactoryDefaultIdent();

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqFactoryGet")]
    internal static extern IntPtr MqFactoryGet([In]string ident);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqFactoryGetCalled")]
    internal static extern IntPtr MqFactoryGetCalled([In]string ident);

  // INSTANCE

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqFactoryNew")]
    internal static extern IntPtr MqFactoryNew([In]IntPtr error, [In]IntPtr data, [In]IntPtr item);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqFactoryCopy")]
    internal static extern IntPtr MqFactoryCopy([In]IntPtr item, [In]string ident);

  // OTHER

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSetupDup")]
    internal static extern MqErrorE MqSetupDup([In]IntPtr context, [In]IntPtr tmpl);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqFactoryItemGetCreateData")]
    internal static extern IntPtr MqFactoryItemGetCreateData([In]IntPtr item);

  } // END - class "MqS"
} // END - namespace "csmsgque"

/// \}


