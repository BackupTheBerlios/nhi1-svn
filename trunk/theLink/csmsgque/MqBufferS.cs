/**
 *  \file       theLink/csmsgque/MqBufferS.cs
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
    private const string MSGQUE_DLL = MqS.MSGQUE_DLL;

    internal IntPtr buf = IntPtr.Zero;

    // INTERNAL

    internal MqBufferS (IntPtr data) {
      buf = data;
    }

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferDelete")]
    private  static extern IntPtr MqBufferDelete([In,Out] ref IntPtr buf);

    public void Delete () {
      MqBufferDelete(ref buf);
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

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferSetY")]
    private  static extern IntPtr MqBufferSetY([In]IntPtr buf, [In] byte val);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferSetO")]
    private  static extern IntPtr MqBufferSetO([In]IntPtr buf, [In] MQ_BOL val);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferSetS")]
    private  static extern IntPtr MqBufferSetS([In]IntPtr buf, [In] short val);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferSetI")]
    private  static extern IntPtr MqBufferSetI([In]IntPtr buf, [In] int val);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferSetF")]
    private  static extern IntPtr MqBufferSetF([In]IntPtr buf, [In] float val);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferSetW")]
    private  static extern IntPtr MqBufferSetW([In]IntPtr buf, [In] long val);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferSetD")]
    private  static extern IntPtr MqBufferSetD([In]IntPtr buf, [In] double val);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferSetB")]
    private  static extern IntPtr MqBufferSetB([In]IntPtr buf, [In] byte[] val, [In] int size);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferSetC")]
    private  static extern IntPtr MqBufferSetC([In]IntPtr buf, [In] string val);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferAppendC")]
    private  static extern IntPtr MqBufferAppendC([In]IntPtr buf, [In] string val);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqBufferDup")]
    private  static extern IntPtr MqBufferDup([In]IntPtr buf);

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

    public byte Y { 
      /// \api #MqBufferGetY
      get {
	byte outV;
	ErrorBufToCsWithCheck(MqBufferGetY(buf, out outV));
	return outV;
      }
      /// \api #MqBufferSetY
      set {
	MqBufferSetY(buf, value);
      }
    }

    public bool O { 
      /// \api #MqBufferGetO
      get {
	MQ_BOL outV;
	ErrorBufToCsWithCheck(MqBufferGetO(buf, out outV));
	return (outV == MQ_BOL.MQ_YES ? true : false);
      }
      /// \api #MqBufferSetO
      set {
	MqBufferSetO(buf, value == true ? MQ_BOL.MQ_YES : MQ_BOL.MQ_NO);
      }
    }

    public short S { 
      /// \api #MqBufferGetS
      get {
	short outV;
	ErrorBufToCsWithCheck(MqBufferGetS(buf, out outV));
	return outV;
      }
      /// \api #MqBufferSetS
      set {
	MqBufferSetS(buf, value);
      }
    }

    public int I { 
      /// \api #MqBufferGetI
      get {
	int outV;
	ErrorBufToCsWithCheck(MqBufferGetI(buf, out outV));
	return outV;
      }
      /// \api #MqBufferSetI
      set {
	MqBufferSetI(buf, value);
      }
    }

    public float F { 
      /// \api #MqBufferGetF
      get {
	float outV;
	ErrorBufToCsWithCheck(MqBufferGetF(buf, out outV));
	return outV;
      }
      /// \api #MqBufferSetF
      set {
	MqBufferSetF(buf, value);
      }
    }

    public long W { 
      /// \api #MqBufferGetW
      get {
	long outV;
	ErrorBufToCsWithCheck(MqBufferGetW(buf, out outV));
	return outV;
      }
      /// \api #MqBufferSetW
      set {
	MqBufferSetW(buf, value);
      }
    }

    public double D { 
      /// \api #MqBufferGetD
      get {
	double outV;
	ErrorBufToCsWithCheck(MqBufferGetD(buf, out outV));
	return outV;
      }
      /// \api #MqBufferSetD
      set {
	MqBufferSetD(buf, value);
      }
    }

    public byte[] B { 
      /// \api #MqBufferGetB
      get {
	IntPtr outV;
	int size;
	ErrorBufToCsWithCheck(MqBufferGetB(buf, out outV, out size));
	byte[] outB = new byte[size];
	Marshal.Copy(outV,outB,0,size);
	return outB;
      }
      /// \api #MqBufferSetB
      set {
	MqBufferSetB(buf, value, value.Length);
      }
    }

    public string C { 
      /// \api #MqBufferGetC
      get {
	IntPtr outV;
	ErrorBufToCsWithCheck(MqBufferGetC(buf, out outV));
	return Marshal.PtrToStringAnsi(outV);
      }
      /// \api #MqBufferSetC
      set {
	MqBufferSetC(buf, value);
      }
    }

    /// \api #MqBufferDup
    public MqBufferS Dup() {
      return new MqBufferS (MqBufferDup (buf));
    }

    /// \api #MqBufferAppendC
    public MqBufferS AppendC(string value) {
      MqBufferAppendC (buf, value);
      return this;
    }

/// \} Mq_Buffer_Cs_API

  } // END - class "MqBufferS"
} // END - namespace "csmsgque"

