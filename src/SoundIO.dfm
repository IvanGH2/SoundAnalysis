object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'Sound analyzer'
  ClientHeight = 404
  ClientWidth = 875
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 875
    Height = 200
    Align = alTop
    TabOrder = 0
    object PaintBox1: TPaintBox
      Left = 1
      Top = 1
      Width = 873
      Height = 198
      Align = alClient
      ExplicitLeft = 216
      ExplicitTop = 88
      ExplicitWidth = 105
      ExplicitHeight = 105
    end
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 385
    Width = 875
    Height = 19
    Panels = <>
  end
  object MainMenu1: TMainMenu
    Left = 200
    Top = 32
    object File1: TMenuItem
      Caption = '&File'
      object Open1: TMenuItem
        Caption = '&Open'
        OnClick = openSoundFile
      end
      object Save1: TMenuItem
        Caption = '&Save'
      end
      object Saveas1: TMenuItem
        Caption = 'S&ave as'
      end
      object Close1: TMenuItem
        Caption = '&Close'
      end
    end
  end
  object OpenDialog1: TOpenDialog
    Left = 416
    Top = 216
  end
end
