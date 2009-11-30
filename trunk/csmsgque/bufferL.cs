/**
 *  \file       csmsgque/bufferL.cs
 *  \brief      \$Id: bufferL.cs 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

using System;
using System.Runtime.InteropServices;

namespace csmsgque {

  public partial class MqS
  {

    /// \api #MqBufferLCreate
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferLCreate")]
    private static extern IntPtr MqBufferLCreate([In]int num);

    /// \api #MqBufferLAppendC
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferLAppendC")]
    private static extern void MqBufferLAppendC([In]IntPtr bufL, [In,MarshalAs(UnmanagedType.LPStr)]string val);

  }

} // END - namespace "csmsgque"


