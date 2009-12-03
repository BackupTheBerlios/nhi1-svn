REM
REM  \file       example/vb/mulclient.vb
REM  \brief      \$Id: mulclient.vb 507 2009-11-28 15:18:46Z dev1usr $
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
  Sub Main(ByVal args() As String)
    Dim ctx As New MqS()
    Try
      ctx.LinkCreate(args)
      ctx.SendSTART()
      ctx.SendD(3.67)
      ctx.SendD(22.3)
      ctx.SendEND_AND_WAIT("MMUL", 5)
      Console.WriteLine(ctx.ReadD())
    Catch ex As Exception
      ctx.ErrorSet(ex)
    Finally
      ctx.Exit()
    End Try
  End Sub
End Module
