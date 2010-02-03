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
    Implements IFactory
    Implements IServerSetup

    ' service definition
    Private Sub Filter()
      Dim ftr As MqS = ServiceGetFilter()
      ftr.SendSTART()
      ftr.SendBDY(ReadBDY)
      If ServiceIsTransaction() Then
        ftr.SendEND_AND_WAIT(ServiceGetToken())
        SendSTART()
        SendBDY(ftr.ReadBDY())
      Else
        ftr.SendEND(ServiceGetToken())
      End If
      SendRETURN()
    End Sub

    Private Sub ServerSetup() Implements IServerSetup.ServerSetup
      Dim ftr As MqS = ServiceGetFilter()
      ServiceCreate("+ALL", AddressOf Filter)
      ftr.ServiceCreate("+ALL", AddressOf Filter)
    End Sub

    Private Function Factory() As csmsgque.MqS Implements IFactory.Factory
      Return New Filter3()
    End Function
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
