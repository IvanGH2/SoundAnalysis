object frmProgressDlg: TfrmProgressDlg
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Action progress'
  ClientHeight = 120
  ClientWidth = 378
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poMainFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object lblProcessing: TLabel
    Left = 125
    Top = 8
    Width = 124
    Height = 13
  end
  object stProcess: TStaticText
    Left = 25
    Top = 8
    Width = 94
    Height = 19
    Caption = 'Now processing:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = 15
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
  end
  object pbProcessing: TProgressBar
    Left = 24
    Top = 33
    Width = 340
    Height = 17
    Smooth = True
    Step = 5
    TabOrder = 1
  end
  object stTotalProcess: TStaticText
    Left = 25
    Top = 63
    Width = 97
    Height = 19
    Caption = 'Total processing:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = 15
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 2
  end
  object pbTotalProcessing: TProgressBar
    Left = 24
    Top = 88
    Width = 340
    Height = 17
    Smooth = True
    Step = 5
    TabOrder = 3
  end
end
