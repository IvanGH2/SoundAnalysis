object pFilterDlg: TpFilterDlg
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Generate filter dialog (windowed sync)'
  ClientHeight = 299
  ClientWidth = 521
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
  object Bevel1: TBevel
    Left = 8
    Top = 8
    Width = 505
    Height = 296
  end
  object Bevel2: TBevel
    Left = 22
    Top = 144
    Width = 475
    Height = 145
  end
  object StaticText1: TStaticText
    Left = 24
    Top = 30
    Width = 57
    Height = 17
    Caption = 'Filter type:'
    TabOrder = 0
  end
  object StaticText2: TStaticText
    Left = 24
    Top = 60
    Width = 62
    Height = 17
    Caption = 'Cuttoff low:'
    TabOrder = 1
  end
  object StaticText4: TStaticText
    Left = 194
    Top = 28
    Width = 97
    Height = 17
    Caption = 'Filter kernel length:'
    TabOrder = 2
  end
  object pCbFilterType: TComboBox
    Left = 92
    Top = 30
    Width = 80
    Height = 21
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 3
    Text = 'Low pass'
    OnSelect = FilterOnSelect
    Items.Strings = (
      'Low pass'
      'High pass'
      'Band pass')
  end
  object pMeFreqLo: TMaskEdit
    Left = 92
    Top = 60
    Width = 80
    Height = 21
    EditMask = '99999;1;_'
    MaxLength = 5
    TabOrder = 4
    Text = '     '
    OnExit = CuttoffLoOnExit
  end
  object btnFilterOk: TButton
    Left = 385
    Top = 28
    Width = 96
    Height = 25
    Caption = 'OK'
    TabOrder = 5
    OnClick = btnFilterOkClick
  end
  object StaticText5: TStaticText
    Left = 22
    Top = 90
    Width = 62
    Height = 17
    Caption = 'Cutoff high:'
    TabOrder = 6
  end
  object pMeFreqHi: TMaskEdit
    Left = 92
    Top = 90
    Width = 80
    Height = 21
    EditMask = '99999;1;_'
    MaxLength = 5
    TabOrder = 7
    Text = '     '
    OnExit = CuttoffHiOnExit
  end
  object StaticText6: TStaticText
    Left = 194
    Top = 58
    Width = 71
    Height = 17
    Caption = 'Window type:'
    TabOrder = 8
  end
  object pCbFilterWnd: TComboBox
    Left = 297
    Top = 55
    Width = 80
    Height = 21
    ItemHeight = 13
    ItemIndex = 1
    TabOrder = 9
    Text = 'Blackman'
    OnSelect = FilterWndOnSelect
    Items.Strings = (
      'Hamming'
      'Blackman'
      'Hanning'
      'Rectangular')
  end
  object StaticText7: TStaticText
    Left = 40
    Top = 160
    Width = 90
    Height = 17
    Caption = 'Number of bands:'
    TabOrder = 10
  end
  object meBandsNum: TMaskEdit
    Left = 147
    Top = 160
    Width = 38
    Height = 21
    Enabled = False
    EditMask = '99999;1;_'
    MaxLength = 5
    TabOrder = 11
    Text = '     '
    OnChange = meBandsNumChange
  end
  object tcFilterBand: TTabControl
    Left = 37
    Top = 191
    Width = 444
    Height = 66
    Style = tsFlatButtons
    TabOrder = 12
    Tabs.Strings = (
      '')
    TabIndex = 0
    OnChange = TcFilterBandChange
    object StaticText8: TStaticText
      Left = 3
      Top = 40
      Width = 61
      Height = 17
      Caption = 'Cutoff low: '
      TabOrder = 0
    end
    object meBandCutoffLo: TMaskEdit
      Tag = 1
      Left = 70
      Top = 38
      Width = 67
      Height = 21
      EditMask = '99999;1;_'
      MaxLength = 5
      TabOrder = 1
      Text = '     '
      OnExit = BandLoHiOnExit
    end
    object StaticText9: TStaticText
      Left = 157
      Top = 40
      Width = 62
      Height = 17
      Caption = 'Cutoff high:'
      TabOrder = 2
    end
    object meBandCutoffHi: TMaskEdit
      Tag = 2
      Left = 225
      Top = 38
      Width = 77
      Height = 21
      EditMask = '99999;1;_'
      MaxLength = 5
      TabOrder = 3
      Text = '     '
      OnExit = BandLoHiOnExit
    end
  end
  object btnFilterClose: TButton
    Left = 383
    Top = 59
    Width = 96
    Height = 25
    Caption = 'Close'
    TabOrder = 13
    OnClick = btnFilterCloseClick
  end
  object cbInverted: TCheckBox
    Left = 194
    Top = 82
    Width = 97
    Height = 17
    Caption = 'Invert'
    TabOrder = 14
    Visible = False
  end
  object pMeFilterLen: TMaskEdit
    Left = 297
    Top = 28
    Width = 78
    Height = 21
    EditMask = '99999;1;_'
    MaxLength = 5
    TabOrder = 15
    Text = '400  '
  end
end
