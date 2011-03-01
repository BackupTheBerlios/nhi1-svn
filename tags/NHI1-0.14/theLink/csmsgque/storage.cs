/**
 *  \file       theLink/csmsgque/storage.cs
 *  \brief      \$Id$
 *  
 *  (C) 2011 - NHI - #1 - Project - Group
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

/// \defgroup Mq_Storage_Cs_API Mq_Storage_Cs_API
/// \ingroup Mq_Cs_API
/// \brief \copybrief Mq_Storage_C_API
/// \details \copydetails Mq_Storage_C_API
/// \{

    // ----------------------------------------------------------------------------

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqStorageOpen")]
    private static extern MqErrorE MqStorageOpen([In]IntPtr context, [In]string storageFile);

    /// \api #MqStorageOpen
    public void StorageOpen(string storageFile) {
      ErrorMqToCsWithCheck(MqStorageOpen(context, storageFile));
    }

    // ----------------------------------------------------------------------------

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqStorageClose")]
    private static extern MqErrorE MqStorageClose([In]IntPtr context);

    /// \api #MqStorageClose
    public void StorageClose() {
      ErrorMqToCsWithCheck(MqStorageClose(context));
    }

    // ----------------------------------------------------------------------------

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqStorageInsert")]
    private static extern MqErrorE MqStorageInsert([In]IntPtr context, out long transLId);

    /// \api #MqStorageInsert
    public long StorageInsert() {
      long transLId;
      ErrorMqToCsWithCheck(MqStorageInsert(context, out transLId));
      return transLId;
    }

    // ----------------------------------------------------------------------------

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqStorageSelect")]
    private static extern MqErrorE MqStorageSelect([In]IntPtr context, [In,Out] ref long transLId);

    /// \api #MqStorageSelect
    public long StorageSelect() {
      long transLId = 0L;
      ErrorMqToCsWithCheck(MqStorageSelect(context, ref transLId));
      return transLId;
    }

    /// \api #MqStorageSelect
    public long StorageSelect(long transLId) {
      ErrorMqToCsWithCheck(MqStorageSelect(context, ref transLId));
      return transLId;
    }

    // ----------------------------------------------------------------------------

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqStorageDelete")]
    private static extern MqErrorE MqStorageDelete([In]IntPtr context, [In] long transLId);

    /// \api #MqStorageDelete
    public void StorageDelete(long transLId) {
      ErrorMqToCsWithCheck(MqStorageDelete(context, transLId));
    }

    // ----------------------------------------------------------------------------

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqStorageCount")]
    private static extern MqErrorE MqStorageCount([In]IntPtr context, out long count);

    /// \api #MqStorageCount
    public long StorageCount() {
      long count;
      ErrorMqToCsWithCheck(MqStorageCount(context, out count));
      return count;
    }

/// \} Mq_Storage_Cs_API

  } // END - class "MqS"
} // END - namespace "csmsgque"

