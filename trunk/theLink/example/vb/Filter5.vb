REM

Imports System
Imports System.IO
Imports csmsgque

Public Module example

  REM F1 ********************************************************
  Private NotInheritable Class F1
    Inherits MqS
    Implements IServerSetup

    ' constructor
    Public Sub New(ByVal tmpl As MqS)
      MyBase.New(tmpl)
    End Sub

    Sub FTR()
      Dim ftr As MqS = ServiceGetFilter()
      ftr.SendSTART()
      ftr.SendC("F1")
      ftr.SendC(ftr.ConfigGetName())
      ftr.SendI(CType(ftr.ConfigGetStartAs(), Integer))
      ftr.SendI(ConfigGetStatusIs())
      While ReadGetNumItems() <> 0
        ftr.SendC(ReadC())
      End While
      ftr.SendEND_AND_WAIT("+FTR")
      SendRETURN()
    End Sub

    Private Sub ServerSetup() Implements IServerSetup.ServerSetup
      ServiceCreate("+FTR", AddressOf FTR)
      ServiceProxy("+EOF")
    End Sub
  End Class

  REM F2 ********************************************************
  Private NotInheritable Class F2
    Inherits MqS
    Implements IServerSetup

    ' constructor
    Public Sub New(ByVal tmpl As MqS)
      MyBase.New(tmpl)
    End Sub

    Private Sub FTR()
      Dim ftr As MqS = ServiceGetFilter()
      ftr.SendSTART()
      ftr.SendC("F2")
      ftr.SendC(ftr.ConfigGetName())
      ftr.SendI(CType(ftr.ConfigGetStartAs(), Integer))
      ftr.SendI(ConfigGetStatusIs())
      While ReadGetNumItems() <> 0
        ftr.SendC(ReadC())
      End While
      ftr.SendEND_AND_WAIT("+FTR")
      SendRETURN()
    End Sub

    Private Sub ServerSetup() Implements IServerSetup.ServerSetup
      ServiceCreate("+FTR", AddressOf FTR)
      ServiceProxy("+EOF")
    End Sub
  End Class

  REM F3 ********************************************************
  Private NotInheritable Class F3
    Inherits MqS
    Implements IServerSetup

    ' constructor
    Public Sub New(ByVal tmpl As MqS)
      MyBase.New(tmpl)
    End Sub

    Private Sub FTR()
      Dim ftr As MqS = ServiceGetFilter()
      ftr.SendSTART()
      ftr.SendC("F3")
      ftr.SendC(ftr.ConfigGetName())
      ftr.SendI(CType(ftr.ConfigGetStartAs(), Integer))
      ftr.SendI(ConfigGetStatusIs())
      While ReadGetNumItems() <> 0
        ftr.SendC(ReadC())
      End While
      ftr.SendEND_AND_WAIT("+FTR")
      SendRETURN()
    End Sub

    Private Sub ServerSetup() Implements IServerSetup.ServerSetup
      ServiceCreate("+FTR", AddressOf FTR)
      ServiceProxy("+EOF")
    End Sub
  End Class

  REM Main ******************************************************
  Sub Main(ByVal args() As String)
    MqFactoryS(Of F1).Add()
    MqFactoryS(Of F2).Add()
    MqFactoryS(Of F3).Add()
    Dim srv As MqS = MqFactoryS(Of MqS).GetCalled(args(0)).[New]()
    Try
      srv.LinkCreate(args)
      srv.ProcessEvent(MqS.WAIT.FOREVER)
    Catch ex As Exception
      srv.ErrorSet(ex)
    End Try
    srv.Exit()
  End Sub
End Module

