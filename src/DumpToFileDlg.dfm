object dlgDumpToFile: TdlgDumpToFile
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Dump to file dialog'
  ClientHeight = 380
  ClientWidth = 450
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poOwnerFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object lblSaveLoc: TLabel
    Left = 103
    Top = 358
    Width = 240
    Height = 13
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentColor = False
    ParentFont = False
  end
  object rgDataSource: TRadioGroup
    Left = 8
    Top = 8
    Width = 162
    Height = 121
    Caption = 'Feature selection'
    ItemIndex = 0
    Items.Strings = (
      'Magnitudes'
      'Spectral centroid'
      'Spectral roll-off'
      'Spectral flux')
    TabOrder = 0
  end
  object rgChannels: TRadioGroup
    Left = 9
    Top = 135
    Width = 161
    Height = 89
    Caption = 'Channels'
    ItemIndex = 1
    Items.Strings = (
      'Both (stereo only)'
      'Left channel'
      'Right channel (stereo only)')
    TabOrder = 1
  end
  object gbLimitRange: TGroupBox
    Left = 176
    Top = 8
    Width = 265
    Height = 105
    Caption = 'Frame/bin select'
    TabOrder = 2
    object StaticText2: TStaticText
      Left = 3
      Top = 24
      Width = 63
      Height = 17
      Caption = 'Start frame:'
      TabOrder = 0
    end
    object meStartFrame: TMaskEdit
      Left = 64
      Top = 24
      Width = 56
      Height = 21
      EditMask = '99999;1;_'
      MaxLength = 5
      TabOrder = 1
      Text = '     '
    end
    object StaticText1: TStaticText
      Left = 126
      Top = 24
      Width = 57
      Height = 17
      Caption = 'End frame:'
      TabOrder = 2
    end
    object StaticText3: TStaticText
      Left = 3
      Top = 72
      Width = 49
      Height = 17
      Caption = 'Start bin:'
      TabOrder = 3
    end
    object StaticText4: TStaticText
      Left = 124
      Top = 72
      Width = 43
      Height = 17
      Caption = 'End bin:'
      TabOrder = 4
    end
    object meEndFrame: TMaskEdit
      Left = 181
      Top = 24
      Width = 68
      Height = 21
      EditMask = '99999;1;_'
      MaxLength = 5
      TabOrder = 5
      Text = '     '
    end
    object meStartBin: TMaskEdit
      Left = 64
      Top = 68
      Width = 54
      Height = 21
      EditMask = '99999;1;_'
      MaxLength = 5
      TabOrder = 6
      Text = '     '
    end
    object meEndBin: TMaskEdit
      Left = 182
      Top = 68
      Width = 67
      Height = 21
      EditMask = '99999;1;_'
      MaxLength = 5
      TabOrder = 7
      Text = '     '
    end
  end
  object btnChooseFile: TButton
    Left = 8
    Top = 283
    Width = 88
    Height = 25
    Caption = 'Choose file'
    TabOrder = 3
    OnClick = btnChooseFileClick
  end
  object btnSave: TButton
    Left = 8
    Top = 252
    Width = 88
    Height = 25
    Caption = 'Save'
    Enabled = False
    TabOrder = 4
    OnClick = btnSaveClick
  end
  object StaticText5: TStaticText
    Left = 8
    Top = 358
    Width = 72
    Height = 17
    Caption = 'Save location:'
    TabOrder = 5
  end
  object GroupBox1: TGroupBox
    Left = 176
    Top = 119
    Width = 265
    Height = 226
    Caption = 'Output layout:'
    TabOrder = 6
    object cbFreq: TCheckBox
      Left = 3
      Top = 24
      Width = 118
      Height = 17
      Caption = 'Show frequencies'
      TabOrder = 0
    end
    object cbBasicInfo: TCheckBox
      Left = 3
      Top = 47
      Width = 97
      Height = 17
      Caption = 'Show basic info'
      TabOrder = 1
    end
    object rgChannelSeparation: TRadioGroup
      Left = 3
      Top = 102
      Width = 238
      Height = 59
      Caption = 'Left/right channel separation'
      ItemIndex = 0
      Items.Strings = (
        'Right channel printed after left channel'
        'Right channel separated from left channel')
      TabOrder = 2
    end
    object rgFrameValues: TRadioGroup
      Left = 3
      Top = 167
      Width = 238
      Height = 56
      Caption = 'Value separation'
      ItemIndex = 1
      Items.Strings = (
        'All frame values in a single row'
        'Single value per row')
      TabOrder = 3
    end
    object cbFrame: TCheckBox
      Left = 3
      Top = 70
      Width = 97
      Height = 17
      Caption = 'Show frames'
      TabOrder = 4
    end
  end
  object btnClose: TButton
    Left = 8
    Top = 314
    Width = 88
    Height = 25
    Caption = 'Close'
    TabOrder = 7
    OnClick = btnCloseClick
  end
  object SaveDialog1: TSaveDialog
    Left = 128
    Top = 248
  end
end
