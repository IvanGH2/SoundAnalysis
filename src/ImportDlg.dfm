object RawImportDialog: TRawImportDialog
  Left = 0
  Top = 0
  AlphaBlend = True
  AlphaBlendValue = 220
  BorderStyle = bsDialog
  Caption = 'Import data'
  ClientHeight = 135
  ClientWidth = 352
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
  object StaticText1: TStaticText
    Left = 8
    Top = 24
    Width = 51
    Height = 17
    Caption = 'Encoding:'
    TabOrder = 0
  end
  object StaticText2: TStaticText
    Left = 8
    Top = 55
    Width = 59
    Height = 17
    Caption = 'Byte order:'
    TabOrder = 1
  end
  object StaticText3: TStaticText
    Left = 8
    Top = 78
    Width = 74
    Height = 17
    Caption = 'Num channels:'
    TabOrder = 2
  end
  object StaticText4: TStaticText
    Left = 8
    Top = 105
    Width = 65
    Height = 17
    Caption = 'Sample rate:'
    TabOrder = 3
  end
  object cbImpEncRaw: TComboBox
    Left = 88
    Top = 20
    Width = 145
    Height = 21
    BevelKind = bkFlat
    ItemHeight = 13
    ItemIndex = 1
    TabOrder = 4
    Text = 'Signed 16 bit'
    Items.Strings = (
      'Unsigned 8 bit'
      'Signed 16 bit'
      'Signed 24 bit'
      'Signed floating point 32 bit')
  end
  object cbImpByteRaw: TComboBox
    Left = 88
    Top = 47
    Width = 145
    Height = 21
    BevelKind = bkFlat
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 5
    Text = 'Little endian'
    Items.Strings = (
      'Little endian'
      'Big endian'
      'No endianess')
  end
  object cbImpChannRaw: TComboBox
    Left = 88
    Top = 74
    Width = 145
    Height = 21
    BevelKind = bkFlat
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 6
    Text = '1 channel (mono)'
    Items.Strings = (
      '1 channel (mono)'
      '2 channels (stereo)')
  end
  object meImpSrRaw: TMaskEdit
    Left = 88
    Top = 101
    Width = 145
    Height = 21
    BorderStyle = bsNone
    TabOrder = 7
    Text = 'meImpSrRaw'
  end
  object btnImportOk: TButton
    Left = 264
    Top = 18
    Width = 75
    Height = 25
    Caption = 'Ok'
    TabOrder = 8
    OnClick = btnImportOkClick
  end
  object btnImportCancel: TButton
    Left = 264
    Top = 49
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 9
    OnClick = btnImportCancelClick
  end
end
