REM
REM  \file       theLink/example/vb/Filter4.vb
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
Imports System.IO
Imports csmsgque
Imports System.Collections.Generic

Public Module example
  Private Class Filter4
    Inherits MqS
    Implements IServerSetup
    Implements IEvent
    Implements IService

    Dim itms As New Queue(Of MqDumpS)
    Dim FH As StreamWriter = Nothing

    ' constructor
    Public Sub New(ByVal tmpl As MqS)
      MyBase.New(tmpl)
    End Sub

    Private Sub LOGF()
      Dim ftr As Filter4 = CType(ServiceGetFilter(), Filter4)
      If (ftr.LinkGetTargetIdent() = "transFilter") Then
        ReadForward(ftr)
      Else
        FH = File.AppendText(ReadC())
      End If
      SendRETURN()
    End Sub

    Private Sub EXI2()
      [Exit]()
    End Sub

    Private Sub WRIT()
      Dim master As Filter4 = CType(ServiceGetFilter(), Filter4)
      master.FH.WriteLine(ReadC())
      master.FH.Flush()
      SendRETURN()
    End Sub

    Private Sub ErrorWrite()
      REM LogC("ErrorWrite", 1, "111111111111\n")
      FH.WriteLine("ERROR: " + ErrorGetText())
      FH.Flush()
      ErrorReset()
    End Sub

    Private Sub ServerSetup() Implements IServerSetup.ServerSetup
      Dim ftr As Filter4 = CType(ServiceGetFilter(), Filter4)
      ServiceCreate("EXIT", AddressOf EXI2)
      ServiceCreate("LOGF", AddressOf LOGF)
      ServiceCreate("+ALL", Me)
      ftr.ServiceCreate("WRIT", AddressOf ftr.WRIT)
      ftr.ServiceProxy("+TRT")
    End Sub

    Private Sub EventF() Implements IEvent.Event
      If (itms.Count > 0) Then
        Dim dump As MqDumpS = itms.Peek()
        Dim ftr As Filter4 = CType(ServiceGetFilter(), Filter4)
        Try
          ftr.LinkConnect()
          ReadForward(ftr, dump)
        Catch ex As Exception
          ErrorSet(ex)
          If (ErrorIsEXIT()) Then
            ErrorReset()
            Return
          Else
            ErrorWrite()
          End If
        End Try
        itms.Dequeue()
      Else
        ErrorSetCONTINUE()
      End If
    End Sub

    Private Sub Service(ByVal ctx As csmsgque.MqS) Implements IService.Service
      itms.Enqueue(ReadDUMP())
      SendRETURN()
    End Sub
  End Class

  Sub Main(ByVal args() As String)
    MqFactoryS(Of Filter4).Default("transFilter")
    Dim srv As Filter4 = New Filter4(Nothing)
    Try
      srv.ConfigSetIgnoreExit(True)
      srv.LinkCreate(args)
      srv.ProcessEvent(MqS.WAIT.FOREVER)
    Catch ex As Exception
      srv.ErrorSet(ex)
    Finally
      srv.Exit()
    End Try
  End Sub
End Module
