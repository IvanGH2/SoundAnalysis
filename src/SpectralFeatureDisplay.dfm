object frmSpecFeatDisplay: TfrmSpecFeatDisplay
  Left = 0
  Top = 0
  Caption = 'Spectral feature display'
  ClientHeight = 448
  ClientWidth = 983
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object pbSpecFeatDisplay: TPaintBox
    Left = 9
    Top = 8
    Width = 720
    Height = 376
    OnPaint = SpectralFeaturesOnPaint
  end
  object StaticText1: TStaticText
    Left = 8
    Top = 392
    Width = 75
    Height = 17
    Caption = 'X axis feature:'
    TabOrder = 0
  end
  object StaticText2: TStaticText
    Left = 8
    Top = 417
    Width = 75
    Height = 17
    Caption = 'Y axis feature:'
    TabOrder = 1
  end
  object cbSpecFeatX: TComboBox
    Left = 97
    Top = 392
    Width = 145
    Height = 21
    ItemHeight = 13
    TabOrder = 2
    Text = 'Flux'
    OnSelect = cbSpectFeatXSelect
    Items.Strings = (
      'Flux'
      'Centroid'
      'Rolloff')
  end
  object cbSpecFeatY: TComboBox
    Left = 97
    Top = 419
    Width = 145
    Height = 21
    Enabled = False
    ItemHeight = 13
    TabOrder = 3
    Text = 'Flux'
    Items.Strings = (
      'Centroid'
      'Flux'
      'Zero crossings'
      'Rolloff')
  end
  object btnRefreshDisplay: TButton
    Left = 248
    Top = 420
    Width = 100
    Height = 25
    Caption = 'Refresh display'
    TabOrder = 4
    OnClick = btnRefreshDisplayClick
  end
  object cbShowAllTracks: TCheckBox
    Left = 248
    Top = 390
    Width = 97
    Height = 17
    Alignment = taLeftJustify
    Caption = 'Show all tracks:'
    Checked = True
    State = cbChecked
    TabOrder = 5
    OnClick = cbShowAllTracksClicked
  end
  object cSeTracks: TCSpinEdit
    Left = 421
    Top = 390
    Width = 41
    Height = 22
    MaxValue = 1
    MinValue = 1
    TabOrder = 6
    Value = 1
    OnChange = SelTrackOnChange
  end
  object Panel1: TPanel
    Left = 744
    Top = 8
    Width = 231
    Height = 376
    BorderStyle = bsSingle
    TabOrder = 7
    object lblFirst: TLabel
      Left = 32
      Top = 32
      Width = 18
      Height = 13
      Caption = 'N/A'
    end
    object lblSecond: TLabel
      Left = 32
      Top = 64
      Width = 18
      Height = 13
      Caption = 'N/A'
    end
    object lblThird: TLabel
      Left = 32
      Top = 88
      Width = 18
      Height = 13
      Caption = 'N/A'
      Color = clMenuHighlight
      ParentColor = False
    end
    object stFirst: TStaticText
      Left = 16
      Top = 32
      Width = 10
      Height = 17
      Caption = '1'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clTeal
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
    end
    object stSecond: TStaticText
      Left = 16
      Top = 64
      Width = 10
      Height = 17
      Caption = '2'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clRed
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      TabOrder = 1
    end
    object StaticText4: TStaticText
      Left = 16
      Top = 88
      Width = 10
      Height = 17
      Caption = '3'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlue
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      TabOrder = 2
    end
  end
  object StaticText3: TStaticText
    Left = 351
    Top = 390
    Width = 64
    Height = 17
    Caption = 'Select track:'
    TabOrder = 8
  end
  object btnMatchSound: TButton
    Left = 480
    Top = 390
    Width = 89
    Height = 55
    Caption = 'Match sounds'
    TabOrder = 9
    OnClick = btnMatchSoundClick
  end
  object btnSpEnvelope: TButton
    Left = 360
    Top = 420
    Width = 100
    Height = 25
    Caption = 'Spectral envelope'
    TabOrder = 10
    OnClick = btnSpEnvelopeClick
  end
  object btnMatchSpEnv: TButton
    Left = 584
    Top = 390
    Width = 97
    Height = 55
    Caption = 'Match envelopes'
    TabOrder = 11
    OnClick = btnMatchSpEnvClick
  end
end
