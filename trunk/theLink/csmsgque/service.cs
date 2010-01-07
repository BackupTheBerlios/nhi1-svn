/**
 *  \file       theLink/csmsgque/service.cs
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
/*                                service                                    */
/*                                                                           */
/*****************************************************************************/

using System;
using System.Runtime.InteropServices;

namespace csmsgque {

  public partial class MqS
  {
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqServiceCreate")]
    private static extern MqErrorE MqServiceCreate([In]IntPtr context, [In]string token, 
	[In]MqTokenF srvcall, [In]IntPtr data, [In]MqTokenDataFreeF srvfree);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqServiceDelete")]
    private static extern MqErrorE MqServiceDelete([In]IntPtr context, [In]string token);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqServiceProxy")]
    private static extern MqErrorE MqServiceProxy([In]IntPtr context, [In]string token, [In]int id);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqServiceGetToken")]
    private static extern IntPtr MqServiceGetToken([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqServiceIsTransaction")]
    private static extern bool MqServiceIsTransaction([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqServiceGetFilter")]
    private static extern MqErrorE MqServiceGetFilter([In]IntPtr context, [In]int id, [Out]out IntPtr filter);

    // PUBLIC  #########################################################################

    /// \api #MqServiceGetToken
    public string   ServiceGetToken()	    { 
      return Marshal.PtrToStringAnsi(MqServiceGetToken(context)); 
    }

    /// \api #MqServiceIsTransaction
    public bool	    ServiceIsTransaction() { 
      return MqServiceIsTransaction(context); 
    }

    /// \api #MqServiceGetFilter
    public MqS	    ServiceGetFilter()	    {
      IntPtr filter;
      ErrorMqToCsWithCheck(MqServiceGetFilter(context,0,out filter));
      return GetSelf(filter);
    }

    /// \api #MqServiceCreate
    public void ServiceCreate(string token, Callback call) {
      ErrorMqToCsWithCheck(MqServiceCreate(context, token, fProcCall, (IntPtr) GCHandle.Alloc(new ProcData(call)), fProcFree));
    }

    /// \api #MqServiceCreate
    public void ServiceCreate(string token, IService call) {
      ErrorMqToCsWithCheck(MqServiceCreate(context, token, fProcCall, (IntPtr) GCHandle.Alloc(new ProcData(call)), fProcFree));
    }

    /// \api #MqServiceDelete
    public void ServiceDelete(string token) {
      ErrorMqToCsWithCheck(MqServiceDelete(context, token));
    }

    /// \api #MqServiceProxy
    public void ServiceProxy(string token) {
      ErrorMqToCsWithCheck(MqServiceProxy(context, token, 0));
    }

    /// \api #MqServiceProxy
    public void ServiceProxy(string token, int id) {
      ErrorMqToCsWithCheck(MqServiceProxy(context, token, id));
    }


  } // END - class "MqS"
} // END - namespace "csmsgque"

