/**
 *  \file       theLink/csmsgque/MqDumpS.cs
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
/*                                  buffer                                   */
/*                                                                           */
/*****************************************************************************/

using System;
using System.Runtime.InteropServices;

namespace csmsgque {

  public class MqDumpS
  {

/// \defgroup Mq_Dump_Cs_API Mq_Dump_Cs_API
/// \ingroup Mq_Cs_API
/// \brief \copybrief Mq_Dump_C_API
/// \details \copydetails Mq_Dump_C_API
/// \{

    private const CallingConvention MSGQUE_CC = CallingConvention.Cdecl;
    private const CharSet MSGQUE_CS = CharSet.Ansi;
    private const string MSGQUE_DLL = MqS.MSGQUE_DLL;

    internal IntPtr hdl = IntPtr.Zero;

    internal MqDumpS (IntPtr data) {
      hdl = data;
    }

    // ----------------------------------------------------------------------------------------------

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSysFreeP")]
    private static extern void MqSysFreeP([In]IntPtr ptr);

    ~MqDumpS() {
      MqSysFreeP(hdl);
      hdl = IntPtr.Zero;
    }

    // ----------------------------------------------------------------------------------------------

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqDumpSize")]
    private static extern int MqDumpSize([In]IntPtr hdl);

    /// \api #MqDumpSize
    public int Size () {
      return MqDumpSize(hdl);
    }

/// \} Mq_Dump_Cs_API

  } // END - class "MqDumpS"
} // END - namespace "csmsgque"

