REM
REM  \file       theLink/example/vb/Filter2.vb
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

Public Module example
  Private Class Filter2
    Inherits MqS
    Implements IFactory

    Private data As New List(Of List(Of String))

    ' service definition
    Public Sub FilterFTR()
      Throw New ApplicationException("my error")
    End Sub

    Public Function [Call]() As csmsgque.MqS Implements csmsgque.IFactory.Call
      Return New Filter2()
    End Function
  End Class

  Sub Main(ByVal args() As String)
    Dim srv As New Filter2()
    Try
      srv.ConfigSetName("filter")
      srv.ConfigSetIsServer(True)
      srv.LinkCreate(args)
      srv.ServiceCreate("+FTR", AddressOf srv.FilterFTR)
      srv.ServiceProxy("+EOF")
      srv.ProcessEvent(MqS.WAIT.FOREVER)
    Catch ex As Exception
      srv.ErrorSet(ex)
    Finally
      srv.Exit()
    End Try
  End Sub
End Module

