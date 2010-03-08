REM
REM  \file       theLink/tests/vbserver.vb
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

  Private NotInheritable Class Client
    Inherits MqS
    Implements IFactory
    Implements IBgError
    Public i As Integer


    Public Function Factory() As csmsgque.MqS Implements csmsgque.IFactory.Factory
      Return New Client()
    End Function

    Public Sub BgError() Implements csmsgque.IBgError.BgError
      Dim master As MqS = SlaveGetMaster()
      If master IsNot Nothing Then
        master.ErrorC("BGERROR", ErrorGetNum(), ErrorGetText())
        master.SendERROR()
      End If
    End Sub

    Public Overloads Sub LinkCreate(ByVal debug As Integer)
      ConfigSetDebug(debug)
      MyBase.LinkCreate("@", "SELF", "--name", "test-server")
    End Sub

    Public Sub ECOI_CB()
      i = ReadI()
    End Sub

  End Class

  Private NotInheritable Class ClientERR
    Inherits MqS
    Public i As Integer

    Public Overloads Sub LinkCreate(ByVal debug As Integer)
      ConfigSetDebug(debug)
      ConfigSetName("test-client")
      ConfigSetSrvName("test-server")
      MyBase.LinkCreate("@", "SELF")
    End Sub

    Public Sub ECOI_CB()
      i = ReadI()
    End Sub

  End Class

  Private NotInheritable Class ClientERR2
    Inherits MqS
    Public i As Integer

    Public Overloads Sub LinkCreate(ByVal debug As Integer)
      ConfigSetDebug(debug)
      ConfigSetName("cl-err-1")
      MyBase.LinkCreate("@", "DUMMY")
    End Sub

    Public Sub ECOI_CB()
      i = ReadI()
    End Sub

  End Class

  Private NotInheritable Class Server
    Inherits MqS
    Implements IServerSetup
    Implements IServerCleanup
    Implements IFactory
    Private cl(3) As Client
    Private buf As MqBufferS

    Private Function Factory() As csmsgque.MqS Implements IFactory.Factory
      Return New Server()
    End Function

    Private Sub ServerCleanup() Implements IServerCleanup.ServerCleanup
      For i As Integer = 0 To 2
        If cl(i) Is Nothing Then
          Continue For
        End If
        cl(i).LinkDelete()
        cl(i) = Nothing
      Next i
    End Sub

    Private Sub ServerSetup() Implements IServerSetup.ServerSetup
      If SlaveIs() Then
        'add "slave" services here
      Else
        For i As Integer = 0 To 2
          cl(i) = New Client()
          cl(i).ConfigSetName("cl-" + CStr(i))
          cl(i).ConfigSetSrvName("sv-" + CStr(i))
        Next i
        'add "master" services here
        ServiceCreate("ECOU", AddressOf ECOU)
        ServiceCreate("ECOC", AddressOf ECOC)
        ServiceCreate("ECOB", AddressOf ECOB)
        ServiceCreate("ECOO", AddressOf ECOO)
        ServiceCreate("ECOY", AddressOf ECOY)
        ServiceCreate("ECOS", AddressOf ECOS)
        ServiceCreate("ECOI", AddressOf ECOI)
        ServiceCreate("ECOW", AddressOf ECOW)
        ServiceCreate("ECOF", AddressOf ECOF)
        ServiceCreate("ECOD", AddressOf ECOD)
        ServiceCreate("GETU", AddressOf GETU)
        ServiceCreate("SETU", AddressOf SETU)
        ServiceCreate("SLEP", AddressOf SLEP)
        ServiceCreate("USLP", AddressOf USLP)
        ServiceCreate("CSV1", AddressOf CSV1)
        ServiceCreate("BUF1", AddressOf BUF1)
        ServiceCreate("BUF2", AddressOf BUF2)
        ServiceCreate("BUF3", AddressOf BUF3)
        ServiceCreate("ERR2", AddressOf ERR2)
        ServiceCreate("ERR3", AddressOf ERR3)
        ServiceCreate("ERR4", AddressOf ERR4)
        ServiceCreate("ERRT", AddressOf ERRT)
        ServiceCreate("INIT", AddressOf INITX)
        ServiceCreate("SND1", AddressOf SND1)

        ServiceCreate("ECUL", AddressOf ECUL)
        ServiceCreate("LST1", AddressOf LST1)
        ServiceCreate("LST2", AddressOf LST2)
        ServiceCreate("ECOL", AddressOf ECOL)
        ServiceCreate("ECLI", AddressOf ECLI)

        ServiceCreate("ERLS", AddressOf ERLS)
        ServiceCreate("ERLR", AddressOf ERLR)

        ServiceCreate("ECON", AddressOf ECON)
        ServiceCreate("SND2", AddressOf SND2)

        ServiceCreate("GTTO", AddressOf GTTO)
        ServiceCreate("RDUL", AddressOf RDUL)
        ServiceCreate("GTCX", AddressOf GTCX)
        ServiceCreate("REDI", AddressOf REDI)

        ServiceCreate("CNFG", AddressOf CNFG)
        ServiceCreate("MSQT", AddressOf MSQT)
        ServiceCreate("CFG1", AddressOf CFG1)
        ServiceCreate("PRNT", AddressOf PRNT)
        ServiceCreate("TRNS", AddressOf TRNS)
        ServiceCreate("TRN2", AddressOf TRN2)

      End If
    End Sub

    Private Sub TRNS()
      SendSTART()
      SendT_START("TRN2")
      SendI(9876)
      SendT_END()
      SendI(ReadI())
      SendEND_AND_WAIT("ECOI")
      ProcessEvent(MqS.WAIT.ONCE)
      SendSTART()
      SendI(i)
      SendI(j)
      SendRETURN()
    End Sub

    Private Sub TRN2()
      ReadT_START()
      i = ReadI()
      ReadT_END()
      j = ReadI()
      SendRETURN()
    End Sub

    Private Sub PRNT()
      SendSTART()
      SendC(CStr(LinkGetCtxId()) + " - " + ReadC())
      SendEND_AND_WAIT("WRIT")
      SendRETURN()
    End Sub

    Private Sub CFG1()
      Dim cmd As String = ReadC()

      SendSTART()

      Select Case cmd
        Case "Buffersize"
          Dim old As Integer = ConfigGetBuffersize()
          ConfigSetBuffersize(ReadI())
          SendI(ConfigGetBuffersize())
          ConfigSetBuffersize(old)
        Case "Debug"
          Dim old As Integer = ConfigGetDebug()
          ConfigSetDebug(ReadI())
          SendI(ConfigGetDebug())
          ConfigSetDebug(old)
        Case "Timeout"
          Dim old As Long = ConfigGetTimeout()
          ConfigSetTimeout(ReadW())
          SendW(ConfigGetTimeout())
          ConfigSetTimeout(old)
        Case "Name"
          Dim old As String = ConfigGetName()
          ConfigSetName(ReadC())
          SendC(ConfigGetName())
          ConfigSetName(old)
        Case "SrvName"
          Dim old As String = ConfigGetSrvName()
          ConfigSetSrvName(ReadC())
          SendC(ConfigGetSrvName())
          ConfigSetSrvName(old)
        Case "Ident"
          Dim old As String = ConfigGetIdent()
          ConfigSetIdent(ReadC())
          Dim check As Boolean = LinkGetTargetIdent() = ReadC()
          SendSTART()
          SendC(ConfigGetIdent())
          SendO(check)
          ConfigSetIdent(old)
        Case "IsSilent"
          Dim old As Boolean = ConfigGetIsSilent()
          ConfigSetIsSilent(ReadO())
          SendO(ConfigGetIsSilent())
          ConfigSetIsSilent(old)
        Case "IsString"
          Dim old As Boolean = ConfigGetIsString()
          ConfigSetIsString(ReadO())
          SendO(ConfigGetIsString())
          ConfigSetIsString(old)
        Case "IoUds"
          Dim old As String = ConfigGetIoUdsFile()
          ConfigSetIoUds(ReadC())
          SendC(ConfigGetIoUdsFile())
          ConfigSetIoUds(old)
        Case "IoTcp"
          Dim h, p, mh, mp As String
          Dim hv, pv, mhv, mpv As String
          h = ConfigGetIoTcpHost()
          p = ConfigGetIoTcpPort()
          mh = ConfigGetIoTcpMyHost()
          mp = ConfigGetIoTcpMyPort()
          hv = ReadC()
          pv = ReadC()
          mhv = ReadC()
          mpv = ReadC()
          ConfigSetIoTcp(hv, pv, mhv, mpv)
          SendC(ConfigGetIoTcpHost())
          SendC(ConfigGetIoTcpPort())
          SendC(ConfigGetIoTcpMyHost())
          SendC(ConfigGetIoTcpMyPort())
          ConfigSetIoTcp(h, p, mh, mp)
        Case "IoPipe"
          Dim old As Integer = ConfigGetIoPipeSocket()
          ConfigSetIoPipe(ReadI())
          SendI(ConfigGetIoPipeSocket())
          ConfigSetIoPipe(old)
        Case "StartAs"
          Dim old As Integer = ConfigGetStartAs()
          ConfigSetStartAs(ReadI())
          SendI(ConfigGetStartAs())
          ConfigSetStartAs(old)
        Case Else
          ErrorC("CFG1", 1, "invalid command: " + cmd)
      End Select
      SendRETURN()
    End Sub

    Private Sub MSQT()
      SendSTART()
      If ConfigGetDebug() <> 0 Then
        SendC("debug")
        SendI(ConfigGetDebug())
      End If
      If Not ConfigGetIsString() Then SendC("binary")
      If ConfigGetIsSilent() Then SendC("silent")
      SendC("sOc")
      If ConfigGetIsServer() Then SendC("SERVER") Else SendC("CLIENT")
      SendC("pOc")
      If LinkIsParent() Then SendC("PARENT") Else SendC("CHILD")
      SendRETURN()
    End Sub


    Private Sub CNFG()
      SendSTART()
      SendO(ConfigGetIsServer())
      SendO(LinkIsParent())
      SendO(SlaveIs())
      SendO(ConfigGetIsString())
      SendO(ConfigGetIsSilent())
      SendO(LinkIsConnected())
      SendC(ConfigGetName())
      SendI(ConfigGetDebug())
      SendI(LinkGetCtxId())
      SendC(ServiceGetToken())
      SendRETURN()
    End Sub

    Private Sub GTTO()
      SendSTART()
      SendC(ServiceGetToken())
      SendRETURN()
    End Sub

    Private Sub RDUL()
      ReadY()
      ReadS()
      ReadI()
      ReadW()
      ReadU()
    End Sub

    Private Sub GTCX()
      SendSTART()
      SendI(LinkGetCtxId())
      SendRETURN()
    End Sub

    Private Sub REDI()
      ReadI()
    End Sub


    Dim i As Integer = -1
    Dim j As Integer = -1

    Private Sub SetMyInt()
      CType(SlaveGetMaster(), Server).i = ReadI()
    End Sub

    Private Sub SND2()
      Dim s As String = ReadC()
      Dim id As Integer = ReadI()
      Dim cl As MqS = SlaveGet(id)

      SendSTART()

      Select Case s
        Case "CREATE"
          Dim LIST As New List(Of String)
          LIST.Add("--name")
          LIST.Add("wk-cl-" + CStr(id))
          LIST.Add("@")
          LIST.Add("--name")
          LIST.Add("wk-sv-" + CStr(id))
          While ReadItemExists()
            LIST.Add(ReadC())
          End While
          SlaveWorker(id, LIST.ToArray())
        Case "CREATE2"
          Dim c As New Client()
          c.LinkCreate(ConfigGetDebug())
          SlaveCreate(id, c)
        Case "CREATE3"
          Dim c As New ClientERR()
          c.LinkCreate(ConfigGetDebug())
          SlaveCreate(id, c)
        Case "DELETE"
          SlaveDelete(id)
          If SlaveGet(id) Is Nothing Then
            SendC("OK")
          Else
            SendC("ERROR")
          End If
        Case "SEND"
          cl.SendSTART()
          Dim TOK As String = ReadC()
          ReadProxy(cl)
          cl.SendEND(TOK)
        Case "WAIT"
          cl.SendSTART()
          ReadProxy(cl)
          cl.SendEND_AND_WAIT("ECOI", 5)
          SendI(cl.ReadI() + 1)
        Case "CALLBACK"
          cl.SendSTART()
          ReadProxy(cl)
          i = -1
          cl.SendEND_AND_CALLBACK("ECOI", AddressOf CType(cl, Server).SetMyInt)
          cl.ProcessEvent(10, MqS.WAIT.ONCE)
          SendI(i + 1)
        Case "MqSendEND_AND_WAIT"
          Dim TOK As String = ReadC()
          cl.SendSTART()
          While ReadItemExists()
            ReadProxy(cl)
          End While
          cl.SendEND_AND_WAIT(TOK, 5)
          While (cl.ReadItemExists())
            cl.ReadProxy(Me)
          End While
        Case "MqSendEND"
          Dim TOK As String = ReadC()
          cl.SendSTART()
          While ReadItemExists()
            ReadProxy(cl)
          End While
          cl.SendEND(TOK)
          Return
        Case "ERR-1"
          Dim c As New ClientERR2()
          c.LinkCreate(ConfigGetDebug())
        Case "isSlave"
          SendO(cl.SlaveIs())
      End Select
      SendRETURN()
    End Sub

    Private Sub ECON()
      SendSTART()
      SendC(ReadC() + "-" + ConfigGetName())
      SendRETURN()
    End Sub

    Private Sub ERLR()
      SendSTART()
      ReadL_START()
      ReadL_START()
      SendRETURN()
    End Sub

    Private Sub ERLS()
      SendSTART()
      SendL_START()
      SendU(ReadU())
      SendL_START()
      SendU(ReadU())
      SendRETURN()
    End Sub

    Private Sub ECUL()
      SendSTART()
      SendY(ReadY())
      SendS(ReadS())
      SendI(ReadI())
      SendW(ReadW())
      ReadProxy(Me)
      SendRETURN()
    End Sub

    Private Sub LST1()
      SendSTART()
      SendL_END()
      SendRETURN()
    End Sub

    Private Sub LST2()
      SendSTART()
      ReadL_END()
      SendRETURN()
    End Sub

    Private Sub EchoList(ByVal doincr As Boolean)
      Dim buf As MqBufferS
      While ReadItemExists()
        buf = ReadU()
        If buf.Type = "L"c Then
          ReadL_START(buf)
          SendL_START()
          EchoList(doincr)
          SendL_END()
          ReadL_END()
        ElseIf doincr Then
          SendI(buf.I + 1)
        Else
          SendU(buf)
        End If
      End While
    End Sub

    Private Sub ECOL()
      SendSTART()
      ReadL_START()
      SendL_START()
      EchoList(False)
      SendL_END()
      ReadL_END()
      SendRETURN()
    End Sub

    Private Sub ECLI()
      Dim opt As MqBufferS = ReadU()
      Dim doincr As Boolean = (opt.Type = "C"c AndAlso opt.C = "--incr")
      If Not doincr Then ReadUndo()
      SendSTART()
      EchoList(doincr)
      SendRETURN()
    End Sub


    Private Sub SND1()
      Dim s As String = ReadC()
      Dim id As Integer = ReadI()

      SendSTART()

      Select Case s
        Case "START"
          Dim parent As Server = CType(LinkGetParent(), Server)
          If parent IsNot Nothing AndAlso parent.cl(id).LinkIsConnected() Then
            cl(id).LinkCreateChild(parent.cl(id))
          Else
            cl(id).LinkCreate(ConfigGetDebug())
          End If
        Case "START2"
          ' object already created ERROR
          cl(id).LinkCreate(ConfigGetDebug())
          cl(id).LinkCreate(ConfigGetDebug())
        Case "START3"
          Dim parent As New Client()
          ' parent not connected ERROR
          cl(id).LinkCreateChild(parent)
        Case "START4"
          ' master not connected ERROR
          cl(id).SlaveWorker(0)
        Case "START5"
          ' the 'master' have to be a 'parent' without 'child' objects
          ' 'slave' identifer out of range (0 <= 10000000 <= 1023)
          SlaveWorker(id, "--name", "wk-cl-" + CStr(id), "--srvname", "wk-sv-" + CStr(id), "--thread")
        Case "STOP"
          cl(id).LinkDelete()
        Case "SEND"
          cl(id).SendSTART()
          Dim TOK As String = ReadC()
          ReadProxy(cl(id))
          cl(id).SendEND(TOK)
        Case "WAIT"
          cl(id).SendSTART()
          ReadProxy(cl(id))
          cl(id).SendEND_AND_WAIT("ECOI", 5)
          SendI(cl(id).ReadI() + 1)
        Case "CALLBACK"
          cl(id).SendSTART()
          ReadProxy(cl(id))
          cl(id).i = -1
          cl(id).SendEND_AND_CALLBACK("ECOI", AddressOf cl(id).ECOI_CB)
          cl(id).ProcessEvent(10, MqS.WAIT.ONCE)
          SendI(cl(id).i + 1)
        Case "ERR-1"
          cl(id).SendSTART()
          Try
            ReadProxy(cl(id))
            cl(id).SendEND_AND_WAIT("ECOI", 5)
          Catch ex As Exception
            ErrorSet(ex)
            SendI(ErrorGetNum())
            SendC(ErrorGetText())
            ErrorReset()
          End Try
      End Select
      SendRETURN()
    End Sub

    Private Sub INITX()
      Dim max As Integer = ReadGetNumItems() - 1
      Dim L(max) As String
      SendSTART()
      For i As Integer = 0 To max
        L(i) = ReadC()
      Next i
      MqS.Init(L)
      SendRETURN()
    End Sub

    Private Sub ERR2()
      SendSTART()
      SendC("some data")
      ErrorC("Ot_ERR2", 10, "some error")
      SendRETURN()
    End Sub

    Private Sub ERR3()
      SendSTART()
      SendRETURN()
      SendRETURN()
    End Sub

    Private Sub ERRT()
      SendSTART()
      ErrorC("MYERR", 9, ReadC())
      SendERROR()
    End Sub

    Private Sub ERR4()
      Environment.Exit(1)
    End Sub

    Private Sub BUF1()
      Dim buf As MqBufferS = ReadU()
      Dim typ As Char = buf.Type
      SendSTART()
      SendC(typ)
      Select Case typ
        Case "Y"c
          SendY(buf.Y)
        Case "O"c
          SendO(buf.O)
        Case "S"c
          SendS(buf.S)
        Case "I"c
          SendI(buf.I)
        Case "F"c
          SendF(buf.F)
        Case "W"c
          SendW(buf.W)
        Case "D"c
          SendD(buf.D)
        Case "C"c
          SendC(buf.C)
        Case "B"c
          SendB(buf.B)
      End Select
      SendRETURN()
    End Sub

    Private Sub BUF2()
      SendSTART()
      For i As Integer = 0 To 2
        buf = ReadU()
        SendC(buf.Type)
        SendU(buf)
      Next i
      SendRETURN()
    End Sub

    Private Sub BUF3()
      SendSTART()
      buf = ReadU()
      SendC(buf.Type)
      SendU(buf)
      SendI(ReadI())
      SendU(buf)
      SendRETURN()
    End Sub

    Public Sub ECOU()
      SendSTART()
      SendU(ReadU())
      SendRETURN()
    End Sub

    Public Sub ECOC()
      SendSTART()
      SendC(ReadC())
      SendRETURN()
    End Sub

    Public Sub ECOB()
      SendSTART()
      SendB(ReadB())
      SendRETURN()
    End Sub

    Public Sub ECOO()
      SendSTART()
      SendO(ReadO())
      SendRETURN()
    End Sub

    Public Sub ECOY()
      SendSTART()
      SendY(ReadY())
      SendRETURN()
    End Sub

    Public Sub ECOS()
      SendSTART()
      SendS(ReadS())
      SendRETURN()
    End Sub

    Public Sub ECOI()
      SendSTART()
      SendI(ReadI())
      SendRETURN()
    End Sub

    Public Sub ECOW()
      SendSTART()
      SendW(ReadW())
      SendRETURN()
    End Sub

    Public Sub ECOF()
      SendSTART()
      SendF(ReadF())
      SendRETURN()
    End Sub

    Public Sub ECOD()
      SendSTART()
      SendD(ReadD())
      SendRETURN()
    End Sub

    Public Sub SETU()
      buf = ReadU()
    End Sub

    Public Sub GETU()
      SendSTART()
      SendU(buf)
      SendRETURN()
    End Sub

    Private Sub SLEP()
      SendSTART()
      Dim i As Integer = ReadI()
      System.Threading.Thread.Sleep(i * 1000)
      SendI(i)
      SendRETURN()
    End Sub

    Private Sub USLP()
      SendSTART()
      Dim i As Integer = ReadI()
      System.Threading.Thread.Sleep(CInt(i / 1000))
      SendI(i)
      SendRETURN()
    End Sub

    Private Sub CSV1()
      ' call an other service
      SendSTART()
      SendI(ReadI() + 1)
      SendEND_AND_WAIT("CSV2", 10)

      ' read the answer and send the result back
      SendSTART()
      SendI(ReadI() + 1)
      SendRETURN()
    End Sub
  End Class


  Sub Main(ByVal args() As String)
    Dim srv As New Server()
    Try
      srv.ConfigSetName("server")
      srv.LinkCreate(args)
      srv.ProcessEvent(MqS.WAIT.FOREVER)
    Catch ex As Exception
      srv.ErrorSet(ex)
    Finally
      srv.Exit()
    End Try
  End Sub
End Module


