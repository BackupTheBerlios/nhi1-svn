/**
 *  \file       theLink/csmsgque/misc.cs
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
/*                                   misc                                    */
/*                                                                           */
/*****************************************************************************/

using System;
using System.Runtime.InteropServices;

namespace csmsgque {

  public partial class MqS
  {
    internal struct ProcData {
      public Callback	callC;
      public IService	callI;

      public ProcData(Callback val) {
	callC = val;
	callI = null;
      }

      public ProcData(IService val) {
	callC = null;
	callI = val;
      }
    }

    private static MqErrorE ProcCall (IntPtr context, IntPtr data)
    {
      ProcData dataC = (ProcData)GCHandle.FromIntPtr(data).Target;

      // call the function
      try {
	if (dataC.callC != null) {
	  dataC.callC();
	} else {
	  dataC.callI.Service(GetSelf(context));
	}
      } catch (Exception ex) {

	return MqErrorSet2 (context, ex);
      }

      // everything is OK
      return MqErrorGetCode(context);
    }

    private static void ProcFree (IntPtr context, ref IntPtr data) {
      GCHandle.FromIntPtr(data).Free();
      data = IntPtr.Zero;
    }

  } // END - class "MqS"
} // END - namespace "csmsgque"


