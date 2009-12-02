/**
 *  \file       csmsgque/read.cs
 *  \brief      \$Id: read.cs 507 2009-11-28 15:18:46Z dev1usr $
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
/*                                   read                                    */
/*                                                                           */
/*****************************************************************************/

using System;
using System.Runtime.InteropServices;

namespace csmsgque {

  public partial class MqS
  {

    // PRIVAT

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqReadY")]
    private static extern MqErrorE MqReadY(IntPtr context, out byte outV);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqReadO")]
    private static extern MqErrorE MqReadO(IntPtr context, out byte outV);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqReadS")]
    private static extern MqErrorE MqReadS(IntPtr context, out short outV);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqReadI")]
    private static extern MqErrorE MqReadI(IntPtr context, out int outV);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqReadF")]
    private static extern MqErrorE MqReadF(IntPtr context, out float outV);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqReadW")]
    private static extern MqErrorE MqReadW(IntPtr context, out long outV);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqReadD")]
    private static extern MqErrorE MqReadD(IntPtr context, out double outV);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqReadB")]
    private static extern MqErrorE MqReadB(IntPtr context, out IntPtr outV, out int sizeV);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqReadU")]
    private static extern MqErrorE MqReadU(IntPtr context, out IntPtr outV);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqReadC")]
    private static extern MqErrorE MqReadC(IntPtr context, out IntPtr outV);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqReadL_START")]
    private static extern MqErrorE MqReadL_START(IntPtr context, IntPtr buf);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqReadL_END")]
    private static extern MqErrorE MqReadL_END(IntPtr context);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqReadGetNumItems")]
    private static extern int MqReadGetNumItems(IntPtr context);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqReadItemExists")]
    private static extern byte MqReadItemExists(IntPtr context);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqReadUndo")]
    private static extern MqErrorE MqReadUndo(IntPtr context);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqReadProxy")]
    private static extern MqErrorE MqReadProxy(IntPtr contextMsgque, IntPtr sendMsgque);

    // PUBLIC

    /// \api #MqReadY
    public byte ReadY() {
      byte outV;
      ErrorMqToCsWithCheck(MqReadY(context, out outV));
      return outV;
    }

    /// \api #MqReadO
    public bool ReadO() {
      byte outV;
      ErrorMqToCsWithCheck(MqReadO(context, out outV));
      return (outV == (byte) MQ_BOL.MQ_YES ? true : false);
    }

    /// \api #MqReadS
    public short ReadS() {
      short outV;
      ErrorMqToCsWithCheck(MqReadS(context, out outV));
      return outV;
    }

    /// \api #MqReadI
    public int ReadI() {
      int outV;
      ErrorMqToCsWithCheck(MqReadI(context, out outV));
      return outV;
    }

    /// \api #MqReadF
    public float ReadF() {
      float outV;
      ErrorMqToCsWithCheck(MqReadF(context, out outV));
      return outV;
    }

    /// \api #MqReadW
    public long ReadW() {
      long outV;
      ErrorMqToCsWithCheck(MqReadW(context, out outV));
      return outV;
    }

    /// \api #MqReadD
    public double ReadD() {
      double outV;
      ErrorMqToCsWithCheck(MqReadD(context, out outV));
      return outV;
    }

    /// \api #MqReadB
    public byte[] ReadB() {
      IntPtr outV;
      int size;
      ErrorMqToCsWithCheck(MqReadB(context, out outV, out size));
      byte[] outB = new byte[size];
      Marshal.Copy(outV,outB,0,size);
      return outB;
    }

    /// \api #MqReadU
    public MqBufferS ReadU() {
      IntPtr outV;
      ErrorMqToCsWithCheck(MqReadU(context, out outV));
      return new MqBufferS(outV);
    }

    /// \api #MqReadC
    public string ReadC() {
      IntPtr outV;
      ErrorMqToCsWithCheck(MqReadC(context, out outV));
      return Marshal.PtrToStringAnsi(outV);;
    }

    /// \api #MqReadProxy
    public void ReadProxy(MqS ctx) {
      ErrorMqToCsWithCheck(MqReadProxy(context, ctx.context));
    }

    /// \api #MqReadL_START
    public void ReadL_START() {
      ErrorMqToCsWithCheck(MqReadL_START(context, IntPtr.Zero));
    }

    /// \api #MqReadL_START with #MqBufferS argument
    public void ReadL_START(MqBufferS buffer) {
      ErrorMqToCsWithCheck(MqReadL_START(context, buffer.buf));
    }

    /// \api #MqReadL_END
    public void ReadL_END() {
      ErrorMqToCsWithCheck(MqReadL_END(context));
    }

    /// \api #MqReadGetNumItems
    public int ReadGetNumItems() {
      return MqReadGetNumItems(context);
    }

    /// \api #MqReadItemExists
    public bool ReadItemExists() {
      return (MqReadItemExists(context) == (byte) MQ_BOL.MQ_YES ? true : false);
    }

    /// \api #MqReadUndo
    public void ReadUndo() {
      ErrorMqToCsWithCheck(MqReadUndo(context));
    }

  } // END - class "MqS"
} // END - namespace "csmsgque"



