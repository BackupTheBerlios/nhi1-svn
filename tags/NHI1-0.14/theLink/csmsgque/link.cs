/**
 *  \file       theLink/csmsgque/link.cs
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
using System.Runtime.InteropServices;

namespace csmsgque {

  public partial class MqS
  {

/// \defgroup Mq_Link_Cs_API Mq_Link_Cs_API
/// \ingroup Mq_Cs_API
/// \brief \copybrief Mq_Link_C_API
/// \details \copydetails Mq_Link_C_API
/// \{
  
  // PRIVATE

    static private MqCreateF	    fDefaultLinkCreate  = MqLinkDefault;

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqLinkIsParent")]
    private static extern MQ_BOL MqLinkIsParent([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqLinkIsConnected")]
    private static extern MQ_BOL MqLinkIsConnected([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqLinkGetParent")]
    private static extern IntPtr MqLinkGetParent([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqLinkGetCtxId")]
    private static extern int MqLinkGetCtxId([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqLinkCreate")]
    private static extern MqErrorE MqLinkCreate([In]IntPtr context, [In,Out]ref IntPtr argv);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqLinkCreateChild")]
    private static extern MqErrorE MqLinkCreateChild([In]IntPtr context, [In]IntPtr parent, [In,Out]ref IntPtr argv);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqLinkDefault")]
    private static extern MqErrorE MqLinkDefault([In]IntPtr context, [In,Out]ref IntPtr argv);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqLinkDelete")]
    private static extern void MqLinkDelete([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqLinkConnect")]
    private static extern MqErrorE MqLinkConnect([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqLinkGetTargetIdent")]
    private static extern IntPtr MqLinkGetTargetIdent([In]IntPtr context);

  // PUBLIC

    /// \api MqLinkGetTargetIdent
    public string LinkGetTargetIdent()	    { 
      return Marshal.PtrToStringAnsi(MqLinkGetTargetIdent(context)); 
    }

    /// \api MqLinkIsParent
    public bool	    LinkIsParent()	    { 
      return MqLinkIsParent(context) == MQ_BOL.MQ_YES; 
    }

    /// \api MqLinkIsConnected
    public bool	    LinkIsConnected()	    { 
      return MqLinkIsConnected(context) == MQ_BOL.MQ_YES; 
    }

    /// \api MqLinkGetCtxId
    public int	    LinkGetCtxId()	    { 
      return MqLinkGetCtxId(context); 
    }

    /// \api MqLinkGetParent
    public MqS	    LinkGetParent()	    { 
      IntPtr parentP = MqLinkGetParent(context);
      return (parentP == IntPtr.Zero ? null : GetSelf(parentP));
    }

    /// \api MqLinkCreate
    public void LinkCreate(params string[] argv) {

      // fill the argv/alfa
      IntPtr largv = IntPtr.Zero;
      if (argv.Length != 0) {
	largv = MqBufferLCreate(argv.Length+1);
	if (argv[0][0] == '-' || argv[0][0] == '@')
	  MqBufferLAppendC(largv, APP);
	foreach (string a in argv) {
	  MqBufferLAppendC(largv, a);
	}
      }

      // create Context
      ErrorMqToCsWithCheck (MqLinkCreate(context, ref largv));
    }

    /// \api MqLinkCreateChild
    public void LinkCreateChild(MqS parent, params string[] argv) {

      // fill the argv/alfa
      IntPtr largv = IntPtr.Zero;
      if (argv.Length != 0) {
	largv = MqBufferLCreate(argv.Length+1);
	if (argv[0][0] == '-' || argv[0][0] == '@')
	  MqBufferLAppendC(largv, APP);
	foreach (string a in argv) {
	  MqBufferLAppendC(largv, a);
	}
      }

      // create Context
      ErrorMqToCsWithCheck (MqLinkCreateChild(context, parent.context, ref largv));
    }

    /// \api MqLinkDelete
    public void LinkDelete() {
      MqLinkDelete (context);
    }

    /// \api MqLinkConnect
    public void LinkConnect() {
      ErrorMqToCsWithCheck (MqLinkConnect (context));
    }

/// \} Mq_Link_Cs_API

  } // END - class "MqS"
} // END - namespace "csmsgque"


