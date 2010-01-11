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
    private static extern bool MqLinkIsParent([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqLinkIsConnected")]
    private static extern bool MqLinkIsConnected([In]IntPtr context);

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

  // PUBLIC

    /// \api #LinkIsParent
    public bool	    LinkIsParent()	    { 
      return MqLinkIsParent(context); 
    }

    /// \api #LinkIsConnected
    public bool	    LinkIsConnected()	    { 
      return MqLinkIsConnected(context); 
    }

    /// \api #LinkGetCtxId
    public int	    LinkGetCtxId()	    { 
      return MqLinkGetCtxId(context); 
    }

    /// \api #LinkGetParent
    public MqS	    LinkGetParent()	    { 
      IntPtr parentP = MqLinkGetParent(context);
      return (parentP == IntPtr.Zero ? null : GetSelf(parentP));
    }

    /// \api #LinkCreate
    public void LinkCreate(params string[] argv) {

      // fill the argv/alfa
      IntPtr largv = IntPtr.Zero;
      if (argv.Length != 0) {
	largv = MqBufferLCreate(argv.Length+1);
	MqBufferLAppendC(largv, APP);
	foreach (string a in argv) {
	  MqBufferLAppendC(largv, a);
	}
      }

      // create Context
      ErrorMqToCsWithCheck (MqLinkCreate(context, ref largv));
    }

    /// \api #LinkCreateChild
    public void LinkCreateChild(MqS parent, params string[] argv) {

      // fill the argv/alfa
      IntPtr largv = IntPtr.Zero;
      if (argv.Length != 0) {
	largv = MqBufferLCreate(argv.Length+1);
	MqBufferLAppendC(largv, APP);
	foreach (string a in argv) {
	  MqBufferLAppendC(largv, a);
	}
      }

      // create Context
      ErrorMqToCsWithCheck (MqLinkCreateChild(context, parent.context, ref largv));
    }

    /// \api #LinkDelete
    public void LinkDelete() {
      MqLinkDelete (context);
    }

/// \} Mq_Link_Cs_API

  } // END - class "MqS"
} // END - namespace "csmsgque"


