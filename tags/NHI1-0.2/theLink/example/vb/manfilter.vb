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
    Implements IFilterFTR

    Public Sub FilterFTR() Implements csmsgque.IFilterFTR.Call
      SendSTART()
      While ReadItemExists()
        SendC("<" + ReadC() + ">")
      End While
      SendFTR(10)
    End Sub
  End Class

  Sub Main(ByVal args() As String)
    Dim srv As New manfilter()
    Try
      srv.ConfigSetName("ManFilter")
      srv.LinkCreate(args)
      srv.ProcessEvent(MqS.WAIT.FOREVER)
    Catch ex As Exception
      srv.ErrorSet(ex)
    Finally
      srv.Exit()
    End Try
  End Sub
End Module
