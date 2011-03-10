REM
REM  \file       theLink/example/vb/Filter6.vb
REM  \brief      \$Id: Filter6.vb 530 2011-03-09 18:03:38Z aotto1968 $
REM  
REM  (C) 2009 - NHI - #1 - Project - Group
REM  
REM  \version    \$Rev: 530 $
REM  \author     EMail: aotto1968 at users.berlios.de
REM  \attention  this software has GPL permissions to copy
REM              please contact AUTHORS for additional information
REM

Imports System
Imports System.IO
Imports csmsgque
Imports System.Collections.Generic

Public Module example
  Private Class Filter6
    Inherits MqS
    Implements IServerSetup
    Implements IEvent
    Implements IService

    Dim FN As String = Nothing

    ' constructor
    Public Sub New(ByVal tmpl As MqS)
      MyBase.New(tmpl)
    End Sub

    Private Sub LOGF()
      Dim ftr As Filter6 = CType(ServiceGetFilter(), Filter6)
      If (ftr.LinkGetTargetIdent() = "transFilter") Then
        ReadForward(ftr)
      Else
        FN = ReadC()
      End If
      SendRETURN()
    End Sub

    Private Sub EXI2()
      [Exit]()
    End Sub

    Private Sub WRIT()
      Dim master As Filter6 = CType(ServiceGetFilter(), Filter6)
      Dim FH As StreamWriter = File.AppendText(master.FN)
      FH.WriteLine(ReadC())
      FH.Flush()
      FH.Close()
      SendRETURN()
    End Sub

    Private Sub ErrorWrite()
      Dim FH As StreamWriter = File.AppendText(FN)
      FH.WriteLine("ERROR: " + ErrorGetText())
      FH.Flush()
      FH.Close()
      ErrorReset()
    End Sub

    Private Sub ServerSetup() Implements IServerSetup.ServerSetup
      Dim ftr As Filter6 = CType(ServiceGetFilter(), Filter6)
      
      ServiceCreate("EXIT", AddressOf EXI2)
      ServiceCreate("LOGF", AddressOf LOGF)
      ServiceCreate("+ALL", Me)
      ServiceStorage("PRNT")
      ftr.ServiceCreate("WRIT", AddressOf ftr.WRIT)
      ftr.ServiceProxy("+TRT")
    End Sub

    Private Sub EventF() Implements IEvent.Event
      If StorageCount() = 0 Then
        ErrorSetCONTINUE()
      Else
        Dim Id As Long = 0L
        Dim ftr As MqS = ServiceGetFilter()
        Try
          ftr.LinkConnect()
          Id = StorageSelect()
          ReadForward(ftr)
        Catch ex As Exception
          ErrorSet(ex)
          If (ErrorIsEXIT()) Then
            ErrorReset()
            Return
          Else
            ErrorWrite()
          End If
        End Try
        StorageDelete(Id)
      End If
    End Sub

    Private Sub Service(ByVal ctx As csmsgque.MqS) Implements IService.Service
      StorageInsert()
      SendRETURN()
    End Sub
  End Class

  Sub Main(ByVal args() As String)
    Dim srv As Filter6 = MqFactoryS(Of Filter6).Add("transFilter").[New]()
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
