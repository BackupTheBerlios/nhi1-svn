REM
REM  \file       example/vb/testclient.vb
REM  \brief      \$Id: testclient.vb 507 2009-11-28 15:18:46Z dev1usr $
REM  
REM  (C) 2009 - NHI - #1 - Project - Group
REM  
REM  \version    \$Rev: 507 $
REM  \author     EMail: aotto1968 at users.sourceforge.net
REM  \attention  this software has GPL permissions to copy
REM              please contact AUTHORS for additional information
REM

Imports System
Imports System.IO
Imports System.Text
Imports System.Collections.Generic
Imports System.Reflection
Imports csmsgque

Public Module example

  Private NotInheritable Class testclient
    Inherits MqS

    Public Function GetV() As StringBuilder
      Dim RET As New StringBuilder()
      SendSTART()
      SendEND_AND_WAIT("GTCX", -1)
      RET.Append(ConfigGetName())
      RET.Append("+")
      RET.Append(ReadC())
      RET.Append(ReadC())
      RET.Append(ReadC())
      RET.Append(ReadC())
      RET.Append(ReadC())
      RET.Append(ReadC())
      Return RET
    End Function
  End Class

  Sub Main(ByVal args() As String)
    Dim dirname As String = New FileInfo(Assembly.GetEntryAssembly().Location).DirectoryName
    Dim server As String = Path.Combine(dirname, "testserver.exe")
    Dim LIST As New List(Of String)
    LIST.Add("--name")
    LIST.Add("c1")
    LIST.Add("--srvname")
    LIST.Add("s1")
    LIST.AddRange(args)
    ' setup the clients
    Dim c0 As New testclient()
    Dim c00 As New testclient()
    Dim c01 As New testclient()
    Dim c000 As New testclient()
    Dim c1 As New testclient()
    Dim c10 As New testclient()
    Dim c100 As New testclient()
    Dim c101 As New testclient()
    Try
      ' create the link
      If Type.GetType("Mono.Runtime") IsNot Nothing Then
        c0.LinkCreate("--name", "c0", "--debug", System.Environment.GetEnvironmentVariable("TS_DEBUG"), _
           "@", "mono", server, "--name", "s0")
      Else
        c0.LinkCreate("--name", "c0", "--debug", System.Environment.GetEnvironmentVariable("TS_DEBUG"), _
           "@", server, "--name", "s0")
      End If
      c00.LinkCreateChild(c0, "--name", "c00", "--srvname", "s00")
      c01.LinkCreateChild(c0, "--name", "c01", "--srvname", "s01")
      c000.LinkCreateChild(c00, "--name", "c000", "--srvname", "s000")
      c1.LinkCreate(LIST.ToArray())
      c10.LinkCreateChild(c1, "--name", "c10", "--srvname", "s10")
      c100.LinkCreateChild(c10, "--name", "c100", "--srvname", "s100")
      c101.LinkCreateChild(c10, "--name", "c101", "--srvname", "s101")
      ' do the tests
      Console.WriteLine(c0.GetV())
      Console.WriteLine(c00.GetV())
      Console.WriteLine(c01.GetV())
      Console.WriteLine(c000.GetV())
      Console.WriteLine(c1.GetV())
      Console.WriteLine(c10.GetV())
      Console.WriteLine(c100.GetV())
      Console.WriteLine(c101.GetV())
    Catch
    Finally
      ' do the cleanup
      c0.LinkDelete()
      c1.LinkDelete()
    End Try
  End Sub

End Module
