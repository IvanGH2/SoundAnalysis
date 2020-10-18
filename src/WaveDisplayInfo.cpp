//---------------------------------------------------------------------------


#pragma hdrstop

#include "WaveDisplayInfo.h"
#include "WaveDisplayContainer.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)
void __fastcall  TWaveDisplayInfo::BtnCloseOnClick(TObject* sender){
	delete Parent;   //Parent is a TWaveDisplayContainer instance
}

__fastcall  TWaveDisplayInfo::TWaveDisplayInfo(TComponent* owner,AnsiString& fName)
	: TPanel(owner)
	{
		Parent = (TWinControl*)owner;
		Height = Parent->Height;
		BorderStyle	= bsSingle;
		BorderWidth = 		3;
		BevelOuter =  bvRaised;
		DoubleBuffered = true;
		Width = 100;
	   //	start = 0;
		OnResize = &WaveDisplayInfoOnResize;
		OnClick =  &WaveDisplayInfoOnClick;
		this->fName =fName;
		Init();
	}
__fastcall  TWaveDisplayInfo::TWaveDisplayInfo(TComponent* owner, FILE_INFO const &fi)
	: TPanel(owner)
	{
		Parent = (TWinControl*)owner;
		Height = Parent->Height;
		Width = nsWaveDisplay::cWaveCtrlWidth;

		OnResize = &WaveDisplayInfoOnResize;
		OnClick =  &WaveDisplayInfoOnClick;
	   	DoubleBuffered = true;

		fileInfo = fi;
		Init();
	}
void  TWaveDisplayInfo::Init(){

  pBtnClose = new TSpeedButton(this);
  pBtnClose->Parent = this;
  pBtnClose->Font->Style =  Font->Style << fsBold;
  pBtnClose->Font->Size = 10;
  pBtnClose->Width = 12;
  pBtnClose->Height = 13;
  pBtnClose->Top = 0;
  pBtnClose->Left = 0;
  pBtnClose->Margins->Top = 2;
  pBtnClose->Margins->Right = 1;
  pBtnClose->Margins->Bottom = 2;
  pBtnClose->Margins->Left = 2;
  pBtnClose->Caption = "x";
  pBtnClose->Flat = true;
  pBtnClose->OnClick = &BtnCloseOnClick;
  //mute and solo buttons
 // pBtnMute = new TButton(this);
  pBtnMute = new TSpeedButton(this);
  pBtnMute->Parent = this;
  pBtnMute->Down = true;
  pBtnMute->Font->Style =  Font->Style << fsBold << fsUnderline;
  pBtnMute->Font->Color = clGray;
  pBtnMute->Width = Width/2-2;
  pBtnMute->Height = 18;
  pBtnMute->Top = Height/2-10;
  pBtnMute->Left = 2;
  pBtnMute->GroupIndex = 1;
  pBtnMute->Caption = "Mute";
  pBtnMute->AllowAllUp = true;
  pBtnMute->Flat = true;
  pBtnMute->OnClick = &BtnMuteSoloOnClick;

  //pBtnSolo = new TButton(this);
  pBtnSolo = new TSpeedButton(this);
  pBtnSolo->Parent = this;
  pBtnSolo->Down = true;
  pBtnSolo->Font->Style =  Font->Style << fsBold << fsUnderline;
  pBtnSolo->Font->Color = clGray;
  pBtnSolo->Width = Width/2-2;
  pBtnSolo->Height = 18;
  pBtnSolo->Top = Height/2-10;
  pBtnSolo->Left = (Width-2)/2;
  pBtnSolo->GroupIndex = 1;
  pBtnSolo->Caption = "Solo";
  pBtnSolo->AllowAllUp = true;
  pBtnSolo->Flat = true;
  pBtnSolo->OnClick = &BtnMuteSoloOnClick;

  pFileName = new TStaticText(this);
  pFileName->Hint = fileInfo.pFileName;
  pFileName->Caption = ((AnsiString)fileInfo.pFileName).SubString(0, 12) + "...";
  pFileName->Width = Width - pBtnClose->Width-2;
  pFileName->Parent = this;
  pFileName->Top = 3;
  pFileName->Left = pBtnClose->Width+2;
  pFileName->Font->Color = clBlue;
  pFileName->Font->Size = 8;
  pFileName->ShowHint = true;

  pFileProp = new TLabel(this);
  pFileProp->Parent = this;
  pFileProp->Top = Height - (pFileProp->Height * 2+5);
  pFileProp->Width = Width - 5;
  pFileProp->Height = 20;
  pFileProp->Left = 3;
  pFileProp->Font->Color = clBlue;
  pFileProp->Font->Size = 8;
  //pFileProp->AutoSize = true;
  pFileProp->WordWrap = true;
  pFileProp->Caption = GetFileProp();
}
const AnsiString&  TWaveDisplayInfo::GetFileProp(){

	prop = fileInfo.numChannels == 1 ? "Mono," : "Stereo,";
	prop +=  fileInfo.samplesPerSec;
	prop =  prop + "Hz "  + IntToStr(fileInfo.bitsPerSample) + " bits";
	return  prop;
}
void __fastcall  TWaveDisplayInfo::ChangeColor(TColor color){
	  Color = color;
	  pFileName->Color = color;
}
void __fastcall  TWaveDisplayInfo::WaveDisplayInfoOnResize(TObject* sender){
		pBtnMute->Top = Height/2-10;
		pBtnSolo->Top = Height/2-10;
		pFileProp->Top =  Height - (pFileProp->Height  + 5);
}
void __fastcall  TWaveDisplayInfo::WaveDisplayInfoOnClick(TObject* sender){

		((TWaveDisplayContainer*)Parent)->WaveContainerOnClick(sender);
}
void __fastcall  TWaveDisplayInfo::BtnMuteSoloOnClick(TObject *sender){

	playbackSelection.mute = pBtnMute->Down;
	playbackSelection.solo = pBtnSolo->Down;
	((TWaveDisplayContainer*)Parent)->PlaybackSelectionChanged();
}
void  TWaveDisplayInfo::ChangeVisibility(bool visible){

		pBtnMute->Visible = visible;
		pBtnSolo->Visible = visible;
		pFileProp->Visible = visible;
}

