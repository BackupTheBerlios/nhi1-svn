REM
REM  \file       theLink/example/vb/testserver.vb
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
  Private Class testserver
    Inherits MqS
    Implements IServerSetup
    
    ' constructor
    Public Sub New(ByVal tmpl As MqS)
      MyBase.New(tmpl)
    End Sub

    Private Sub GTCX()
      SendSTART()
      SendI(LinkGetCtxId())
      SendC("+")
      If LinkIsParent() Then
        SendI(-1)
      Else
        SendI(LinkGetParent().LinkGetCtxId())
      End If
      SendC("+")
      SendC(ConfigGetName())
      SendC(":")
      SendRETURN()
    End Sub

    Private Sub ServerSetup() Implements IServerSetup.ServerSetup
      ServiceCreate("GTCX", AddressOf GTCX)
    End Sub
  End Class

  Sub Main(ByVal args() As String)
    Dim srv As testserver = MqFactoryS(Of testserver).Add().[New]()
    Try
      srv.LinkCreate(args)
      srv.ProcessEvent(MqS.WAIT.FOREVER)
    Catch ex As Exception
      srv.ErrorSet(ex)
    Finally
      srv.Exit()
    End Try
  End Sub
End Module
