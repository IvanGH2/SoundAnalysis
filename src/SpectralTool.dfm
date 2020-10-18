object pSpectralForm: TpSpectralForm
  Left = 0
  Top = 0
  AlphaBlendValue = 220
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Spectral analisys/manipulation'
  ClientHeight = 412
  ClientWidth = 466
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  OnClose = SpectralFormOnClose
  OnShow = SpectralFormOnShow
  PixelsPerInch = 96
  TextHeight = 13
  object pcSpectralTool: TPageControl
    Left = 8
    Top = 56
    Width = 450
    Height = 353
    ActivePage = tsPowerSpectrum
    TabOrder = 0
    OnChange = SpectralToolOnChange
    object tsPowerSpectrum: TTabSheet
      Caption = 'Power spectrum'
      object sgSpectralData: TStringGrid
        Left = 0
        Top = 0
        Width = 442
        Height = 290
        Align = alTop
        BevelInner = bvLowered
        BevelOuter = bvNone
        BevelWidth = 2
        BorderStyle = bsNone
        Color = clHighlightText
        ColCount = 4
        DefaultColWidth = 60
        RowCount = 10
        Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing]
        TabOrder = 0
      end
      object StaticText5: TStaticText
        Left = 3
        Top = 300
        Width = 38
        Height = 17
        Caption = 'Frame:'
        TabOrder = 1
      end
      object meFrame: TMaskEdit
        Left = 47
        Top = 300
        Width = 42
        Height = 21
        EditMask = '99999;1;_'
        MaxLength = 5
        TabOrder = 2
        Text = '     '
        OnExit = meFrameOnExit
      end
      object btnFrameRefresh: TButton
        Left = 95
        Top = 300
        Width = 75
        Height = 20
        Caption = 'Refresh'
        TabOrder = 3
        OnClick = btnFrameRefreshClick
      end
      object btnDumpToFile: TButton
        Left = 350
        Top = 300
        Width = 75
        Height = 20
        Caption = 'Save'
        TabOrder = 4
        OnClick = btnDumpToFileClick
      end
    end
    object tsCentroid: TTabSheet
      Caption = 'Centroid'
      ImageIndex = 1
      object sgSpectralCentroid: TStringGrid
        Left = 0
        Top = 0
        Width = 442
        Height = 290
        Align = alTop
        ColCount = 3
        DefaultColWidth = 60
        RowCount = 10
        Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing]
        TabOrder = 0
        RowHeights = (
          24
          24
          24
          24
          24
          24
          24
          24
          24
          24)
      end
      object StaticText6: TStaticText
        Left = 3
        Top = 305
        Width = 55
        Height = 17
        Caption = 'Threshold:'
        TabOrder = 1
      end
      object meCentroidThreshold: TMaskEdit
        Left = 64
        Top = 300
        Width = 44
        Height = 21
        EditMask = '-99;1;_'
        MaxLength = 3
        TabOrder = 2
        Text = '-  '
        OnExit = meCentroidOnExit
      end
      object btnCentroidRefresh: TButton
        Left = 114
        Top = 300
        Width = 57
        Height = 20
        Caption = 'Refresh'
        TabOrder = 3
        OnClick = btnThresholdRefresh
      end
    end
    object tsFlux: TTabSheet
      Caption = 'Flux'
      ImageIndex = 2
      object sgSpectralFlux: TStringGrid
        Left = 0
        Top = 0
        Width = 442
        Height = 290
        Align = alTop
        ColCount = 3
        DefaultColWidth = 60
        RowCount = 10
        TabOrder = 0
      end
    end
    object tsRolloff: TTabSheet
      Caption = 'Rolloff'
      ImageIndex = 3
      object sgSpectralRolloff: TStringGrid
        Left = 0
        Top = 0
        Width = 442
        Height = 300
        Align = alTop
        ColCount = 3
        DefaultColWidth = 60
        RowCount = 10
        Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing]
        TabOrder = 0
      end
    end
    object tsAvgPowerSpectrum: TTabSheet
      Caption = 'Average power spectrum'
      ImageIndex = 5
      TabVisible = False
      object sgAvgSpectralData: TStringGrid
        Left = 3
        Top = 0
        Width = 545
        Height = 290
        ColCount = 3
        DefaultColWidth = 60
        RowCount = 10
        Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing]
        TabOrder = 0
        ColWidths = (
          60
          91
          96)
      end
    end
  end
  object GroupBox1: TGroupBox
    Left = 8
    Top = 3
    Width = 450
    Height = 38
    Caption = 'Selection'
    TabOrder = 1
    object lblStartFrame: TLabel
      Left = 75
      Top = 15
      Width = 6
      Height = 13
      Caption = '1'
    end
    object lblEndFrame: TLabel
      Left = 173
      Top = 15
      Width = 6
      Height = 13
      Caption = '1'
    end
    object lblStartBin: TLabel
      Left = 279
      Top = 15
      Width = 6
      Height = 13
      Caption = '1'
    end
    object lblEndBin: TLabel
      Left = 379
      Top = 15
      Width = 6
      Height = 13
      Caption = '1'
    end
    object StaticText1: TStaticText
      Left = 10
      Top = 15
      Width = 63
      Height = 17
      Caption = 'Start frame:'
      TabOrder = 0
    end
    object StaticText2: TStaticText
      Left = 113
      Top = 15
      Width = 57
      Height = 17
      Caption = 'End frame:'
      TabOrder = 1
    end
    object StaticText3: TStaticText
      Left = 219
      Top = 15
      Width = 49
      Height = 17
      Caption = 'Start bin:'
      TabOrder = 2
    end
    object StaticText4: TStaticText
      Left = 330
      Top = 15
      Width = 43
      Height = 17
      Caption = 'End bin:'
      TabOrder = 3
    end
  end
end
