REM
REM  \file       theLink/example/vb/Filter3.vb
REM  \brief      \$Id$
REM  
REM  (C) 2009 - NHI - #1 - Project - Group
REM  
REM  \version    \$Rev$
REM  \author     EMail: aotto1968 at users.berlios.de
REM  \attention  this software has GPL permissions to copy
REM              please contact AUTHORS for additional information
REM

Imports System
Imports csmsgque
Imports System.Collections.Generic

Public Module example
  Private Class Filter3
    Inherits MqS
    Implements IServerSetup

    ' service definition
    Private Sub FilterFTR() Implements csmsgque.IFilterFTR.Call
      Dim d As New List(Of String)
      While ReadItemExists()
        d.Add("<" + ReadC() + ">")
      End While
      data.Add(d)
    End Sub
  End Class

  Sub Main(ByVal args() As String)
    Dim srv As New Filter3()
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
