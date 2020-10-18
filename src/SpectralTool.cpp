//$$---- Form CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SpectralTool.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TpSpectralForm *pSpectralForm;
//---------------------------------------------------------------------------

__fastcall TpSpectralForm::TpSpectralForm(TComponent* Owner)
	: TForm(Owner),
	rWaveDisplayManager(WaveDisplayContainerManager::GetInstance(0))
	,pSpectralData(NULL)
   //	,pParent(Owner)
	{}

void TpSpectralForm::ShowSelectionInfo(){

	lblStartFrame->Caption 	= IntToStr(startFrame);
	lblEndFrame->Caption 	= IntToStr(endFrame);
	lblStartBin->Caption 	= IntToStr(startBin);
	lblEndBin->Caption 		= IntToStr(endBin);

}
void TpSpectralForm::DrawLabels(){

	 const char * pLabelsMono[]	  = { "Bin", "Frequency", "Magnitude" };
	 const char * pLabelsStereo[] = { "Bin", "Frequency", "Magnitude left","Magnitude right" };
	 const unsigned monoLen		  = 3;
	 const unsigned stereoLen	  = 4;
	 const char ** pLabels = pSpectralData->GetNumChannels()  == 1 ? pLabelsMono : pLabelsStereo;
	 const unsigned lblSize = pSpectralData->GetNumChannels() == 1 ? monoLen : stereoLen;
	 for(int i=0; i<lblSize;i++)
		sgSpectralData->Cells[i][0] =  pLabels[i];
}
void TpSpectralForm::ContainerChanged(){

   if( rWaveDisplayManager.GetActiveContainer()->GetContainerView() == WAVEFORM ) return;

   refreshCentroid 	  = true;
   refreshFlux		  = true;
   refreshSpectrogram = true;
   refreshRolloff	  = true;
   refreshAvgMag	  = true;


   SELECTION_RANGE stftSel = rWaveDisplayManager.GetActiveContainer()->GetSelectionStftRange();

	pSpectralData = rWaveDisplayManager.GetActiveContainer()->GetSpectralData();

	if(/*stftSel.selEnd.x != 0 &&*/stftSel.selEnd.x>stftSel.selStart.x){
		startFrame 	= pSpectralData->GetFrameAtPos(stftSel.selStart.x );
		endFrame 	= pSpectralData->GetFrameAtPos(stftSel.selEnd.x ) ;
		startBin 	= pSpectralData->GetSelStartBin();
		endBin 		= pSpectralData->GetSelEndBin();
	}else{
		startFrame 	= 0;
		endFrame   	= pSpectralData->GetEndFrame();
		startBin 	= 1;  //0 is DC and is not used
		endBin 		= binsNum;
	}

	DrawLabels();
  //	refreshCentroid = true;
    pcSpectralTool->ActivePage = pCurrTab;
	pcSpectralTool->OnChange(0);
	ShowSelectionInfo();
	rWaveDisplayManager.GetActiveContainer()->SetSpectrogramSelectionMessageCallback(&SelectionChanged);
	rWaveDisplayManager.GetActiveContainer()->SetSpectralTracking(true);
	rWaveDisplayManager.GetActiveContainer()->DialogueShown(true);
	Caption = rWaveDisplayManager.GetActiveContainer()->GetTrackName().c_str();
}
/**
  this method  populates spectral data for a single frame. By default, the spectral data for the first frame
  are shown unless there is a selection, in which case we show spectral data for the first selected frame.
  In order to get spectral data for subsequent frames, the user should enter a frame number and hit the Refresh button.
*/
void TpSpectralForm::PopulateSpectralData(unsigned frameNum){

	float *pLMagnitudes , *pRMagnitudes;
	SELECTION_RANGE stftSel = rWaveDisplayManager.GetActiveContainer()->GetSelectionStftRange();
	SPECTROGRAM_PARAMS sData = pSpectralData->GetSpectralInputData();

	binsNum = sData.frameLen>>1;
	const float freqResolution = static_cast<float>(sData.samplesPerSec)/sData.frameLen;
	const float centralFreq = freqResolution/2;

	//refreshSpectrogram = false;
	sgSpectralData->RowCount = 0;

	pSpectralData->SetSelection(stftSel.selStart.x,stftSel.selEnd.x,stftSel.selStart.y,stftSel.selEnd.y);
	SetStartAndEndFrame();
	startBin = pSpectralData->GetSelStartBin();
	endBin = pSpectralData->GetSelEndBin();
	//sgSpectralData->RowCount = binsNum;
	if(startBin==0) startBin = 1; //skipping the DC component (zero frequency)
	pLMagnitudes = pSpectralData->GetFrameMagnitudesLeft(frameNum);
	if(sData.numChannels == 2){
		pRMagnitudes = pSpectralData->GetFrameMagnitudesRight(frameNum);
	}
	sgSpectralData->RowCount = endBin-startBin+2;

	if(endBin == binsNum) endBin--;
	for(int i=startBin,j=1; i<=endBin;i++,j++){
		sgSpectralData->Cells[0][j] = i;
		sgSpectralData->Cells[1][j] = FloatToStrF(i * freqResolution - centralFreq, ffNumber,5,2);
		sgSpectralData->Cells[2][j] = FloatToStrF(pLMagnitudes[i],ffNumber, 4, 2);
		if(sData.numChannels == 2){
			sgSpectralData->Cells[3][j] = FloatToStrF(pRMagnitudes[i],ffNumber, 4, 2);
		}
	}

	meFrame->Text = frameNum;
}
void TpSpectralForm::SelectionChanged(){

  SELECTION_RANGE stftSel = rWaveDisplayManager.GetActiveContainer()->GetSelectionStftRange();
  SetStartAndEndFrame();
  pSpectralData->SetSelection(stftSel.selStart.x,stftSel.selEnd.x,stftSel.selStart.y,stftSel.selEnd.y);
  startBin = pSpectralData->GetSelStartBin();
  endBin = pSpectralData->GetSelEndBin();
 // refreshCentroid = refreshRolloff = refreshFlux = refreshSpectrogram = true;
  ShowSelectionInfo();
  SpectralToolOnChange(pcSpectralTool);
}
//---------------------------------------------------------------------------
void TpSpectralForm::SetStartAndEndFrame(){
	SELECTION_RANGE stftSel = rWaveDisplayManager.GetActiveContainer()->GetSelectionStftRange();

	if(stftSel.selEnd.x > stftSel.selStart.x){
		startFrame = pSpectralData->GetFrameAtPos(stftSel.selStart.x );
		endFrame =   pSpectralData->GetFrameAtPos(stftSel.selEnd.x ) ;
	}else{
		startFrame = 0;
		endFrame = pSpectralData->GetEndFrame();
	}
}
void __fastcall TpSpectralForm::SpectralFormOnShow(TObject *Sender)
{
	pCurrTab = tsPowerSpectrum;
	ContainerChanged();
}
//---------------------------------------------------------------------------
void __fastcall TpSpectralForm::meFrameOnExit(TObject *Sender)
{
	int frameNum = StrToInt(Trim(meFrame->Text));
	if(frameNum < startFrame)
		meFrame->Text = startFrame;
	else if(frameNum > endFrame)
		meFrame->Text = endFrame;
}
//---------------------------------------------------------------------------
void __fastcall TpSpectralForm::btnFrameRefreshClick(TObject *Sender)
{
	int frameNum = StrToInt(Trim(meFrame->Text));
   //	refreshCentroid = refreshRolloff = refreshFlux = refreshSpectrogram = true;
	PopulateSpectralData(frameNum);
}
void TpSpectralForm::FeatureChange(nsAnalysis::EAnalysisSource eAs){

	 switch(eAs){
	 case nsAnalysis::CENTROID:
		pcSpectralTool->ActivePage = tsCentroid;
		break;
	 case nsAnalysis::FLUX:
		pcSpectralTool->ActivePage = tsFlux;
		break;
	 case nsAnalysis::ROLLOFF:
		pcSpectralTool->ActivePage = tsRolloff;
		break;
	 case nsAnalysis::AVG_MAGNITUDE:
		pcSpectralTool->ActivePage = tsAvgPowerSpectrum;
		break;
	 }
	 if(eAs!=nsAnalysis::MAGNITUDE) SpectralToolOnChange(pcSpectralTool);
}
//---------------------------------------------------------------------------
void __fastcall TpSpectralForm::SpectralToolOnChange(TObject *Sender)
{
	if(pcSpectralTool->ActivePage == tsFlux ){
		PopulateSpectralFluxData();
	   //	Caption = "flux";
	}else if( pcSpectralTool->ActivePage == tsCentroid ){
		PopulateSpectralCentroidData();
	}else if( pcSpectralTool->ActivePage == tsRolloff )
		PopulateSpectralRolloffData();
	 else if( pcSpectralTool->ActivePage == tsAvgPowerSpectrum )
		PopulateAverageMagnitudes();
	 else{
		PopulateSpectralData(startFrame);
	 }

	 pCurrTab = pcSpectralTool->ActivePage;
}

void  TpSpectralForm::PopulateAverageMagnitudes(){

 	const unsigned numChannels = pSpectralData->GetSpectralInputData().numChannels;

	if(refreshAvgMag  || sgAvgSpectralData->Cells[1][1].IsEmpty()){
		refreshAvgMag = false;
		sgAvgSpectralData->RowCount = 0;

		SelectFramesToProcess();
		const std::vector<float> *pvAvgMag = pSpectralData->GetAverageMagnitudes();

		const unsigned framesNum = pvAvgMag->size();

		sgAvgSpectralData->RowCount =  endFrame - startFrame + 2;
		LabelColumns(sgAvgSpectralData);

		for(int i=0,j=1;i<framesNum;i+=numChannels,j++){
			sgAvgSpectralData->Cells[0][j] = startFrame + (j-1) ;  //sgSpectralCentroid->Cells[0][j] = startFrame + j - 1;
			sgAvgSpectralData->Cells[1][j] = FloatToStrF(pvAvgMag->at(i),ffNumber, 4, 2);
			if(numChannels == 2)
				sgAvgSpectralData->Cells[2][j] = FloatToStrF(pvAvgMag->at(i+1),ffNumber, 4, 2);
		}
	}
}
void  TpSpectralForm::PopulateSpectralFluxData(){

	const unsigned numChannels = pSpectralData->GetSpectralInputData().numChannels;

	if(refreshFlux  || sgSpectralFlux->Cells[1][1].IsEmpty()){
		refreshFlux = false;
		sgSpectralFlux->RowCount = 0;

		SelectFramesToProcess();
		const std::vector<float> *pvSFlux = pSpectralData->GetSpectralFlux();

		const unsigned framesNum = pvSFlux->size();

		sgSpectralFlux->RowCount =  endFrame - startFrame + 2;
		LabelColumns(sgSpectralFlux);

		for(int i=0,j=1;i<framesNum;i+=numChannels,j++){
			sgSpectralFlux->Cells[0][j] = startFrame + (j-1) ;  //sgSpectralCentroid->Cells[0][j] = startFrame + j - 1;
			sgSpectralFlux->Cells[1][j] = FloatToStrF(pvSFlux->at(i),ffNumber, 4, 2);
			if(numChannels == 2)
				sgSpectralFlux->Cells[2][j] = FloatToStrF(pvSFlux->at(i+1),ffNumber, 4, 2);
		}
	}
}
void  TpSpectralForm::SelectFramesToProcess(){

	SELECTION_RANGE stftSel = rWaveDisplayManager.GetActiveContainer()->GetSelectionStftRange();

	SetStartAndEndFrame();//set the start and end frames so we don't have to don't process all frames
	if(stftSel.selEnd.x > stftSel.selStart.x){
		pSpectralData->SetSelection(stftSel.selStart.x,stftSel.selEnd.x,stftSel.selStart.y,stftSel.selEnd.y);
	}else{
		pSpectralData->SetStartAndEndBins(startBin, endBin);
		pSpectralData->SetStartAndEndFrames(startFrame, endFrame);
		}
}
/**
this method displays the spectral centroid data for multiple frames (each frame has a single value)
*/
void  TpSpectralForm::PopulateSpectralCentroidData(){

	const unsigned numChannels = pSpectralData->GetSpectralInputData().numChannels;

	if(refreshCentroid || sgSpectralCentroid->Cells[1][1].IsEmpty()){

		refreshCentroid = false;
		sgSpectralCentroid->RowCount = 0;

		SelectFramesToProcess();

		const std::vector<float> *pvSCentroid = pSpectralData->GetSpectralCentroid();

		const unsigned framesNum = pvSCentroid->size();

		sgSpectralCentroid->RowCount = endFrame - startFrame + 2;// + 1;

		LabelColumns(sgSpectralCentroid);

		for(int i=0,j=1;i<framesNum;i+=numChannels,j++){
			sgSpectralCentroid->Cells[0][j] = startFrame + (j-1);
			sgSpectralCentroid->Cells[1][j] = FloatToStrF(pvSCentroid->at(i),ffNumber, 4, 2);
			if(numChannels==2)
				sgSpectralCentroid->Cells[2][j] = FloatToStrF(pvSCentroid->at(i+1),ffNumber, 4, 2);
		}
	}
}
/**
this method will display the spectral rolloff data for multiple frames (each frame has a single rolloff value)
*/
void  TpSpectralForm::PopulateSpectralRolloffData(){

 //  SELECTION_RANGE stftSel = rWaveDisplayManager.GetActiveContainer()->GetSelectionStftRange();
	const unsigned numChannels = pSpectralData->GetSpectralInputData().numChannels;
	if(refreshRolloff || sgSpectralRolloff->Cells[1][1].IsEmpty()){
	   refreshRolloff = false;
	   sgSpectralRolloff->RowCount = 0;

		SelectFramesToProcess();

		const std::vector<float> *pvSRolloff = pSpectralData->GetSpectralRolloff();
	   //	const unsigned startFrameIndex = startFrame * numChannels;
		const unsigned framesNum = pvSRolloff->size();

		sgSpectralRolloff->RowCount = endFrame - startFrame + 2;// + 1;

		LabelColumns(sgSpectralRolloff);

		for(int i=0,j=1;i<framesNum;i+=numChannels, j++){
			sgSpectralRolloff->Cells[0][j] = startFrame + (j-1);
			sgSpectralRolloff->Cells[1][j] = FloatToStrF(pvSRolloff->at(i),ffNumber, 4, 2);
			if(numChannels==2)
				sgSpectralRolloff->Cells[2][j] = FloatToStrF(pvSRolloff->at(i+1),ffNumber, 4, 2);
		}
	}
}
void  TpSpectralForm::LabelColumns(TStringGrid *pSg){

	 const char **	pLabels;
	 const char *  	pLabelsMono[] 	=   { "Frame", "Frequency" };
	 const char *  	pLabelsStereo[] = 	{ "Frame", "Frequency left","Frequency right" };
	 const unsigned monoLen 		= 	2;
	 const unsigned stereoLen 		= 	3;
	 unsigned 	  	lblSize;

	 if(pSpectralData->GetNumChannels() == 1){
		pLabels = pLabelsMono ;
		lblSize = monoLen;
	 }else{
		pLabels = pLabelsStereo;
		lblSize = stereoLen;
	 }
	 pSg->Cells[0][0] = pLabels[0];
	 for(int i=1; i<lblSize;i++)
	 	pSg->Cells[i][0] = pLabels[i];
}
//---------------------------------------------------------------------------

void __fastcall TpSpectralForm::btnThresholdRefresh(TObject *Sender)
{
	pSpectralData->GetSpectralCentroid();
}
//---------------------------------------------------------------------------

void __fastcall TpSpectralForm::meCentroidOnExit(TObject *Sender)
{
	float val = StrToFloat(Trim(meCentroidThreshold->Text));
	if(val < -50.0f )
		return;
}
//---------------------------------------------------------------------------

void TpSpectralForm::DelegateMainWnd(const FeatureMatcher &fM) {

   //	(static_cast<TForm1*>(pParent))->DrawMatched(fM);
}

void __fastcall TpSpectralForm::btnDumpToFileClick(TObject *Sender)
{
	TdlgDumpToFile *pFileDumpDlg = new TdlgDumpToFile(this, pSpectralData);
	pFileDumpDlg->Init(startFrame, endFrame, startBin, endBin);
	pFileDumpDlg->Show();

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void __fastcall TpSpectralForm::SpectralFormOnClose(TObject *Sender,
      TCloseAction &Action)
{
   //	rWaveDisplayManager.GetActiveContainer()->DialogueShown(false);
}
//---------------------------------------------------------------------------


