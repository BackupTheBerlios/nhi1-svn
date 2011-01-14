/**
 *  \file       theLink/csmsgque/send.cs
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
/*                                   send                                    */
/*                                                                           */
/*****************************************************************************/

using System;
using System.Runtime.InteropServices;

namespace csmsgque {

public partial class MqS
{

/// \defgroup Mq_Send_Cs_API Mq_Send_Cs_API
/// \ingroup Mq_Cs_API
/// \brief \copybrief Mq_Send_C_API
/// \details \copydetails Mq_Send_C_API
/// \{

  // PRIVAT

  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendSTART")]
  private static extern MqErrorE MqSendSTART(IntPtr context);
  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendEND")]
  private static extern MqErrorE MqSendEND(IntPtr context, string token);
  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendEND_AND_WAIT")]
  private static extern MqErrorE MqSendEND_AND_WAIT(IntPtr context, string token, long timeout);
  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendEND_AND_CALLBACK")]
  private static extern MqErrorE MqSendEND_AND_CALLBACK(IntPtr context, string token, 
	    MqTokenF callback, IntPtr data, MqTokenDataFreeF datafreeF);
  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendRETURN")]
  private static extern MqErrorE MqSendRETURN(IntPtr context);
  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendERROR")]
  private static extern MqErrorE MqSendERROR(IntPtr context);

  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendY")]
  private static extern MqErrorE MqSendY(IntPtr context, byte val);
  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendO")]
  private static extern MqErrorE MqSendO(IntPtr context, byte val);
  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendS")]
  private static extern MqErrorE MqSendS(IntPtr context, short val);
  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendI")]
  private static extern MqErrorE MqSendI(IntPtr context, int val);
  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendF")]
  private static extern MqErrorE MqSendF(IntPtr context, float val);
  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendW")]
  private static extern MqErrorE MqSendW(IntPtr context, long val);
  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendD")]
  private static extern MqErrorE MqSendD(IntPtr context, double val);
  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendB")]
  private static extern MqErrorE MqSendB(IntPtr context, byte[] val, int size);
  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendN")]
  private static extern MqErrorE MqSendN(IntPtr context, byte[] val, int size);
  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendBDY")]
  private static extern MqErrorE MqSendBDY(IntPtr context, byte[] val, int size);
  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendU")]
  private static extern MqErrorE MqSendU(IntPtr context, IntPtr val);
  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendC")]
  private static extern MqErrorE MqSendC(IntPtr context, string val);
  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendL_START")]
  private static extern MqErrorE MqSendL_START(IntPtr context);
  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendL_END")]
  private static extern MqErrorE MqSendL_END(IntPtr context);
  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendT_START")]
  private static extern MqErrorE MqSendT_START(IntPtr context, string token);
  [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSendT_END")]
  private static extern MqErrorE MqSendT_END(IntPtr context);
  

  // PUBLIC

  /// \api #MqSendSTART
  public void SendSTART() {
    ErrorMqToCsWithCheck(MqSendSTART(context));
  }

  /// \api #MqSendEND
  public void SendEND(string token) {
    ErrorMqToCsWithCheck(MqSendEND(context, token));
  }

  /// \api #MqSendEND_AND_WAIT
  public void SendEND_AND_WAIT(string token, long timeout) {
    ErrorMqToCsWithCheck(MqSendEND_AND_WAIT(context, token, timeout));
  }

  /// \api #MqSendEND_AND_WAIT
  public void SendEND_AND_WAIT(string token) {
    ErrorMqToCsWithCheck(MqSendEND_AND_WAIT(context, token, -1));
  }

  /// \api #MqSendEND_AND_CALLBACK
  public void SendEND_AND_CALLBACK(string token, Callback call) {
    ErrorMqToCsWithCheck(MqSendEND_AND_CALLBACK(context, token, fProcCall,
    (IntPtr) GCHandle.Alloc(new ProcData(call)), fProcFree));
  }

  /// \api #MqSendEND_AND_CALLBACK
  public void SendEND_AND_CALLBACK(string token, IService call) {
    ErrorMqToCsWithCheck(MqSendEND_AND_CALLBACK(context, token, fProcCall,
    (IntPtr) GCHandle.Alloc(new ProcData(call)), fProcFree));
  }

  /// \api #MqSendRETURN
  public void SendRETURN() {
    ErrorMqToCsWithCheck(MqSendRETURN(context));
  }

  /// \api #MqSendERROR
  public void SendERROR() {
    ErrorMqToCsWithCheck(MqSendERROR(context));
  }

  /// \api #MqSendY
  public void SendY(byte val) {
    ErrorMqToCsWithCheck(MqSendY(context, val));
  }

  /// \api #MqSendO
  public void SendO(bool val) {
    ErrorMqToCsWithCheck(MqSendO(context, (byte) (val ? MQ_BOL.MQ_YES : MQ_BOL.MQ_NO)));
  }

  /// \api #MqSendS
  public void SendS(short val) {
    ErrorMqToCsWithCheck(MqSendS(context, val));
  }

  /// \api #MqSendI
  public void SendI(int val) {
    ErrorMqToCsWithCheck(MqSendI(context, val));
  }

  /// \api #MqSendF
  public void SendF(float val) {
    ErrorMqToCsWithCheck(MqSendF(context, val));
  }

  /// \api #MqSendW
  public void SendW(long val) {
    ErrorMqToCsWithCheck(MqSendW(context, val));
  }

  /// \api #MqSendD
  public void SendD(double val) {
    ErrorMqToCsWithCheck(MqSendD(context, val));
  }

  /// \api #MqSendB
  public void SendB(byte[] val) {
    ErrorMqToCsWithCheck(MqSendB(context, val, val.Length));
  }

  /// \api #MqSendN
  public void SendN(byte[] val) {
    ErrorMqToCsWithCheck(MqSendN(context, val, val.Length));
  }

  /// \api #MqSendBDY
  public void SendBDY(byte[] val) {
    ErrorMqToCsWithCheck (MqSendBDY(context, val, val.Length));
  }

  /// \api #MqSendU
  public void SendU(MqBufferS buffer) {
    ErrorMqToCsWithCheck(MqSendU(context, buffer.buf));
  }

  /// \api #MqSendC
  public void SendC(string val) {
    ErrorMqToCsWithCheck(MqSendC(context, val));
  }

  /// \api #MqSendL_START
  public void SendL_START() {
    ErrorMqToCsWithCheck(MqSendL_START(context));
  }

  /// \api #MqSendL_END
  public void SendL_END() {
    ErrorMqToCsWithCheck(MqSendL_END(context));
  }

  /// \api #MqSendT_START
  public void SendT_START(string token) {
    ErrorMqToCsWithCheck(MqSendT_START(context, token));
  }

  /// \api #MqSendT_END
  public void SendT_END() {
    ErrorMqToCsWithCheck(MqSendT_END(context));
  }

/// \} Mq_Send_Cs_API

// END - Class MqS
}

// END - NameSpace csmsgque
}

