//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "TrackControl.h"
#include "stdlib.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
TTrackControlDlg *TrackControlDlg;
//--------------------------------------------------------------------- 
__fastcall TTrackControlDlg::TTrackControlDlg(TComponent* owner)
	: TForm(owner)
{
}
//---------------------------------------------------------------------------
void  TTrackControlDlg::UpdateTrackInfo()
{
	const char *pChannels[] = { "mono", "stereo" };

	const int topPos 	 = 30;
	const int ctrlHeight = 30;
	const int leftPos	 = 15;
	const int h			 = 17;
	const int w			 = 100;

	RemoveOldTracks();

	const unsigned numTracks = pVTrackInfo->size();
	for(int i=0; i<numTracks; ++i){

		TRACK_INFO  currTrackInfo   = pVTrackInfo->at(i);
		PFILE_INFO  pCurrFileInfo   = currTrackInfo.pFileInfo;

		TLabel *pCurrTrack 			= new TLabel(sbTracks);
		pCurrTrack->Parent			= sbTracks;

		pCurrTrack->Top				= topPos + i*ctrlHeight;
		pCurrTrack->Left			= leftPos + 180;
		pCurrTrack->Height			= h;
		pCurrTrack->Width			= w;
		pCurrTrack->Caption			= pCurrFileInfo->pFileName;//std::string(pCurrFileInfo->pFileName).substr(0, 70).c_str();

		TCheckBox *pCurrTrackSel	= new TCheckBox(sbTracks);
		pCurrTrackSel->Parent		= sbTracks;
		pCurrTrackSel->Top			= topPos + i*ctrlHeight;
		pCurrTrackSel->Left			= leftPos;
		pCurrTrackSel->Height		= h;
		pCurrTrackSel->Width		= 40;
		pCurrTrackSel->Tag			= i+1;
		pCurrTrackSel->Checked		= true;
		pCurrTrackSel->OnClick		= &TrackSelOnClick;

		TCheckBox *pCurrTrackHide	= new TCheckBox(sbTracks);
		pCurrTrackHide->Parent		= sbTracks;
		pCurrTrackHide->Top			= topPos + i*ctrlHeight;
		pCurrTrackHide->Left		= leftPos + 50;
		pCurrTrackHide->Height		= h;
		pCurrTrackHide->Width		= 40;
		pCurrTrackHide->Tag			= 100+i;
		pCurrTrackHide->Checked		= false;
		pCurrTrackHide->OnClick		= &TrackHideOnClick;

		TLabel *pCurrTrackFormat	= new TLabel(sbTracks);
		pCurrTrackFormat->Parent	= sbTracks;
		pCurrTrackFormat->Top       = topPos + i*ctrlHeight;
		pCurrTrackFormat->Left		= leftPos + 90;
		pCurrTrackFormat->Height	= h;
		pCurrTrackFormat->Caption	= String(pChannels[pCurrFileInfo->numChannels-1])+"/"+IntToStr(pCurrFileInfo->samplesPerSec)+"/"+IntToStr(pCurrFileInfo->bitsPerSample);
	  //	pCurrTrackSe
	}
	//parent->SetPlaybackData(pVTrackInfo);
}
void TTrackControlDlg::RemoveOldTracks(){

	const int numComps = sbTracks->ControlCount;

	for(int i=numComps-1; i>=0;--i){
		if ( String(sbTracks->Controls[i]->ClassType()->ClassName()) ==  "TStaticText" ) continue;
		sbTracks->RemoveControl(sbTracks->Controls[i]);
	}
}
void __fastcall TTrackControlDlg::TrackSelOnClick(TObject *Sender)
{
	TCheckBox *pCurrSel = static_cast<TCheckBox*>(Sender);

	TRACK_INFO &currTrackInfo =  pVTrackInfo->at (pCurrSel->Tag-1 );
	currTrackInfo.selForPlayback = pCurrSel->Checked;
}
void __fastcall TTrackControlDlg::TrackHideOnClick(TObject *Sender)
{
	TCheckBox *pCurrSel = static_cast<TCheckBox*>(Sender);

	TRACK_INFO &currTrackInfo =  pVTrackInfo->at ( pCurrSel->Tag-100 );
	currTrackInfo.hide = pCurrSel->Checked;
}
void __fastcall TTrackControlDlg::TrackControlOnShow(TObject *Sender)
{
	 UpdateTrackInfo();
}
//---------------------------------------------------------------------------
void __fastcall TTrackControlDlg::cbPlayOnClick(TObject *Sender)
{
	bool sel = cbPlaybackSelection->Checked;
	const int numComps = sbTracks->ControlCount;

	for(int i=0; i<numComps; ++i){

		TControl *pCurrCtrl = sbTracks->Controls[i];

		if(pCurrCtrl->Tag<100 && pCurrCtrl->Tag>0 /*String(pCurrCtrl->ClassType()->ClassName()) ==  "TCheckBox"*/){
			TCheckBox *pCurrCheckBox 	 = static_cast<TCheckBox*>( pCurrCtrl );
			pCurrCheckBox->Checked 		 = sel;
			TRACK_INFO &currTrackInfo 	 = pVTrackInfo->at ( pCurrCtrl->Tag-1);
			currTrackInfo.selForPlayback = sel;
		}
	}
}
void __fastcall TTrackControlDlg::cbHideOnClick(TObject *Sender)
{
	const bool sel = static_cast<TCheckBox*>( Sender )->Checked;
	const int numComps = sbTracks->ControlCount;

	for(int i=0; i<numComps; ++i){

		TControl *pCurrCtrl = sbTracks->Controls[i];

		if(pCurrCtrl->Tag>=100 /*String(pCurrCtrl->ClassType()->ClassName()) ==  "TCheckBox"*/){
			TCheckBox *pCurrCheckBox 	 = static_cast<TCheckBox*>( pCurrCtrl );
			pCurrCheckBox->Checked 		 = sel;
			TRACK_INFO &currTrackInfo 	 = pVTrackInfo->at ( pCurrCtrl->Tag-100 );
			currTrackInfo.hide = sel;
		}
	}
}
void TTrackControlDlg::SetTrackInfo(std::vector<TRACK_INFO> *pT) {

	pVTrackInfo = pT;
	const int len = pT->size();

	for(int i=0; i<len;++i){
		TRACK_INFO &ti = pT->at(i);
		ti.hide = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TTrackControlDlg::sbHideTracksClick(TObject *Sender)
{
        	parent->HideTracks(pVTrackInfo);

}
//---------------------------------------------------------------------------

