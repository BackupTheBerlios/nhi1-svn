/**
 *  \file       csmsgque/error.cs
 *  \brief      \$Id: error.cs 507 2009-11-28 15:18:46Z dev1usr $
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
/*                                   error                                   */
/*                                                                           */
/*****************************************************************************/

using System;
using System.Runtime.InteropServices;
using System.Runtime.Serialization;

namespace csmsgque {

  /// \brief a C# error class wrapper for the \e MqErrorS struct
  public class MqSException : Exception
  {
    private int		  p_num;
    private MqErrorE	  p_code;
    private string	  p_txt;

    public MqSException() : base("csmsgque ERROR") {
      p_txt = "csmsgque ERROR";
      p_num = -1;
      p_code = 0;
    }

    internal MqSException(int mynum, MqErrorE mycode, string mytxt) : base(mytxt) {
      p_txt = mytxt;
      p_num = mynum;
      p_code = mycode;
    }

    internal MqSException(int mynum, MqErrorE mycode, string mytxt, Exception inner) : base(mytxt, inner) {
      p_txt = mytxt;
      p_num = mynum;
      p_code = mycode;
    }

    public MqSException(SerializationInfo info, StreamingContext context) {
      p_txt   = (String)    info.GetValue("p_txt",  typeof(string));
      p_num   = (int)	    info.GetValue("p_num",  typeof(int));
      p_code  = (MqErrorE)  info.GetValue("p_code", typeof(MqErrorE));
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context){
      info.AddValue("p_txt",  p_txt);
      info.AddValue("p_num",  p_num);
      info.AddValue("p_code", p_code);
      base.GetObjectData (info, context);
    }

    public string   txt	  { get { return p_txt;	  } }
    public int	    num	  { get { return p_num;	  } }
    internal MqErrorE code  { get { return p_code;  } }
  }

  public partial class MqS
  {
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqErrorGetText")]
    private static extern IntPtr MqErrorGetText(IntPtr context);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqErrorGetNum")]
    private static extern int MqErrorGetNum(IntPtr context);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqErrorGetCode")]
    private static extern MqErrorE MqErrorGetCode(IntPtr context);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqErrorReset")]
    private static extern void MqErrorReset(IntPtr context);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqErrorPrint")]
    private static extern void MqErrorPrint(IntPtr context);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqErrorC")]
    private static extern MqErrorE MqErrorC(IntPtr context, string proc, int num, string message);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqErrorSet")]
    private static extern void MqErrorSet(IntPtr context, int num, MqErrorE code, string message);

    private void ErrorMqToCsWithCheck(MqErrorE err) {
      if (err <= MqErrorE.MQ_CONTINUE) return;
      MqSException ex = new MqSException(
	      MqErrorGetNum(context),
	      MqErrorGetCode(context),
	      Marshal.PtrToStringAnsi(MqErrorGetText(context))
      );
      MqErrorReset(context);
      throw ex;
     }

    public void ErrorC(string proc, int num, string message) {
      MqErrorC(context, proc, num, message);
    }

    public void ErrorRaise() {
      ErrorMqToCsWithCheck(MqErrorGetCode(context));
    }

    public void ErrorReset() {
      MqErrorReset(context);
    }

    public void ErrorPrint() {
      MqErrorPrint (context);
    }

    public int ErrorGetNum() {
      return MqErrorGetNum(context);
    }

    public string ErrorGetText() {
      return Marshal.PtrToStringAnsi(MqErrorGetText(context));
    }

    static private MqErrorE MqErrorSet2 (IntPtr context, Exception ex) {
      if (ex is MqSException) {
	MqSException exm = (MqSException) ex;
	MqErrorSet (context, exm.num, exm.code, exm.txt);
      } else {
	MqErrorC(context, "ErrorSet", -1, ex.ToString());
      }
      return MqErrorGetCode (context);
    }

    public void ErrorSet(Exception ex) {
      MqErrorSet2 (context, ex);
    }
  } // END - class "MqS"
} // END - namespace "csmsgque"




