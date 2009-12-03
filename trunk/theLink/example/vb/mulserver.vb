REM
REM  \file       example/vb/mulserver.vb
REM  \brief      \$Id: mulserver.vb 507 2009-11-28 15:18:46Z dev1usr $
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
  Private Class mulserver
    Inherits MqS
    Implements IServerSetup
    Implements IFactory

    Public Function Factory() As csmsgque.MqS Implements csmsgque.IFactory.Call
      Return New mulserver()
    End Function

    Private Sub MMUL()
      SendSTART()
      SendD(ReadD() * ReadD())
      SendRETURN()
    End Sub

    Public Sub ServerSetup() Implements csmsgque.IServerSetup.Call
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
