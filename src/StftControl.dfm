object pStftControl: TpStftControl
  Left = 0
  Top = 0
  AlphaBlendValue = 80
  BorderStyle = bsDialog
  Caption = 'Spectrogram control'
  ClientHeight = 90
  ClientWidth = 320
  Color = cl3DLight
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  OnClose = StftControlOnClose
  OnShow = StftControlOnShow
  PixelsPerInch = 96
  TextHeight = 13
  object StaticText1: TStaticText
    Left = 8
    Top = 8
    Width = 46
    Height = 17
    Caption = 'Window:'
    TabOrder = 0
  end
  object StaticText2: TStaticText
    Left = 182
    Top = 8
    Width = 39
    Height = 17
    Caption = 'Colour:'
    TabOrder = 1
  end
  object pStftWnd: TComboBox
    Left = 85
    Top = 12
    Width = 90
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    BevelOuter = bvRaised
    ItemHeight = 13
    ItemIndex = 1
    TabOrder = 2
    Text = 'Blackman'
    OnChange = StftWndOnChange
    Items.Strings = (
      'Hamming'
      'Blackman'
      'Hann'
      'Rectangular')
  end
  object StaticText3: TStaticText
    Left = 8
    Top = 35
    Width = 59
    Height = 17
    Caption = 'FFT length:'
    TabOrder = 3
  end
  object pFftLen: TComboBox
    Left = 85
    Top = 37
    Width = 90
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ItemHeight = 13
    ParentFont = False
    TabOrder = 4
    Text = '1024'
    OnChange = SfftFLenOnChange
    Items.Strings = (
      '128'
      '256'
      '512'
      '1024'
      '2048'
      '4096')
  end
  object StaticText4: TStaticText
    Left = 8
    Top = 232
    Width = 50
    Height = 17
    Caption = 'Contrast:'
    TabOrder = 5
  end
  object pStftContrast: TTrackBar
    Left = 64
    Top = 233
    Width = 150
    Height = 25
    Max = 7
    Min = 1
    PageSize = 1
    Position = 2
    SelEnd = 6
    SelStart = 1
    TabOrder = 6
    ThumbLength = 14
  end
  object StaticText5: TStaticText
    Left = 20
    Top = 264
    Width = 46
    Height = 17
    Caption = 'Overlap:'
    TabOrder = 7
  end
  object pStftOverlap: TComboBox
    Left = 76
    Top = 264
    Width = 145
    Height = 21
    ItemHeight = 13
    ItemIndex = 1
    TabOrder = 8
    Text = '50%'
    Items.Strings = (
      '25%'
      '50%'
      '75%')
  end
  object rgScale: TRadioGroup
    Left = 182
    Top = 30
    Width = 133
    Height = 53
    Caption = 'Scale'
    Enabled = False
    ItemIndex = 0
    Items.Strings = (
      'Linear'
      'Log')
    TabOrder = 9
    OnClick = rgScaleClick
  end
  object cbGradColor: TComboBox
    Left = 220
    Top = 6
    Width = 95
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    ItemHeight = 13
    TabOrder = 10
    Text = 'Orange yellow'
    OnChange = GradColorOnSelect
    Items.Strings = (
      'Orange glow'
      'Blue sky'
      'Pink orange'
      'Emerald green'
      'White on black'
      'Black on white')
  end
  object StaticText6: TStaticText
    Left = 8
    Top = 63
    Width = 77
    Height = 17
    Caption = 'Frame overlap:'
    TabOrder = 11
  end
  object pFftOverlap: TComboBox
    Left = 85
    Top = 61
    Width = 90
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 12
    Text = '50%'
    OnChange = FftOverlapOnChange
    Items.Strings = (
      '50%'
      '75%'
      '88%')
  end
end
