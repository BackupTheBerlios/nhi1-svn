REM
REM  \file       theLink/example/vb/mulserver.vb
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
  Private Class mulserver
    Inherits MqS
    Implements IServerSetup
    Implements IFactory

    Private Function Factory() As csmsgque.MqS Implements IFactory.Factory
      Return New mulserver()
    End Function

    Private Sub MMUL()
      SendSTART()
      SendD(ReadD() * ReadD())
      SendRETURN()
    End Sub

    Private Sub ServerSetup() Implements IServerSetup.ServerSetup
      ServiceCreate("MMUL", AddressOf MMUL)
    End Sub
  End Class

  Sub Main(ByVal args() As String)
    Dim srv As New mulserver()
    Try
      srv.ConfigSetName("MyMulServer")
      srv.LinkCreate(args)
      srv.ProcessEvent(MqS.WAIT.FOREVER)
    Catch ex As Exception
      srv.ErrorSet(ex)
    Finally
      srv.Exit()
    End Try
  End Sub
End Module
