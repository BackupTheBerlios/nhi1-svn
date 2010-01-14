REM
REM  \file       theLink/example/vb/manfilter.vb
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
  Private Class manfilter
    Inherits MqS
    Implements IFactory

    Public Sub FilterFTR()
      Dim ftr As MqS = ServiceGetFilter()
      ftr.SendSTART()
      While ReadItemExists()
        ftr.SendC("<" + ReadC() + ">")
      End While
      ftr.SendEND_AND_WAIT("+FTR")
      SendRETURN()
    End Sub

    Public Function [Call]() As csmsgque.MqS Implements IFactory.Factory
      Return New manfilter()
    End Function
  End Class

  Sub Main(ByVal args() As String)
    Dim srv As New manfilter()
    Try
      srv.ConfigSetName("ManFilter")
      srv.ConfigSetIsServer(True)
      srv.LinkCreate(args)
      srv.ServiceCreate("+FTR", AddressOf srv.FilterFTR)
      srv.ServiceProxy("+EOF")
      srv.ProcessEvent(MqS.WAIT.FOREVER)
    Catch ex As Exception
      srv.ErrorSet(ex)
    Finally
      srv.Exit()
    End Try
  End Sub
End Module
