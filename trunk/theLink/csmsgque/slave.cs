/**
 *  \file       theLink/csmsgque/slave.cs
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
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

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSlaveWorker")]
    private  static extern MqErrorE MqSlaveWorker([In]IntPtr context, [In]int id, [In,Out]ref IntPtr argv);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSlaveCreate")]
    private  static extern MqErrorE MqSlaveCreate([In]IntPtr context, [In]int id, [In]IntPtr slaveP);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSlaveDelete")]
    private  static extern MqErrorE MqSlaveDelete([In]IntPtr context, [In]int id);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSlaveGet")]
    private  static extern IntPtr MqSlaveGet([In]IntPtr context, [In]int id);

    // PUBLIC

    /// \brief constructor used to create a \e worker object
    public void SlaveWorker(int master_id, params string[] argv) {

      // fill the argv/alfa
      IntPtr largv = IntPtr.Zero;
      if (argv.Length != 0) {
	largv = MqBufferLCreate(argv.Length);
	foreach (string a in argv) {
	  MqBufferLAppendC(largv, a);
	}
      }

      ErrorMqToCsWithCheck (MqSlaveWorker(context, master_id, ref largv));
    }

    /// \api #MqSlaveCreate
    public void SlaveCreate(int id, MqS slave) {
      ErrorMqToCsWithCheck(MqSlaveCreate(context, id, slave.context));
    }

    /// \api #MqSlaveDelete
    public void SlaveDelete(int id) {
      ErrorMqToCsWithCheck(MqSlaveDelete(context, id));
    }

    /// \api #MqSlaveGet
    public MqS SlaveGet(int id) {
      IntPtr slaveP = MqSlaveGet (context, id);
      return (slaveP == IntPtr.Zero ? null : GetSelf(slaveP));
    }

  } // END - class "MqS"
} // END - namespace "csmsgque"




