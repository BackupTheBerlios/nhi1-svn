REM
REM  \file       theLink/example/vb/MyServer.vb
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
  Private Class MyServer
    Inherits MqS
    Implements IServerSetup
    Implements IFactory

    ' service definition
    Public Function Factory() As csmsgque.MqS Implements csmsgque.IFactory.Call
      Return New MyServer()
    End Function

    ' service to serve all incomming requests for token "HLWO"
    Private Sub MyFirstService()
      SendSTART()
      SendC("Hello World")
      SendRETURN()
    End Sub

    ' define a service as link between the token "HLWO" and the callback "MyFirstService"
    Public Sub ServerSetup() Implements csmsgque.IServerSetup.Call
      ServiceCreate("HLWO", AddressOf MyFirstService)
    End Sub
  End Class

  Sub Main(ByVal args() As String)
    Dim srv As New MyServer()
    Try
      srv.ConfigSetName("MyServer")
      srv.LinkCreate(args)
      srv.ProcessEvent(MqS.WAIT.FOREVER)
    Catch ex As Exception
      srv.ErrorSet(ex)
    Finally
      srv.Exit()
    End Try
  End Sub
End Module
