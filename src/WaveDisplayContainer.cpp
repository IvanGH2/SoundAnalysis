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

#include "WaveDisplayContainer.h"
#include "ToneGenerator.h"
#include "DigitalFilter.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

TWaveDisplayContainer* waveContainer;

unsigned TWaveDisplayContainer::containersNum = 0;
unsigned TWaveDisplayContainer::containersVisibleNum = 0;
bool 	 TWaveDisplayContainer::selectionExtExtend = false;
bool 	 TWaveDisplayContainer::scrollBarVisible = false;

__fastcall TWaveDisplayContainer::~TWaveDisplayContainer(){


	pfWaveMsg(nsWaveDisplay::REMOVE, containerId);

	FreeResources();
}
 __fastcall TWaveDisplayContainer::TWaveDisplayContainer(TComponent* owner/*, const std::string& fileName*/,
	unsigned containerNum, SoundIO* const pSD)
	: TPanel(owner) {

	  containerId = 		containerNum;
	  Parent = 				(TWinControl*)owner;

	  pSoundData  =			pSD;

	  if(pSD->getFileName().c_str()){
		fName = pSD->getFileName().c_str();
		fName = fName.SubString(fName.LastDelimiter("\\")+1, fName.Length());
	  }

	  containersVisibleNum++;
	  containersNum++;
	  InitCtrls();
	  Init();
}
void TWaveDisplayContainer::InitCtrls(){

	  numChannels =	  pSoundData->GetHeaderInfo()->wfex.nChannels;//pWaveDisplay->GetInputData().numChannels;
	//  heightCorrection = 	cNonClientCtrlHeight + cWaveformVertMargin; //the height of the toolbar + statusbar  + 30px vertical margin
	  eStretchMode = 		NORMAL;
	  eContainerView = 		WAVEFORM;
	  eScale = 				LINEAR;
	  eWnd = 				WindowFunc::BLACKMAN;//WindowFunc::RECTANGULAR;//
	  frameLen = 			1024;
	  Color 	=			clMedGray;
	  origColor = 			Color;

	  Align = 				alCustom;
	  BorderStyle = 		bsSingle;
	  BorderWidth =			1;
	  BevelOuter =			bvNone;
	  DoubleBuffered =      true;
	//  int pw =              Parent->ClientWidth;
	  Width = 				scrollBarVisible ? Parent->ClientWidth : Parent->ClientWidth-(cScrollBarWidth+BorderWidth);
	  Height = 				cWaveformHeight;  //default height
	  waveHeight = 			cWaveformHeight;
	  heightCorrection = 	cWaveformVertMargin;

	  prevPos = 0;
	  gColor = clGreen;
	  foregroundColour = 	clGreen;
	  backgroundColour = 	clWhite;
	 // Color = clYellow;
	  gGradStep = 			2;
	  frameOverlap = 		SPECTROGRAM_PARAMS::FO2;//0.5f;

	  adjustEvery = 		4;
	  //playbackPosition = 0;
	  viewHeight = cWaveformHeight;

	  selectedForPlayback 	= true;
	  hidden			  	= false;
	  drawSelection		   	= true;
	  pSpectrogramView 	   	= NULL;
	  pWaveformView 	   	= NULL;

	  fInfo.pFileName		= fName.c_str();
	  fInfo.numChannels		= numChannels;
	  fInfo.samplesPerSec	= pSoundData->GetHeaderInfo()->wfex.nSamplesPerSec;
	  fInfo.bitsPerSample	= pSoundData->GetHeaderInfo()->wfex.wBitsPerSample;
	  fInfo.fileId			= containerId;
	   //lets setup some event handlers (the other event handlers can be found in Init)
	  OnClick 				= &WaveContainerOnClick;
	  OnResize 				= &WaveContainerOnResize;
	 // OnPaint				= &WaveContainerOnPaint;
	  OnMouseDown 			= &WaveContainerMouseDown;
	  OnMouseUp			    = &WaveContainerMouseUp;
	  OnMouseMove 			= &WaveContainerMouseMove;

}

 void TWaveDisplayContainer::SetWaveDisplayData(){

	const unsigned numSamples =  pSoundData->GetNumSamples();//pSoundData->GetWaveformNumSamples();//
  //	const unsigned numChannels = pSoundData->GetHeaderInfo()->wfex.nChannels;
	float *pData;
	#if defined _USE_INTERNAL_FORMAT_
		pData = (float*)pSoundData->GetData();
	#else
		pData = (pSoundData->GetHeaderInfo()->wfex.wFormatTag == WAVE_FORMAT_IEEE_FLOAT) ? (float*)pSoundData->GetData()
		: DspUtil::ConvertToFloat(pSoundData->GetData(),
		numSamples,pSoundData->GetHeaderInfo()->wfex.wBitsPerSample);
	#endif
	pWaveDisplay = new WaveDisplayData(ClientWidth - (cWaveformRightMargin + cWaveCtrlWidth + cAmpWidth), cWaveformHeight-nsWaveDisplay::cWaveformMargin);
	//
	SoundInput *pSndIn = static_cast<SoundInput*>(pSoundData);
	if(pSndIn->IsMultipleBuffers()){//numBuffers>0 only when we're dealing with multiple buffers
		pWaveDisplay->SetDataBuffers(pSndIn->GetMemoryBuffers(),
		pSndIn->GetMemoryBufferSize(),
		pSndIn->GetLastMemoryBufferSize());
	}
	pWaveDisplay->SetInputData(WAVE_DISPLAY_DATA( numChannels, pData, numSamples));
 }

 void TWaveDisplayContainer::DrawFreqLabelsLog(){

	const char **pFreqVal;
	const float *pFreqValf;
	float maxFreqLog;  //used only for log scale
 		//labels for log scale
	const char* posValuesFour_3[] =   { "0K", "1K", "4K"  };
	const char* posValuesFour_2[] =   { "0K", "4K"  };
	const char* posValuesEight_3[] =  { "0K", "1K",  "8K" };
	const char* posValuesEight_2[] =  { "0K",  "8K" };
	const char* posValuesEleven_3[] = { "0K", "1K", "11K" };
	const char* posValuesEleven_2[] = { "0K",  "11K" };
	const char* posValues22_3[] = 	  { "0K", "1K", "22K" };
	const char* posValues22_2[] =     { "0K",  "22K" };

	const float posValuesFour_3f[] =   { 0, 1,  4 };
	const float posValuesFour_2f[] =   { 0,  4 };

	const float posValuesEight_3f[] =  { 0, 1,  8};
	const float posValuesEight_2f[] =  { 0, 8 };

	const float posValuesEleven_3f[] = { 0, 1.0f, 11.0f };
	const float posValuesEleven_2f[] = { 0, 11.0f  };

	const float posValuesTwentyTwo_3f[] = { 0, 1.0f, 22.0f };
	const float posValuesTwentyTwo_2f[] = { 0, 22.0f  };


		const unsigned ampIndicatorWidth = 4;
		const unsigned ampValueWidth =  pBmpAmpLeft->Canvas->TextWidth("5.5K") + ampIndicatorWidth;//this is just an approx but it should do
		const unsigned ampValueHeight =  pBmpAmpLeft->Canvas->TextHeight("5");
		const unsigned ampValueHeightHalf =  ampValueHeight/2 + 1;

		const unsigned ampWidth =  pbAmpLeft->Width;
		const unsigned ampHeight = pbAmpLeft->Height;

		pBmpAmpLeft->Height = ampHeight;
		pBmpAmpLeft->Canvas->Brush->Color = clActiveCaption;
		pBmpAmpLeft->Canvas->Rectangle(0,0, ampWidth, ampHeight);

		unsigned numDiv;
		const unsigned sampleRate = pSoundData->GetHeaderInfo()->wfex.nSamplesPerSec;
		switch(sampleRate){
		case 8000:
			if( ampHeight > 150 ){
			pFreqVal = posValuesFour_3;
			pFreqValf = posValuesFour_3f;
			numDiv = 3;

		}else{
			pFreqVal = posValuesFour_2;
			pFreqValf = posValuesFour_2f;
			numDiv = 2;
		}
	   //	maxFreqLog = log(4000);
		break;
		case 16000:
		if( ampHeight > 150 ){
			pFreqVal = posValuesEight_3;
			pFreqValf = posValuesEight_3f;
			numDiv = 3;

		}else{
			pFreqVal = posValuesEight_2;
			pFreqValf = posValuesEight_2f;
			numDiv = 2;
		}
	   //	maxFreqLog = log(8000);
			break;
		case 22050:
		if( ampHeight > 150 ){
			pFreqVal = posValuesEleven_3;
			pFreqValf = posValuesEleven_3f;
			numDiv = 3;

		}else{
			pFreqVal = posValuesEleven_2;
			pFreqValf = posValuesEleven_2f;
			numDiv = 2;
		}

			break;
		case 44100:
			if( ampHeight > 150 ){
				pFreqVal = posValues22_3;
				pFreqValf = posValuesTwentyTwo_3f;
				numDiv = 3;
			}else{
				pFreqVal = posValues22_2;
				pFreqValf = posValuesTwentyTwo_2f;
				numDiv = 2;
		}
	   //	maxFreqLog = log(22050);
		break;
		}
		float interval;

		float div;

			maxFreqLog = log10(pFreqValf[numDiv-1]*1000);
			//draw labels in log scale
			for(int i=0; i<numDiv; i++){

				div = pFreqValf[i] == 0 ? 0 : static_cast<float>(log10(pFreqValf[i]*1000)) / maxFreqLog;

				interval =  static_cast<float>(ampHeight) * div;//(i * div);
			//positive values
				pBmpAmpLeft->Canvas->MoveTo(ampWidth - ampIndicatorWidth, ampHeight - interval);
				pBmpAmpLeft->Canvas->LineTo(ampWidth, ampHeight - interval);
				if(i==0){
					pBmpAmpLeft->Canvas->TextOut(ampWidth - ampValueWidth, ampHeight - interval - (2*ampValueHeightHalf), pFreqVal[i]);
				}else if(i == numDiv-1){
					pBmpAmpLeft->Canvas->TextOut(ampWidth - ampValueWidth, ampHeight - interval + ampValueHeightHalf/2, pFreqVal[i]);
				}else
					pBmpAmpLeft->Canvas->TextOut(ampWidth - ampValueWidth, ampHeight - interval - ampValueHeightHalf, pFreqVal[i]);
			}
 }

 /**
   draws the frequency panel for linear scale
 */
 void TWaveDisplayContainer::DrawFreqLabels(){

	char **pFreqVal;

	//linear scale
	char* posValuesFour_5[] =   { "0K", "1K", "2K", "3K", "4K"  };
	char* posValuesFour_3[] =   { "0K",  "2K", "4K"  };
	char* posValuesFour_2[] =   { "0K",  "4K"  };
	char* posValuesEight_5[] =  { "0K", "2K",  "4K", "6K", "8K" };
	char* posValuesEight_3[] =  { "0K",  "4K", "8K" };
	char* posValuesEight_2[] =  { "0K",  "8K" };
	char* posValuesEleven_5[] = { "0K", "2.7K",  "5.5K", "8K", "11K" };
	char* posValuesEleven_3[] = { "0K",  "5.5K", "11K" };
	char* posValuesEleven_2[] = { "0K",  "11K" };
	char* posValues22_5[] = 	{ "0K", "5.5K",  "11K", "16.5", "22K" };
	char* posValues22_3[] =     { "0K",  "11K", "22K" };
	char* posValues22_2[] =     { "0K", "22K" };


		const unsigned ampWidth =  pbAmpLeft->Width;
		const unsigned ampHeight = pbAmpLeft->Height;

		pBmpAmpLeft->Height = ampHeight;
		pBmpAmpLeft->Canvas->Brush->Color = clActiveCaption;
		pBmpAmpLeft->Canvas->Rectangle(0,0, ampWidth, ampHeight);

	   unsigned sampleRate = pSoundData->GetHeaderInfo()->wfex.nSamplesPerSec;

	   if(sampleRate > 8000 && sampleRate < 16000)
		sampleRate = 16000;
	   else if (sampleRate > 16000 && sampleRate < 22050)
		 sampleRate = 22050;
	   else if (sampleRate > 22050 && sampleRate < 44100)
			sampleRate = 44100;

       if(eScale == LOG){
	//	DrawFreqLabelsLog(sampleRate);
		return;
	   }
	   //	unsigned ampHeightHalf = ampHeight/2;
		const unsigned ampIndicatorWidth = 4;
		const unsigned ampValueWidth =  pBmpAmpLeft->Canvas->TextWidth("5.5K") + ampIndicatorWidth;//this is just an approx but it should do
		const unsigned ampValueHeight =  pBmpAmpLeft->Canvas->TextHeight("5");
		const unsigned ampValueHeightHalf =  ampValueHeight/2 + 1;

		unsigned numDiv;

		switch(sampleRate){
		case 8000:
			if( ampHeight > 150 ){
			pFreqVal = posValuesFour_5;
			//pFreqValf = posValuesFour_5f;
			numDiv = 5;

		}else if( ampHeight > 80 ){
			pFreqVal = posValuesFour_3;
			numDiv = 3;
		}else{
			pFreqVal = posValuesFour_2;
			numDiv = 2;
		}
	   //	maxFreqLog = log(4000);
		break;
		case 16000:
		if( ampHeight > 150 ){
			pFreqVal = posValuesEight_5;
		   //	pFreqValf = posValuesEight_5f;
			numDiv = 5;

		}else if(ampHeight > 80 ){
			pFreqVal = posValuesEight_3;
			numDiv = 3;
		}else{
		   pFreqVal = posValuesEight_2;
			numDiv = 2;
		}
	   //	maxFreqLog = log(8000);
		break;
		case 22050:
		if( ampHeight > 150 ){
			pFreqVal = posValuesEleven_5;
		  //	pFreqValf = posValuesEleven_5f;
			numDiv = 5;

		}else if (ampHeight > 80){
			pFreqVal = posValuesEleven_3;
			numDiv = 3;
		}else{
			pFreqVal = posValuesEleven_2;
			numDiv = 2;
		}

		break;
		case 44100:
		if( ampHeight > 150 ){
			pFreqVal = posValues22_5;
			numDiv = 5;
		}else if(ampHeight>80){
			pFreqVal = posValues22_3;
			numDiv = 3;

		}else{
			pFreqVal = posValues22_2;
			numDiv = 2;
		}
	   //	maxFreqLog = log(22050);
		break;
		}

	 //	pBmpAmpLeft->Canvas->FillRect(TRect(0, 0, pbAmpLeft->Width, pbAmpLeft->Height));

		float interval;

		float div;

		div = 1.0f / (numDiv-1);
		const unsigned offset=0;

		for(int i=0; i<numDiv; i++){

			interval =  (float)ampHeight * (i * div);
			//positive values
			pBmpAmpLeft->Canvas->MoveTo(ampWidth - ampIndicatorWidth, ampHeight - interval);
			pBmpAmpLeft->Canvas->LineTo(ampWidth, ampHeight - interval);
			if(i==0){
				pBmpAmpLeft->Canvas->TextOut(ampWidth - ampValueWidth-offset, ampHeight - interval - (2*ampValueHeightHalf), pFreqVal[i]);
			}else if(i == numDiv-1){
				pBmpAmpLeft->Canvas->TextOut(ampWidth - ampValueWidth-offset, ampHeight - interval + ampValueHeightHalf/2, pFreqVal[i]);
			}else
				pBmpAmpLeft->Canvas->TextOut(ampWidth - ampValueWidth-offset, ampHeight - interval - ampValueHeightHalf, pFreqVal[i]);

			}
 }
/**
this method draws the waveform amplitude panel ( from -1 to 1 )
*/
 void TWaveDisplayContainer::DrawAmpLabels(){

		int i, j;
		const char* startAmp = "0.0";
		const char* posValuesAll[] = { "0.1", "0.2", "0.3", "0.4", "0.5", "0.6",  "0.7", "0.8", "0.9", "1.0" };
		const char* negValuesAll[] = { "-0.1", "-0.2", "-0.3", "-0.4", "-0.5", "-0.6", "-0.7", "-0.8", "-0.9", "-1.0"  };

		const char* valuesTwo[] = {  "1.0", "-1.0"  };
		const char* valuesFour[] = {  "0.5", "-0.5", "1.0", "-1.0"  };

		const float positions[] = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0  };

	   //prebaci u init
		/*if(!pBmpAmpLeft){
			pBmpAmpLeft = new Graphics::TBitmap;
			pBmpAmpLeft->Width = pbAmpLeft->Width;
			pBmpAmpLeft->Canvas->Brush->Color = Color;
		}*/

		pBmpAmpLeft->Height = pbAmpLeft->Height;

		unsigned ampWidth =  pBmpAmpLeft->Width;
		unsigned ampHeight = pBmpAmpLeft->Height;
		unsigned ampHeightHalf = ampHeight/2;

		 const unsigned ampIndicatorWidth = 4;
		 const unsigned ampValueWidth =  pBmpAmpLeft->Canvas->TextWidth(startAmp) + ampIndicatorWidth;//this is just an approx but it should do
		 const unsigned ampNegValueWidth =  pBmpAmpLeft->Canvas->TextWidth("-0.0") + ampIndicatorWidth;//this is just an approx but it should do
		 const unsigned ampValueHeight =  pBmpAmpLeft->Canvas->TextHeight(startAmp);
		 const unsigned ampValueHeightHalf =  ampValueHeight/2 + 1;
		pBmpAmpLeft->Canvas->Brush->Color = clActiveCaption;
		pBmpAmpLeft->Canvas->Rectangle(0,0, pbAmpLeft->Width, ampHeight);

		//0 is always drawn
		pBmpAmpLeft->Canvas->MoveTo(ampWidth - ampIndicatorWidth, ampHeightHalf );
		pBmpAmpLeft->Canvas->LineTo(ampWidth, ampHeightHalf);
		pBmpAmpLeft->Canvas->TextOut(ampWidth - ampValueWidth,  ampHeightHalf - ampValueHeightHalf, startAmp);

		float interval ;

	   if(ampHeight > 350){
		for (i = 0; i < 10; i++){
			interval =  (float)ampHeightHalf * positions[i];
			//positive values
			pBmpAmpLeft->Canvas->MoveTo(ampWidth - ampIndicatorWidth, ampHeightHalf - interval);
			pBmpAmpLeft->Canvas->LineTo(ampWidth, ampHeightHalf - interval);

			if(i==9)
				pBmpAmpLeft->Canvas->TextOut(ampWidth - ampValueWidth, ampHeightHalf - interval + 2, posValuesAll[i]);
			else
				pBmpAmpLeft->Canvas->TextOut(ampWidth - ampValueWidth, ampHeightHalf - interval - ampValueHeightHalf, posValuesAll[i]);

			//negative values
			pBmpAmpLeft->Canvas->MoveTo(ampWidth - ampIndicatorWidth, ampHeightHalf + interval);
			pBmpAmpLeft->Canvas->LineTo(ampWidth, ampHeightHalf + interval);
			if(i==9)
				pBmpAmpLeft->Canvas->TextOut(ampWidth - ampNegValueWidth, ampHeightHalf + interval - ampValueHeight - 2, negValuesAll[i]);
			else
				pBmpAmpLeft->Canvas->TextOut(ampWidth - ampNegValueWidth, ampHeightHalf + interval - ampValueHeightHalf, negValuesAll[i]);
		}
		}else if(ampHeight > 150){
		   for (i = 0, j=1; i < 4; i+= 2, j++){

			interval = (float)(j * ampHeight * 0.25) ;  //0.125 = 0.25 / 2 because  the entire height is divided by only 4 values
				//positive values
			pBmpAmpLeft->Canvas->MoveTo(ampWidth - ampIndicatorWidth, ampHeightHalf - interval);
			pBmpAmpLeft->Canvas->LineTo(ampWidth, ampHeightHalf - interval);
			if(i==2)
				pBmpAmpLeft->Canvas->TextOut(ampWidth - ampValueWidth, ampHeightHalf - interval + 2, valuesFour[i]);
			else
				pBmpAmpLeft->Canvas->TextOut(ampWidth - ampValueWidth, ampHeightHalf - interval - ampValueHeightHalf, valuesFour[i]);
			//neg values
			pBmpAmpLeft->Canvas->MoveTo(ampWidth - ampIndicatorWidth, ampHeightHalf + interval);
			pBmpAmpLeft->Canvas->LineTo(ampWidth, ampHeightHalf + interval);
			if(i==2)
				pBmpAmpLeft->Canvas->TextOut(ampWidth - ampNegValueWidth, ampHeightHalf + interval - ampValueHeight - 2, valuesFour[i+1]);
			else
				pBmpAmpLeft->Canvas->TextOut(ampWidth - ampNegValueWidth, ampHeightHalf + interval - ampValueHeightHalf, valuesFour[i+1]);
		   }
		}else if(ampHeight <= 150 && ampHeight > 40){
				//1.0
			pBmpAmpLeft->Canvas->MoveTo(ampWidth - ampIndicatorWidth, 0);
			pBmpAmpLeft->Canvas->LineTo(ampWidth, 0);
			pBmpAmpLeft->Canvas->TextOut(ampWidth - ampValueWidth, 2, valuesTwo[0]);
			//-1.0
			pBmpAmpLeft->Canvas->MoveTo(ampWidth - ampIndicatorWidth, ampHeight);
			pBmpAmpLeft->Canvas->LineTo(ampWidth, ampHeight);
			pBmpAmpLeft->Canvas->TextOut(ampWidth - ampNegValueWidth, ampHeight  - ampValueHeight-2, valuesTwo[1]);
		}
 }
 void __fastcall TWaveDisplayContainer::WaveAmpOnClick(TObject *sender){

	ActivateContainer();
 }
 void  TWaveDisplayContainer::Init(){

	  SetWaveDisplayData();
	  channelHeight = numChannels == 1 ? Height - cWaveformMargin
									   : (Height - (cWaveformMargin+cWaveformSpacing))/2;

	  pbAmpLeft = new TPaintBox(this);
	  pbAmpLeft->Parent = this;
	  pbAmpLeft->Tag = nsWaveDisplay::LEFT_AMP;
	  pbAmpLeft->Left = 0;
	  pbAmpLeft->Width = cAmpWidth;
	  pbAmpLeft->Height = channelHeight;
	  pbAmpLeft->OnPaint = &WaveOnPaint;
	  pbAmpLeft->OnClick = &WaveAmpOnClick;

	  //create the left channel container
	  pWaveLeft = new TPaintBox(this);
	  pWaveLeft->Parent = this;
	  pWaveLeft->Tag = nsWaveDisplay::LEFT_CHANNEL;
	  pWaveLeft->Width = ClientWidth - (pbAmpLeft->Width+cWaveformRightMargin-vScrollBarWidth)  ;
	  pWaveLeft->Height = channelHeight ;
	  pWaveLeft->Left = pbAmpLeft->Width ;
	  pWaveLeft->Top = 0;
	//  pWaveLeft->Cursor = crIBeam;
		//event Handlers
	  pWaveLeft->OnPaint = 		&WaveOnPaint;
	  pWaveLeft->OnClick = 		&WaveOnClick;
	  pWaveLeft->OnMouseDown = 	&WaveMouseDown;
	  pWaveLeft->OnMouseUp = 	&WaveMouseUp;
	  pWaveLeft->OnMouseMove = 	&WaveMouseMove;

	  pBmpAmpLeft = new Graphics::TBitmap;
	  pBmpAmpLeft->Width = pbAmpLeft->Width;
	  pBmpAmpLeft->Canvas->Brush->Color = Color;

	  pCloseTrack = new TPanel(this);
	  pCloseTrack->Parent = this;
  //	  pCloseTrack->Flat   = true;
	  pCloseTrack->Caption = "x";
	  pCloseTrack->Color   = clInactiveCaption;
	  pCloseTrack->Font->Style  = TFontStyles() << fsBold;
	//  pCloseTrack->Font->Color  = clBlue;
	  pCloseTrack->Height = 14;
	  pCloseTrack->Width  = cAmpWidth;
	  pCloseTrack->Left   = 2;
	  pCloseTrack->Top 	  = Height-cWaveformMargin;
	  pCloseTrack->Cursor = crDefault;
	  pCloseTrack->BevelOuter = bvNone;
	  pCloseTrack->DoubleBuffered = true;
	  pCloseTrack->OnClick = &WaveCloseTrack;

	  pFileNameLbl = new TLabel(this);
	  pFileNameLbl->Parent = this;
	  pFileNameLbl->Caption = fName;
	  pFileNameLbl->Font->Color = clGreen;
	  pFileNameLbl->Font->Height = 12;
	  pFileNameLbl->Height = 12;
	  pFileNameLbl->Top    = Height -cWaveformMargin;
	  pFileNameLbl->Left   = cAmpWidth+5;

	  //if 2-channels
	  if(numChannels == 2){
	  //create the right channel container
		pWaveRight = new TPaintBox(this);
		pWaveRight->Parent = this;
		pWaveRight->Tag = nsWaveDisplay::RIGHT_CHANNEL;
		pWaveRight->Width = pWaveLeft->Width;
		pWaveRight->Height = channelHeight ;
		pWaveRight->Left = pbAmpLeft->Width ;
		pWaveRight->Top = channelHeight + cWaveformSpacing;
	 //	pWaveRight->Cursor = crIBeam;
		pWaveRight->OnPaint = &WaveOnPaint;
		pWaveRight->OnClick = &WaveOnClick;
		pWaveRight->OnMouseDown = &WaveMouseDown;
		pWaveRight->OnMouseUp = &WaveMouseUp;
		pWaveRight->OnMouseMove = &WaveMouseMove;
	  //create the value panel for the right channel
		pbAmpRight = new TPaintBox(this);
		pbAmpRight->Parent = this;
		pbAmpRight->Tag = nsWaveDisplay::RIGHT_AMP;
		pbAmpRight->Left = 0;
		pbAmpRight->Width = cAmpWidth;
		pbAmpRight->Height = pWaveRight->Height ;
		pbAmpRight->Top = pWaveRight->Top;
		pbAmpRight->OnPaint = &WaveOnPaint;
	  }
	  active = true;
	  sfftRange.selStart.x 	= 0;
	  sfftRange.selStart.y 	= 0;
	  sfftRange.selEnd.x 	= 0;//bbilo je 1
	  sfftRange.selStart.y 	= pWaveLeft->Height;

 }

  void  TWaveDisplayContainer::AdjustToMinimumHeight(){

	//resize(Width, nsWaveDisplay::cWaveformMinHeight);
	Height = nsWaveDisplay::cWaveformMinHeight;
	eStretchMode = MIN;

	if(eContainerView == WAVEFORM)
	   AdjustWaveformVertically(nsWaveDisplay::cWaveformMinHeight);
	else
		AdjustSpectrogramVertically(nsWaveDisplay::cWaveformMinHeight);


	pfWaveMsg(nsWaveDisplay::REPOSITION, containerId);
 }

void  TWaveDisplayContainer::AdjustToNormalHeight(){

	eStretchMode = NORMAL;
	Height = nsWaveDisplay::cWaveformHeight;

	if(eContainerView == WAVEFORM)
	   AdjustWaveformVertically(waveHeight);
	else
		AdjustSpectrogramVertically(waveHeight);

	pfWaveMsg(nsWaveDisplay::REPOSITION, containerId);
}
 void  TWaveDisplayContainer::AdjustMinimum(){

	// Resize(Width, nsWaveDisplay::cWaveformMinHeight); //možda Resize ne zvati veæ postaviti samo Height
	eStretchMode = MIN;
	Height = nsWaveDisplay::cWaveformMinHeight;   //??? poslije Resize
	Top =  layoutPos * nsWaveDisplay::cWaveformMinHeight;


	if(eContainerView == WAVEFORM)
	   AdjustWaveformVertically(nsWaveDisplay::cWaveformMinHeight);
	else
		AdjustSpectrogramVertically(nsWaveDisplay::cWaveformMinHeight);

 }
 //new refactoring
 unsigned TWaveDisplayContainer::GetTopPosition(unsigned height){

	const int spacing = layoutPos > 0 ? 2 : 0;

	return layoutPos * height + spacing * layoutPos;
 }
 void  TWaveDisplayContainer::AdjustContainerHeight(unsigned height, EWaveStretchMode eWsm){

	eStretchMode = eWsm;
	Height = height;

	if( eWsm != CUSTOM ) Top =  GetTopPosition( height );

	switch (eContainerView) {
		case WAVEFORM:
			AdjustWaveformVertically(height);
			break;
		case SPECTROGRAM:

			AdjustSpectrogramVertically(height);
			break;
		case MIXED:
		  //	completeViewRepaint = true;
			DoDrawMixed();
			break;
		default:
		;
	}
 }
 void  TWaveDisplayContainer::AdjustNormal(){

	AdjustContainerHeight( nsWaveDisplay::cWaveformHeight, NORMAL );
 }

 void  TWaveDisplayContainer::AdjustVertically(){

	AdjustContainerHeight( ( Parent->ClientHeight - heightCorrection) / containersVisibleNum, MAX );
 }

 void  TWaveDisplayContainer::CustomAdjustVertically(unsigned newHeight){

	if( newHeight < nsWaveDisplay::cWaveformMinHeight ) return;

	AdjustContainerHeight( newHeight, CUSTOM );

	pfWaveMsg(nsWaveDisplay::REPOSITION, containerId);
  }

  void  TWaveDisplayContainer::AdjustWaveformVertically(unsigned newHeight){

	if(!stretching){
		//completeViewRepaint = true;
		drawSelection = true;
		if(numChannels == 2)
			pWaveDisplay->AdjustViewportVertically((newHeight - cWaveformMargin - cWaveformSpacing)/2);
		else
			pWaveDisplay->AdjustViewportVertically(newHeight - cWaveformMargin);
	}
	DoDraw();
 }
  //poziva se samo iz WaveContainerOnResize
  void  TWaveDisplayContainer::AdjustSpectrogramSize(unsigned newWidth, unsigned newHeight){

	if(!stretching && completeViewRepaint){
		if(numChannels == 2) {
			pSpectralData->AdjustViewportSize(pWaveLeft->Width, (newHeight - cWaveformMargin - cWaveformSpacing)/2);
		}else{
			pSpectralData->AdjustViewportSize(pWaveLeft->Width, newHeight - cWaveformMargin);
		}
	}else{//stretching == true or completeViewRepaint==false
		pSpectralData->SetViewPortHeight(newHeight - cWaveformMargin); //?? dovoljnan height
	}
	RecalculateStftRegionPos();
	DoDrawSfft();
 }
 void  TWaveDisplayContainer::AdjustSpectrogramVertically(unsigned newHeight){

	if(!stretching){
	//	completeViewRepaint = true;
		if(numChannels == 2) {
			pSpectralData->AdjustViewportSize(pWaveLeft->Width, (newHeight - cWaveformMargin - cWaveformSpacing)/2);
		}else{
			pSpectralData->AdjustViewportSize(pWaveLeft->Width, newHeight - cWaveformMargin);
		}
	}else{//stretching == true
			pSpectralData->SetViewPortHeight(newHeight - cWaveformMargin);
	}

	RecalculateStftRegionPos();
	DoDrawSfft();
 }

 void TWaveDisplayContainer::DrawWaveform(){
	//if switching from the SPECTROGRAM view, let's readjust vertically
	if(eContainerView == SPECTROGRAM )	AdjustWaveformVertically(Height);

 //	if( eStretchMode != CUSTOM ) Top =  GetTopPosition( Height );

	eContainerView = WAVEFORM;

	completeViewRepaint = true;
	DoDraw();
 //pWaveLeft->Cursor = crIBeam;
 }
 void TWaveDisplayContainer::RedrawSpectrogram(){

	bool recalculate = false;
  
	if(pSpectralData->GetViewPortWidth()!=pWaveLeft->Width)
		pSpectralData->AdjustViewportSize( pWaveLeft->Width, channelHeight);

	if(pSpectralData->GetFrameLength() != frameLen){
		pSpectralData->SetFrameLength(frameLen);
		recalculate = true;
	}
	if(pSpectralData->GetFrameOverlap() != frameOverlap){
		pSpectralData->SetFrameOverlap(frameOverlap);
		recalculate = true;
	}
	if(pSpectralData->GetFrameWindow() != eWnd){

		pSpectralData->SetFrameWindow(eWnd);
		recalculate = true;
	}
	if(pSpectralData->GetFrameScale() != eScale){

		pSpectralData->SetFrameScale(eScale);
		recalculate = true;
	}
	if(pSpectralData->GetNumSamples() != pWaveDisplay->GetViewPortSamplesTotal()){

	   pSpectralData->SetData(pWaveDisplay->GetCurrentData(), pWaveDisplay->GetViewPortSamples());
	   recalculate = true;
	}

	if(recalculate ){
		if (pSpectralData->GetOverlappingFramesNumber()>0) {
			completeViewRepaint = true;
			pSpectralData->SetViewPortRange(pWaveDisplay->GetStartSample(), pWaveDisplay->GetEndSample());
			pSpectralData->RecalculateFFT();
		}else{

			throw SoundAnalyzerException("The number of samples is fewer than the frame length!");
		}

	}

	if(pSpectralData->GetOverlappingFramesNumber()>0) DoDrawSfft();
	if(eContainerView == MIXED) DrawMixedView();
 }
 void TWaveDisplayContainer::DrawMixedView(){

	if(!pSpectralData) return;

  //	if( eStretchMode != CUSTOM ) Top =  GetTopPosition( Height );

	eContainerView = MIXED;

	completeViewRepaint = true;

	DoDrawMixed();
 }
 void TWaveDisplayContainer::DrawSpectrogram(){

 // if( eStretchMode != CUSTOM ) Top =  GetTopPosition( Height );

  if(pSpectralData && pSpectralData->GetOverlappingFramesNumber()>0){

	RedrawSpectrogram();
	return;
  }
  completeViewRepaint = true;
  pSpectralData = new SpectrogramDisplayData(pWaveLeft->Width, pWaveLeft->Height); //da li uvijek kreirati

  //multiple buffers
  SoundInput *pSndIn = static_cast<SoundInput*>(pSoundData);

  if(pSndIn->IsMultipleBuffers()){//numBuffers>0 only when we're dealing with multiple buffers
		pSpectralData->SetDataBuffers(pSndIn->GetMemoryBuffers(),
		pSndIn->GetMemoryBufferSize(),
		pSndIn->GetLastMemoryBufferSize());
  }
  pSpectralData->SetInputData(  SPECTROGRAM_PARAMS(pWaveDisplay->GetCurrentData(),
								pWaveDisplay->GetViewPortSamplesTotal(), numChannels,
								pSoundData->GetHeaderInfo()->wfex.nSamplesPerSec, eWnd,
								frameLen, frameOverlap, eScale ));          //nsWaveDisplay::LINEAR


  pSpectralData->SetViewPortRange(pWaveDisplay->GetStartSample(), pWaveDisplay->GetEndSample());
   //end multiple buffers
  if(pSpectralData->GetOverlappingFramesNumber()>0){
	startTime = GetTickCount();
	if(useMultiThreading){
		pSpectralData->UseBkgThread(true);
		pSpectralData->SetDrawSpectrogramCallback(&DrawSpectrogramCallback);
		pSpectralData->RecalculateFFT();

	}else{
		pSpectralData->RecalculateFFT();
		DrawSpectrogramCallback();
	}
  }else{

	 throw SoundAnalyzerException("The number of samples is fewer than the frame length!");
	}
 }
 void  TWaveDisplayContainer::DrawSpectrogramCallback(){

   DoDrawSfft();
   frameLen = pSpectralData->GetFrameLength();
 }
 /**
   this method is used only in the MIXED view when either the waveform or spectrogram view has finished resizing
   it basically adjust the spectrogram and waveform views to the new height
 */
 void  TWaveDisplayContainer::StretchBitmapToFit(unsigned height){

	if(pWaveformView->GetHeight()!= height || pWaveformView->GetWidth() != pWaveLeft->Width){
		if(pWaveformView->GetWidth()!=pWaveLeft->Width){
			pWaveDisplay->AdjustViewportSize(pWaveLeft->Width, height);
		}else{
			pWaveDisplay->AdjustViewportVertically(height);
		}
		pWaveformView->SetHeight(height);
		pWaveformView->SetWidth(pWaveLeft->Width);
		pWaveformView->SetInputData(pWaveDisplay->GetSampleValues(), pWaveDisplay->GetSamplesForDisplay());
		pWaveBmpLeft = pWaveformView->GetContainerViewLeft();
		if(numChannels==2)pWaveBmpRight = pWaveformView->GetContainerViewRight();
	}
	if(pSpectrogramView->GetHeight()!=height || pSpectrogramView->GetWidth()!=pWaveLeft->Width){
		if(pSpectrogramView->GetWidth()!=pWaveLeft->Width)
			pSpectralData->AdjustViewportSize(pWaveLeft->Width, height);
		else
			pSpectralData->AdjustViewportVertically(height);

		unsigned **pMagLines = pSpectralData->GetSpectralGrid();
		pSpectrogramView->SetData(pMagLines);
		pSpectrogramView->SetMaxMagnitude(pSpectralData->GetMaxMagnitude());
		pSpectrogramView->SetHeight(height);
		pSpectrogramView->SetWidth(pWaveLeft->Width);
		pSfftBmpLeft = pSpectrogramView->GetContainerViewLeft();
		if(numChannels==2)pSfftBmpRight = pSpectrogramView->GetContainerViewRight();
	}
 }
 void  TWaveDisplayContainer::DoDrawMixed(){

	channelHeight =  GetChannelHeight();

	pWaveLeft->Height = channelHeight;
	pbAmpLeft->Height = channelHeight;
  //	if( eStretchMode != CUSTOM ) Top = GetTopPosition( Height );
	if(numChannels==2){
		pWaveRight->Height = channelHeight;
		pbAmpRight->Height = channelHeight;
		pWaveRight->Top = pWaveLeft->Height+cWaveformSpacing;
		pbAmpRight->Top = pWaveRight->Top;
	}
	// the waveform and the spectrogram bitmaps may not be the same size
	eScale==LOG ? DrawFreqLabelsLog() : DrawFreqLabels();
	if(!pMixedView){
		pMixedView = new TMixedView(pWaveLeft->Width, channelHeight,numChannels);
	}
	pMixedView->SetHeight(channelHeight);
	pMixedView->SetWidth(pWaveLeft->Width);    //dodao 10.12.2018 ???

	if(!stretching && completeViewRepaint){
		completeViewRepaint = false;
		StretchBitmapToFit(channelHeight);
		pMixedView->SetTransparencyLevel(transparencyLevel);
		pMixedView->SetViewsToMix(pWaveBmpLeft, pSfftBmpLeft, LEFT);

		if(numChannels==2)pMixedView->SetViewsToMix(pWaveBmpRight, pSfftBmpRight, RIGHT);

		pMixedBmpLeft = pMixedView->GetContainerViewLeft();
		pMixedBmpLeft0 = pMixedBmpLeft;
		pWaveLeft->Canvas->Draw(0, 0, pMixedBmpLeft);
		DrawSelectedRegion();  //zašto tu ovo

		if(numChannels==2){
   //	pMixedView->SetViewsToMix(pWaveBmpRight, pSfftBmpRight, RIGHT);
			pMixedBmpRight = pMixedView->GetContainerViewRight();
			pMixedBmpRight0 = pMixedBmpRight;
			pWaveRight->Canvas->Draw(0, 0, pMixedBmpRight);
			}
		}else{   //stretching == true
	   //if(!pMixedView->IsContainerViewLeft())
			drawSelection = false;
			SetViewToStretch();
			pMixedBmpLeft = pMixedView->StretchContainerViewLeft();
			pbAmpLeft->Canvas->Draw(0, 0, pBmpAmpLeft);
   //	pWaveLeft->Canvas->Draw(0, 0, pMixedBmpLeft);
			if(numChannels==2){
				pMixedBmpRight = pMixedView->StretchContainerViewRight();
				pbAmpRight->Canvas->Draw(0, 0, pBmpAmpLeft);
				pWaveRight->Canvas->Draw(0, 0, pMixedBmpRight);
			}
		}

 }
 void  TWaveDisplayContainer::DoDrawSfft(){

   if(eContainerView != MIXED) eContainerView = SPECTROGRAM;

   channelHeight =  GetChannelHeight();

   pWaveLeft->Height = channelHeight;
   pbAmpLeft->Height = channelHeight;

//  if( eStretchMode != CUSTOM ) Top = GetTopPosition( Height );
   if(numChannels == 2){
		pWaveRight->Height =  channelHeight;
		pbAmpRight->Height =  channelHeight;
		pWaveRight->Top    =  pWaveLeft->Height + cWaveformSpacing;
		pbAmpRight->Top    =  pWaveRight->Top;
   }
   unsigned binsPerFrame, numFrames, frameWidth, magLen;
   float maxMagnitude, minMagnitude;
   unsigned ** pMagLines;

  // pFileNameLbl->Caption = completeViewRepaint ? "true" : "false";

   if(!stretching /*&& completeViewRepaint*/){
		maxMagnitude =   pSpectralData->GetMaxMagnitude();
		minMagnitude =   pSpectralData->GetMinMagnitude();
		pMagLines    = 	 pSpectralData->GetSpectralGrid();
   }

	if(!pSpectrogramView){
		pSpectrogramView = new TSpectrogramView(pWaveLeft->Width, channelHeight, numChannels, maxMagnitude, minMagnitude, pMagLines);
		pSpectrogramView->SetColorMap(gColorMap);  //ovo maknuti odavde?
		pSpectrogramView->SetGradientStep(gGradStep);
	}else{
		 pSpectrogramView->SetWidth(pWaveLeft->Width);
		 pSpectrogramView->SetHeight(channelHeight);
	}

	if(!stretching && completeViewRepaint){

		pSpectrogramView->SetColorMap(gColorMap);
		//pSpectrogramView->SetGradientStep(gGradStep);
		pSpectrogramView->SetData(pMagLines);

		pSfftBmpLeft  = pSpectrogramView->GetContainerViewLeft();
		pSfftBmpLeft0 = pSfftBmpLeft;

		if(numChannels == 2){
			pSfftBmpRight  = pSpectrogramView->GetContainerViewRight();
			pSfftBmpRight0 = pSfftBmpRight;
		}
	}else{
		drawSelection = false;
		SetViewToStretch();
		pSfftBmpLeft = pSpectrogramView->StretchContainerViewLeft();
		if(numChannels == 2)
			pSfftBmpRight = pSpectrogramView->StretchContainerViewRight();

	  }
	   //??? možda premjestiti ovo
  eScale==LOG ? DrawFreqLabelsLog() : DrawFreqLabels();//ovo prebaciti dolje

  if(!stretching && eContainerView != MIXED){

	pbAmpLeft->Canvas->Draw(0, 0, pBmpAmpLeft);
	pWaveLeft->Repaint();

	if(numChannels==2){
		pbAmpRight->Canvas->Draw(0, 0, pBmpAmpLeft);
		pWaveRight->Repaint();
	}
  }
   if(eContainerView == MIXED){
		DoDraw();
		DoDrawMixed();
   }
   completeViewRepaint = false;
}
 void  TWaveDisplayContainer::DoDraw(){

 PWAVE_POINT wpValues =  pWaveDisplay->GetSampleValues();

 unsigned len = pWaveDisplay->GetSamplesForDisplay();

 channelHeight =  GetChannelHeight();

 pWaveLeft->Height = channelHeight;
 pbAmpLeft->Height = channelHeight;
// if( eStretchMode != CUSTOM ) Top = GetTopPosition( Height );

 if(numChannels == 2){
	pWaveRight->Top =  pWaveLeft->Height + cWaveformSpacing;
	pWaveRight->Height = pWaveLeft->Height;
	pbAmpRight->Top = pWaveRight->Top;
	pbAmpRight->Height = pWaveRight->Height;
 }

	if(!pWaveformView)
		pWaveformView = new TWaveformView(pWaveLeft->Width, channelHeight,numChannels,wpValues, len);

			 pWaveformView->SetWidth(pWaveLeft->Width);
			 pWaveformView->SetHeight(channelHeight);
			 pWaveformView->SetWaveColour(foregroundColour);
			 pWaveformView->SetBkgColour(backgroundColour);

			if(!stretching && completeViewRepaint){  //not stretching
			  if(pWaveDisplay->GetViewPortWidth()!=pWaveLeft->Width || pWaveDisplay->GetViewPortHeight()!=channelHeight){

				pWaveDisplay->AdjustViewportSize(pWaveLeft->Width, channelHeight);
				wpValues =  pWaveDisplay->GetSampleValues();
				len = pWaveDisplay->GetSamplesForDisplay();
			  }
			  pWaveformView->SetInputData(wpValues, len);
			  pWaveformView->SetEnabled(selectedForPlayback);
			  pWaveformView->SetWaveformDisplayMode(pWaveDisplay->GetWaveformDisplayMode());
			  pWaveBmpLeft  = pWaveformView->GetContainerViewLeft();
			  pWaveBmpLeft0 = pWaveBmpLeft;

			  if(numChannels==2){
				pWaveBmpRight  = pWaveformView->GetContainerViewRight();
				pWaveBmpRight0 = pWaveBmpRight;
				//pWaveRight->Canvas->Draw(0, 0, pWaveBmpRight);
			  }
			}else{ //stretching
			 // drawSelection = false;
			  SetViewToStretch();
			  pWaveBmpLeft = pWaveformView->StretchContainerViewLeft();

			  if(numChannels==2){
				pWaveBmpRight = pWaveformView->StretchContainerViewRight();
			  }
			}
			//draw the amplitude panel
			DrawAmpLabels();
			// show bitmaps on the screen (we dont call Repaint/Refresh or Invalidate, which all would trigger OnPaint event--this is primarily done for performance
			if(!stretching && eContainerView != MIXED){
				pWaveLeft->Repaint();
				if(numChannels==2)
					pWaveRight->Repaint();
		   }

}
unsigned TWaveDisplayContainer::GetChannelHeight(){

	return numChannels == 1 ? Height - cWaveformMargin
							: (Height - (cWaveformMargin+cWaveformSpacing))/2;
}
 void TWaveDisplayContainer::ShowContainerView(TPaintBox *pView, EChannel eCh){

	Graphics::TBitmap *pViewBmp;
	const bool sel = selRange.selStart.x < selRange.selEnd.x ? true : false;

	switch(eContainerView){
		case WAVEFORM:
			pViewBmp = eCh==LEFT ? pWaveBmpLeft : pWaveBmpRight;
		   //	++paintCalled;
		  //	pFileNameLbl->Caption = "waveform"+IntToStr(++paintCalled);
			break;
		case SPECTROGRAM:
			pViewBmp = eCh==LEFT ? pSfftBmpLeft : pSfftBmpRight;
		//	pFileNameLbl->Caption = "spectrogram"+IntToStr(++paintCalled);
			break;
		case MIXED:
			pViewBmp = eCh==LEFT ? pMixedBmpLeft : pMixedBmpRight;
	}
	pView->Canvas->Draw(0, 0, pViewBmp);
   //	unsigned cId = containerId;
	if(!stretching && drawSelection/*&& completeViewRepaint && drawSelection  */)

		DrawSelectedRegion();

	else if( !sel )
		DrawCursorPosition( selRange.selStart.x);

	//pFileNameLbl->Caption = completeViewRepaint ? "true" : "false";
   //	completeViewRepaint = false;
	drawSelection = true;
  //pFileNameLbl->Caption = IntToStr(paintCalled);
}
 void __fastcall TWaveDisplayContainer::WaveOnPaint(TObject *sender)
{
	   TPaintBox *pWaveBox = (TPaintBox*)sender;

	   switch(pWaveBox->Tag){

		case nsWaveDisplay::LEFT_CHANNEL:
			ShowContainerView(pWaveBox, LEFT);
			break;

		case nsWaveDisplay::RIGHT_CHANNEL:
			ShowContainerView(pWaveBox, RIGHT);
			break;

		case nsWaveDisplay::LEFT_AMP:
		case nsWaveDisplay::RIGHT_AMP:

			pWaveBox->Canvas->Draw(0, 0, pBmpAmpLeft);
			break;
	  }
	/*pWaveBox->Canvas->Pen->Color = clBlack;
	  pWaveBox->Canvas->Brush->Style = bsClear;
	  pWaveBox->Canvas->Rectangle( TRect (0, 0, pWaveBox->Width, pWaveBox->Height) );
	  Canvas->Brush->Style = bsClear;
	  Canvas->Rectangle( TRect(0,0, Width, Height) );    */
	 if(eContainerView == WAVEFORM && (pWaveBox->Tag == LEFT_CHANNEL || pWaveBox->Tag == RIGHT_CHANNEL) )
		DrawCentralLine(pWaveBox);
}

void TWaveDisplayContainer::DrawCentralLine(TPaintBox *pView){

   pView->Canvas->Pen->Color = pWaveformView->GetWaveColour();
   pView->Canvas->MoveTo(0, channelHeight/2);
   pView->Canvas->LineTo(pView->Width, channelHeight/2);
   pView->Canvas->Pen->Color = clBlack;
}
/**
  handles paint messages for TPaintBox components, not the container itself
*/

void  TWaveDisplayContainer::Unselect(){
	//this entails changing the container color, removing the selected region if it exists
	 active = false;
	 pCloseTrack->Color = origColor;
	 Color = origColor;
	 //drawSelection = true;
}
void TWaveDisplayContainer::SetFileName(AnsiString &fileName){

	fName = fileName;
	fName = fName.SubString(fName.LastDelimiter("\\"), fName.Length()-1);
}
void TWaveDisplayContainer::SetSize(unsigned int w, unsigned int h){

	  Height = h;
	  Width =  scrollBarVisible ? Parent->ClientWidth
								: Parent->ClientWidth-(cScrollBarWidth+BorderWidth);
	  // to do - remove this line and set Pos from the manager
	 // Top =  (layoutPos * Height);
}
void TWaveDisplayContainer::ParentResized(unsigned w, unsigned h){
 // parentResized = true;
  pWaveDisplay->SetViewPortWidth(w - (cWaveformRightMargin + cWaveCtrlWidth + cAmpWidth));
  RecomputeSelection(w);
  Resize(w,h);
  ResizeContainer();
}
void TWaveDisplayContainer::RecomputeSelection(unsigned newWidth){

	const float wRatio 		= static_cast<float>(newWidth) / Width;
	selRange.selStart.x 	*= wRatio;
	selRange.selEnd.x 		= (selRange.selStart.x < selRange.selEnd.x) ? selRange.selEnd.x * wRatio : selRange.selStart.x;
	sfftRange.selStart.x 	= selRange.selStart.x;
	sfftRange.selEnd.x 		= selRange.selEnd.x;
	playbackPosition		= selRange.selStart.x;
}
//TODO - ovo ne treba zvati setsize ali trenutno bez toga ne radi zoom normal
void TWaveDisplayContainer::Resize(unsigned w, unsigned h){

 if(eStretchMode == NORMAL )
		SetSize(w, h);
	 else if(eStretchMode == MAX)
		SetSize(w, Height);
	 else if (eStretchMode == MIN) {      //cWaveformMinHeight
		SetSize(w, cWaveformMinHeight);
		  }
	 else if( eStretchMode == CUSTOM || eWaveMsgType == nsWaveDisplay::RESIZE)
		SetSize(w, Height);

}
void TWaveDisplayContainer::Position(int pos){

	 Top =  pos;
}
void __fastcall TWaveDisplayContainer::WaveContainerOnClick(TObject *Sender){

	 ActivateContainer();
}
void TWaveDisplayContainer::ResizeContainer(){

   pWaveLeft->Width = ClientWidth - (pbAmpLeft->Width + cWaveformRightMargin);

   if(numChannels==2){
		pWaveRight->Width =  pWaveLeft->Width;
	 }

	switch (eContainerView) {
		case WAVEFORM:
			completeViewRepaint = false;
			SetViewToStretch();
			DoDraw();
			break;
		case SPECTROGRAM:
			pSpectralData->SetViewPortWidth(pWaveLeft->Width);
			pSpectralData->SetViewPortHeight(channelHeight);

			AdjustSpectrogramSize(pWaveLeft->Width, channelHeight);
			break;
		case MIXED:

			DoDrawMixed();
			break;
		}
}
void __fastcall TWaveDisplayContainer::WaveContainerOnResize(TObject *Sender){

	pFileNameLbl->Top    = Height - cWaveformMargin;
	pCloseTrack->Top	 = Height - cWaveformMargin;
   //	drawSelection		 = false;
}
 void TWaveDisplayContainer::SetViewToStretchClear(){

   switch(eContainerView){
   case WAVEFORM:
  // if(pWaveBmpLeft->Height == pWaveBmpLeft0->Height) return;
   pWaveformView->SetContainerViewLeft(pWaveBmpLeft0);
   if(numChannels==2)pWaveformView->SetContainerViewRight(pWaveBmpRight);

   break;
   case MIXED:
 //  if(pMixedBmpLeft0->Height == pMixedBmpLeft->Height) return;
   pMixedView->SetContainerViewLeft(pMixedBmpLeft0);
   pMixedView->SetHeight(pMixedBmpLeft->Height);
   pMixedBmpLeft = pMixedView->StretchContainerViewLeft();
   if(numChannels==2)pMixedView->SetContainerViewRight(pMixedBmpRight0);
   break;
   case SPECTROGRAM:
//   if(pSfftBmpLeft->Height == pSfftBmpLeft0->Height) return;
   pSpectrogramView->SetContainerViewLeft(pSfftBmpLeft0);
   pSpectrogramView->SetHeight(pSfftBmpLeft->Height);

   pSfftBmpLeft = pSpectrogramView->StretchContainerViewLeft();
   if(numChannels==2){
		pSpectrogramView->SetContainerViewRight(pSfftBmpRight0);
		pSfftBmpRight = pSpectrogramView->StretchContainerViewRight();
   }
   break;
   }

}
void TWaveDisplayContainer::SetViewToStretch(){

   TContainerView * pView;

   Graphics::TBitmap *pBmp0, *pBmp1;

   const bool regionSelected = stftRegionSelection ? sfftRange.selEnd.x>sfftRange.selStart.x : selRange.selEnd.x > selRange.selStart.x;

   drawSelection = false;

   switch(eContainerView){
	case WAVEFORM:
		pView = pWaveformView;
		pBmp0 = pWaveBmpLeft0;
		if(numChannels==2) pBmp1 = pWaveBmpRight0;
		break;
   case MIXED:
		pView = pMixedView;
		pBmp0 = pMixedBmpLeft0;
		if(numChannels==2) pBmp1 = pMixedBmpRight0;
		break;
   case SPECTROGRAM:
		pView = pSpectrogramView;
		pBmp0 = pSfftBmpLeft0;
		if(numChannels==2) pBmp1 = pSfftBmpRight0;
   }
   if(!regionSelected){
		pView->SetContainerViewLeft(pBmp0);
		if(numChannels==2) pView->SetContainerViewRight(pBmp1);
	}else{
		pView->SetContainerViewLeft(pSelOverlay->GetSelectionOverlay(LEFT));
		if(numChannels==2)pView->SetContainerViewRight(pSelOverlay->GetSelectionOverlay(RIGHT));
   }
}
void TWaveDisplayContainer::ActivateContainer(){

	   pfWaveMsg(nsWaveDisplay::SELECT, containerId);
	   Color = clInactiveCaption;
	   pCloseTrack->Color   = clInactiveCaption;
	   active = true;
   	   drawSelection = true;
}
void __fastcall TWaveDisplayContainer::WaveContainerMouseDown(TObject *Sender, TMouseButton Button,
	  TShiftState Shift, int X, int Y)
{
	 if (Button == mbLeft){
		leftBtnDown = true;
		stretching = true;

	   SetViewToStretch();
	  // ActivateContainer();
	}
}
void __fastcall TWaveDisplayContainer::WaveContainerMouseMove(TObject *Sender, TShiftState Shift, int X,
	  int Y)
{
		 if(Y<0) return;
		 Cursor = (Height - cWaveformMargin < Y) ? crSizeNS : crDefault;
		 if(leftBtnDown){

			if( Y%adjustEvery==0 ) {
				CustomAdjustVertically(Y);
			}
		}
}
void __fastcall TWaveDisplayContainer::WaveContainerMouseUp(TObject *Sender, TMouseButton Button,
	  TShiftState Shift, int X, int Y)
{
	   if(leftBtnDown){
		pfWaveMsg(nsWaveDisplay::RESIZE, containerId);

		   leftBtnDown = false;
		   Cursor  = crDefault;
	  // we don't use CustomAdjustVertically(Y) because we've already done it in the WaveContainerMouseMove
		RecalculateStftRegionPos();
		stretching = false;
	   }
}
void TWaveDisplayContainer::RecalculateStftRegionPos(){

	 if(stftRegionSelection){
		const unsigned prevSelHeight = stftSelEndBin-stftSelStartBin;
		const float heightCorrection = (float)stftSelStartBin/prevHeight;//(sfftRange.selEnd.y-sfftRange.selStart.y);

		sfftRange.selStart.y =  Height*heightCorrection;
		sfftRange.selEnd.y =  sfftRange.selStart.y+prevSelHeight*((float)Height/prevHeight);
	}
}
void TWaveDisplayContainer::SpectrogramMouseDown(TPaintBox *pWaveContainer, int x, int y, bool shiftKey){

		if(shiftKey){
			sfftRange.selEnd.x = x;
			sfftRange.selEnd.y = y;
		//DrawSelectedSfftRegion();
			DrawSelectedRegion();
			return;
		}
		sfftRange.selStart.x < sfftRange.selEnd.x ? ClearPreviousSelection(TRect(sfftRange.selStart.x, 0, sfftRange.selEnd.x, pWaveContainer->Height)):ClearPrevCursorPosition( sfftRange.selStart.x );
		sfftRange.selStart.x = x;
		sfftRange.selStart.y = y;
		sfftRange.selEnd.x = x;
		sfftRange.selEnd.y = y;
		DrawSelectedRegion();
}
void TWaveDisplayContainer::SpectrogramMouseMove(TPaintBox *pWaveContainer, int x, int y){

			if(leftBtnDown){
				if(y>pWaveContainer->Height || y<0) return;
				sfftRange.selEnd.x = x ;
				sfftRange.selEnd.y = y ;
				eWaveMsgType = nsWaveDisplay::SELECT;
				pfWaveMsg(nsWaveDisplay::POSITION_INDICATOR, containerId);

				DrawSelectedRegion();
				//if( spectralSelectionNotification )		pfSpectrogramSelMsg();
			}
}
void TWaveDisplayContainer::SpectrogramMouseUp(TPaintBox *pWaveContainer, int x, int y){

	   if(leftBtnDown ){
			leftBtnDown = false;
			if(sfftRange.selStart.x > sfftRange.selEnd.x){
			int x = sfftRange.selStart.x;
			sfftRange.selStart.x = sfftRange.selEnd.x;
			sfftRange.selEnd.x = x;
			}
			if(sfftRange.selStart.y > sfftRange.selEnd.y){
			int y = sfftRange.selStart.y;
			sfftRange.selStart.y = sfftRange.selEnd.y;
			sfftRange.selEnd.y = y;
			}
			viewHeight = Height-cWaveformMargin;
			stftSelStartBin = sfftRange.selStart.y;
			stftSelEndBin = sfftRange.selEnd.y;

			prevHeight = Height;
		   /*	pSpectralData->SetSelection(sfftRange.selStart.x, sfftRange.selEnd.x, sfftRange.selStart.y, sfftRange.selEnd.y);
			stftSelStartBin = pSpectralData->GetSelStartBin();
			stftSelEndBin = pSpectralData->GetSelEndBin();   */
			if(spectralSelectionNotification )
				pfSpectrogramSelMsg();
		  }
}

void __fastcall TWaveDisplayContainer::WaveMouseDown(TObject *Sender, TMouseButton Button,
	  TShiftState Shift, int X, int Y)
{
	 TPaintBox *pWaveContainer = (TPaintBox*)Sender;
	 //TPaintBox *pOther; //used for 2-channel tracks
	 if (Button == mbLeft) {
		leftBtnDown = true; //CHANGE THIS ON TPanel
		ActivateContainer();

		bool shiftKeyPressed = Shift.Contains(ssShift);
	 //if in the spectrogram view
		if((eContainerView == SPECTROGRAM || eContainerView == MIXED) && stftRegionSelection){
			SpectrogramMouseDown(pWaveContainer, X, Y, shiftKeyPressed);
			return;
	 }

	 if(shiftKeyPressed){   //if the shift key is down , we can select  a region and return immediately
		selRange.selEnd.x = X;
		selRange.selEnd.y = Y;
		DrawSelectedRegion();
		return;
	 }

	 if(selRange.selStart.x < selRange.selEnd.x)  {   //clear the selection if there is one
		ClearPreviousSelection(TRect(selRange.selStart.x, 0, selRange.selEnd.x, pWaveContainer->Height));
	 }else if (selRange.selStart.x == selRange.selEnd.x){ //clear the previous pos indicator
		ClearPrevCursorPosition( selRange.selStart.x) ;
	 }
		selRange.selStart.x = X;
		selRange.selStart.y = Y;
		selRange.selEnd.x = X;
		selRange.selEnd.y = Y;
		DrawSelectedRegion();
		//send a callback to the manager in order to notify the main app window to update the position
		playbackPosition = selRange.selStart.x;
		pfWaveMsg(nsWaveDisplay::POSITION_INDICATOR, containerId);
		if(containersNum > 1){   //if we have multiple tracks, notify the manager in order to unselect other tracks
			pfWaveMsg(nsWaveDisplay::REMOVE_SELECTION, containerId);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayContainer::WaveMouseMove(TObject *Sender, TShiftState Shift, int X,
	  int Y)
{
		 TPaintBox* pWaveContainer = (TPaintBox*)Sender;

		 if(X > pWaveContainer->Width) return;
		 //if stretching is true, it means that we're still resizing the waveform/spectrogram container,
		 //so let's just finish resizing and return
		if(eContainerView == SPECTROGRAM || eContainerView == MIXED){
		  //	ShowFFTBinData(X,pWaveContainer->Height-Y);
			if(stftRegionSelection){
				SpectrogramMouseMove(pWaveContainer, X, Y);
				return;
			}
		}
		if(leftBtnDown){
				selRange.selEnd.x = X;
				selRange.selEnd.y = Y;

				DrawSelectedRegion();
				eWaveMsgType = nsWaveDisplay::SELECT;
                	//this part will select multiple tracks (it calls the manager through a INIT_SELECTION callback )
				if(containersNum > 1){
					if (Y > Height  || Y < 0 || selectionExtExtend)
						pfWaveMsg(nsWaveDisplay::INIT_SELECTION, containerId);
				}
				pfWaveMsg(nsWaveDisplay::POSITION_INDICATOR, containerId);
		}
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayContainer::WaveMouseUp(TObject *Sender, TMouseButton Button,
	  TShiftState Shift, int X, int Y)
{
	   TPaintBox* pWaveContainer = (TPaintBox*)Sender;
	   if(Button == mbLeft && leftBtnDown ){

			if(eContainerView == SPECTROGRAM && stftRegionSelection){
				SpectrogramMouseUp(pWaveContainer, X, Y);
				return;
			}
			leftBtnDown = false;
			selectionExtExtend = false;

			if(selRange.selStart.x > selRange.selEnd.x){
				int x = selRange.selStart.x;
				selRange.selStart.x = selRange.selEnd.x;
				selRange.selEnd.x = x;
				playbackPosition = selRange.selStart.x;
				pfWaveMsg(nsWaveDisplay::POSITION_INDICATOR, containerId);
			}
			if(eContainerView == SPECTROGRAM){

				sfftRange.selStart.x = selRange.selStart.x;
				sfftRange.selEnd.x =  selRange.selEnd.x;
				sfftRange.selStart.y = pWaveContainer->Height;
				sfftRange.selEnd.y =  0;

			if( spectralSelectionNotification )
				pfSpectrogramSelMsg();
			}
		 // 	drawSelection = true;
	   }
		 stretching = false;
}
void TWaveDisplayContainer::DrawCursorPosition(int curPos){

	pWaveLeft->Canvas->MoveTo(curPos, 0);
	pWaveLeft->Canvas->LineTo(curPos, pWaveLeft->Height);
	if(numChannels == 2){
		pWaveRight->Canvas->MoveTo(curPos, 0);
		pWaveRight->Canvas->LineTo(curPos, pWaveRight->Height);
	}
}
void TWaveDisplayContainer::ClearPrevCursorPosition(int prevCurPos){


	Graphics::TBitmap *pBmp = eContainerView == WAVEFORM
	? pWaveBmpLeft : eContainerView == SPECTROGRAM ? pSfftBmpLeft : pMixedBmpLeft;

	TRect selRect(prevCurPos, 0, prevCurPos+1, pWaveLeft->Height);
	pWaveLeft->Canvas->CopyRect(selRect, pBmp->Canvas, selRect);

	if(numChannels == 2){
		pBmp = eContainerView == WAVEFORM ? pWaveBmpRight : eContainerView == SPECTROGRAM ? pSfftBmpRight : pMixedBmpRight;
		pWaveRight->Canvas->CopyRect(selRect,pBmp->Canvas,selRect);
	}
}
void TWaveDisplayContainer::ClearPreviousSelection(TRect& const selRect){

	 SetViewToStretchClear();
	 Graphics::TBitmap *pWaveBmp = eContainerView == WAVEFORM ? pWaveformView->StretchContainerViewLeft() :
	 eContainerView == SPECTROGRAM ? pSpectrogramView->StretchContainerViewLeft() :
	 pMixedView->StretchContainerViewLeft();
	 pWaveLeft->Canvas->Draw(0, 0, pWaveBmp);

		  if(numChannels == 2){
			pWaveBmp = eContainerView == WAVEFORM ? pWaveBmpRight : eContainerView == SPECTROGRAM ? pSfftBmpRight : pMixedBmpRight;
			pWaveRight->Canvas->CopyRect(selRect,pWaveBmp->Canvas,selRect);
		  }
}
void TWaveDisplayContainer::DrawSelectedRegion(){

	Graphics::TBitmap *pBmp, *pBmp_;
	const SELECTION_RANGE &sRange = stftRegionSelection ? sfftRange : selRange;

	if(sRange.selStart.x == sRange.selEnd.x){
			DrawCursorPosition( sRange.selStart.x);
			return;
	}

	if	(stretching)  return;

	switch(eContainerView){
		case WAVEFORM:
			pBmp = pWaveBmpLeft;
			if(numChannels==2)pBmp_ = pWaveBmpRight;
			break;

		case SPECTROGRAM:
			pBmp = pSfftBmpLeft;
			if(numChannels==2)	pBmp_ = pSfftBmpRight;
			break;

		case MIXED:
			pBmp = pMixedBmpLeft;
			if(numChannels==2)	pBmp_ = pMixedBmpRight;
	}

	if(!pSelOverlay) pSelOverlay = new TSelectionOverlay();

	TRect selRect = stftRegionSelection ? TRect(sRange.selStart.x, sRange.selStart.y, sRange.selEnd.x, sRange.selEnd.y)
										: TRect(sRange.selStart.x, 0, sRange.selEnd.x, pWaveLeft->Height);

	Graphics::TBitmap * pOverlayLeft = pSelOverlay->GetSelectionOverlay(pBmp, clGray, selRect, LEFT);

	if(numChannels == 2){
		Graphics::TBitmap * pOverlayRight = pSelOverlay->GetSelectionOverlay(pBmp_, clGray, selRect, RIGHT);
		pWaveRight->Canvas->Draw(0, 0, pOverlayRight);
	}

	pWaveLeft->Canvas->Draw(0, 0, pOverlayLeft);

}

void TWaveDisplayContainer::FreeResources(){

  containersVisibleNum--;
  containersNum--;

  if( pWaveDisplay )		delete pWaveDisplay;   //WaveDisplayData

  if( pSoundData ) 			delete pSoundData;

  if( pSpectrogramView )  	delete pSpectrogramView;

  if( pWaveformView ) 		delete pWaveformView;

  if( pMixedView )			delete pMixedView;

  if( pSelOverlay )			delete pSelOverlay;

  if( pFFTBinInfo )			delete pFFTBinInfo;

  if( pBmpAmpLeft )			delete pBmpAmpLeft;

  if( pSpectralData ) 		delete pSpectralData;

  if( pSoundSource ){
	switch(eSoundSource){
		case TONE_GENERATOR:
			delete reinterpret_cast<Tone*>(pSoundSource);
			break;

		case DIGITAL_FILTER:
			delete reinterpret_cast<DigitalFilter*>(pSoundSource);
			break;
	}
  }

 /* if(numChannels == 2){
	if( pBmpAmpRight ) delete pBmpAmpRight;
  }*/ //end numChannels
}
/**
this method draws the playback indicator ( | ) in response to nsPlayback::POSITIONED message
sent by the wave player implementation class
*/
void TWaveDisplayContainer::PositionPlaybackIndicator(unsigned currSample){

	Graphics::TBitmap *pBmpl, *pBmpr;
   //	 static  unsigned cPos = 0;
	if(eContainerView == WAVEFORM){
		pBmpl = pWaveBmpLeft;
		if(numChannels==2) pBmpr = pWaveBmpRight;
	}else{
		pBmpl = pSfftBmpLeft;
		if(numChannels==2) pBmpr = pSfftBmpRight;
	}
	//static unsigned prevPos = 0;
	unsigned int viewPortSamples = pWaveDisplay->GetViewPortSamples();
	unsigned currPos = pWaveLeft->Width * static_cast<float>(currSample)/viewPortSamples;  //dont have to calc this for every pass

	if(numChannels == 2) currPos /= 2;
	if( prevPos == currPos ) return;
	 //clear the previous playback indicator; if there was a selection, redraw the selection, which clears the indicator
	 //otherwise, just clear the previous indicator line using the previous indicator position

	 if(selRange.selStart.x < selRange.selEnd.x){
		DrawSelectedRegion();
	 }else{
		TRect selRect(prevPos + selRange.selStart.x, 0, prevPos + selRange.selEnd.x+1, pWaveLeft->Height);
		pWaveLeft->Canvas->CopyRect(selRect, pBmpl->Canvas, selRect);
		if(numChannels == 2){
			pWaveRight->Canvas->CopyRect(selRect,pBmpr->Canvas,selRect) ;
		}
	}
	unsigned cPos = currPos + selRange.selStart.x;
	if(selRange.selEnd.x>selRange.selStart.x && cPos > selRange.selEnd.x) cPos = selRange.selEnd.x;

	pWaveLeft->Canvas->MoveTo(cPos , 0); //currPos + selRange.selStart.x
	pWaveLeft->Canvas->LineTo(cPos, pWaveLeft->Height);

	if(numChannels == 2){
		pWaveRight->Canvas->MoveTo(cPos, 0);
		pWaveRight->Canvas->LineTo(cPos, pWaveRight->Height);

	}

	prevPos 		 = currPos;
	playbackPosition = cPos;
	this->currSample = currSample;

	pfWaveMsg(nsWaveDisplay::POSITION_INDICATOR, containerId);
}
/**
you can use the arrow keys ( <-  and -> ) to move the playback indicator to a new position
*/
int TWaveDisplayContainer::CalculateSnapToSampleOffset(EPlaybackIndicatorMove eMove){

	PWAVE_POINT wpValues =  pWaveDisplay->GetSampleValues();
	int currSampleIndex = pWaveDisplay->GetSamplePosition(selRange.selStart.x);
	int currSamplePos = wpValues[currSampleIndex].x;// ? wpValues[currSampleIndex].x>selRange.selStart.x : wpValues[currSampleIndex-1].x>selRange.selStart.x;
	int moveOffset=0;
	snapToSample = pWaveDisplay->GetWaveformDisplayMode()==PIXEL_AVERAGING ? true : false;
	if(snapToSample){
	  moveOffset = currSamplePos-selRange.selStart.x;
	  if(eMove == FORWARD && moveOffset<=0)
		moveOffset = wpValues[currSampleIndex+1].x-selRange.selStart.x;
	  else if (eMove == BACKWARD && moveOffset>=0)
		moveOffset = wpValues[currSampleIndex-1].x-selRange.selStart.x;
	}else{
		moveOffset = eMove == FORWARD ? 1 : -1;
	}
	return moveOffset;
}
void TWaveDisplayContainer::MovePlaybackIndicator(EPlaybackIndicatorMove eMove){

	const int moveOffset = CalculateSnapToSampleOffset(eMove);//eMove == FORWARD ? 1 : -1;

	if(selRange.selStart.x < selRange.selEnd.x){
		DrawSelectedRegion();
	}else{
		TRect selRect(prevPos + selRange.selStart.x, 0, prevPos + selRange.selEnd.x+1, pWaveLeft->Height);

		pWaveLeft->Canvas->CopyRect(selRect, pWaveBmpLeft->Canvas, selRect);
		if(numChannels == 2)
			pWaveRight->Canvas->CopyRect(selRect, pWaveBmpRight->Canvas, selRect);
	}

	ClearPrevCursorPosition(selRange.selStart.x);
	selRange.selStart.x += moveOffset;

	pWaveLeft->Canvas->MoveTo(selRange.selStart.x, 0);
	pWaveLeft->Canvas->LineTo(selRange.selStart.x, pWaveLeft->Height);

	if(numChannels == 2){
		pWaveRight->Canvas->MoveTo(selRange.selStart.x, 0);
		pWaveRight->Canvas->LineTo(selRange.selStart.x, pWaveRight->Height);

	}
	playbackPosition = selRange.selStart.x;
	pfWaveMsg(nsWaveDisplay::POSITION_INDICATOR, containerId);
}
void TWaveDisplayContainer::ZoomWave(){

	 completeViewRepaint = true;

	 if(eContainerView == WAVEFORM){
		DoDraw();
	 }else{

		pSpectralData->SetViewPortRange(pWaveDisplay->GetStartSample(), pWaveDisplay->GetEndSample());
		pSpectralData->SetInputData(
			SPECTROGRAM_PARAMS(pWaveDisplay->GetCurrentData(),
			pWaveDisplay->GetViewPortSamplesTotal(), numChannels,
			pSoundData->GetHeaderInfo()->wfex.nSamplesPerSec, eWnd,
			frameLen, SPECTROGRAM_PARAMS::FO2, eScale ));

		if(pSpectralData->GetOverlappingFramesNumber()>0){
			pSpectralData->RecalculateFFT();
			DoDrawSfft();
			}
	 }
}
bool TWaveDisplayContainer::ZoomIn(){

	 bool max = pWaveDisplay->ZoomIn(selRange.selStart.x);

	 ZoomWave();

	 return max;
}
bool TWaveDisplayContainer::ZoomOut(){

	 bool min = pWaveDisplay->ZoomOut(selRange.selStart.x);

	 ZoomWave();

	 return min;
}
bool TWaveDisplayContainer::ZoomInFull(){

	 bool max = pWaveDisplay->ZoomInFull(selRange.selStart.x);
	 ZoomWave();

	 return max;
}
bool TWaveDisplayContainer::ZoomOutFull(){

	 bool min = pWaveDisplay->ZoomOutFull(selRange.selStart.x);
	 ZoomWave();
	 return min;
}
void TWaveDisplayContainer::PlaybackSelectionChanged(){

	pfWaveMsg(nsWaveDisplay::SELECT_WAVEFORM, containerId);
	ActivateContainer();
}
/**
this method selects or unselects a particular waveform track for playback
this method is called as a result of either the Mute or Solo button being clicked
*/
void TWaveDisplayContainer::SelectForPlayback(bool selected) {
	if(!hidden){
		bool repaint = selectedForPlayback == selected ? false : true;
		selectedForPlayback = selected;
		if( repaint ) DoDraw();
	}
}
const WAVE_PLAYBACK_DATA& TWaveDisplayContainer::GetWavePlaybackData(){

	wavePlaybackData.samplesPerSec = pSoundData->GetHeaderInfo()->wfex.nSamplesPerSec;
	wavePlaybackData.bitsPerSample = pSoundData->GetHeaderInfo()->wfex.wBitsPerSample;

	wavePlaybackData.pWaveData = (PWAVE_DISPLAY_DATA)&pWaveDisplay->GetInputData();
	SetSampleRange();
	wavePlaybackData.sampleRange.startSample =  sampleRange.startSample;
	wavePlaybackData.sampleRange.endSample 	 =  sampleRange.endSample;
	return wavePlaybackData;
}
/**
this method sets the start and end samples for playback from the selection range.
If there is no selection, the end sample is set to the last sample.
*/
void TWaveDisplayContainer::SetSampleRange(){

		sampleRange.startSample = pWaveDisplay->GetAbsoluteSample(selRange.selStart.x, true);
		sampleRange.endSample =   selRange.selStart.x == selRange.selEnd.x  ?
												  pWaveDisplay->GetEndSample()
												: pWaveDisplay->GetAbsoluteSample(selRange.selEnd.x, false);

}
const SAMPLE_RANGE& TWaveDisplayContainer::GetSampleRange() const{
	 return sampleRange;
}
//this method is called by the manager to draw a selection (in response to INIT_SELECTION )
void TWaveDisplayContainer::DrawSelection(unsigned int cPosStart, unsigned int cPosEnd){
		selRange.selStart.x = cPosStart;
		selRange.selEnd.x = cPosEnd;
		DrawSelectedRegion();
	   //	drawSelection = true;
}

//this method is called by the manager to clear the selection (in response to REMOVE_SELECTION)
void TWaveDisplayContainer::ClearSelection(){
		selRange.selStart.x = 0;
		selRange.selEnd.x = 0;
		pWaveLeft->Canvas->Draw(0, 0, eContainerView==WAVEFORM ? pWaveBmpLeft : eContainerView==MIXED ? pMixedBmpLeft : pSfftBmpLeft);   //invalid rect
		if(numChannels == 2){
			pWaveRight->Canvas->Draw(0, 0, eContainerView==WAVEFORM ? pWaveBmpRight : eContainerView==MIXED ? pMixedBmpRight : pSfftBmpRight);
		}
}






