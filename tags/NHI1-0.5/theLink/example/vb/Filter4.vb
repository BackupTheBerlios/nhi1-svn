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
    Implements IFactory
    Implements IServerSetup
    Implements IEvent
    Implements IService

    Structure FilterItmS
      Public token As String
      Public isTransaction As Boolean
      Public bdy As Byte()
    End Structure

    Dim itms As New Queue(Of FilterItmS)
    Dim FH As StreamWriter = Nothing

    Private Function Factory() As csmsgque.MqS Implements IFactory.Factory
      Return New Filter4()
    End Function

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

    Private Sub LOGF()
      Dim ftr As Filter4 = CType(ServiceGetFilter(), Filter4)
      If (ftr.LinkGetTargetIdent() = "transFilter") Then
        ftr.SendSTART()
        ftr.SendC(ReadC())
        ftr.SendEND_AND_WAIT("LOGF")
      Else
        ftr.FH = File.AppendText(ReadC())
      End If
      SendRETURN()
    End Sub

    Private Sub EXI2()
      Me.Exit()
    End Sub

    Private Sub WRIT()
      FH.WriteLine(ReadC())
      FH.Flush()
      SendRETURN()
    End Sub

    Private Sub ErrorWrite()
      FH.WriteLine("ERROR: " + ErrorGetText())
      FH.Flush()
      ErrorReset()
    End Sub

    Private Sub ServerSetup() Implements IServerSetup.ServerSetup
      Dim ftr As Filter4 = CType(ServiceGetFilter(), Filter4)
      ServiceCreate("+ALL", Me)
      ServiceCreate("EXIT", AddressOf EXI2)
      ServiceCreate("LOGF", AddressOf LOGF)
      ftr.ServiceCreate("WRIT", AddressOf ftr.WRIT)
    End Sub

    Private Sub EventF() Implements IEvent.Event
      If (itms.Count > 0) Then
        Dim it As FilterItmS = itms.Peek()
        Dim ftr As Filter4 = CType(ServiceGetFilter(), Filter4)
        Try
          ftr.LinkConnect()
          ftr.SendSTART()
          ftr.SendBDY(it.bdy)
          If (it.isTransaction) Then
            ftr.SendEND_AND_WAIT(it.token)
          Else
            ftr.SendEND(it.token)
          End If
        Catch ex As Exception
          ftr.ErrorSet(ex)
          If (ftr.ErrorIsEXIT()) Then
            ftr.ErrorReset()
            Return
          Else
            ftr.ErrorWrite()
          End If
        End Try
        itms.Dequeue()
      Else
        ErrorSetCONTINUE()
      End If
    End Sub

    Private Sub Service(ByVal ctx As csmsgque.MqS) Implements IService.Service
      Dim it As FilterItmS
      it.token = ServiceGetToken()
      it.isTransaction = ServiceIsTransaction()
      it.bdy = ReadBDY()
      itms.Enqueue(it)
      SendRETURN()
    End Sub
  End Class

  Sub Main(ByVal args() As String)
    Dim srv As New Filter4()
    Try
      srv.ConfigSetIgnoreExit(True)
      srv.ConfigSetName("Filter4")
      srv.ConfigSetIdent("transFilter")
      srv.LinkCreate(args)
      srv.ProcessEvent(MqS.WAIT.FOREVER)
    Catch ex As Exception
      srv.ErrorSet(ex)
    Finally
      srv.Exit()
    End Try
  End Sub
End Module
