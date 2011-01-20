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

/// \defgroup Mq_Service_Cs_API Mq_Service_Cs_API
/// \ingroup Mq_Cs_API
/// \brief \copybrief Mq_Service_C_API
/// \details \copydetails Mq_Service_C_API
/// \{

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqServiceCreate")]
    private static extern MqErrorE MqServiceCreate([In]IntPtr context, [In]string token, 
	[In]MqTokenF srvcall, [In]IntPtr data, [In]MqDataFreeF srvfree);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqServiceDelete")]
    private static extern MqErrorE MqServiceDelete([In]IntPtr context, [In]string token);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqServiceProxy")]
    private static extern MqErrorE MqServiceProxy([In]IntPtr context, [In]string token, [In]int id);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqServiceGetToken")]
    private static extern IntPtr MqServiceGetToken([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqServiceIsTransaction")]
    private static extern MQ_BOL MqServiceIsTransaction([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqServiceGetFilter")]
    private static extern MqErrorE MqServiceGetFilter([In]IntPtr context, [In]int id, [Out]out IntPtr filter);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqProcessEvent")]
    private static extern MqErrorE MqProcessEvent([In]IntPtr context, [In]long timeout, [In]int flag);

    // PUBLIC  #########################################################################

    /// \api #MqServiceGetToken
    public string   ServiceGetToken()	    { 
      return Marshal.PtrToStringAnsi(MqServiceGetToken(context)); 
    }

    /// \api #MqServiceIsTransaction
    public bool	    ServiceIsTransaction() { 
      return MqServiceIsTransaction(context) == MQ_BOL.MQ_YES; 
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

    /// \api #MqWaitOnEventE
    public enum WAIT {
      NO      = 0,
      ONCE    = 1,
      FOREVER = 2,
    };

    /// \api timeout default values
    public enum TIMEOUT {
      DEFAULT = -1,   ///< #MQ_TIMEOUT_DEFAULT
      USER    = -2,   ///< #MQ_TIMEOUT_USER
      MAX     = -3,   ///< #MQ_TIMEOUT_MAX
    };

    /// \api #MqProcessEvent, wait for \e timeout seconds and process event or raise an error
    public void ProcessEvent ( long timeout, WAIT wait) {
      ErrorMqToCsWithCheck(MqProcessEvent(context, timeout, (int)wait));
    }
    /// \api #MqProcessEvent, wait for \e TIMEOUT seconds and process event or raise an error
    public void ProcessEvent ( TIMEOUT timeout, WAIT wait) {
      ErrorMqToCsWithCheck(MqProcessEvent(context, (long)timeout, (int)wait));
    }
    /// \api #MqProcessEvent, wait application default time #MQ_TIMEOUT_DEFAULT
    public void ProcessEvent (WAIT wait) {
      ErrorMqToCsWithCheck(MqProcessEvent(context, -2, (int)wait));
    }
    /// \api #MqProcessEvent, don't wait just check for an event
    public void ProcessEvent () {
      ErrorMqToCsWithCheck(MqProcessEvent(context, -2, (int)WAIT.NO));
    }

/// \} Mq_Service_Cs_API

  } // END - class "MqS"
} // END - namespace "csmsgque"

