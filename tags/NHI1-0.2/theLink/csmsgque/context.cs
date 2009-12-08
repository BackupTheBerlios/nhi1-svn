/**
 *  \file       theLink/csmsgque/context.cs
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
/*                                context                                    */
/*                                                                           */
/*****************************************************************************/

using System;
using System.Runtime.InteropServices;

namespace csmsgque {

  public partial class MqS
  {

    private static void ProcessExit (int num)
    {
      System.GC.Collect();
      Environment.Exit (num);
    }

    private static void ThreadExit (int num)
    {
      System.GC.Collect();
    }

  } // END - class "MqS"
} // END - namespace "csmsgque"






