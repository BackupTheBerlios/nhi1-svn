/**
 *  \file       csmsgque/service.cs
 *  \brief      \$Id: service.cs 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
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

    // PUBLIC  #########################################################################

    /// \api #MqServiceCreate
    public void ServiceCreate(string token, Callback call) {
        MqServiceCreate(context, token, fProcCall, (IntPtr) GCHandle.Alloc(new ProcData(call)), fProcFree);
    }

    /// \api #MqServiceCreate
    public void ServiceCreate(string token, IService call) {
        MqServiceCreate(context, token, fProcCall, (IntPtr) GCHandle.Alloc(new ProcData(call)), fProcFree);
    }

    /// \api #MqServiceDelete
    public void ServiceDelete(string token) {
      MqServiceDelete(context, token);
    }


  } // END - class "MqS"
} // END - namespace "csmsgque"




