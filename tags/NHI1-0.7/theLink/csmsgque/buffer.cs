/**
 *  \file       theLink/csmsgque/buffer.cs
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

  public partial class MqS
  {
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferLCreate")]
    private static extern IntPtr MqBufferLCreate([In]int num);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferLAppendC")]
    private static extern void MqBufferLAppendC([In]IntPtr bufL, [In,MarshalAs(UnmanagedType.LPStr)]string val);
  }

  /// \ingroup Mq_Buffer_Cs_API
  /// \api #MqBufferS
  public class MqBufferS
  {

/// \defgroup Mq_Buffer_Cs_API Mq_Buffer_Cs_API
/// \ingroup Mq_Cs_API
/// \brief \copybrief Mq_Buffer_C_API
/// \details \copydetails Mq_Buffer_C_API
/// \{

    private const CallingConvention MSGQUE_CC = CallingConvention.Cdecl;
    private const CharSet MSGQUE_CS = CharSet.Ansi;
    private const string MSGQUE_DLL = "libmsgque";

    internal IntPtr buf = IntPtr.Zero;

    // INTERNAL

    internal MqBufferS (IntPtr data) {
      buf = data;
    }

    // PRIVAT

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferGetY")]
    private  static extern MqErrorE MqBufferGetY([In]IntPtr buf, out byte outV);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferGetO")]
    private  static extern MqErrorE MqBufferGetO([In]IntPtr buf, out MQ_BOL outV);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferGetS")]
    private  static extern MqErrorE MqBufferGetS([In]IntPtr buf, out short outV);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferGetI")]
    private  static extern MqErrorE MqBufferGetI([In]IntPtr buf, out int outV);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferGetF")]
    private  static extern MqErrorE MqBufferGetF([In]IntPtr buf, out float outV);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferGetW")]
    private  static extern MqErrorE MqBufferGetW([In]IntPtr buf, out long outV);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferGetD")]
    private  static extern MqErrorE MqBufferGetD([In]IntPtr buf, out double outV);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferGetB")]
    private  static extern MqErrorE MqBufferGetB([In]IntPtr buf, out IntPtr outV, out int size);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferGetC")]
    private  static extern MqErrorE MqBufferGetC([In]IntPtr buf, out IntPtr outV);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferGetContext")]
    private static extern IntPtr MqBufferGetContext(IntPtr bufP);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqErrorGetText")]
    private static extern IntPtr MqErrorGetText(IntPtr errorP);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqErrorGetNum")]
    private static extern int MqErrorGetNum(IntPtr errorP);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqErrorGetCode")]
    private static extern MqErrorE MqErrorGetCode(IntPtr errorP);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqErrorReset")]
    private static extern void MqErrorReset(IntPtr errorP);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferGetType")]
    private static extern char MqBufferGetType([In]IntPtr buf);

    private void ErrorBufToCsWithCheck(MqErrorE err) {
      if (err <= MqErrorE.MQ_CONTINUE) return;
      IntPtr errorP = MqBufferGetContext (buf);
      MqSException ex = new MqSException(
	MqErrorGetNum(errorP), MqErrorGetCode(errorP), Marshal.PtrToStringAnsi(MqErrorGetText(errorP))
      );
      MqErrorReset(errorP);
      throw ex;
     }

    // PUBLIC

    /// \api #MqBufferGetType
    public char Type { get {
      return MqBufferGetType(buf);
    }}

    /// \api #MqBufferGetY
    public byte Y { get {
      byte outV;
      ErrorBufToCsWithCheck(MqBufferGetY(buf, out outV));
      return outV;
    }}

    /// \api #MqBufferGetO
    public bool O { get {
      MQ_BOL outV;
      ErrorBufToCsWithCheck(MqBufferGetO(buf, out outV));
      return (outV == MQ_BOL.MQ_YES ? true : false);
    }}

    /// \api #MqBufferGetS
    public short S { get {
      short outV;
      ErrorBufToCsWithCheck(MqBufferGetS(buf, out outV));
      return outV;
    }}

    /// \api #MqBufferGetI
    public int I { get {
      int outV;
      ErrorBufToCsWithCheck(MqBufferGetI(buf, out outV));
      return outV;
    }}

    /// \api #MqBufferGetF
    public float F { get {
      float outV;
      ErrorBufToCsWithCheck(MqBufferGetF(buf, out outV));
      return outV;
    }}

    /// \api #MqBufferGetW
    public long W { get {
      long outV;
      ErrorBufToCsWithCheck(MqBufferGetW(buf, out outV));
      return outV;
    }}

    /// \api #MqBufferGetD
    public double D { get {
      double outV;
      ErrorBufToCsWithCheck(MqBufferGetD(buf, out outV));
      return outV;
    }}

    /// \api #MqBufferGetB
    public byte[] B { get {
      IntPtr outV;
      int size;
      ErrorBufToCsWithCheck(MqBufferGetB(buf, out outV, out size));
      byte[] outB = new byte[size];
      Marshal.Copy(outV,outB,0,size);
      return outB;
    }}

    /// \api #MqBufferGetC
    public string C { get {
      IntPtr outV;
      ErrorBufToCsWithCheck(MqBufferGetC(buf, out outV));
      return Marshal.PtrToStringAnsi(outV);
    }}

/// \} Mq_Buffer_Cs_API

  } // END - class "MqBufferS"
} // END - namespace "csmsgque"

