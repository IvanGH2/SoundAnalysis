//$$---- Form CPP ----
//---------------------------------------------------------------------------
/*	Copyright 2020 Ivan Balen
	This file is part of the Sound analysis library.
	The Sound analysis library is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	The Sound analysis library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the Lesser GNU General Public License
	along with the Sound analysis library.  If not, see <http://www.gnu.org/licenses/>.*/
#include <vcl.h>
#pragma hdrstop

#include "SoundAnalysis.h"
#include "ImportDlg.h"
#include "ToneGenDlg.h"
#include "FilterGeneratorDlg.h"
#include "SpectralTool.h"
#include "StftControl.h"
#include "ProgressIndicator.h"
#include "WaveDisplayContainer.h"
#include "Winuser.h"
#include "TrackControl.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

using namespace nsToneGenerator;

std::vector<TRACK_INFO> vTrackInfo;
TForm1 *Form1;
//WaveDisplayContainerManager::pWaveContainerManager;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner),
	rWaveDisplayManager(WaveDisplayContainerManager::GetInstance(pWaveScroller)),
	rWavePlayer(TWavePlayer<WinWavePlayer>::GetInstance( Handle))
{
	closingApp = false;
	maxRecentFiles = 20;
	DoubleBuffered = true;
	pPlayerCtrls->Parent = panMainCtrls;
	pPlayerCtrls->Align = alLeft;
	panMainCtrls->DoubleBuffered = true;
	pPlayerCtrls->DoubleBuffered = true;
	Panel1->DoubleBuffered = true;
	rWavePlayer.SetPlaybackPosCallback(&WaveDisplayContainerManager::OnPlaybackMessage);
	rWavePlayer.SetPlaybackMsgCallback(&SetPlayerCtrlsState);
	rWaveDisplayManager.SetPlaybackMsg(&SetPlayerCtrlsState) ;
	rWaveDisplayManager.SetMainWndCallback(&MainWndCallback);
	rWaveDisplayManager.SetContextMenu(pmContainerMenu);

	 ReadFileRecent();
}
//---------------------------------------------------------------------------
int TForm1::GetDesktopClientHeight(){

   RECT rect;

   bool success = ::GetClientRect(GetDesktopWindow(), &rect);
   return success ? rect.bottom-rect.top : Height;
}

void TForm1::DrawTrackTimePanel(unsigned totalTime, unsigned selTime, unsigned startTime, unsigned zoomLevel){

	 trackTimePanel.SetWidth(pTrackTimer->Width);
	 trackTimePanel.SetHeight(pTrackTimer->Height);
	 trackTimePanel.SetTrackTime(selTime);
	 trackTimePanel.SetTotalTrackTime(totalTime);
	 trackTimePanel.SetStartTime(startTime);
	 trackTimePanel.SetZoomLevel(zoomLevel);
	// drawTimePanel = true;
}
void  TForm1::OpenFile(std::string fName){

	SetSoundInput();
	SoundInput * pSndIn = static_cast<SoundInput*>(pSoundData);

	try{
		pSndIn->SetViewPortWidth(pWaveScroller->ClientWidth - (cWaveformRightMargin + cWaveCtrlWidth + cAmpWidth)-4);
		pSndIn->Open(fName);
	}catch(SoundIOException &e){
		ShowMessage(e.what());
		return;
	}


	unsigned waveId = DoDraw();
	//intializing the wave player
	rWavePlayer.AddTrack(new WinWavePlayer(),&pSoundData->GetHeaderInfo()->wfex,
	pSoundData->GetData(), pSoundData->GetDataSize(), waveId);

	if(pSndIn->IsMultipleBuffers()){

		rWavePlayer.SetDataBuffers_(pSndIn->GetMemoryBuffers(),
									pSndIn->GetMemoryBufferSize(),
									pSndIn->GetLastMemoryBufferSize());
	}
	DrawTrackTimePanel(pSoundData->GetTrackTime(),pSoundData->GetTrackTime(),0,1);
	pTrackTimer->Repaint();

	DisplayFileInfo();
}

void __fastcall TForm1::OpenSoundFile(TObject *Sender){
	float *pData;
 
	if(OpenDialog1->Execute() && OpenDialog1->FileName != 0 ){

		AnsiString fileName = OpenDialog1->FileName;
		AnsiString fileExt = fileName.SubString(fileName.Length()-2, fileName.Length());
		if(fileExt != "wav"){
			ShowMessage("For files other than wave (*.wav), use the Import action dialog!");
			return;
		}
	SetSoundInput();
	SoundInput * pSndIn = static_cast<SoundInput*>(pSoundData);

	try{
		((SoundInput*)pSoundData)->Open(OpenDialog1->FileName.c_str());
	}catch(SoundIOException &e){
		ShowMessage(e.what());
		return;
	}

	unsigned waveId = DoDraw();//DrawWave(pData, numSamples, numChannels);

//intializing the wave player
	rWavePlayer.AddTrack(new WinWavePlayer(),&pSoundData->GetHeaderInfo()->wfex, pSoundData->GetData(),
	pSoundData->GetDataSize(), waveId);
   //rWavePlayer.SetInputData(&pSoundData->GetHeaderInfo()->wfex, (char*)rWaveDisplayManager.GetActiveContainer()->GetDisplayData()->GetData(),
   //pSoundData->GetDataSize(), waveId);
	if(pSndIn->IsMultipleBuffers()){
		rWavePlayer.SetDataBuffers_(pSndIn->GetMemoryBuffers(),
									pSndIn->GetMemoryBufferSize(),
									pSndIn->GetLastMemoryBufferSize());
	   //	rWavePlayer.SetPlaybackRange();
	}

	DrawTrackTimePanel(pSoundData->GetTrackTime(),pSoundData->GetTrackTime(),0,1);
	pTrackTimer->Repaint();
	vRecentFiles.insert(std::string(OpenDialog1->FileName.c_str()));

	DisplayFileInfo();

	}
}
void __fastcall TForm1::OpenRawFile(PRAW_DATA_INFO pRdi){

	pSoundData->SetHeaderInfo(pRdi);

	((SoundInput*)pSoundData)->OpenRaw(pSoundData->getFileName());

	unsigned waveId = DoDraw();

	rWavePlayer.AddTrack(new WinWavePlayer(),&pSoundData->GetHeaderInfo()->wfex, pSoundData->GetData(),
	pSoundData->GetDataSize(), waveId);

	DisplayFileInfo();
}
void __fastcall TForm1::Import1Click(TObject *Sender)
{
	OpenDialog1->DefaultExt = ".raw";
	if(OpenDialog1->Execute() && OpenDialog1->FileName != 0 ){
		 SetSoundInput();
		 pSoundData->SetFileName(OpenDialog1->FileName.c_str());
		 TRawImportDialog* rawImportDlg = new TRawImportDialog(this);
		 rawImportDlg->Show();
	}
}
void __fastcall TForm1::DisplayFileInfo(){

	StatusBar1->Panels->Items[0]->Width = 0.60 * StatusBar1->Width;
	StatusBar1->Panels->Items[1]->Width = 0.25 * StatusBar1->Width;
	StatusBar1->Panels->Items[2]->Width = 0.15 * StatusBar1->Width;

	StatusBar1->Panels->Items[0]->Text = "";
	StatusBar1->Panels->Items[1]->Text = "sample rate\\bit depth\\channels: ";
	StatusBar1->Panels->Items[2]->Text = "length in samples: ";

	SoundIO *pSd = rWaveDisplayManager.GetActiveContainer()->GetSoundData();
	WAVEFORMATEX &wf = pSd->GetHeaderInfo()->wfex;
	AnsiString  tFormat = IntToStr(wf.nSamplesPerSec) + "\\" +  IntToStr(wf.wBitsPerSample) + "\\" + IntToStr(wf.nChannels);
	StatusBar1->Panels->Items[0]->Text = StatusBar1->Panels->Items[0]->Text + pSd->getFileName().c_str();
	StatusBar1->Panels->Items[1]->Text = StatusBar1->Panels->Items[1]->Text + tFormat;
	StatusBar1->Panels->Items[2]->Text = StatusBar1->Panels->Items[2]->Text + pSd->GetNumSamples();

}

//---------------------------------------------------------------------------
unsigned  TForm1::DoDraw(){


	TWaveDisplayContainer::SetScrollBarVisible(pWaveScroller->VertScrollBar->Visible);

	rWaveDisplayManager.SetSoundData(pSoundData);
//	rWaveDisplayManager.SetFileName(pSoundData->getFileName()); //ovo maknuti jer bi se ime trebalo dobiti iz SoundIO
	return rWaveDisplayManager.AddContainer();  //this creates a new waveform container
}
unsigned int TForm1::DrawWave(float *pData, unsigned int numSamples, unsigned int numChannels){

		 return DoDraw();

}
AnsiString& TForm1::CreateFilterName(EFilterType eFilterType, unsigned cutoffLo, unsigned cutoffHi){

	switch(eFilterType){
		case nsDigitalFilter::LOPASS:
			filterName = IntToStr(cutoffLo)+"_low";
			break;
		case nsDigitalFilter::HIPASS:
			filterName = IntToStr(cutoffHi)+"_high";
			break;
		case nsDigitalFilter::BANDPASS:
			filterName = IntToStr(cutoffLo)+"_"+IntToStr(cutoffHi);
			break;
		}
   return filterName;
}
void TForm1::DrawFilteredWave(EFilterType eFilterType, EWindow eWnd, unsigned cutoffFreqLo,
unsigned cutoffFreqHi, unsigned filterLength, vector<PFILTERBAND_INFO> &vBandInfo, bool invert ){

	int numFilters = 0;
	WaveDisplayData *pWaveDisplayData = rWaveDisplayManager.GetActiveContainer()->GetDisplayData();
	float *pSelData = pWaveDisplayData->GetCurrentData();
	pSoundData 		= rWaveDisplayManager.GetActiveContainer()->GetSoundData();

   //	PWAVE_HDR pWaveHdr = pSoundData->GetHeaderInfo();
	const unsigned numSamples 	 = pSoundData->GetNumSamples();
   //	const unsigned numChannels   = pSoundData->GetHeaderInfo()->wfex.nChannels;
	const unsigned samplesPerSec = pSoundData->GetHeaderInfo()->wfex.nSamplesPerSec;

	RAW_DATA_INFO rdi;
	rdi.encoding = BIT_32;
	rdi.numChannels = 1;
	rdi.sampleRate = pSoundData->GetHeaderInfo()->wfex.nSamplesPerSec;

	numToProcess = eFilterType == BANDPASS ? vBandInfo.size()+1:1;

	DigitalFilter *pDigitalFilter = new DigitalFilter(eFilterType, eWnd, filterLength, cutoffFreqLo, cutoffFreqHi, samplesPerSec );
	pDigitalFilter->SetInverted(invert);
	pDigitalFilter->CreateFilterKernel();
	   //	pDigitalFilter->SetProgressCallback(&UpdateProgress);
	pDigitalFilter->SetMainWndCallback(&MainWndCallback);

  //	pDigitalFilter->UseBackgroundThread(true);
	pDigitalFilter->SetNumFilters(eFilterType == BANDPASS ? vBandInfo.size()+1 : 1);

	SoundInput *pSndIn = static_cast<SoundInput*>(pSoundData);

	if(pSndIn->IsMultipleBuffers()){//numBuffers>0 only when we're dealing with multiple buffers
		pDigitalFilter->SetViewPortRange(pWaveDisplayData->GetStartSample(), pWaveDisplayData->GetEndSample());
		pDigitalFilter->SetDataBuffers(pSndIn->GetMemoryBuffers(),
		pSndIn->GetMemoryBufferSize(),
		pSndIn->GetLastMemoryBufferSize());
	}

	pDigitalFilter->SetFilterName( CreateFilterName(eFilterType, cutoffFreqLo, cutoffFreqHi ).c_str() );
	pDigitalFilter->ApplyFilter( pSelData, numSamples );

	numFilters++; //???

	if(eFilterType == BANDPASS){
		for(int i=0;i<vBandInfo.size();i++){
			float cutoffLo = vBandInfo.at(i)->cutoffLo;
			float cutoffHi = vBandInfo.at(i)->cutoffHi;
			DigitalFilter *pDigitalFilter = new DigitalFilter(eFilterType, eWnd, filterLength, cutoffLo, cutoffHi, pSoundData->GetHeaderInfo()->wfex.nSamplesPerSec );
			pDigitalFilter->SetMainWndCallback(&MainWndCallback);

			pDigitalFilter->CreateFilterKernel(); //ovo ne treba jer smo ju veæ kreirali, ali filter mora biti static

		    if(pSndIn->IsMultipleBuffers()){//numBuffers>0 only when we're dealing with multiple buffers
				pDigitalFilter->SetViewPortRange(pWaveDisplayData->GetStartSample(), pWaveDisplayData->GetEndSample());
				pDigitalFilter->SetDataBuffers(pSndIn->GetMemoryBuffers(),
				pSndIn->GetMemoryBufferSize(),
				pSndIn->GetLastMemoryBufferSize());
			}

		 //	pDigitalFilter->UseBackgroundThread(true);
			pDigitalFilter->SetFilterName( CreateFilterName( eFilterType, cutoffLo, cutoffHi ).c_str() );
			pDigitalFilter->ApplyFilter(pSelData, numSamples);

		   //	pSoundData->setFileName(filterName.c_str());
			numFilters++;
			if(!pDigitalFilter->IsBackgroundThread())
				AddFilter(pDigitalFilter);
		}
	}
	if(pDigitalFilter->IsBackgroundThread()){
		UpdateProgressFromThread();
	}else {
		AddFilter(pDigitalFilter);
	}
}

void TForm1::AddFilter(DigitalFilter * pDf){

	SoundIO *pSd = rWaveDisplayManager.GetActiveContainer()->GetSoundData();
	const unsigned numSamples = pSd->GetNumSamples();

	RAW_DATA_INFO rdi;
	rdi.encoding = BIT_32;
	rdi.numChannels = 1;
	rdi.sampleRate = pSd->GetHeaderInfo()->wfex.nSamplesPerSec;

	SetSoundOutput();

	pSoundData->SetHeaderInfo(&rdi);
	pSoundData->SetNumSamples(numSamples);
	pSoundData->SetDataSize(pDf->GetDataLen() * sizeof(float));
	pSoundData->SetData((char*)pDf->GetFilteredData());

	pSoundData->SetFileName(pDf->GetFilterName());

	unsigned waveId =	DoDraw();//DrawWave((float*)pSoundData->GetData(), numSamples, numChannels);
	rWaveDisplayManager.GetActiveContainer()->SetSoundSourceObject(pDf, DIGITAL_FILTER);

	rWavePlayer.AddTrack(new WinWavePlayer(),&pSoundData->GetHeaderInfo()->wfex, pSoundData->GetData(),
		pSoundData->GetDataSize(), waveId);
}
void TForm1::UpdateProgressFromThread(){

	static int numFilters = 0;
	numFilters = numFilters == 0 ? DigitalFilter::GetNumFilters() : numFilters;
	//Caption = IntToStr(numFilters);
	while( numFilters > 0 ) {
		int totalProgress = DigitalFilter::GetTotalProgressValue();
		DigitalFilter *pCurrFilter = reinterpret_cast<DigitalFilter*>(DigitalFilter::GetCurrentFilter());
			if( pCurrFilter ) {
				if(totalProgress > 0){
					UpdateActionProgress(&FILTER_PROCESSING_PROGRESS(pCurrFilter->GetProgressValue(),
					totalProgress, pCurrFilter->GetFilterName()));
				}

				if( pCurrFilter->IsBackgroundThreadFinished()){
					AddFilter( pCurrFilter );
					numFilters--;
			}
		}
	}
}
void __fastcall TForm1::Button1Click(TObject *Sender)
{
	rWaveDisplayManager.GetActiveContainer()->ShowBmp();
}
//---------------------------------------------------------------------------
bool TForm1::AllowZoomIn(const TWaveDisplayContainer *pContainer ){

	if(pContainer->GetContainerView()==WAVEFORM) return true;

	const unsigned newViewPortSamples = pContainer->GetDisplayData()->GetViewPortSamples() / 2;
	const unsigned frameLen =  pContainer->GetSpectralData()->GetFrameLength();

	   if(frameLen>newViewPortSamples) {
		   MessageBox( 0, "Zoom in not possible because the number of remaining samples is fewer than "
		   "the frame length! Either switch to the waveform view or reduce the frame length.", (LPTSTR)"Sound analyzer", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
		   return false;
	   }
	return true;
}
void __fastcall TForm1::ZoomInBtnClick(TObject *Sender)
{
	 ZoomIn();
}
void  TForm1::ZoomIn(){

    const TWaveDisplayContainer *pContainer = rWaveDisplayManager.GetActiveContainer();

	const bool zoomInAllowed = AllowZoomIn(pContainer);
	if(!zoomInAllowed)return;

	maxZoom = pContainer->ZoomIn();
	minZoom = false;

	if(maxZoom)   {
		ZoomInBtn->Enabled  	= false;
		ZoomOutBtn->Enabled 	= true;
		ZoomOutFullBtn->Enabled = true;
	}
	if(!minZoom ) {
		ZoomOutBtn->Enabled		= true;
		ZoomOutFullBtn->Enabled = true;
	}
	trackTimeMilis = pSoundData->GetTimeForSamples(pContainer->GetDisplayData()->GetViewPortSamplesTotal());
	startTime = pSoundData->GetTimeForSample(pContainer->GetDisplayData()->GetStartSample());
   //	DrawTrackTimer();
   //	const unsigned viewPortTime = waveDisplay!=0 ? rWaveDisplayManager.GetActiveContainer()->GetDisplayData()->GetCurrentZoomLevel();
	DrawTrackTimePanel(pSoundData->GetTrackTime(),trackTimeMilis, startTime,
					   rWaveDisplayManager.GetActiveContainer()->GetDisplayData()->GetCurrentZoomLevel());

	pTrackTimer->Repaint();

}
//---------------------------------------------------------------------------
void __fastcall TForm1::ZoomOutBtnClick(TObject *Sender)
{
	ZoomOut();
}
void  TForm1::ZoomOut(){

	const TWaveDisplayContainer *pContainer = rWaveDisplayManager.GetActiveContainer();

	minZoom = pContainer->ZoomOut();
	maxZoom = false;

	if(minZoom)   {
		ZoomOutBtn->Enabled = false;
		ZoomInBtn->Enabled = true;
	}
	if(!maxZoom) ZoomInBtn->Enabled = true;

	trackTimeMilis = pSoundData->GetTimeForSamples(pContainer->GetDisplayData()->GetViewPortSamplesTotal());
	startTime = pSoundData->GetTimeForSample(pContainer->GetDisplayData()->GetStartSample());
	DrawTrackTimePanel(pSoundData->GetTrackTime(),trackTimeMilis, startTime,
	rWaveDisplayManager.GetActiveContainer()->GetDisplayData()->GetCurrentZoomLevel());
	pTrackTimer->Repaint();

}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormOnResize(TObject *Sender)
{
	if (closingApp) {
		return;
	}
	const unsigned numContainers = rWaveDisplayManager.GetContainersCount();
	if(numContainers>0){
		 rWaveDisplayManager.ResizeContainers(pWaveScroller->ClientWidth, cWaveformHeight);
		 AddTimePanel();
		 DisplayFileInfo();
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Close1Click(TObject *Sender)
{
	rWaveDisplayManager.GetActiveContainer()->RemoveContainer();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Adjustvertically1Click(TObject *Sender)
{
	 rWaveDisplayManager.AdjustContainersVertically();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Zoomnormal1Click(TObject *Sender)
{
	 rWaveDisplayManager.AdjustContainersNormal();
}
void __fastcall TForm1::ProcessWaveDoneMsg(TMessage& doneMsg)
{
	rWavePlayer.ProcessWaveMsg(doneMsg);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::TrackTimerOnPaint(TObject *Sender)
{
	 if(rWaveDisplayManager.GetContainersCount()>0 && trackTimePanel.GetTrackTime()>0 ){
		trackTimePanel.SetTrackWidth( rWaveDisplayManager.GetActiveContainer()->GetTrackWidth() );
		trackTimePanel.DrawTimeLabels();
	   //	pTrackTimer->Canvas->Draw(0, 0, trackTimePanel.GetTimeBitmap());
		DrawTimePanel();
		//Caption = IntToStr( trackTimePanel.GetEvery() )+":	"+IntToStr(trackTimePanel.GetLen() );
	 }
}
void TForm1::DrawTimePanel()
{
	 if(rWaveDisplayManager.GetContainersCount()>0 && trackTimePanel.GetTrackTime()>0 ){

		const unsigned pos = trackTimePanel.GetPlaybackIndicatorPos(
			rWaveDisplayManager.GetActiveContainer()->GetPlaybackIndicatorPosition() );

		pTrackTimer->Canvas->Draw(0, 0, trackTimePanel.GetTimeBitmap());

		trackTimePanel.DrawPlaybackIndicatorAtPos(pTrackTimer->Canvas, pos);
	 }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SpeedButton1Click(TObject *Sender)
{
	Caption = "working";
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PlayerCtrlsOnClick(TObject *Sender)
{
	 static bool toggle ;
	 TSpeedButton *pCurrent = static_cast<TSpeedButton*>(Sender);
	 try{
	 switch (pCurrent->Tag) {
		  case nsPlayback::PAUSED:
			toggle = !toggle;
			if(toggle){
				rWavePlayer.PausePlayback();
				pCurrent->Caption = "Resume";
			} else{
				rWavePlayer.ResumePlayback();
				pCurrent->Caption = "Pause";
			}

			break;
		  case nsPlayback::STARTED:   //
			rWavePlayer.SetPlaybackTracks( GetPlaybackData() );
			rWavePlayer.StartPlayback();
			break;
		  case nsPlayback::STOPPED:
			rWavePlayer.StopPlayback();
			break;
		  }
	 }catch(SoundAnalyzerException &ex){
		MessageBox( (HWND)NULL, ex.what(), (LPTSTR)"Sound analyzer", MB_OK | MB_ICONINFORMATION);
		return;
		}
	 SetPlayerCtrlsState((nsPlayback::EPlaybackEvent)pCurrent->Tag,0);
}
vector<nsPlayback::PTRACK_SELECTION>* TForm1::GetPlaybackData(){

	int i, j;
	if( !cbTrackControl->Checked  ) return rWaveDisplayManager.GetSelectedForPlayback();
	vector<nsPlayback::PTRACK_SELECTION>* pVPlaybackData;
	std::vector<TRACK_INFO> *pVTrackInfo = TrackControlDlg->GetTrackInfo();
	if( pVTrackInfo->size() > 0 ){
		 // pass 1, change the waveform colour based on the current track being selected for playback or not
		for(j=0;j<pVTrackInfo->size();++j){
				TRACK_INFO &ti = pVTrackInfo->at( j );
				rWaveDisplayManager.GetContainerById(ti.pFileInfo->fileId)->SelectForPlayback(ti.selForPlayback);
				//make the first track active
				if(j==0)  rWaveDisplayManager.GetContainerById(ti.pFileInfo->fileId)->ActivateContainer();
			}
		  //pass 2  -- remove the tracks not selected for playback from the playback list
		pVPlaybackData = rWaveDisplayManager.GetSelectedForPlayback();
		const int playSize = pVPlaybackData->size();
		for(i=playSize-1; i >= 0; --i){

			PTRACK_SELECTION pTrackSel = pVPlaybackData->at( i ) ;

			for(j=0;j<pVTrackInfo->size();++j){
				TRACK_INFO &ti = pVTrackInfo->at( j );

				if(ti.pFileInfo->fileId == pTrackSel->waveId && !ti.selForPlayback){
					pVPlaybackData->erase(pVPlaybackData->begin()+i);
					break;
				}
			}
		}
	}
	return  pVPlaybackData;
}
void TForm1::HideTracks( std::vector<TRACK_INFO> *pVTrackInfo ){

	for(int j=0;j<pVTrackInfo->size();++j){
				TRACK_INFO &ti = pVTrackInfo->at( j );
				rWaveDisplayManager.GetContainerById(ti.pFileInfo->fileId)->SetHidden(ti.hide);
			}
	rWaveDisplayManager.RepositionContainers();
}
 //EMainWndNotification
void TForm1::MainWndCallback(nsWaveDisplay::EMainWndNotification eNotification, const void *p){

	 PTRACK_POSITION pTs;
	 PTRACK_TIME pTrackTime;
	 switch(eNotification){

		case nsWaveDisplay::INV_TRACK_POS:
			pTs = (PTRACK_POSITION)p;
			SetWaveformInfo( pTs->startSample, pTs->endSample, pTs->currSample );
			DrawTimePanel();
			break;
		case nsWaveDisplay::INV_TRACK_TIME:
			pTrackTime = (PTRACK_TIME)p;
			DrawTrackTimePanel(pTrackTime->trackTime,pTrackTime->selectionTime, pTrackTime->startTime,
			rWaveDisplayManager.GetActiveContainer()->GetDisplayData()->GetCurrentZoomLevel());
			pTrackTimer->Repaint();
		   	if(pSpectralForm && pSpectralForm->Visible) pSpectralForm->ContainerChanged();
			DisplayFileInfo();
			break;
		case nsWaveDisplay::CONTAINER_ADDED:
		case nsWaveDisplay::CONTAINER_REMOVED:
		case nsWaveDisplay::CONTAINER_RESIZED:
			ShowScrollbarsIfNeeded(*(unsigned*)p); //*p ->totalHeight
			EnableActions();
			break;
		case nsWaveDisplay::SHOW_ACTION_PROGRESS:
			UpdateActionProgress((PFILTER_PROCESSING_PROGRESS)p);
	 }

}
void TForm1::UpdateActionProgress(PFILTER_PROCESSING_PROGRESS progressInfo){


   frmProgressDlg->pbTotalProcessing->Position = progressInfo->progressTotalPercentage;
   frmProgressDlg->pbProcessing->Position = progressInfo->progressPercentage;
   frmProgressDlg->lblProcessing->Caption = progressInfo->currProcessFile.c_str();

	if(frmProgressDlg->pbTotalProcessing->Position >= 95) {
		frmProgressDlg->pbTotalProcessing->Position = 0;
		frmProgressDlg->Close();
   }else{
		if(!(frmProgressDlg->Visible))	frmProgressDlg->Show();
			frmProgressDlg->lblProcessing->Update();
		}
}

void TForm1::EnableActions(){
	const bool enabled = rWaveDisplayManager.GetContainersCount()>0 ? true : false;

	Save1->Enabled =   enabled;
	Saveas1->Enabled = enabled;
	Close1->Enabled  = enabled;
	Closeall1->Enabled  = enabled;
	Zoomin1->Enabled = enabled;
	Zoomout1->Enabled = enabled;
	Zoomnormal1->Enabled = enabled;
	Adjustvertically1->Enabled = enabled;
	Shrink1->Enabled = enabled;
	Filter1->Enabled = enabled;
	STFT1->Enabled = enabled;
	ZoomInBtn->Enabled = enabled;
	ZoomInFullBtn->Enabled = enabled;
	zoomNormalBtn->Enabled = enabled;
	zoomVertBtn->Enabled = enabled;

	tbTransparency->Enabled = enabled;//rWaveDisplayManager.GetActiveContainer()->GetContainerView()!=MIXED ? false :
}
void TForm1::ShowScrollbarsIfNeeded(unsigned totalHeight){

   if(totalHeight > pWaveScroller->Height){
		pWaveScroller->VertScrollBar->Range = totalHeight;
		pWaveScroller->VertScrollBar->Visible=true;
	}else{
		pWaveScroller->VertScrollBar->Visible=false;
	}
}
void TForm1::SetWaveformInfo(unsigned startSample, unsigned endSample, unsigned currSample){

	pStartPos->Text = IntToStr(startSample);
	pEndPos->Text = IntToStr(endSample);
	pPlaybackPos->Text = IntToStr(currSample);
	this->startSample = startSample;  //????
}
/**
  this method is called from the spectral dialog in response to the Preview button being clicked
*/
void  TForm1::PlaySoundPreview(char *pData, unsigned size){

	pBtnPlay->Click();
}
/**
  this method simply toggles the states (enabled/disabled) in response to the Play/Pause/Stop buttons being clicked
*/
void  TForm1::SetPlayerCtrlsState(nsPlayback::EPlaybackEvent e, unsigned containerId){

	  switch(e){
		case nsPlayback::OPENED:
			pPlayerCtrls->Enabled = true;
			pBtnPause->Enabled = false;
			pBtnStop->Enabled = false;
			break;
		case	nsPlayback::CLOSED:
	 // pPlayerCtrls->Enabled = false;
			rWavePlayer.RemoveTrack(containerId);
			break;
		case	nsPlayback::CLOSED_ALL:     //all waveforms are closed
			pPlayerCtrls->Enabled = false;
			rWavePlayer.RemoveTrack(containerId);
			break;
		case	nsPlayback::STARTED:
	 // pPlayerCtrls->Enabled = false;
			pBtnPlay->Enabled = false;
			pBtnStop->Enabled = true;
			pBtnPause->Enabled = true;
			break;
		case	nsPlayback::PAUSED:
	 // pPlayerCtrls->Enabled = false;
			pBtnPlay->Enabled = false;
	 // pBtnStop->Enabled = true;
			pBtnPause->Enabled = true;
			break;
		case	nsPlayback::STOPPED:
	 // pPlayerCtrls->Enabled = false;
			pBtnPlay->Enabled  = true;
			if ( rWaveDisplayManager.GetContainersCount()<2 ){
				pBtnStop->Enabled  = false;
				pBtnPause->Enabled = false;
			}
			break;
	  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ZoomOutFullBtnClick(TObject *Sender)
{
	minZoom = rWaveDisplayManager.GetActiveContainer()->ZoomOutFull();
	maxZoom = false;

	if(minZoom){
		ZoomOutFullBtn->Enabled = false;
		ZoomOutBtn->Enabled = 	  false;
		ZoomInBtn->Enabled = 	  true;
		ZoomInFullBtn->Enabled =  true;
	}
	if(!maxZoom) ZoomInBtn->Enabled = true;
	const TWaveDisplayContainer *pContainer = rWaveDisplayManager.GetActiveContainer();
	trackTimeMilis = pSoundData->GetTimeForSamples(pContainer->GetDisplayData()->GetViewPortSamplesTotal());
	DrawTrackTimePanel(trackTimeMilis,trackTimeMilis,0,1);
	pTrackTimer->Repaint();
	startTime = 0;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ZoomInFullBtnClick(TObject *Sender)
{
   maxZoom = rWaveDisplayManager.GetActiveContainer()->ZoomInFull();
   minZoom = false;

	if(maxZoom)   {
		ZoomInBtn->Enabled = false;
		ZoomInFullBtn->Enabled =  false;
		ZoomOutBtn->Enabled = true;
		ZoomOutFullBtn->Enabled = true;
	}
	if(!minZoom ) {
		ZoomOutBtn->Enabled = true;
		ZoomOutFullBtn->Enabled = true;
	}
	const TWaveDisplayContainer *pContainer = rWaveDisplayManager.GetActiveContainer();
	trackTimeMilis = pSoundData->GetTimeForSamples(pContainer->GetDisplayData()->GetViewPortSamplesTotal());
	startTime = pSoundData->GetTimeForSample(pContainer->GetDisplayData()->GetStartSample());
	DrawTrackTimePanel(trackTimeMilis,trackTimeMilis,startTime,
		rWaveDisplayManager.GetActiveContainer()->GetDisplayData()->GetCurrentZoomLevel());
	pTrackTimer->Repaint();
	//DrawTrackTimer();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::zoomVertBtnClick(TObject *Sender)
{
	rWaveDisplayManager.AdjustContainersVertically();	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::zoomNormalBtnClick(TObject *Sender)
{
	rWaveDisplayManager.AdjustContainersNormal();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Shrink1Click(TObject *Sender)
{
	rWaveDisplayManager.AdjustContainersMinimum();
}
//--------------------------------------------------------------------------
void __fastcall TForm1::Tone1Click(TObject *Sender)
{
	TToneGeneratorDlg *pToneGenDlg = new TToneGeneratorDlg(this);
	pToneGenDlg->Show();
}
void  TForm1::AddTimePanel(){

	trackTimeMilis = pSoundData->GetTimeForSamples(rWaveDisplayManager.GetActiveContainer()->GetDisplayData()->GetViewPortSamples());
	startTime = pSoundData->GetTimeForSample(rWaveDisplayManager.GetActiveContainer()->GetDisplayData()->GetStartSample());
	totalTrackTimeMilis = trackTimeMilis;
	DrawTrackTimePanel(trackTimeMilis, trackTimeMilis, startTime,
		rWaveDisplayManager.GetActiveContainer()->GetDisplayData()->GetCurrentZoomLevel());
	pTrackTimer->Repaint();
}
void  TForm1::GenerateTone(nsToneGenerator::TONE_INFO& const rToneInfo, std::vector<nsToneGenerator::PHARMONICS_INFO>& pHarmonicsInfo)
{
	SetSoundOutput();
	DoGenerateTone(rToneInfo, pHarmonicsInfo);
} /*
void  TForm1::GenerateMatchedSound(){

}*/
void  TForm1::DoGenerateTone(nsToneGenerator::TONE_INFO& const rToneInfo, std::vector<nsToneGenerator::PHARMONICS_INFO>  &rHarmonicsInfo)
{
	RAW_DATA_INFO rdi;

	if(!pTone){
		switch(rToneInfo.eToneType){

		case nsToneGenerator::SINE:
			pTone = new SineTone();
			break;
		case nsToneGenerator::SQUARE:
			pTone = new SquareTone();
			break;
		case nsToneGenerator::TRIANGLE:
			pTone = new TriangleTone();
			break;
		case nsToneGenerator::SAWTOOTH:
			pTone = new SawToothTone();
			break;
		}
	}
	//get amps
	pTone->SetSampleRate(rToneInfo.sampleRate);
	pTone->SetFrequency(rToneInfo.freq);
	pTone->SetAmplitudes(rToneInfo.amp);
	pTone->SetPhases(rToneInfo.phase );
	pTone->SetDuration(rToneInfo.duration);

	if(rHarmonicsInfo.size() > 0){
		for(int i=0; i<rHarmonicsInfo.size();i++){
		PHARMONICS_INFO pHarmInfo = rHarmonicsInfo.at(i);
		//set amplitudes for the harmonics
		pTone->SetAmplitudes(pHarmInfo->amp);
		pTone->SetPhases(pHarmInfo->phase);
		}
	}
	pTone->SetNumPartials(rHarmonicsInfo.size()+1);//fundamental + partials
	pTone->GenerateTone();

	if(pSoundData){

		rdi.encoding = BIT_32;
		rdi.numChannels = 1;
		rdi.sampleRate = rToneInfo.sampleRate;

		pSoundData->SetFileName(IntToStr((int)rToneInfo.freq).c_str());
	   //	pSoundData->SetFileSize(pTone->getDataLen() * 4);
		pSoundData->SetDataSize(pTone->GetDataLen() * sizeof(float));
		pSoundData->SetHeaderInfo(&rdi);
		pSoundData->SetData((char*)pTone->GetData());

		const unsigned waveId = DoDraw();
		//pass the ToneGenerator object to the container in order to free it once the container has been destroyed
		rWaveDisplayManager.GetActiveContainer()->SetSoundSourceObject(pTone, TONE_GENERATOR);
		//init the playback by passing the sound samples to the wave player
		rWavePlayer.AddTrack(new WinWavePlayer(),&pSoundData->GetHeaderInfo()->wfex, pSoundData->GetData(),
		pSoundData->GetDataSize(), waveId);

		trackTimeMilis = pSoundData->GetTimeForSamples(rWaveDisplayManager.GetActiveContainer()->GetDisplayData()->GetViewPortSamples());
		startTime = pSoundData->GetTimeForSample(rWaveDisplayManager.GetActiveContainer()->GetDisplayData()->GetStartSample());
		totalTrackTimeMilis = trackTimeMilis;
	//	DrawTrackTimer();
		DrawTrackTimePanel(trackTimeMilis, trackTimeMilis, startTime,
		rWaveDisplayManager.GetActiveContainer()->GetDisplayData()->GetCurrentZoomLevel());
		pTrackTimer->Repaint();
	}
}
//---------------------------------------------------------------------------
void  TForm1::SaveFile(){

	  if(SaveDialog1->Execute() && SaveDialog1->FileName != 0 ){
		pSoundData = rWaveDisplayManager.GetActiveContainer()->GetSoundData();
		((SoundOutput*)pSoundData)->Save(SaveDialog1->FileName.c_str(), WAVE );
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Save1Click(TObject *Sender)
{
	SaveFile();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Applyfilter1Click(TObject *Sender)
{
	TpFilterDlg *pFilterDlg = new TpFilterDlg(this, pSoundData->GetHeaderInfo()->wfex.nSamplesPerSec);
	pFilterDlg->Show();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Filter1Click(TObject *Sender)
{
	TpFilterDlg *pFilterDlg = new TpFilterDlg(this, pSoundData->GetHeaderInfo()->wfex.nSamplesPerSec);
	pFilterDlg->Show();
}

void __fastcall TForm1::STFT1Click(TObject *Sender)
{
	SwitchContainerView();
}
void TForm1::SwitchContainerView(){

   const TWaveDisplayContainer *pContainer = rWaveDisplayManager.GetActiveContainer();

   bool spectrogramOn = pContainer->GetContainerView() != WAVEFORM;

   if(!spectrogramOn){   //draw  a spectrogram
	try{
		cbSpectDlg->Enabled = true;
		pContainer->EnableMultiThreading(cbEnableThreads->Checked);
		pContainer->DrawSpectrogram();
		//if(rWaveDisplayManager.GetActiveContainer()->GetContainerView() == SPECTROGRAM)

	}catch(SoundAnalyzerException &ex){
		MessageBox( (HWND)NULL, ex.what(), (LPTSTR)"Sound analyzer", MB_OK | MB_ICONINFORMATION);
	}
	Spectral1->Enabled = true;  //???
   }else{   //draw  a waveform
		pContainer->DrawWaveform();
		cbSpectDlg->Enabled = false;
		if(pStftControl && pStftControl->Visible)
			if(rWaveDisplayManager.GetContainersCount()<2)
				pStftControl->Close();

		Spectral1->Enabled = false;
   }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Spectral1Click(TObject *Sender)
{
   /*	if(pSpectralForm) delete pSpectralForm;

	pSpectralForm = new  TpSpectralForm(this);  */
	pSpectralForm->Show();
}
void __fastcall TForm1::FormOnShortCut(TWMKey &Msg, bool &Handled)
{

	if(Msg.CharCode == 0x25){      //LEFT ARROW
		rWaveDisplayManager.GetActiveContainer()->MovePlaybackIndicator(BACKWARD);
		Handled = true;
	}else if(Msg.CharCode == 0x27){ //RIGHT ARROW
		rWaveDisplayManager.GetActiveContainer()->MovePlaybackIndicator(FORWARD);
		Handled = true;
	}
}
void __fastcall TForm1::btnSelNormalClick(TObject *Sender)
{
	rWaveDisplayManager.SetSpectrogramSelectionMode(nsWaveDisplay::FULL_BANDWIDTH);
   //	rWaveDisplayManager.GetActiveContainer()->SetStftRegionSelection(false);
}

void __fastcall TForm1::btnSelSpectClick(TObject *Sender)
{
	rWaveDisplayManager.SetSpectrogramSelectionMode(nsWaveDisplay::PARTIAL_BANDWIDTH);
}
void __fastcall TForm1::Shrink2Click(TObject *Sender)
{
   rWaveDisplayManager.GetActiveContainer()->AdjustToMinimumHeight();
   //rWaveDisplayManager.RepositionContainers();
}

void __fastcall TForm1::Adjustvertic1Click(TObject *Sender)
{
   rWaveDisplayManager.GetActiveContainer()->AdjustToNormalHeight();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::tbTransparencyOnChange(TObject *Sender)
{
	float tLevel = static_cast<float>(tbTransparency->Position)/tbTransparency->Max;
	rWaveDisplayManager.GetActiveContainer()->SetTransparencyLevel(tLevel);

	rWaveDisplayManager.GetActiveContainer()->DrawMixedView();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::cbTrackControlOnClick(TObject *Sender)
{
	const bool checked = cbSpectDlg->Checked;

	if(checked){
		pStftControl->Show();

	}else{
		if(pStftControl) pStftControl->Close();
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Closeall1Click(TObject *Sender)
{
	rWaveDisplayManager.RemoveAllContainers();
}
void __fastcall TForm1::Button2Click(TObject *Sender)
{
   //Caption = IntToStr(rWaveDisplayManager.GetActiveContainer()->GetOperationTime());

 //  rWaveDisplayManager.GetActiveContainer()->ThreadPoint();

   rWaveDisplayManager.GetActiveContainer()->TotalFrames();
}
void TForm1::AddMenuItem(std::string fileName, int mTag){

  TMenuItem *newMenuItem = new TMenuItem(this);
  Openrecent1->Add(newMenuItem);
  newMenuItem->Caption =  fileName.c_str();
  newMenuItem->Tag = mTag;
  newMenuItem->OnClick =  &OpenRecentSoundFile;
}

void TForm1::WriteFileRecent(){

	std::ofstream out("C:\\test\\recent_files.txt");
	std::set<std::string>::iterator sIter;

   if(out.is_open()){
	for(sIter=vRecentFiles.begin(); sIter!=vRecentFiles.end();sIter++){
		out << *sIter  << std::endl;
	  //	if(++numFiles>maxRecentFiles)break;
		}
	out.close();
   }
}

void TForm1::ReadFileRecent(){

	std::ifstream in("C:\\test\\recent_files.txt");
	std::string file;
	int tag=1, numFiles=0;

	while(std::getline(in, file)){
		vRecentFiles.insert(file);
		AddMenuItem(file, tag++);
		if(++numFiles>maxRecentFiles)break;
	}

	in.close();
}
void __fastcall TForm1::FormOnClose(TObject *Sender, TCloseAction &Action)
{
	WriteFileRecent();
	closingApp = true;
}
void __fastcall TForm1::OpenRecentSoundFile(TObject *Sender){

		TMenuItem *selItem = dynamic_cast<TMenuItem*>(Sender);
		std::string fName(selItem->Caption.c_str());
		int removePos = fName.find_first_of('&');
		fName = removePos==0 ?  fName.substr(1,fName.length()) : fName.substr(0,removePos)+fName.substr(removePos+1,fName.length());
		OpenFile(fName);
}
void __fastcall TForm1::FormOnShow(TObject *Sender)
{
	const int taskBarHeight = 40;
	Height = GetDesktopClientHeight()-taskBarHeight;
}
void __fastcall TForm1::Zoomin1Click(TObject *Sender)
{
	ZoomIn();
}
void __fastcall TForm1::Zoomout1Click(TObject *Sender)
{
	ZoomOut();
}
void __fastcall TForm1::cbTrackControlClick(TObject *Sender)
{

	vTrackInfo.clear();

	std::vector<PFILE_INFO> *pvFi = rWaveDisplayManager.GetContainersTrackInfo();

	for(int i=0;i<pvFi->size();++i){

		vTrackInfo.push_back( TRACK_INFO(pvFi->at( i ) ) );
	}
	if(cbTrackControl->Checked){
		TrackControlDlg->SetTrackInfo(&vTrackInfo);
		TrackControlDlg->SetParent(this);
		TrackControlDlg->Show();
	}else{
		TrackControlDlg->Close();
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button3Click(TObject *Sender)
{
	 // rWaveDisplayManager.GetActiveContainer()->ShowTitle();
}
//---------------------------------------------------------------------------

