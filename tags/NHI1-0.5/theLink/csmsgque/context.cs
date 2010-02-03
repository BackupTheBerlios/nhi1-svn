/**
 *  \file       theLink/csmsgque/context.cs
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

/*****************************************************************************/
/*                                                                           */
/*                                context                                    */
/*                                                                           */
/*****************************************************************************/

using System;
using System.Runtime.InteropServices;

namespace csmsgque {

  public partial class MqS
  {

/// \defgroup Mq_Context_Cs_API Mq_Context_Cs_API
/// \ingroup Mq_Cs_API
/// \brief \copybrief Mq_Context_C_API
/// \details \copydetails Mq_Context_C_API
/// \{

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqContextCreate")]
    private static extern IntPtr MqContextCreate([In]int size, [In]IntPtr tmpl);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqContextDelete")]
    private static extern void MqContextDelete([In,Out]ref IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqExit")]
    private static extern void MqExit([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqLogData")]
    private static extern void MqLogData([In]IntPtr context, [In]string prefix);

  // Private

    private static void ProcessExit (int num)
    {
      System.GC.Collect();
      Environment.Exit (num);
    }

    private static void ThreadExit (int num)
    {
      System.GC.Collect();
    }

  // Public

#if _DEBUG
    /// \api #MqLogData
    public void LogData (string data) {
      MqLogData (context, data);
    }
#endif // _DEBUG

    /// \api #MqExit
    public void Exit() {
      MqExit (context);
    }

    /// \api #MqContextCreate
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
	  new ProcData((Callback)((IServerSetup) this).ServerSetup)), fProcFree, IntPtr.Zero);
      }

      if (this is IServerCleanup) {
	MqConfigSetServerCleanup (context, fProcCall, (IntPtr) GCHandle.Alloc(
	  new ProcData((Callback)((IServerCleanup) this).ServerCleanup)), fProcFree, IntPtr.Zero);
      }

      if (this is IBgError) {
	MqConfigSetBgError (context, fProcCall, (IntPtr) GCHandle.Alloc(
	  new ProcData((Callback)((IBgError) this).BgError)), fProcFree, IntPtr.Zero);
      }

      if (this is IEvent) {
	MqConfigSetEvent (context, fProcCall, (IntPtr) GCHandle.Alloc(
	  new ProcData((Callback)((IEvent) this).Event)), fProcFree, IntPtr.Zero);
      }
    }

    /// \api #MqContextDelete
    ~MqS() {
      MqContextDelete(ref context);
    }

/// \} Mq_Context_Cs_API

  } // END - class "MqS"
} // END - namespace "csmsgque"
