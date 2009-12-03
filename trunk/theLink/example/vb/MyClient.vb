REM
REM  \file       theLink/example/vb/MyClient.vb
REM  \brief      \$Id$
REM  
REM  (C) 2009 - NHI - #1 - Project - Group
REM  
REM  \version    \$Rev$
REM  \author     EMail: aotto1968 at users.sourceforge.net
REM  \attention  this software has GPL permissions to copy
REM              please contact AUTHORS for additional information
REM

Imports System
Imports csmsgque

Public Module example
    Sub Main(ByVal args() As String)
        Dim c As New MqS()
        Try
            c.LinkCreate(args)
	    c.SendSTART()
	    c.sendEND_AND_WAIT("HLWO", 5)
	    Console.WriteLine(c.ReadC())
        Catch ex As Exception
            c.ErrorSet(ex)
        Finally
            c.Exit()
        End Try
    End Sub
End Module
