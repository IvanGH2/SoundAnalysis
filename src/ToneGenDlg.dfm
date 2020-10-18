object ToneGeneratorDlg: TToneGeneratorDlg
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Tone generator'
  ClientHeight = 336
  ClientWidth = 418
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
  object Bevel1: TBevel
    Left = 16
    Top = 8
    Width = 393
    Height = 169
  end
  object Bevel2: TBevel
    Left = 16
    Top = 199
    Width = 393
    Height = 129
  end
  object StaticText1: TStaticText
    Left = 32
    Top = 24
    Width = 57
    Height = 17
    Caption = 'Tone type:'
    TabOrder = 0
  end
  object StaticText2: TStaticText
    Left = 30
    Top = 47
    Width = 59
    Height = 17
    Caption = 'Frequency:'
    TabOrder = 1
  end
  object StaticText3: TStaticText
    Left = 32
    Top = 80
    Width = 55
    Height = 17
    Caption = 'Amplitude:'
    TabOrder = 2
  end
  object StaticText4: TStaticText
    Left = 33
    Top = 143
    Width = 49
    Height = 17
    Caption = 'Duration:'
    TabOrder = 3
  end
  object cbToneType: TComboBox
    Left = 103
    Top = 24
    Width = 114
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 4
    Text = 'Sine'
    Items.Strings = (
      'Sine'
      'Square'
      'Triangle'
      'Sawtooth')
  end
  object meToneFreq: TMaskEdit
    Left = 104
    Top = 51
    Width = 113
    Height = 21
    EditMask = '99999;1;_'
    MaxLength = 5
    TabOrder = 5
    Text = '200  '
  end
  object meToneAmp: TMaskEdit
    Left = 105
    Top = 78
    Width = 112
    Height = 21
    EditMask = '0,99;1;_'
    MaxLength = 4
    TabOrder = 6
    Text = '0,6 '
    OnExit = meToneAmpExit
  end
  object meToneTime: TMaskEdit
    Left = 103
    Top = 135
    Width = 114
    Height = 21
    EditMask = '999999;1;_'
    MaxLength = 6
    TabOrder = 7
    Text = '1000  '
  end
  object btnToneOK: TButton
    Left = 232
    Top = 22
    Width = 97
    Height = 25
    Caption = 'OK'
    TabOrder = 8
    OnClick = btnToneOKClick
  end
  object btnToneCancel: TButton
    Left = 232
    Top = 53
    Width = 97
    Height = 25
    Caption = 'Cancel'
    TabOrder = 9
    OnClick = btnToneCancelClick
  end
  object StaticText5: TStaticText
    Left = 32
    Top = 213
    Width = 109
    Height = 17
    Caption = 'Number of harmonics:'
    TabOrder = 10
  end
  object meToneHarNum: TMaskEdit
    Left = 147
    Top = 209
    Width = 35
    Height = 21
    TabOrder = 11
    Text = '0'
    OnChange = meToneHarNumChange
  end
  object tcHarm: TTabControl
    Left = 32
    Top = 236
    Width = 361
    Height = 77
    Style = tsFlatButtons
    TabOrder = 12
    Tabs.Strings = (
      '')
    TabIndex = 0
    OnChange = tcHarmChange
    object StaticText6: TStaticText
      Left = 130
      Top = 40
      Width = 55
      Height = 17
      Caption = 'Amplitude:'
      TabOrder = 0
    end
    object meHarmAmp: TMaskEdit
      Left = 191
      Top = 40
      Width = 48
      Height = 21
      EditMask = '0,99;1;_'
      MaxLength = 4
      TabOrder = 1
      Text = ' ,  '
      OnChange = meHarmAmpExit
    end
    object StaticText8: TStaticText
      Left = 1
      Top = 40
      Width = 59
      Height = 17
      Caption = 'Frequency:'
      TabOrder = 2
    end
    object meHarmFreq: TMaskEdit
      Left = 64
      Top = 40
      Width = 60
      Height = 21
      EditMask = '99999;1;_'
      MaxLength = 5
      TabOrder = 3
      Text = '     '
      OnChange = memeHarmFreqExit
    end
    object StaticText9: TStaticText
      Left = 254
      Top = 40
      Width = 37
      Height = 17
      Caption = 'Phase:'
      TabOrder = 4
    end
    object meHarmPhase: TMaskEdit
      Left = 297
      Top = 40
      Width = 58
      Height = 21
      EditMask = '9,99;1;_'
      MaxLength = 4
      TabOrder = 5
      Text = ' ,  '
      OnChange = meHarmPhaseExit
    end
  end
  object StaticText7: TStaticText
    Left = 32
    Top = 112
    Width = 65
    Height = 17
    Caption = 'Sample rate:'
    TabOrder = 13
  end
  object meToneSampleRate: TMaskEdit
    Left = 103
    Top = 105
    Width = 114
    Height = 21
    EditMask = '99999;1;_'
    MaxLength = 5
    TabOrder = 14
    Text = '8000 '
  end
end
