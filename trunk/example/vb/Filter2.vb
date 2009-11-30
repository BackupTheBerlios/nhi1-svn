REM
REM  \file       example/vb/Filter2.vb
REM  \brief      \$Id: Filter2.vb 507 2009-11-28 15:18:46Z dev1usr $
REM  
REM  (C) 2009 - NHI - #1 - Project - Group
REM  
REM  \version    \$Rev: 507 $
REM  \author     EMail: aotto1968 at users.sourceforge.net
REM  \attention  this software has GPL permissions to copy
REM              please contact AUTHORS for additional information
REM


Imports System
Imports csmsgque

Public Module example
  Private Class Filter2
    Inherits MqS
    Implements IFilterFTR

    Private data As New List(Of List(Of String))

    ' service definition
    Public Sub FilterFTR() Implements csmsgque.IFilterFTR.Call
      Throw New ApplicationException("my error")
    End Sub
  End Class

  Sub Main(ByVal args() As String)
    Dim srv As New Filter2()
    Try
      srv.ConfigSetName("filter")
      srv.LinkCreate(args)
      srv.ProcessEvent(MqS.WAIT.FOREVER)
    Catch ex As Exception
      srv.ErrorSet(ex)
    Finally
      srv.Exit()
    End Try
  End Sub
End Module

