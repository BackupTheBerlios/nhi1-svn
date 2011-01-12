REM
REM  \file       theLink/example/vb/Filter5.vb
REM  \brief      \$Id$
REM  
REM  (C) 2011 - NHI - #1 - Project - Group
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
  Private Class Filter5
    Inherits MqS
    Implements IFactory

    Private data As New List(Of List(Of String))

    ' service definition
    Public Sub FilterEOF()
      Dim ftr As MqS = ServiceGetFilter()
      For Each d As List(Of String) In data
        ftr.SendSTART()
        For Each s As String In d
          ftr.SendC(s)
        Next s
        ftr.SendEND_AND_WAIT("+FTR")
      Next d
      ftr.SendSTART()
      ftr.SendEND_AND_WAIT("+EOF")
      SendRETURN()
    End Sub

    ' service definition
    Public Sub FilterFTR()
      Dim d As New List(Of String)
      While ReadItemExists()
        d.Add("<" + ReadC() + ">")
      End While
      data.Add(d)
      SendRETURN()
    End Sub

    Public Function Factory() As csmsgque.MqS Implements IFactory.Factory
      Return New Filter5()
    End Function

  End Class

  Sub Main(ByVal args() As String)
    Dim srv As New Filter5()
    Try
      srv.ConfigSetName("filter")
      srv.ConfigSetIsServer(True)
      srv.LinkCreate(args)
      srv.ServiceCreate("+FTR", AddressOf srv.FilterFTR)
      srv.ServiceCreate("+EOF", AddressOf srv.FilterEOF)
      srv.ProcessEvent(MqS.WAIT.FOREVER)
    Catch ex As Exception
      srv.ErrorSet(ex)
    Finally
      srv.Exit()
    End Try
  End Sub
End Module
