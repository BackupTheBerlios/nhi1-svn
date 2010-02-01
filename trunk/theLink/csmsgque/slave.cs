/**
 *  \file       theLink/csmsgque/slave.cs
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
/*                                   read                                    */
/*                                                                           */
/*****************************************************************************/

using System;
using System.Runtime.InteropServices;

namespace csmsgque {

  public partial class MqS
  {

/// \defgroup Mq_Slave_Cs_API Mq_Slave_Cs_API
/// \ingroup Mq_Cs_API
/// \brief \copybrief Mq_Slave_C_API
/// \details \copydetails Mq_Slave_C_API
/// \{

    // PRIVAT

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSlaveWorker")]
    private  static extern MqErrorE MqSlaveWorker([In]IntPtr context, [In]int id, [In,Out]ref IntPtr argv);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSlaveCreate")]
    private  static extern MqErrorE MqSlaveCreate([In]IntPtr context, [In]int id, [In]IntPtr slaveP);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSlaveDelete")]
    private  static extern MqErrorE MqSlaveDelete([In]IntPtr context, [In]int id);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSlaveGet")]
    private  static extern IntPtr MqSlaveGet([In]IntPtr context, [In]int id);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSlaveIs")]
    private static extern MQ_BOL MqSlaveIs([In]IntPtr context);
    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSlaveGetMaster")]
    private static extern IntPtr MqSlaveGetMaster([In]IntPtr context);

    // PUBLIC

    /// \api MqSlaveWorker
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

    /// \api #MqSlaveIs
    public bool	    SlaveIs()	    { 
      return MqSlaveIs(context) == MQ_BOL.MQ_YES; 
    }

    /// \api #MqSlaveGetMaster
    public MqS	    SlaveGetMaster()	    {
      IntPtr masterP = MqSlaveGetMaster(context);
      return (masterP == IntPtr.Zero ? null : GetSelf(masterP));
    }

/// \} Mq_Slave_Cs_API

  } // END - class "MqS"
} // END - namespace "csmsgque"

