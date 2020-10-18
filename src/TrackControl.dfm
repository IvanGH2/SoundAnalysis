object TrackControlDlg: TTrackControlDlg
  Left = 227
  Top = 108
  VertScrollBar.ButtonSize = 10
  AlphaBlendValue = 200
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Track control'
  ClientHeight = 232
  ClientWidth = 549
  Color = clBtnFace
  ParentFont = True
  FormStyle = fsStayOnTop
  OldCreateOrder = True
  Position = poScreenCenter
  OnShow = TrackControlOnShow
  PixelsPerInch = 96
  TextHeight = 13
  object sbHideTracks: TSpeedButton
    Left = 448
    Top = 12
    Width = 93
    Height = 24
    Caption = 'Hide/show tracks'
    OnClick = sbHideTracksClick
  end
  object cbPlaybackSelection: TCheckBox
    Left = 8
    Top = 12
    Width = 101
    Height = 17
    Caption = 'Select all tracks'
    TabOrder = 0
    OnClick = cbPlayOnClick
  end
  object sbTracks: TScrollBox
    Left = 0
    Top = 72
    Width = 549
    Height = 160
    Align = alBottom
    TabOrder = 1
    object StaticText4: TStaticText
      Left = 72
      Top = 5
      Width = 37
      Height = 17
      Caption = 'Hidden'
      TabOrder = 0
    end
    object StaticText2: TStaticText
      Left = 8
      Top = 5
      Width = 51
      Height = 17
      Caption = 'Play track'
      TabOrder = 1
    end
    object StaticText3: TStaticText
      Left = 163
      Top = 5
      Width = 38
      Height = 17
      Caption = 'Format'
      TabOrder = 2
    end
    object StaticText1: TStaticText
      Left = 225
      Top = 5
      Width = 63
      Height = 17
      Caption = 'Track name:'
      TabOrder = 3
    end
  end
  object cbHiddenTracks: TCheckBox
    Left = 8
    Top = 35
    Width = 126
    Height = 17
    Caption = 'Select hidden tracks'
    TabOrder = 2
    OnClick = cbHideOnClick
  end
end
