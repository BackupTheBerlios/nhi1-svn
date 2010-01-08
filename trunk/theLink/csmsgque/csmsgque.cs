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

[assembly:AssemblyVersionAttribute("4.1.*")]
[assembly:ComVisible(false)]
[assembly:CLSCompliantAttribute(true)]
[assembly:SecurityPermission(SecurityAction.RequestMinimum, UnmanagedCode = true)]


/// \brief \libmsgque interface for C#
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

  /// \api #MqConfigSetServerSetup 
  public interface IServerSetup {
    void Call();
  }

  /// \api #MqConfigSetServerCleanup 
  public interface IServerCleanup {
    void Call();
  }

  /// \api #MqConfigSetFactory 
  public interface IFactory {
    MqS Call();
  }

  /// \api #MqConfigSetBgError 
  public interface IBgError {
    void Call();
  }

  /// \api #MqConfigSetEvent 
  public interface IEvent {
    void Call();
  }

  /// \api #MqServiceCreate
  public interface IService {
    void Call(MqS ctx);
  }

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

  /// Main \libmsgque class
  public partial class MqS
  {

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqInitCreate")]
    private static extern IntPtr MqInitCreate();

    /// \api #MqInitCreate
    protected static void Init(params string[] argv) {
      IntPtr initB = MqInitCreate();
      foreach (string s in argv) {
	MqBufferLAppendC(initB, s);
      }
    }

    private const CallingConvention MSGQUE_CC = CallingConvention.Cdecl;
    private const CharSet MSGQUE_CS = CharSet.Ansi;
    private const string MSGQUE_DLL = "libmsgque";
    private static string APP = Assembly.GetEntryAssembly().Location;

    static MqS() {
      IntPtr initB = MqInitCreate();
      if(Type.GetType ("Mono.Runtime") != null) MqBufferLAppendC(initB, "mono");
      MqBufferLAppendC(initB, APP);
    }

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
  /*                         default enums and types                           */
  /*                                                                           */
  /*****************************************************************************/

    /// \api #MqWaitOnEventE flags for #MqProcessEvent
    public enum WAIT {
      NO      = 0,
      ONCE    = 1,
      FOREVER = 2,
    };

  /*****************************************************************************/
  /*                                                                           */
  /*                            context management                             */
  /*                                                                           */
  /*****************************************************************************/

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqContextCreate")]
    private static extern IntPtr MqContextCreate([In]int size, [In]IntPtr tmpl);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqContextDelete")]
    private static extern void MqContextDelete([In,Out]ref IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqExit")]
    private static extern void MqExit([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqProcessEvent")]
    private static extern MqErrorE MqProcessEvent([In]IntPtr context, [In]long timeout, [In]int flag);

    /// \api #MqProcessEvent, wait for \e timeout seconds and process event or raise an error
    public void ProcessEvent ( long timeout, WAIT wait) {
      ErrorMqToCsWithCheck(MqProcessEvent(context, timeout, (int)wait));
    }
    /// \api #MqProcessEvent, wait application default time
    public void ProcessEvent (WAIT wait) {
      ErrorMqToCsWithCheck(MqProcessEvent(context, -2, (int)wait));
    }
    /// \api #MqProcessEvent, don't wait just check for an event
    public void ProcessEvent () {
      ErrorMqToCsWithCheck(MqProcessEvent(context, -2, (int)WAIT.NO));
    }

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqLogData")]
    private static extern void MqLogData([In]IntPtr context, [In]string prefix);

    public void LogData (string prefix) {
      MqLogData (context, prefix);
    }

    /// \api #MqExit
    public void Exit() {
      MqExit (context);
    }

    static private MqS GetSelf (IntPtr context) {
      return ((MqS)GCHandle.FromIntPtr(MqConfigGetSelf(context)).Target);
    }

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigDup")]
    private static extern MqErrorE MqConfigDup([In]IntPtr context, [In]IntPtr tmpl);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSetupDup")]
    private static extern MqErrorE MqSetupDup([In]IntPtr context, [In]IntPtr tmpl);

    static private MqErrorE FactoryCreate (IntPtr tmpl, MqFactoryE create, IntPtr data, ref IntPtr contextP) {
      try {
	contextP = ((IFactory)GCHandle.FromIntPtr(data).Target).Call().context;
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

    /// \brief default constructor used for the object factory
    public MqS() {
      context = MqContextCreate(0, IntPtr.Zero);
    //DEBUG.P("context", context);
      MqConfigSetSelf(context, (IntPtr) GCHandle.Alloc(this));
      MqConfigSetIgnoreFork(context, MQ_BOL.MQ_YES);
      MqConfigSetSetup(context, fDefaultLinkCreate, null, fDefaultLinkCreate, null, fProcessExit, fThreadExit);

      if (this is IFactory) {
	MqConfigSetFactory (context, 
	  fFactoryCreate,  (IntPtr) GCHandle.Alloc(((IFactory) this)), fProcFree,  IntPtr.Zero,
	  fFactoryDelete,  IntPtr.Zero,				      null,	  IntPtr.Zero);
      } else {
	MqConfigSetFactory (context, 
	  null,		  IntPtr.Zero,				      null,	  IntPtr.Zero,
	  fFactoryDelete,  IntPtr.Zero,				      null,	  IntPtr.Zero
	);
      }

      if (this is IServerSetup) {
	MqConfigSetServerSetup (context, fProcCall, (IntPtr) GCHandle.Alloc(
	  new ProcData((Callback)((IServerSetup) this).Call)), fProcFree, IntPtr.Zero);
      }

      if (this is IServerCleanup) {
	MqConfigSetServerCleanup (context, fProcCall, (IntPtr) GCHandle.Alloc(
	  new ProcData((Callback)((IServerCleanup) this).Call)), fProcFree, IntPtr.Zero);
      }

      if (this is IBgError) {
	MqConfigSetBgError (context, fProcCall, (IntPtr) GCHandle.Alloc(
	  new ProcData((Callback)((IBgError) this).Call)), fProcFree, IntPtr.Zero);
      }

      if (this is IEvent) {
	MqConfigSetEvent (context, fProcCall, (IntPtr) GCHandle.Alloc(
	  new ProcData((Callback)((IEvent) this).Call)), fProcFree, IntPtr.Zero);
      }
    }

    /// \brief default destructor
    ~MqS() {
//DEBUG.P("context", context);
      MqContextDelete(ref context);
    }
  } // END - class "MqS"
} // END - namespace "csmsgque"



