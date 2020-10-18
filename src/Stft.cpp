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

#pragma hdrstop

#include "Stft.h"
#include "FileLayout.h"
#include "GenPtrAlloc.h"

const float overlapOversampling[] = { 2.0f, 4.0f, 8.0f };

const float overlapPart[] 		  = { 0.5f, 0.25f, 0.125f };

const unsigned stepFactor[] 	  = { 4, 2, 1 };
//---------------------------------------------------------------------------
#if defined _DEBUG_
SpectrogramDisplayData::SpectrogramDisplayData(unsigned width, unsigned height)
: cMagPosDelta(200), debugger(Debugger::GetInstance()), newLine('\n'){

	viewPortWidth = width;
	viewPortHeight = height;
	Init();                                          
}
#else
SpectrogramDisplayData::SpectrogramDisplayData(unsigned width, unsigned height)
: cMagPosDelta(200)
{
	viewPortWidth = width;
	viewPortHeight = height;
	Init();                                        
}
#endif

void SpectrogramDisplayData::Init(){

		minFrameWidth = 4;
		framesPerPixel = 0;
		fFrameWidth = 1.0f;
		wndSum0 = 1;
		wndSum1 = 1;
		maxFFTVal = -100;
		threshHold = -20.0f;
		selStartBin = 1;
		selEndBin = 512;
	// 32;
		//normally the compiler should initialize pointers to a null value (but this doesn't always work - a compiler bug)
		pWndData = 0;
		pBinIndex = 0;
		pWaveData = 0;
		maxMagnitude = minMagnitude = 0;
		pLock = 0;
		useBkgThreads = false;
		multipleBuffers = false;
	   	prevTotalBinsPerFrame = 0;
	   //	UseBkgThread(true);//false;
}
void SpectrogramDisplayData::FreeResources(){

	int i;
	GenPtrAlloc<unsigned>::DeallocateArray<unsigned>(pBinIndex);

	GenPtrAlloc<float>::DeallocateArray<float>(pWndData);

	const unsigned fftWndNum = vFFT.size();
	const unsigned fftOverlappedWndNum = vWndOverlappedFFT.size();

	for( i=0; i<fftWndNum;i++){
		float *p = vFFT[i];
		GenPtrAlloc<float>::DeallocateArray<float>(p);
	}
	for( i=0; i<fftOverlappedWndNum;i++){
		float *pFFT = vWndOverlappedFFT[i];
		GenPtrAlloc<float>::DeallocateArray<float>( pFFT );
	}

	vWndOverlappedFFT.clear();
	vWndCombinedFFT.clear();
	vWndFFT.clear();
	vWThread.clear();
	vWndMagnitude.clear();
	vWndTotalMagnitude.clear();
}
void SpectrogramDisplayData::AdjustViewportVertically(unsigned newHeight){

	 prevViewPortHeight = viewPortHeight;
	 viewPortHeight = newHeight;
	 SetNumWindows();
	 SetSpectrogramDisplayData();

}
void SpectrogramDisplayData::AdjustViewportSize(unsigned newWidth, unsigned newHeight){

	 if(viewPortWidth != newWidth) {
		prevViewPortHeight = viewPortHeight;
		prevViewPortWidth = viewPortWidth;
		viewPortHeight = newHeight;
		viewPortWidth =  newWidth;
		(useBkgThreads) ? DoStft() : RecalculateFFT(); //??? why DoSfft
	 }else{
		AdjustViewportVertically(newHeight);
	 }

}
void SpectrogramDisplayData::SetFrameWindow(WindowFunc::EWindow eWnd){

	sdData.eWnd = eWnd;
}
void SpectrogramDisplayData::SetFrameOverlap(SPECTROGRAM_PARAMS::EFrameOverlap efOverlap){
  // frameOverlap =  efOverlap;
   sdData.frameOverlap = efOverlap;
   overlapLen = overlapPart[efOverlap] * sdData.frameLen;
   stepLen = sdData.frameLen-overlapLen;
   SetNumWindows();
}
void SpectrogramDisplayData::SetFrameLength(unsigned frameLen){

	sdData.frameLen = frameLen;
	frameLenHalf =  frameLen/2;
	overlapLen = frameLen * overlapPart[sdData.frameOverlap]; 
	frameWidth =  viewPortWidth / (( static_cast<float>(sdData.numSamples)/sdData.numChannels) / frameLen);
	freqResolution =  static_cast<float>(sdData.samplesPerSec) / frameLen;
	fFrameWidth = frameWidth;   //???
	if(frameWidth < 1) frameWidth = 1;

	SetNumWindows();
}
unsigned SpectrogramDisplayData::GetFrameLength() const{
	return sdData.frameLen;
}
/**
this method computes the frame width, which corresponds to the number of pixels taken
by a single STFT frame

*/
void SpectrogramDisplayData::CalculateFrameWidth(){

	frameWidth =  static_cast<float>(viewPortWidth) / (( static_cast<float>(sdData.numSamples)/sdData.numChannels) / sdData.frameLen);
    fFrameWidth = frameWidth;
	if(frameWidth < 1){
		frameWidth = 1;
	}
	SetNumWindows();
	//if we have only 1 frame let's make the frame width the same as the viewport width
	if(numFrames==1) frameWidth = viewPortWidth;
}
void SpectrogramDisplayData::SetInputData( SPECTROGRAM_PARAMS& const rSpectData){

	sdData = rSpectData;
	//buduæi je pData, eScale i numChanneels clanovi sdData možda nije potrebno ih koprati u samostalne varijable
	numChannels 	=  rSpectData.numChannels;
	pData 			=  rSpectData.pData;
	eScale 			=  rSpectData.eScale;
	frameLenHalf 	=  sdData.frameLen/2;
	overlapLen 		=  sdData.frameLen * overlapPart[rSpectData.frameOverlap]; //windowLen / 2
	//stepLen = windowLen - overlapLen;
	CalculateFrameWidth();

	freqResolution =  static_cast<float>(rSpectData.samplesPerSec) / rSpectData.frameLen;
	maxFrequency = static_cast<float>(sdData.samplesPerSec) / 2;  //Nyquist frequency

}
void SpectrogramDisplayData::RecalculateFFT(){

	FreeResources();
	DoStft();
}
void SpectrogramDisplayData::DoStft(){

	vFFT.clear();
	vWndFFT.clear();
	vWndOverlappedFFT.clear();
	vWndTotalMagnitude.clear();

	vFFT.resize(numWindows*numChannels);
	vWndFFT.reserve(numWindows*numChannels);
	vWndOverlappedFFT.reserve((numWindows*numChannels)/2+10);
	vWndTotalMagnitude.reserve((numWindows*numChannels)/2+10);
	totalFrames = 0;
	if(useBkgThreads){

		vWndMagnitude.clear();
		vThreadsEx.clear();
		vWndMagnitude.reserve((numWindows*numChannels)/2+1);
		vWndCombinedFFT.reserve((numWindows*numChannels)/2+1);
		vThreadsEx.reserve(numWindows/2+1);

		CreateAndInitThreads();
		if( multipleBuffers )SetThreadParams(numThreads, numFrames);
		//start the threads
		for(int i=0;i<numThreads;i++){
			vWThread.at(i)->Resume();
		}
	}else{
		SetWndData();
		CalculateWindowFFTs();
		GetMaxVal(maxIndex, maxFrame);
		SetSpectrogramDisplayData();
	}
}
void SpectrogramDisplayData::SetWndData(){

	wndData.SetNumChannels(numChannels);
	wndData.SetWndParams(sdData.eWnd, sdData.frameLen);
	wndData.SetWndFuncSums(wndSum0, wndSum1);
}

void SpectrogramDisplayData::UseBkgThread(bool useThread){

	useBkgThreads = useThread;

	if(useThread && !pLock){
		pLock = new TCriticalSection();
	}
}
void SpectrogramDisplayData::CreateAndInitThreads(){

	numThreads = TWorkerThread::GetNumCores();
  //	numThreads = numCores;
	vWThread.clear();
	 for(int i=0;i<numThreads;i++){
		TWorkerThread * pWorkerThread = new TWorkerThread( true );
		pWorkerThread->SetWorkerMethodCallback( &CalculateWindowFFTs );
		pWorkerThread->SetAllThreadsFinishedCallback( &ComputeFFT );
		vWThread.push_back( pWorkerThread );
	 }
}
void SpectrogramDisplayData::ComputeFFT(){

	SortFftFrames();
	GetMaxVal(maxIndex, maxFrame);
	SetSpectrogramDisplayData();

	if(pfSpectCallback) pfSpectCallback();
}
/**
  this copies and pads input data with zeros to  sdData.frameLen length
  used for the last frame when needed
*/
void SpectrogramDisplayData::CopyAndPadData(float *pPaddedLeft, float *pPaddedRight){

	const unsigned numInSamples = numChannels*(sdData.frameLen - numSamplesPadding);
	const unsigned len = numChannels*sdData.frameLen;

	const unsigned offset = numChannels*overlapLen*(numWindows-1);
	const float *pData = sdData.pData;
	int i, j;
	if(sdData.numChannels==1){
		for( i=0; i<numInSamples; i++){
			pPaddedLeft[i] = pData[i+offset];
		}
		for(i=numInSamples; i<len; i++){
			pPaddedLeft[i] = 0;
			}
	}else{
		for( i=0,j=0; i<numInSamples; i+=2,j++){
			pPaddedLeft[j]  = pData[i+offset];
			pPaddedRight[j] = pData[i+offset+1];
		}
		for( i=numInSamples/2; i<len/2; i++){
			pPaddedLeft[i]  = 0;
			pPaddedRight[i] = 0;
		}
	}
}
void SpectrogramDisplayData::CopyAndPadData(float *pPaddedLeft, float *pPaddedRight,
float *pData, unsigned numSamplesPadding){

	const unsigned frameLen = sdData.frameLen;
	const unsigned len = numChannels * frameLen;
 //	const unsigned numInSamples = numChannels * (sdData.frameLen-numSamplesPadding);
	const unsigned numInSamples = len - numSamplesPadding;
	const unsigned numInHalfSamples = numInSamples / 2;

 //	const unsigned offset = 0;//numChannels*overlapLen*(numWindows-1);
   //	const float *pData = sdData.pData;
   int i,j;
	if(sdData.numChannels==1){
		for( i=0; i<numInSamples; i++){
			pPaddedLeft[i] = pData[i];
		}
		for( i=numInSamples; i<len; i++){
			pPaddedLeft[i] = 0;
		}
	}else{
		for( i=0, j=0; j<numInSamples; j+=2, i++){
			pPaddedLeft[i]  = pData[j];
			pPaddedRight[i] = pData[j+1];
		}
		for(i=numInHalfSamples; i<frameLen; i++){
			pPaddedLeft[i] = 0;
			pPaddedRight[i] = 0;
		}
	}
}

/**
 since we're using an in-place FFT algorithm, we should copy the original sound data
*/
void SpectrogramDisplayData::CopyData(float *pRealLeft, float *pRealRight, unsigned offset){

	float *pData = sdData.pData;
	const unsigned len = numChannels*sdData.frameLen;//numChannels==2 ? numChannels*sdData.frameLen : sdData.frameLen;
	if(sdData.numChannels==1){
		for(int i=0; i<len; i++){
			pRealLeft[i] = pData[i+offset];
		}
	}else{
		for(int i=0,j=0; j<len; i++,j+=2){
			pRealLeft[i]  = pData[j+offset];
			pRealRight[i] = pData[j+offset+1];

	   }
	}
}
/**
 since we're using an in-place FFT algorithm, we should copy the original sound data
*/
void SpectrogramDisplayData::CopyData(float * pRealLeft, float * pRealRight, float * pData){

   //	float *pData = sdData.pData;
	const unsigned len = numChannels*sdData.frameLen;//numChannels==2 ? numChannels*sdData.frameLen : sdData.frameLen;
	if(sdData.numChannels==1){
		for(int i=0; i<len; i++){
			pRealLeft[i] = pData[i];
		}
	}else{
		for(int i=0,j=0; j<len; i++,j+=2){
			pRealLeft[i]  = pData[j];
			pRealRight[i] = pData[j+1];

	   }
	}
}
/*
  numThreads - number of threads
  numWnd -number of non-overlapping windows
*/
void SpectrogramDisplayData::SetThreadParams(unsigned numThreads, unsigned numWnd){

	unsigned startSample, endSample, startBuffer, endBuffer;
	const unsigned nSamplesPerThread = static_cast<float>(sdData.numSamples)/numThreads;
	vMthread.clear();

	for(int i=0; i<numThreads;i++){
		startSample = this->startSample + (i * nSamplesPerThread);
		if(numChannels == 2 && startSample % 2 != 0) startSample++;
		endSample	= startSample + nSamplesPerThread;
		startBuffer = startSample / samplesPerBuffer;
		endBuffer 	= endSample / samplesPerBuffer;
		vMthread.push_back( MULTI_THREAD_PARAMS( startBuffer, endBuffer, startSample, endSample ) );
	}
}
void SpectrogramDisplayData::ComputeFFTsMulBuffers(unsigned startWnd, unsigned& endWnd, unsigned tId){

	//kod multithreading-a , svaki thread mora imati svoju vlastitu varijablu za startBuffer, lastBuff, currBuffer itd
	unsigned startBuffer, lastBuffer, startSample = this->startSample, endSample;
	if( useBkgThreads ){
		MULTI_THREAD_PARAMS & mtp = vMthread.at(tId-1);
		startSample = mtp.startSample;
		endSample   = mtp.endSample;
		startBuffer = mtp.startBuffer;
		lastBuffer  = mtp.endBuffer;
	}else{
		endSample = this->endSample == 0 ? startSample + sdData.numSamples : this->endSample;
		startBuffer = startSample / samplesPerBuffer;
		lastBuffer  = endSample / samplesPerBuffer;
	}

	unsigned relStartSample  = startSample > 0 ? startSample-(startBuffer * samplesPerBuffer) : 0;
	unsigned relEndSample    = endSample-(lastBuffer * samplesPerBuffer) ;

	float * pCurrBufferPtr = &dataBuffers->at(startBuffer)[relStartSample];
	float * pRealLeft, * pRealRight;

	unsigned currBuff = startBuffer;
	unsigned currBuffLen = startBuffer==lastBuffer ? relEndSample : samplesPerBuffer;

	unsigned offset = relStartSample;
	unsigned paddLen = 0;
	const unsigned fftLen =  sdData.frameLen;
	const unsigned totalFftLen = fftLen * numChannels;
	const unsigned fftSize = fftLen * sizeof(float);
	int i;

	for(i=startWnd; i<endWnd; i+=numChannels){

		pRealLeft = new float[fftLen+2];

		if(numChannels==2){
			pRealRight = new float[fftLen+2];
		}

		if(offset >= currBuffLen && startBuffer!=lastBuffer) {//we should switch to the next buffer
			offset = 0;
			pCurrBufferPtr = dataBuffers->at(++currBuff);//currBuffer mora biti globalna zbog multithreading-a , odnosno svaki thread mora imati svoju vlastitu
			if(currBuff == lastBuffer) currBuffLen = relEndSample;
		}
		//provjeri dokud ide endWnd u odnosu na lastBuff
		if (currBuff == lastBuffer && offset + totalFftLen > currBuffLen/*relEndSample*/) {//padding necessary
				paddLen = offset + totalFftLen - currBuffLen;//lastBuffEndSample - pCurrBufferPtr;
				CopyAndPadData(&pRealLeft[0], &pRealRight[0], pCurrBufferPtr, paddLen);
			  //	offset = currBuffLen;
		}else{
			if(offset + totalFftLen > currBuffLen){
				paddLen = offset+totalFftLen-currBuffLen;//-pCurrBufferPtr;
				CopyAndPadData(&pRealLeft[0], &pRealRight[0], pCurrBufferPtr, paddLen);

			}else{ //we're still completely inside the current buffer, which means no padding is needed
				if(numChannels==1)
					memcpy(&pRealLeft[0], pCurrBufferPtr, fftSize);
				else
					CopyData(pRealLeft, pRealRight, pCurrBufferPtr);
			}
		}
		offset += (overlapLen*numChannels);
		pCurrBufferPtr += (overlapLen*numChannels);

		wndData.CreateWndData(&pRealLeft[0], &pRealRight[0]);
		fft.ComputeRealFFT(&pRealLeft[0], fftLen, false);

		if(sdData.numChannels==2){
			fft.ComputeRealFFT(&pRealRight[0], fftLen, false);
		}

		vFFT[i] = &pRealLeft[0];
		if(numChannels==2)	vFFT[i+1] = pRealRight;

		totalFrames++;

		if(currBuff == lastBuffer && offset >= currBuffLen){
			break;
		}

		}
		//with mutlipleThreads, we often have fewer processed frames than needed (this is a bug) so this is a temporary fix
		if(tId != 0) endWnd = i;
}

void SpectrogramDisplayData::CalculateWindowFFTs(){

	unsigned offset = 0;
	const unsigned nWnd = numWindows * numChannels;
	unsigned startWnd = 0, endWnd = nWnd, threadId = 0;
	if( useBkgThreads ){
	  //	bLen = bSize;
		threadId = TWorkerThread::GetCurrentThreadId();
		unsigned wndPerThread = nWnd / numThreads;
		if(numChannels == 2 && wndPerThread % 2 != 0)wndPerThread++;
		startWnd = (threadId-1) * wndPerThread;
		endWnd	 =	threadId * wndPerThread;
		offset  += (startWnd * overlapLen); //maknuti odavde

		if( numThreads == threadId ){
			endWnd = nWnd;
		}
	} //end bkg threads
	float * pRealLeft, * pRealRight;
	const unsigned fftLen = sdData.frameLen;
	unsigned fftSize = fftLen * sizeof(float);
	const unsigned nSamples = sdData.numSamples;

	if(multipleBuffers){
		ComputeFFTsMulBuffers(startWnd, endWnd, threadId);
	}else{

	for(int i=startWnd; i<endWnd; i+=numChannels){
	   //	if(j==0)
		pRealLeft = new float[fftLen+2];

		if(numChannels==2){
			pRealRight = new float[fftLen+2];
		}

		if(i*overlapLen+fftLen>nSamples-1)fftSize -= (((i*overlapLen+fftLen)-(nSamples+1))*sizeof(float));
	  // memcpy(&pRealLeft[fftLen*j], &pData[i*overlapLen], fftSize);
			//we keep the original samples intact; here we copy the samples (and pad with zeros if necessary)
		if(framePadding && i==nWnd-numChannels)
			CopyAndPadData(&pRealLeft[0], &pRealRight[0]);
		else{
		 if(numChannels==1)
			memcpy(&pRealLeft[0], &pData[i*overlapLen], fftSize);
		 else
			CopyData(pRealLeft, pRealRight, offset);
		}

		//WindowData(&pRealLeft[0], &pRealRight[0]);
		wndData.CreateWndData(&pRealLeft[0], &pRealRight[0]);
		fft.ComputeRealFFT(&pRealLeft[0], fftLen, false);

		if(sdData.numChannels==2){
			fft.ComputeRealFFT(&pRealRight[0], fftLen, false);
		}
		  if(useBkgThreads){
			pLock->Acquire();
			vFFT[i] = &pRealLeft[0];
			if(numChannels==2)	vFFT[i+1] = pRealRight;
			pLock->Release();

		  }else{
			vFFT[i] = &pRealLeft[0];
			if(numChannels==2)	vFFT[i+1] = pRealRight;
		  }

		  offset += (overlapLen*numChannels);  //?????možda maknuti (CopyAndPad maknut izvan petlje)
		  totalFrames++;
		}
	}
		numChannels == 2 ? 	CombineOverlappedTwoChannel(startWnd, endWnd, threadId)
						 :	CombineOverlappedOneChannel(startWnd, endWnd, threadId);
   //	}
}
void SpectrogramDisplayData::SortFftFrames(){

	  int i,j,k = 0;

	  const unsigned numWnd = numChannels*numFrames; ///vWndOverlappedFFT.size()

	  for(i=1;i<=numThreads;i++){

		   for(j=0, k=0;k<numWnd;j++,k+=numChannels){
			const unsigned tId = vThreadsEx[j];
			if(tId == i){
				vWndOverlappedFFT.push_back(vWndCombinedFFT[k]);
				if(numChannels==2)vWndOverlappedFFT.push_back(vWndCombinedFFT[k+1]);
				}
			}
	  }
}

/**
this method calculates the magnitudes for a FFT frame
totalLeft and totalRight are used for  spectral centroid computaions
*/
void SpectrogramDisplayData::CalculateMagnitude(float* pLr, float *pLi, float* pRr, float *pRi, float &totalLeft, float &totalRight){

	const float fftNorm = wndSum0*wndSum0 != 0 ? (wndSum0*wndSum0) : 1 ;
	const unsigned fftLenHalf = sdData.frameLen >> 1;
	const float zeroCorr = 0.000001f;

	totalLeft = totalRight = 0;
	float val, fFftS;

	for(int i=0; i<fftLenHalf; i++){

		fFftS =  (pLr[i]*pLr[i] + pLi[i]*pLi[i]);
		if(fFftS == 0) fFftS = zeroCorr;
		val = 10*log10(sqrt( fFftS/fftNorm));
		pLr[i] = val;//10 * log10(val);
		if(val >= threshHold)
			totalLeft += val;

		if(sdData.numChannels == 2){
			fFftS =  pRr[i] * pRr[i] + pRi[i] * pRi[i];
			if(fFftS==0) fFftS = zeroCorr;
			val = 10*log10(sqrt(fFftS/fftNorm));
			pRr[i] = val;
			if(val >= threshHold)
				totalRight += val;
		}
	}
}
void SpectrogramDisplayData::CalculateMagnitudeAndPhase(float* pLr, float *pLi, float* pRr, float *pRi, float &totalLeft, float &totalRight){

	const float fftNorm = 1;//wndSum0*wndSum0 != 0 ? wndSum0*wndSum0 : 1 ;
	const unsigned fftLenHalf = sdData.frameLen >> 1;
	const float zeroCorr = 0.000001f;

	totalLeft = totalRight = 0;
	float valMag, valPhase, fFftS;
 //	const float f = 1;
	for(int i=0; i<fftLenHalf; i++){

		fFftS =  pLr[i]*pLr[i] + pLi[i]*pLi[i];
		if(fFftS==0) fFftS = zeroCorr;

		valMag = 10*log10(sqrt( fFftS/fftNorm ));
		valPhase = atan(pLi[i]/pLr[i]);
		pLr[i] = valMag;
		pLi[i] = valPhase;

		if(valMag >= threshHold)
			totalLeft += valMag;

		if(sdData.numChannels == 2){
			fFftS =  pRr[i] * pRr[i] + pRi[i] * pRi[i];
			if(fFftS==0) fFftS = zeroCorr;

			valMag = 10*log10(sqrt(fFftS/fftNorm));
			valPhase = atan(pRi[i]/pRr[i]);
			pRr[i] = valMag;//10 * log10(val);
			pRi[i] = valPhase;

			if(valMag >= threshHold)
				totalRight += valMag;
		}
	}
}
/**
 since we're using an in-place FFT algorithm, we should copy the original sound data and pad with zeros to an appropriate length
*/
void SpectrogramDisplayData::CopyAndPadData(PFFT_RESULT pFFTRes, unsigned offset){

	const unsigned origDataLen = numChannels*(sdData.frameLen - numSamplesPadding);
	const unsigned len = numChannels*sdData.frameLen;
	float *pData = sdData.pData;
	if(sdData.numChannels==1){
	for(unsigned i=0; i<origDataLen; i++){
		pFFTRes->pRealLeft[i] = pData[i+offset];
	}
	for(int i=origDataLen/*numSamplesPadding*/; i<len; i++){
		pFFTRes->pRealLeft[i] = 0;
	}
	}else{

	for(int i=0,j=0; j<origDataLen; i++,j+=2){
		pFFTRes->pRealLeft[i]  = pData[j+offset];
		pFFTRes->pRealRight[i] = pData[j+offset+1];
		pFFTRes->pImgLeft[i]   = 0;
		pFFTRes->pImgRight[i]  = 0;
	   }
	for(int i=0,j=origDataLen; j<len; i++,j+=2){
		pFFTRes->pRealLeft[i]  = 0;
		pFFTRes->pRealRight[i] = 0;
		pFFTRes->pImgLeft[i]   = 0;
		pFFTRes->pImgRight[i]  = 0;
	   }
	}
}
//this method is not used currently and may be depracated in a future release
const unsigned SpectrogramDisplayData::GetBinStep(float fDelta){
	
	unsigned binStep = fDelta;
	float decPart = fDelta - binStep;
	return decPart>0.5f ? ++binStep : binStep;
}
void SpectrogramDisplayData::CombineOverlappedTwoChannel(unsigned startWnd,unsigned endWnd, unsigned threadId){

	float *pFFT0, *pFFT1, *pFFT2, *pFFT3, *pFFT4, *pFFT5, *pFFT6, *pFFT7, *pFFT8, *pFFT9, *pFFT10, *pFFT00, *pFFT12;
	float *pFFT11, *pFFT13, *pFFT14, *pFFT22, *pFFT33, *pFFT44, *pFFT55, *pFFT66, *pFFT77, *pFFT88, *pFFT99, *pFFT100, *pFFT111,
	*pFFT122, *pFFT133, *pFFT144;

	int max,k, i, j, indexAdjust0, indexAdjust1;
	float totalLeft, totalRight;
	const unsigned overSampling = overlapOversampling[sdData.frameOverlap];
	const unsigned numFramesToOverlap = (overSampling-1)*2+1;//overlapOversampling[sdData.frameOverlap];
	const unsigned numOverlap = numFramesToOverlap/2;
	const unsigned frameLen = sdData.frameLen;

	switch (sdData.frameOverlap) {
			case SPECTROGRAM_PARAMS::FO2:
				indexAdjust0 = 2;
				indexAdjust1 = 3;
				break;
			case SPECTROGRAM_PARAMS::FO4:
				indexAdjust0 = 6;
				indexAdjust1 = 7;
				break;
			case SPECTROGRAM_PARAMS::FO8:
				indexAdjust0 = 14;
				indexAdjust1 = 15;
				break;
	}
	const unsigned startWndOverlap = startWnd+indexAdjust0;
//	const unsigned numOverlapRem = (endWnd-startWndOverlap+1)%overSampling;
	const unsigned endWndOverlap =  endWnd-indexAdjust1;
	const unsigned step = overSampling*2;
	float *pFFTLReal, *pFFTLImg, *pFFTRReal, *pFFTRImg;

	//let's first handle the first and last frames that aren't fully overlapped
	
		pFFTLReal = new float[frameLenHalf+1];
		pFFTLImg =  new float[frameLenHalf+1];
		pFFTRReal =  new float[frameLenHalf+1];
		pFFTRImg  =  new float[frameLenHalf+1];
				
				switch (sdData.frameOverlap) {
						case SPECTROGRAM_PARAMS::FO2:
						//handle first several frames
							pFFT0 = vFFT[0];							
							pFFT00 = vFFT[1];
						
							break;
							
						case SPECTROGRAM_PARAMS::FO4:
							pFFT0 = vFFT[0];
							pFFT1 = vFFT[2];
							pFFT2 = vFFT[4];
							
							pFFT00 = vFFT[1];
							pFFT11 = vFFT[3];
							pFFT22 = vFFT[5];
							break;
						case SPECTROGRAM_PARAMS::FO8:
							pFFT0 = vFFT[0];
							pFFT1 = vFFT[2];
							pFFT2 = vFFT[4];
							pFFT3 = vFFT[6];
							pFFT4 = vFFT[8];
							pFFT5 = vFFT[10];
							pFFT6 = vFFT[12];
							
							pFFT00 = vFFT[1];
							pFFT11 = vFFT[3];
							pFFT22 = vFFT[5];
							pFFT33 = vFFT[7];
							pFFT44 = vFFT[9];
							pFFT55 = vFFT[11];
							pFFT66 = vFFT[13];
							break;
				}
				for(j=0,i=0; j<=frameLen;j+=2,i++){
					switch (sdData.frameOverlap){
						case SPECTROGRAM_PARAMS::FO2:
							pFFTLReal[i] = pFFT0[j];
							pFFTLImg[i]  = pFFT0[j+1];
							
							pFFTRReal[i] = pFFT00[j];
							pFFTRImg[i]  = pFFT00[j+1];
							break;
						case SPECTROGRAM_PARAMS::FO4:
							pFFTLReal[i] = ( pFFT0[j] + pFFT1[j] + pFFT2[j] )/numOverlap;
							pFFTLImg[i]  = ( pFFT0[j+1] + pFFT1[j+1] + pFFT2[j+1] )/numOverlap;
							
							pFFTRReal[i] = ( pFFT00[j] + pFFT11[j] + pFFT22[j] )/numOverlap;
							pFFTRImg[i]  = ( pFFT00[j+1] + pFFT11[j+1] + pFFT22[j+1] )/numOverlap;
							break;
						case SPECTROGRAM_PARAMS::FO8:
							pFFTLReal[i] = ( pFFT0[j] + pFFT1[j] + pFFT2[j]+ pFFT3[j] + pFFT4[j] + pFFT5[j]+pFFT6[j] )/numOverlap;
							pFFTLImg[i]  = ( pFFT0[j+1] + pFFT1[j+1] + pFFT2[j+1]+ pFFT3[j+1] + pFFT4[j+1] + pFFT5[j+1]+pFFT6[j+1] )/numOverlap;
							
							pFFTRReal[i] = ( pFFT00[j] + pFFT11[j] + pFFT22[j]+ pFFT33[j] + pFFT44[j] + pFFT55[j]+pFFT66[j] )/numOverlap;
							pFFTRImg[i]  = ( pFFT00[j+1] + pFFT11[j+1] + pFFT22[j+1]+ pFFT3[j+1] + pFFT44[j+1] + pFFT55[j+1]+pFFT66[j+1] )/numOverlap;
					}
				}

	CalculateMagnitude(pFFTLReal, pFFTLImg, pFFTRReal, pFFTRImg, totalLeft, totalRight);
	
	if(useBkgThreads){
			pLock->Acquire();
			vWndCombinedFFT.push_back(pFFTLReal);
			vWndCombinedFFT.push_back(pFFTRReal);
			vWndMagnitude.push_back(totalLeft);
			vWndMagnitude.push_back(totalRight);
			vThreadsEx.push_back(threadId);
			pLock->Release();
		}else{
			vWndOverlappedFFT.push_back(pFFTLReal);
			vWndOverlappedFFT.push_back(pFFTRReal);
			vWndTotalMagnitude.push_back(totalLeft);
			vWndTotalMagnitude.push_back(totalRight);
		}
		delete [] pFFTLImg;  delete [] pFFTRImg;

	for(i=startWndOverlap; i<endWndOverlap; i+=step){ // for(i=startWnd; i<endWnd; i++)

		pFFTLReal =  new float[frameLenHalf+1];
		pFFTLImg  =  new float[frameLenHalf+1];
		pFFTRReal =  new float[frameLenHalf+1];
		pFFTRImg  =  new float[frameLenHalf+1];
		
		switch (sdData.frameOverlap) {
					case SPECTROGRAM_PARAMS::FO2:
						pFFT0 = vFFT[i];
						pFFT1 = vFFT[i-2];
						pFFT2 = vFFT[i+2];
						
						pFFT00 = vFFT[i+1];
						pFFT11 = vFFT[i-1];
						pFFT22 = vFFT[i+3];
						break;
					case SPECTROGRAM_PARAMS::FO4:
						pFFT0 = vFFT[i];
						pFFT1 = vFFT[i-2];
						pFFT2 = vFFT[i+2];
						pFFT3 = vFFT[i-4];
						pFFT4 = vFFT[i+4];
						pFFT5 = vFFT[i-6];
						pFFT6 = vFFT[i+6];
						
						pFFT00 = vFFT[i+1];
						pFFT11 = vFFT[i-1];
						pFFT22 = vFFT[i+3];
						pFFT33 = vFFT[i-3];
						pFFT44 = vFFT[i+5];
						pFFT55 = vFFT[i-5];
						pFFT66 = vFFT[i+7];
						break;
					case SPECTROGRAM_PARAMS::FO8:
						pFFT0  = vFFT[i];
						pFFT1  = vFFT[i-2];
						pFFT2  = vFFT[i+2];
						pFFT3  = vFFT[i-4];
						pFFT4  = vFFT[i+4];
						pFFT5  = vFFT[i-6];
						pFFT6  = vFFT[i+6];
						pFFT7  = vFFT[i-8];
						pFFT8  = vFFT[i+8];
						pFFT9  = vFFT[i-10];
						pFFT10 = vFFT[i+10];
						pFFT11 = vFFT[i-12];
						pFFT12 = vFFT[i+12];
						pFFT13 = vFFT[i-14];
						pFFT14 = vFFT[i+14];
						
						pFFT00  = vFFT[i+1];
						pFFT11  = vFFT[i-1];
						pFFT22  = vFFT[i+3];
						pFFT33  = vFFT[i-3];
						pFFT44  = vFFT[i+5];
						pFFT55  = vFFT[i-5];
						pFFT66  = vFFT[i+7];
						pFFT77  = vFFT[i-7];
						pFFT88  = vFFT[i+9];
						pFFT99  = vFFT[i-9];
						pFFT100 = vFFT[i+11];
						pFFT111 = vFFT[i-11];
						pFFT122 = vFFT[i+13];
						pFFT133 = vFFT[i-13];
						pFFT144 = vFFT[i+15];
						break;
			}
				//fftSize = overlapLen*2
			for(j=0,k=0; j<=frameLen;j+=2,k++){
				
				switch (sdData.frameOverlap) {
					case SPECTROGRAM_PARAMS::FO2:
						pFFTLReal[k] = (pFFT0[j]+pFFT1[j]+pFFT2[j])/numFramesToOverlap;
						pFFTLImg[k]  = (pFFT0[j+1]+pFFT1[j+1]+pFFT2[j+1])/numFramesToOverlap;
						
						pFFTRReal[k] = (pFFT00[j]+pFFT11[j]+pFFT22[j])/numFramesToOverlap;
						pFFTRImg[k]  = (pFFT00[j+1]+pFFT11[j+1]+pFFT22[j+1])/numFramesToOverlap;
						break;
					case SPECTROGRAM_PARAMS::FO4:
						pFFTLReal[k] = (pFFT0[j]+pFFT1[j]+pFFT2[j]+pFFT3[j]+pFFT4[j]+pFFT5[j]+pFFT6[j])/numFramesToOverlap;
						pFFTLImg[k]  = (pFFT0[j+1]+pFFT1[j+1]+pFFT2[j+1]+pFFT3[j+1]+pFFT4[j+1]+pFFT5[j+1]+pFFT6[j+1])/numFramesToOverlap;  	

						pFFTRReal[k] = (pFFT00[j]+pFFT11[j]+pFFT22[j]+pFFT33[j]+pFFT44[j]+pFFT55[j]+pFFT66[j])/numFramesToOverlap;
						pFFTRImg[k]  = (pFFT00[j+1]+pFFT11[j+1]+pFFT22[j+1]+pFFT33[j+1]+pFFT44[j+1]+pFFT55[j+1]+pFFT66[j+1])/numFramesToOverlap;  	

						break;
					case SPECTROGRAM_PARAMS::FO8:
					
						pFFTLReal[k] = (pFFT0[j]+pFFT1[j]+pFFT2[j]+pFFT3[j]+pFFT4[j]+pFFT5[j]+pFFT6[j]+pFFT7[j]+pFFT8[j]+
										pFFT9[j]+pFFT10[j]+pFFT11[j]+pFFT12[j]+pFFT13[j]+pFFT14[j])/numFramesToOverlap;
						
						pFFTLImg[k] = (pFFT0[j+1]+pFFT1[j+1]+pFFT2[j+1]+pFFT3[j+1]+pFFT4[j+1]+pFFT5[j+1]+pFFT6[j+1]+pFFT7[j+1]+pFFT8[j+1]+
										pFFT9[j+1]+pFFT10[j+1]+pFFT11[j+1]+pFFT12[j+1]+pFFT13[j+1]+pFFT14[j+1])/numFramesToOverlap;

						pFFTRReal[k] = (pFFT00[j]+pFFT11[j]+pFFT22[j]+pFFT33[j]+pFFT44[j]+pFFT55[j]+pFFT66[j]+pFFT77[j]+pFFT88[j]+
										pFFT99[j]+pFFT100[j]+pFFT111[j]+pFFT122[j]+pFFT133[j]+pFFT144[j])/numFramesToOverlap;
						
						pFFTRImg[k] = (pFFT00[j+1]+pFFT11[j+1]+pFFT22[j+1]+pFFT33[j+1]+pFFT44[j+1]+pFFT55[j+1]+pFFT66[j+1]+pFFT77[j+1]+pFFT88[j+1]+
										pFFT99[j+1]+pFFT100[j+1]+pFFT111[j+1]+pFFT122[j+1]+pFFT133[j+1]+pFFT144[j+1])/numFramesToOverlap;
						break;
				}
		
			}
			CalculateMagnitude(pFFTLReal, pFFTLImg, pFFTRReal, pFFTRImg, totalLeft, totalRight);
			if(useBkgThreads){
				pLock->Acquire();
				vWndCombinedFFT.push_back(pFFTLReal);
				vWndCombinedFFT.push_back(pFFTRReal);
				vWndMagnitude.push_back(totalLeft);
				vWndMagnitude.push_back(totalRight);
				vThreadsEx.push_back(threadId);
				pLock->Release();
		}else{
				vWndOverlappedFFT.push_back(pFFTLReal);
				vWndOverlappedFFT.push_back(pFFTRReal);
				vWndTotalMagnitude.push_back(totalLeft);
				vWndTotalMagnitude.push_back(totalRight);
		}
		delete [] pFFTLImg;  delete [] pFFTRImg;

		}
			switch (sdData.frameOverlap) {
					case SPECTROGRAM_PARAMS::FO2:
						pFFT0 = vFFT[endWnd-2];
						pFFT1 = vFFT[endWnd-4];
						
						pFFT00 = vFFT[endWnd-1];
						pFFT11 = vFFT[endWnd-3];
						break;
					case SPECTROGRAM_PARAMS::FO4:
						pFFT0 = vFFT[endWnd-2];
						pFFT1 = vFFT[endWnd-4];
						pFFT2 = vFFT[endWnd-6];
						pFFT3 = vFFT[endWnd-8];
						
						pFFT00 = vFFT[endWnd-1];
						pFFT11 = vFFT[endWnd-3];
						pFFT22 = vFFT[endWnd-5];
						pFFT33 = vFFT[endWnd-7];
						break;
					case SPECTROGRAM_PARAMS::FO8:
						pFFT0  = vFFT[endWnd-2];
						pFFT1  = vFFT[endWnd-4];
						pFFT2  = vFFT[endWnd-6];
						pFFT3  = vFFT[endWnd-8];
						pFFT4  = vFFT[endWnd-10];
						pFFT5  = vFFT[endWnd-12];
						pFFT6  = vFFT[endWnd-14];
						pFFT7  = vFFT[endWnd-16];
						
						pFFT00  = vFFT[endWnd-1];
						pFFT11  = vFFT[endWnd-3];
						pFFT22  = vFFT[endWnd-5];
						pFFT33  = vFFT[endWnd-7];
						pFFT44  = vFFT[endWnd-9];
						pFFT55  = vFFT[endWnd-11];
						pFFT66  = vFFT[endWnd-13];
						pFFT77  = vFFT[endWnd-15];
						break;
			}
				pFFTLReal =  new float[frameLenHalf+1];
				pFFTLImg  =  new float[frameLenHalf+1];
				pFFTRReal =  new float[frameLenHalf+1];
				pFFTRImg  =  new float[frameLenHalf+1];
				
				for(j=0,k=0; j<=frameLen+2;j+=2,k++){
					switch (numFramesToOverlap) {
						case SPECTROGRAM_PARAMS::FO2:
							pFFTLReal[k] = ( pFFT0[j] + pFFT1[j] )/overSampling;//????
							pFFTLImg[k]  = ( pFFT0[j+1] + pFFT1[j+1] )/overSampling;
							
							pFFTRReal[k] = ( pFFT00[j] + pFFT11[j] )/overSampling;//????
							pFFTRImg[k]  = ( pFFT00[j+1] + pFFT11[j+1] )/overSampling;
							break;
						case SPECTROGRAM_PARAMS::FO4:
							pFFTLReal[k] = ( pFFT0[j] + pFFT1[j] + pFFT2[j]+ pFFT3[j] )/overSampling;
							pFFTLImg[k]  = ( pFFT0[j+1] + pFFT1[j+1] + pFFT2[j+1] + pFFT3[j+1])/overSampling;
							
							pFFTRReal[k] = ( pFFT00[j] + pFFT11[j] + pFFT22[j]+  pFFT33[j] )/overSampling;
							pFFTRImg[k]  = ( pFFT00[j+1] + pFFT11[j+1] + pFFT22[j+1] + pFFT33[j+1])/overSampling;
							break;
						case SPECTROGRAM_PARAMS::FO8:
							pFFTLReal[k] = ( pFFT0[j] + pFFT1[j] + pFFT2[j]+ pFFT3[j] + pFFT4[j] + pFFT5[j]+pFFT6[j]+pFFT7[j] )/overSampling;
							pFFTLImg[k]  = ( pFFT0[j+1] + pFFT1[j+1] + pFFT2[j+1]+ pFFT3[j+1] + pFFT4[j+1] + pFFT5[j+1]+pFFT6[j+1] + pFFT7[j+1] )/overSampling;

							pFFTRReal[k] = ( pFFT00[j] + pFFT11[j] + pFFT22[j]+ pFFT33[j] + pFFT44[j] + pFFT55[j]+pFFT66[j]+pFFT77[j] )/overSampling;
							pFFTRImg[k]  = ( pFFT00[j+1] + pFFT11[j+1] + pFFT22[j+1]+ pFFT33[j+1] + pFFT44[j+1] + pFFT55[j+1]+pFFT66[j+1] + pFFT77[j+1] )/overSampling;
					}
				}
			CalculateMagnitude(pFFTLReal, pFFTLImg, pFFTRReal, pFFTRImg, totalLeft, totalRight);

			if(useBkgThreads){
				pLock->Acquire();
				vWndCombinedFFT.push_back(pFFTLReal);
				vWndCombinedFFT.push_back(pFFTRReal);
				vWndMagnitude.push_back(totalLeft);
				vWndMagnitude.push_back(totalRight);
				vThreadsEx.push_back(threadId);
				pLock->Release();
		}else{
			vWndOverlappedFFT.push_back(pFFTLReal);
			vWndOverlappedFFT.push_back(pFFTRReal);
			vWndTotalMagnitude.push_back(totalLeft);
			vWndTotalMagnitude.push_back(totalRight);
		}
		delete [] pFFTLImg;  delete [] pFFTRImg;

}
/**
 The magnitudes that we display to to the end user are averaged over several analysis frames (3) and this method
 does exactly that
*/
void SpectrogramDisplayData::CombineOverlappedOneChannel(unsigned startWnd,unsigned endWnd, unsigned threadId){
 // const unsigned cFFT = 100;
	//these represent consecutive, overlapping FFT frames, which are averaged
	float *pFFT0, *pFFT1, *pFFT2, *pFFT3, *pFFT4, *pFFT5, *pFFT6, *pFFT7, *pFFT8, *pFFT9, *pFFT10, *pFFT11, *pFFT12, *pFFT13, *pFFT14 ;

	float totalLeft, totalRight;

	int max,k, i, j;
	const unsigned overSampling = overlapOversampling[sdData.frameOverlap];
  //	const unsigned numFramesToOverlap = (overSampling-1)*2+1;//overlapOversampling[sdData.frameOverlap];
	const unsigned numFramesToOverlap = overSampling+1;
	const unsigned numOverlap = numFramesToOverlap/2;
	
	const unsigned startWndOverlap = startWnd+numOverlap;
//	const unsigned numOverlapRem = (endWnd-startWndOverlap+1)%overSampling;
	const unsigned endWndOverlap =  endWnd-numOverlap;
	const unsigned frameLen = sdData.frameLen;
	
	float * pFFTLReal, * pFFTLImg;
	
	//let's first handle the first and last frames that aren't fully overlapped

		pFFTLReal = new float[frameLenHalf+1];
		pFFTLImg =  new float[frameLenHalf+1];
				
				switch ( sdData.frameOverlap ){
						case SPECTROGRAM_PARAMS::FO2:
						//handle first several frames
							pFFT0 = vFFT[startWnd];
							break;
						case SPECTROGRAM_PARAMS::FO4:
							pFFT0 = vFFT[startWnd];
							pFFT1 = vFFT[startWnd+1];
							pFFT2 = vFFT[startWnd+2];
							break;
						case SPECTROGRAM_PARAMS::FO8:
							pFFT0 = vFFT[startWnd+0];
							pFFT1 = vFFT[startWnd+1];
							pFFT2 = vFFT[startWnd+2];
							pFFT3 = vFFT[startWnd+3];
							pFFT4 = vFFT[startWnd+4];
							pFFT5 = vFFT[startWnd+5];
							pFFT6 = vFFT[startWnd+6];
							break;
				}
				for(j=0,i=0; j<frameLen;j+=2,i++){
					switch (sdData.frameOverlap){
						case SPECTROGRAM_PARAMS::FO2:
							pFFTLReal[i] = pFFT0[j];
							pFFTLImg[i]  = pFFT0[j+1];
							break;
						case SPECTROGRAM_PARAMS::FO4:
							pFFTLReal[i] = ( pFFT0[j] + pFFT1[j] + pFFT2[j] )/numOverlap;
							pFFTLImg[i]  = ( pFFT0[j+1] + pFFT1[j+1] + pFFT2[j+1] )/numOverlap;
							break;
						case SPECTROGRAM_PARAMS::FO8:
							pFFTLReal[i] = ( pFFT0[j] + pFFT1[j] + pFFT2[j]+ pFFT3[j] + pFFT4[j] + pFFT5[j]+pFFT6[j] )/numOverlap;
							pFFTLImg[i]  = ( pFFT0[j+1] + pFFT1[j+1] + pFFT2[j+1]+ pFFT3[j+1] + pFFT4[j+1] + pFFT5[j+1]+pFFT6[j+1] )/numOverlap;
					}
				}
				CalculateMagnitude(pFFTLReal, pFFTLImg, 0, 0, totalLeft, totalRight);

				if(useBkgThreads){
					pLock->Acquire();
					vWndCombinedFFT.push_back(pFFTLReal);
					vWndMagnitude.push_back(totalLeft);
					vThreadsEx.push_back(threadId);
					pLock->Release();
			}else{
				vWndOverlappedFFT.push_back(pFFTLReal);
				vWndTotalMagnitude.push_back(totalLeft);
			}

			delete [] pFFTLImg; //delete phase information
   //	if(threadId == 4) startWndOverlap++; //endWndOverlap -= 2;
	for(i=startWndOverlap; i<endWndOverlap; i+=overSampling){

		pFFTLReal = new float[frameLenHalf+1];
		pFFTLImg =  new float[frameLenHalf+1];

		switch (sdData.frameOverlap) {
					case SPECTROGRAM_PARAMS::FO2:
						pFFT0 = vFFT[i];
						pFFT1 = vFFT[i-1];
						pFFT2 = vFFT[i+1];
						break;
					case SPECTROGRAM_PARAMS::FO4:
						pFFT0 = vFFT[i];
						pFFT1 = vFFT[i-1];
						pFFT2 = vFFT[i+1];
						pFFT3 = vFFT[i-2];
						pFFT4 = vFFT[i+2];
						break;
					case SPECTROGRAM_PARAMS::FO8:
						pFFT0  = vFFT[i];
						pFFT1  = vFFT[i-1];
						pFFT2  = vFFT[i+1];
						pFFT3  = vFFT[i-2];
						pFFT4  = vFFT[i+2];
						pFFT5  = vFFT[i-3];
						pFFT6  = vFFT[i+3];
						pFFT7  = vFFT[i-4];
						pFFT8  = vFFT[i+4];
						break;
			}
				//fftSize = overlapLen*2
			for(j=0,k=0; j<frameLen;j+=2,k++){
				switch (sdData.frameOverlap) {
					case SPECTROGRAM_PARAMS::FO2:
						pFFTLReal[k] = (pFFT0[j]+pFFT1[j]+pFFT2[j])/numFramesToOverlap;
						pFFTLImg[k]  = (pFFT0[j+1]+pFFT1[j+1]+pFFT2[j+1])/numFramesToOverlap;
						break;
					case SPECTROGRAM_PARAMS::FO4:
						pFFTLReal[k] = (pFFT0[j]+pFFT1[j]+pFFT2[j]+pFFT3[j]+pFFT4[j]/*+pFFT5[j]+pFFT6[j]*/)/numFramesToOverlap;
						pFFTLImg[k]  = (pFFT0[j+1]+pFFT1[j+1]+pFFT2[j+1]+pFFT3[j+1]+pFFT4[j+1]/*+pFFT5[j+1]+pFFT6[j+1]*/)/numFramesToOverlap;
						//pFFTLReal[k] = vFFT[i][j]+vFFT[i-1][j]+vFFT[i-2][j]+vFFT[i-3][j]+vFFT[i+1][j]+vFFT[i+2][j]+vFFT[i+3][j])/numFramesToOverlap;
						//pFFTLImg[k]  = vFFT[i][j+1]+vFFT[i-1][j+1]+vFFT[i-2][j+1]+vFFT[i-3][j+1]+vFFT[i+1][j+1]+vFFT[i+2][j+1]+vFFT[i+3][j+1])/numFramesToOverlap;
						break;
					case SPECTROGRAM_PARAMS::FO8:
					
						pFFTLReal[k] = (pFFT0[j]+pFFT1[j]+pFFT2[j]+pFFT3[j]+pFFT4[j]+pFFT5[j]+pFFT6[j]+pFFT7[j]+pFFT8[j]
										/*+pFFT9[j]+pFFT10[j]+pFFT11[j]+pFFT12[j]+pFFT13[j]+pFFT14[j]*/)/numFramesToOverlap;
						
						pFFTLImg[k] = (pFFT0[j+1]+pFFT1[j+1]+pFFT2[j+1]+pFFT3[j+1]+pFFT4[j+1]+pFFT5[j+1]+pFFT6[j+1]+pFFT7[j+1]+pFFT8[j+1]
										/*+pFFT9[j+1]+pFFT10[j+1]+pFFT11[j+1]+pFFT12[j+1]+pFFT13[j+1]+pFFT14[j+1]*/)/numFramesToOverlap;
						break;
				} //end awitch
		
			} //end inner for
			CalculateMagnitude(pFFTLReal, pFFTLImg, 0, 0, totalLeft, totalRight);

			if(useBkgThreads){
				pLock->Acquire();
				vWndCombinedFFT.push_back(pFFTLReal);
				vWndMagnitude.push_back(totalLeft);
				vThreadsEx.push_back(threadId);
				pLock->Release();
			}else{
				vWndOverlappedFFT.push_back(pFFTLReal);
				vWndTotalMagnitude.push_back(totalLeft);
			}
			delete [] pFFTLImg; //delete phase information

		}  //end outer for loop
		 if (threadId == 4) {
			   i=i;
		 }
		//handle last several frames
	  //	endWnd--;
				switch (sdData.frameOverlap) {
					case SPECTROGRAM_PARAMS::FO2:
						pFFT0 = vFFT[endWnd-1];
						pFFT1 = vFFT[endWnd-2];
						break;
					case SPECTROGRAM_PARAMS::FO4:
						pFFT0 = vFFT[endWnd-1];
						pFFT1 = vFFT[endWnd-2];
						pFFT2 = vFFT[endWnd-3];
						pFFT3 = vFFT[endWnd-4];
						break;
					case SPECTROGRAM_PARAMS::FO8:
						pFFT0  = vFFT[endWnd-1];
						pFFT1  = vFFT[endWnd-2];
						pFFT2  = vFFT[endWnd-3];
						pFFT3  = vFFT[endWnd-4];
						pFFT4  = vFFT[endWnd-5];
						pFFT5  = vFFT[endWnd-6];
						pFFT6  = vFFT[endWnd-7];
						pFFT7  = vFFT[endWnd-8];
						break;
			}
				pFFTLReal = new float[frameLenHalf+1];
				pFFTLImg =  new float[frameLenHalf+1];

				for(j=0,i=0; j<frameLen;j+=2,i++){
					switch (sdData.frameOverlap) {
						case SPECTROGRAM_PARAMS::FO2:
							pFFTLReal[i] = ( pFFT0[j] + pFFT1[j] )/overSampling;//????
							pFFTLImg[i]  = ( pFFT0[j+1] + pFFT1[j+1] )/overSampling;
							break;
						case SPECTROGRAM_PARAMS::FO4:
							pFFTLReal[i] = ( pFFT0[j] + pFFT1[j] + pFFT2[j]+ + pFFT3[j] )/overSampling;
							pFFTLImg[i]  = ( pFFT0[j+1] + pFFT1[j+1] + pFFT2[j+1] + pFFT3[j+1])/overSampling;
							break;
						case SPECTROGRAM_PARAMS::FO8:
							pFFTLReal[i] = ( pFFT0[j] + pFFT1[j] + pFFT2[j]+ pFFT3[j] + pFFT4[j] + pFFT5[j]+pFFT6[j]+pFFT7[j] )/overSampling;
							pFFTLImg[i]  = ( pFFT0[j+1] + pFFT1[j+1] + pFFT2[j+1]+ pFFT3[j+1] + pFFT4[j+1] + pFFT5[j+1]+pFFT6[j+1] + pFFT7[j+1] )/overSampling;
					}
				}

			CalculateMagnitude(pFFTLReal, pFFTLImg, 0, 0, totalLeft, totalRight);

			if(useBkgThreads){
				pLock->Acquire();
				vWndCombinedFFT.push_back(pFFTLReal);
				vWndMagnitude.push_back(totalLeft);
				vThreadsEx.push_back(threadId);
				pLock->Release();
			}else{
				vWndOverlappedFFT.push_back(pFFTLReal);
				vWndTotalMagnitude.push_back(totalLeft);
			}
			delete [] pFFTLImg; //delete phase information
	   //	delete [] pFFTReal0; //delete raw FFT - probably makes sense to do this now that the magnitudes are computed
}

void SpectrogramDisplayData::DumpMagnitudesToFile(const char *pFileName, unsigned startFrame, unsigned endFrame,unsigned startBin, unsigned endBin){

float * pLMagnitude, * pRMagnitude;
const char newLine = '\n';
const char columnSeparator = FileLayout::eFv == FileLayout::FRAME_PER_COLUMN ? newLine : ' ';
 
char val[6];
bool leftChannelOut = FileLayout::eCh == FileLayout::BOTH || FileLayout::eCh == FileLayout::LEFT;
bool rightChannelOut = numChannels == 2 && (FileLayout::BOTH || FileLayout::eCh == FileLayout::RIGHT);

//make sure that we don't start at an odd frame unless we're printing only the right channel
if((numChannels == 2 && FileLayout::BOTH && startFrame%2!=0) || (rightChannelOut && startFrame%2==0))
	startFrame = startFrame+1;

ofstream outFile(pFileName);
if(FileLayout::stftInfo){
   outFile << "!!!!!!!!!! SOUND INFO !!!!!!!!!!"<< newLine;
   outFile << "Frame range: start frame: " << startFrame << " , end frame: " << endFrame << newLine;
   outFile << "Frequency resolution: " << freqResolution << " Hz" << newLine;
   outFile << "Sampling rate: " << sdData.samplesPerSec << " Hz" << newLine;
   outFile << "FFT length: " << sdData.frameLen << newLine;
	}
//DumpInfoToFile(ofstream);

if(leftChannelOut){
 if( FileLayout::eFv == FileLayout::FRAME_PER_COLUMN ){
	 for(int j=startBin; j<endBin; j++){
		for(int i=startFrame; i<endFrame; i+=numChannels){
		 pLMagnitude = vWndOverlappedFFT.at(i) ;
		 sprintf(val, "%f", pLMagnitude[j]);
		 outFile.write((const char*)val, sizeof(val));
		 outFile << ',' ;//<< columnSeparator;
		 if(rightChannelOut && FileLayout::eCs == FileLayout::INTERLEAVED){
			sprintf(val, "%f", pRMagnitude[j]);
			outFile.write((const char*)val, sizeof(val));
			outFile << ',' ;//<< columnSeparator;
				}    //end  rightChannelOut
			}   //end inner for

		outFile << newLine;
		}  //end outer for
  }    //end FRAME_PER_COLUMN
  else{
	for(int i=startFrame; i<endFrame; i+=numChannels){

		pLMagnitude = vWndOverlappedFFT.at(i) ;
		if(rightChannelOut){
				pRMagnitude = vWndOverlappedFFT.at(i+1) ;
		}
		for(int j=startBin; j<endBin; j++){
			sprintf(val, "%f", pLMagnitude[j]);
			outFile.write((const char*)val, sizeof(val));
			outFile << ',' ;//<< columnSeparator;
		if(rightChannelOut && FileLayout::eCs == FileLayout::INTERLEAVED){
			sprintf(val, "%f", pRMagnitude[j]);
			outFile.write((const char*)val, sizeof(val));
			outFile << ',' << columnSeparator;
		}
	}
		outFile << newLine;
		}
	}
}//end outer if
if(rightChannelOut && FileLayout::eCs == FileLayout::SEPARATED || (!leftChannelOut && FileLayout::eCs == FileLayout::INTERLEAVED)){
 
for(int i=startFrame+1; i<endFrame; i+=numChannels){
 
pRMagnitude = vWndOverlappedFFT.at(i) ;
 
for(int j=startBin; j<endBin; j++){
  sprintf(val, "%f", pRMagnitude[j]);
  outFile.write((const char*)val, sizeof(val));
  outFile << ',' << columnSeparator;
}
if(FileLayout::eFv==FileLayout::FRAME_PER_COLUMN) outFile << newLine;
	}
}

outFile.close();
}

//dump rolloff or flux to file
void SpectrogramDisplayData::DumpToFile(const char *pFileName, unsigned startFrame, unsigned endFrame){
 
const char newLine = '\n';
float *pLMagnitude, *pRMagnitude;

//int k = 0;
float lValue,rValue;
char val[6];

const vector<float> &v = FileLayout::eSrc==FileLayout::ROLLOFF ? vSRolloff : FileLayout::eSrc==FileLayout::FLUX ? vSFlux : vSCentroid;

bool leftChannelOut = FileLayout::eCh == FileLayout::BOTH || FileLayout::eCh == FileLayout::LEFT;

bool rightChannelOut = numChannels == 2 && (FileLayout::BOTH || FileLayout::eCh == FileLayout::RIGHT);

//make sure that we don't start at an odd frame unless we're printing only the right channel
if((numChannels == 2 && FileLayout::BOTH && startFrame%2!=0) || (rightChannelOut && startFrame%2==0))
	startFrame = startFrame+1;

if(v.size()<(endFrame-startFrame+1)) endFrame = startFrame+v.size();

const unsigned numCentr = v.size();
ofstream outFile(pFileName);
//DumpInfoToFile(ofstream);
if(FileLayout::stftInfo){

   outFile << "!!!! SOUND INFO !!!!"<< newLine;
   outFile << "Frame range: start frame: " << startFrame << " , end frame: " << endFrame << newLine;
   outFile << "Frequency resolution: " << freqResolution << " Hz" << newLine;
   outFile << "Sampling rate: " << sdData.samplesPerSec << " Hz" << newLine;
   outFile << "FFT length: " << sdData.frameLen << newLine;
}
if(leftChannelOut){
for(int i=startFrame,j=0; i<endFrame; i++,j+=numChannels){
	if(j>numCentr-1) break;
	lValue = v.at(j);
 
	sprintf(val, "%f", lValue);
	if(FileLayout::enumFrames) outFile << i << '	';
	outFile.write((const char*)val, sizeof(val));
	outFile << ',' ;
if(rightChannelOut && FileLayout::eCs == FileLayout::INTERLEAVED){
	rValue = v.at(j+1);
	if(FileLayout::enumFrames) outFile << i << '	';
	sprintf(val, "%f", rValue);
	outFile.write((const char*)val, sizeof(val));
	outFile << ',' ;
  } 
	if(FileLayout::eFv==FileLayout::FRAME_PER_COLUMN)  outFile << newLine;
}
}//end outer if
if(rightChannelOut && FileLayout::eCs == FileLayout::SEPARATED || (!leftChannelOut && FileLayout::eCs == FileLayout::INTERLEAVED)){
 
for(int i=startFrame+1; i<endFrame; i+=numChannels){
  rValue = v.at(i) ;
  sprintf(val, "%f", rValue);
  outFile.write((const char*)val, sizeof(val));
  outFile << ',' ;
  if(FileLayout::eFv==FileLayout::FRAME_PER_COLUMN) outFile << newLine;
				}
	}
	outFile.close();
}
void SpectrogramDisplayData::DumpToFile(const char *pFileName, unsigned startFrame, unsigned endFrame,unsigned startBin, unsigned endBin, int &status){

	status = -1;
	if(FileLayout::eSrc == FileLayout::MAGNITUDES){
		DumpMagnitudesToFile(pFileName, startFrame, endFrame,startBin, endBin);
	}else{
		DumpToFile(pFileName, startFrame, endFrame);
	}
	status = 0;
}

void SpectrogramDisplayData::CalculateSpectralFlux(unsigned frame0, unsigned frame1){

	 float *pLMagnitude0, *pLMagnitude1, *pRMagnitude0, *pRMagnitude1;
	 float accum0,accum1 ;
	 const unsigned numFrames = (frame1 - frame0 + 1)*numChannels;

	 const unsigned len = numChannels==2 ? numFrames : numFrames-1;

	 vSFlux.clear();

	 for(int j=0;j<len;j++){
		if(numChannels == 1){
			pLMagnitude0 = vWndOverlappedFFT.at(frame0+j);
			pLMagnitude1 = vWndOverlappedFFT.at(frame0+j+1);
		}else{
			pLMagnitude0 = vWndOverlappedFFT.at(frame0+j);
			pRMagnitude0 = vWndOverlappedFFT.at(frame0+j+1);

			pLMagnitude1 = vWndOverlappedFFT.at(frame0+j+2);
			pRMagnitude1 = vWndOverlappedFFT.at(frame0+j+3);
		}
	 accum0 = 0;
	 accum1 = 0;
	 for(int i=0;i<frameLenHalf;i++){
		if(pLMagnitude0[i] < threshHold && pLMagnitude1[i] < threshHold) continue;

		accum0 += sqrt(fabs(pLMagnitude0[i]-pLMagnitude1[i]));
		if(numChannels==2)
			accum1 += sqrt(fabs(pRMagnitude0[i]-pRMagnitude1[i]));
	 }

	 vSFlux.push_back(accum0);
	 if(numChannels==2)
		vSFlux.push_back(accum1);
	 }
}
void SpectrogramDisplayData::CalculateSpectralRolloff(unsigned frame0, unsigned frame1){

	const float rolloffThreshold = 0.85f;
	float *pLMagnitude0, *pRMagnitude0;
	float frameLMagnitude, frameRMagnitude ;  //total frame magnitudes
	float currLMagnitude, currRMagnitude;
	float rolloffLMagnitude, rolloffRMagnitude;
	float lDone, rDone;
	const unsigned numFrames = (frame1 - frame0 + 1)*numChannels;
	vSRolloff.clear();
	for(int j=0;j<numFrames;j+=numChannels){
		lDone = rDone = false;
		currLMagnitude = currRMagnitude = 0;
		pLMagnitude0 = vWndOverlappedFFT[frame0+j];
		frameLMagnitude = vWndTotalMagnitude[frame0+j];
		rolloffLMagnitude = frameLMagnitude * rolloffThreshold;
	if(numChannels == 2){

		pRMagnitude0 = vWndOverlappedFFT.at(frame0+j+1);
		frameRMagnitude = vWndTotalMagnitude.at(frame0+j+1);
		rolloffRMagnitude = frameRMagnitude * rolloffThreshold;
	 }
	 for(int i=1;i<frameLenHalf;i++){

		currLMagnitude += pLMagnitude0[i];
		if(!lDone && currLMagnitude < rolloffLMagnitude ){
			vSRolloff.push_back(i*freqResolution);
			lDone = true;
			}
	   //	}
		if(numChannels == 2){

		if(pRMagnitude0[i] >= threshHold){

		currRMagnitude += pRMagnitude0[i];
		if(!rDone && currRMagnitude < rolloffRMagnitude ){
			vSRolloff.push_back(i*freqResolution);
			rDone = true;
					}
				}
		}
		if(lDone){
			if(numChannels == 2 && rDone){ break; }
			break;
			}
		}
	 }
}

void SpectrogramDisplayData::CalculateAverageMagnitudes(unsigned frame0, unsigned frame1){

	  const int nFrames = frame1-frame0+1;
	  const int fftLenHalf = sdData.frameLen>>1;

	  vAvgMag.clear();
	  vAvgMag.resize(fftLenHalf+1);

	  for(int i=0;i<=fftLenHalf;i++)
		vAvgMag[i] = 0;

	  for(int i=0;i<nFrames;i++){

		for(int j=0;j<=fftLenHalf;j++){
		   //	vFrameVals[j-1] = 0;
			vAvgMag[j] += vWndOverlappedFFT[frame0+i][j];
			}
		}
		//average every bin
		for(int j=1;j<fftLenHalf;j++)
			vAvgMag[j] /= nFrames;

}
void SpectrogramDisplayData::CalculateSpectralCentroid(unsigned frame0, unsigned frame1){

	 float *pLMagnitude0, *pRMagnitude0;
	 float accum0, accum00, accum1, accum11 ;
	 const unsigned numFrames = (frame1-frame0+1)*numChannels;

	 const float freqRes = freqResolution/maxFrequency; //in fraction of the max frequency e.g. for sample rate 4K 0.5 correspods to 2KHz
	 const float freqResHalf = freqRes/2 ;
	 vSCentroid.clear();
	 for(int j=0;j<numFrames;j+=numChannels){

		pLMagnitude0 = vWndOverlappedFFT[frame0+j];
		if(numChannels == 2){
			pRMagnitude0 = vWndOverlappedFFT[frame0+j+1];
		}
		accum0 = 0;
		accum00 = 0;
		accum1 = 0;
		accum11 = 0;

		for(int i=1;i<frameLenHalf;i++){

			if(pLMagnitude0[i] < threshHold)
				continue;

		float binCentralFreq = i * freqRes - freqResHalf;

			accum0  += binCentralFreq * pLMagnitude0[i];
			accum00 += pLMagnitude0[i];

			if(numChannels==2) {
				if(pRMagnitude0[i] < threshHold)
					continue;
				accum1 += binCentralFreq * pRMagnitude0[i];
				accum11 += pRMagnitude0[i];
			}
		}
	 accum0 = accum0 != 0 ? accum0 / accum00 : 0;
	 vSCentroid.push_back(accum0 * maxFrequency);

	 if(numChannels==2)  {
		accum1 = accum1 != 0 ? accum1 / accum11 : 0;
		vSCentroid.push_back(accum1 * maxFrequency);
		}
	 }
}

//gets a peak FFT value
void SpectrogramDisplayData::GetMaxVal(unsigned &index,unsigned &frameIndex){
	
	const unsigned size = vWndOverlappedFFT.size();
	maxMagnitude = vWndOverlappedFFT[0][0];
	minMagnitude = maxMagnitude;
	index = 0;
	frameIndex = 0;
	for(int i=0; i<size; i++){
		float *pFFT = vWndOverlappedFFT[i];
		for(int j=0; j<overlapLen; j++){
			float val = pFFT[j];
			if(maxMagnitude >= val && minMagnitude <= val) continue;
			if(maxMagnitude < val) {
				maxMagnitude = val;
				index = j;
				frameIndex = i;
			}else{
				minMagnitude = val;
			}
		} //end inner for
	}
	maxMagnitude += cMagPosDelta;
	minMagnitude += cMagPosDelta;
  //	if(minMagnitude<0)minMagnitude=0;
}
void SpectrogramDisplayData::SetNumWindows(){

   //	prevNumFrames = numFrames;
	numFrames = (sdData.numSamples/sdData.numChannels) / sdData.frameLen;      //per channel

	numSamplesRem = (sdData.numSamples/sdData.numChannels) % sdData.frameLen;

	if( numSamplesRem >= overlapLen ){ //we ignore numSamplesRem - overlapLen samples and don't have to zero pad the signal
		framePadding = false;
		numSamplesPadding = 0;
	}else{
		framePadding = true;
		numSamplesPadding = overlapLen - numSamplesRem;   //the number of zeros to pad the signal with
	}

	//numWindows =  numFrames*2; //numWindows is always an even number
	prevNumWindows = numWindows;
	numWindows =  numFrames * overlapOversampling[sdData.frameOverlap];
	//we're ignore the samples beyond the last overlapping window

  //	numWindows = numSamplesRem >= overlapLen ? numWindows * 2 : numWindows * 2 - 1;
  //we ignore some remaining samples though it would be possible to pad them to an appropriate FFT size (low priority)
}
void SpectrogramDisplayData::SetBinIndex(unsigned f0,unsigned f1, unsigned rep0, unsigned rep1, unsigned limiter){

	GenPtrAlloc<unsigned>::DeallocateArray(pBinIndex);
	pBinIndex = GenPtrAlloc<unsigned>(binsPerFrame).GetPtr();

	int i, j, k, l;
	for( i=0, j=0; j<frameLenHalf-limiter;  /*l+=numChannels*/){       //j+=freqStep,

	for(k=0; k<f0; k++) {
		for(l=0;l<rep0;l++)
		 pBinIndex[i++] = j;

		j++;
	}
	for(k=0; k<f1; k++)  {
		for(l=0;l<rep1;l++)
			pBinIndex[i++] = j;

		j++;
		}
	}
	//let's handle the remaining bins
	dispBinsRem = binsPerFrame - i;
	binsRem = frameLenHalf - j;
	if(dispBinsRem==0 || binsRem==0) return;

	binsDup = dispBinsRem / binsRem > 0 ? dispBinsRem / binsRem : 1 ;
	//if(binsDup==0) return;

	for(i=binsPerFrame-dispBinsRem; i<binsPerFrame; i+=binsDup){
		for(k=0;k<binsDup;k++)
			pBinIndex[i+k]  =  j;
		j++;
	}
}
unsigned SpectrogramDisplayData::GetBinIndex(unsigned index){
	return pBinIndex[index];
}
/**
this method is used when the number of fft bins is fewer than the viewport height
so we repeat some bins to fill the entire height
**/
void SpectrogramDisplayData::SetStftForFewerBins(){

	unsigned int skip1, skip2, skip, length;

	float skipRate, fRepeatFraction;

	int repeatFraction;

	fFreqStep  =  static_cast<float>(viewPortHeight) / frameLenHalf;

	skipRate =  fFreqStep;

	skip1 = skipRate;

	skip2 = skip1+1 ;

	unsigned f0, f1;

	DspUtil::SetRepeatValues(fFreqStep, f0, f1);
	float *pLMagnitude, *pRMagnitude;
	binsPerFrame = viewPortHeight;

	const unsigned totalBinsPerFrame = binsPerFrame * numChannels;

	const unsigned totalFrames = vScaledFrames.size();//numFrames * numChannels;

	bufferLen = binsPerFrame * totalFrames; //per channel ???

	GenPtrAlloc<unsigned> genAlloc(bufferLen, false);
	unsigned *pOut = genAlloc.GetPtr();

	int i, j, h, k, l, relMag, ii, jj, iii, jjj;

	const unsigned step 	= f0 * skip1 + f1 * skip2;
	const unsigned maxIndex = frameLenHalf-step;

	SetBinIndex(f0, f1, skip1, skip2, step);

	for(int i=0,m=0; i<totalFrames; i++,m+=numChannels){

		pLMagnitude = vScaledFrames.at(m);

		if(numChannels==2)
			pRMagnitude = vScaledFrames.at(m+1);

		k = i*binsPerFrame*numChannels;

		for( j=0, h=0, l=k; j<maxIndex;  ){       //j+=freqStep,

			for(ii=0; ii<f0; ii++) {

				for(jj=0;jj<skip1;jj++)
					pOut[l++] = pLMagnitude[j]+cMagPosDelta;
				j++;
			}
		for(ii=0; ii<f1; ii++)  {
			for(jj=0;jj<skip2;jj++)
				pOut[l++] = pLMagnitude[j]+cMagPosDelta;
			j++;
		}
		if(numChannels==2){
			for(ii=0; ii<f0; ii++)
				for(jj=0;jj<skip1;jj++)
					pOut[l++] = pRMagnitude[h]+cMagPosDelta;

			h++;
							   ///iii=1
			for(ii=0; ii<f1; ii++)
				for(jj=0;jj<skip2;jj++)
					pOut[l++] = pRMagnitude[h]+cMagPosDelta;

			h++;
		} //end if
		}
		for(int s=binsPerFrame-dispBinsRem; s<binsPerFrame; s+=binsDup){
			for(int t=0;t<binsDup;t++)
				pOut[k+s+t]  =   pLMagnitude[j]+cMagPosDelta;
			j++;
		}
	}

	GenPtrAlloc<unsigned>::DeallocateArray<unsigned>(pOutColumn, prevTotalBinsPerFrame/*prevViewPortHeight * numChannels*/);

	pOutColumn = GenPtrAlloc<unsigned>(totalBinsPerFrame, viewPortWidth).GetPtrDbl();

   int offset = 0;
   for(i=0; i<viewPortWidth; i++){
	offset = i * totalBinsPerFrame;

	for(j=0; j<totalBinsPerFrame; j+=numChannels){
		pOutColumn[j][i] = pOut[j+offset];
		if(numChannels==2)
			pOutColumn[j+1][i] = pOut[j+offset+1];

		}
   }
   prevTotalBinsPerFrame = totalBinsPerFrame;

}

void SpectrogramDisplayData::SetSpectrogramDisplayData(){


	if( vScaledFrames.size() != viewPortWidth * sdData.numChannels || prevNumWindows != numWindows ){

			FreeVectorResources(vScaledFrames);
			vScaledFrames = AdjustFftFrames(vWndOverlappedFFT);
	}

	if(frameLenHalf < viewPortHeight){
		SetStftForFewerBins();
		return;
	}

	fFreqStep  =  	static_cast<float>(frameLenHalf) / viewPortHeight > 0 ? //?????
					static_cast<float>(frameLenHalf) / viewPortHeight : 1.0f;

	unsigned freqStep, prevPos=0 ;

	binsPerFrame = viewPortHeight;

	const unsigned totalBinsPerFrame = binsPerFrame * numChannels;

	float *pLMagnitude, *pRMagnitude;

	const unsigned totalFrames = vScaledFrames.size();//numFrames*numChannels;
	bufferLen = binsPerFrame * totalFrames;

	GenPtrAlloc<unsigned> genAlloc(bufferLen, false);
	unsigned *pOut = genAlloc.GetPtr();

	int i, j, l, k, m, relMag;
	float localMax;

	//try{
	for(i=0, m=0; m < totalFrames;i++, m+=numChannels){
		pLMagnitude = vScaledFrames[m];
		if(numChannels==2)
			pRMagnitude = vScaledFrames[m+1];

		k = i*binsPerFrame*numChannels;
		prevPos=0;
		for(j=0,l=k; j<binsPerFrame; j++, l+=numChannels){       //j+=freqStep,

			freqStep = j*fFreqStep;

		 //find the local max
			localMax = pLMagnitude[prevPos] ;
		 //BOUNDS CHECKING
			 if( freqStep>frameLenHalf-1 )	break;

			 for(int ii=prevPos+1;ii<freqStep;ii++){
					if(pLMagnitude[ii] > localMax)
						localMax = pLMagnitude[ii];
			 }
			//BOUND CHECKING
			if(l>bufferLen-1)	break;

			pOut[l] = localMax + cMagPosDelta;
			if(numChannels==2){
		   //find the local max
				localMax = pRMagnitude[prevPos] ;
				for(int ii=prevPos+1;ii<freqStep;ii++){
					if(pRMagnitude[ii] > localMax)
						localMax = pRMagnitude[ii];
				}   //end for
				pOut[l+1] = localMax + cMagPosDelta;
			}
		   prevPos = freqStep;
	   }
   }

	GenPtrAlloc<unsigned>::DeallocateArray<unsigned>(pOutColumn, prevTotalBinsPerFrame/*prevViewPortHeight * numChannels*/);

	pOutColumn = GenPtrAlloc<unsigned>(totalBinsPerFrame, viewPortWidth).GetPtrDbl();

   int offset = 0;
   for(i=0; i<viewPortWidth; i++){
	offset = i * totalBinsPerFrame;

	for(j=0; j<totalBinsPerFrame; j+=numChannels){
		pOutColumn[j][i] = pOut[j+offset];
		if(numChannels==2)
			pOutColumn[j+1][i] = pOut[j+offset+1];

		}
   }
   prevTotalBinsPerFrame = totalBinsPerFrame;
}

/**
this method computes the selection (start and end frames/bins form x,y positions)
xStart - x position (cursor position) of the selection start
xEnd - x position of the selection end
yStart - y position of the selection start
yEnd - y position of the selection end
This method calculates the start/end frames as well as the start and end bins of the selection

**/
void SpectrogramDisplayData::SetSelection(unsigned xStart,unsigned xEnd, unsigned yStart, unsigned yEnd){

	fFreqStep  =  (float)frameLenHalf / viewPortHeight > 0 ? (float)frameLenHalf / viewPortHeight : 1;
	const int xREnd = frameWidth * numWindows/2;
	if(xEnd > xREnd)
		xEnd = xREnd-1;
	selStartFrame = xStart / frameWidth;  //non-overlapping , which means that we should multiply by 2 in order to get the overlapping frame
	selEndFrame = xEnd / frameWidth;

	if(frameLenHalf > viewPortHeight){
		selStartBin = frameLenHalf - (viewPortHeight - yStart>yEnd ? yStart:yEnd) * fFreqStep;
		selEndBin = frameLenHalf - (viewPortHeight - yStart>yEnd ? yEnd:yStart) * fFreqStep;
	}else{
		 selStartBin = 	 GetBinIndex(viewPortHeight - yStart>yEnd ? yStart:yEnd);
		 selEndBin = 	 GetBinIndex(viewPortHeight - yStart>yEnd ? yEnd:yStart);
	}
	unsigned tmp;
	if(selStartBin > selEndBin){
		tmp = selEndBin;
		selEndBin = selStartBin;
		selStartBin = tmp;
	}
}
void SpectrogramDisplayData::SetBinPositions(unsigned height, unsigned &binStart, unsigned &binEnd){

	if(frameLenHalf > height){
		binStart =   (height - binEnd) * fFreqStep;
		binEnd =  	(height - binStart) * fFreqStep;
	}else{
		binStart = 	 GetBinIndex(height - binEnd);
		binEnd = 	 GetBinIndex(height - binStart);
	}
}
 std::vector<float>  *SpectrogramDisplayData::GetSpectralFlux() {

	if(selEndFrame==0)	selEndFrame = numFrames-1;
	CalculateSpectralFlux(selStartFrame, selEndFrame);
	return &vSFlux;

}
std::vector<float>  *SpectrogramDisplayData::GetSpectralCentroid()  {
	if(selEndFrame==0)	selEndFrame = numFrames-1;
	CalculateSpectralCentroid(selStartFrame, selEndFrame);
	return &vSCentroid;
}
std::vector<float>  *SpectrogramDisplayData::GetSpectralRolloff()  {

	if(selEndFrame==0)	selEndFrame = numFrames-1;
	CalculateSpectralRolloff(selStartFrame, selEndFrame);
	return &vSRolloff;
}
std::vector<float>  *SpectrogramDisplayData::GetAverageMagnitudes()  {

	if(selEndFrame==0)	selEndFrame = numFrames-1;
	CalculateAverageMagnitudes(selStartFrame, selEndFrame);
	return &vAvgMag;
}
std::vector<float*> &SpectrogramDisplayData::ExpandFftFrames(std::vector<float*> &vMag,
unsigned totalPixelsRepeat0, unsigned totalPixelsRepeat1, unsigned singlePassRepeat0, unsigned singlePassRepeat1 ){

	float *pMag0, *pMag1; //left / right channel mags
	float magDelta0, magDelta1, currMag0, currMag1, currSinglePassRepeat, currPixelsRepeat;
	unsigned col,  currFrame;
	int row, i, k, p, m;

	const unsigned viewPortWidth = this->viewPortWidth;
	const unsigned fftLenHalf = sdData.frameLen / 2;
	const unsigned totalRows	= fftLenHalf * numChannels;

	GenPtrAlloc<float> genPtrAlloc(totalRows, viewPortWidth);
	genPtrAlloc.TransferOwnershipToCaller(false);

	float **pColumnGrid = genPtrAlloc.GetPtrDbl();

	currSinglePassRepeat = singlePassRepeat1;

	for( row = 0; row < totalRows; row += numChannels){
		currFrame = 0;
		col = 0;
	  //	currPixelsRepeat = totalPixelsRepeat1;
		pMag0  = vMag[row]; //fft rows - not bins to speed up processing
		if(numChannels==2) pMag1 = vMag[row+1];

		for( m = 0; m <viewPortWidth; m += currPixelsRepeat){ //outer loop

		 //	if(col >= this->viewPortWidth) break;  //bounds check
			if( currSinglePassRepeat == singlePassRepeat0 ){
				currPixelsRepeat = totalPixelsRepeat1;
				currSinglePassRepeat = singlePassRepeat1;
			}else{
				currPixelsRepeat = totalPixelsRepeat0;
				currSinglePassRepeat = singlePassRepeat0;
			}
			const unsigned inFac = currSinglePassRepeat;// / numChannels;

			for( p = 0; p < currPixelsRepeat; p += currSinglePassRepeat ) { //dio koji èita samo odreðen broj frame-ova

				if(currFrame >= numFrames) break;  //bounds check

				currMag0 = pMag0[currFrame];
				magDelta0 = (currMag0 - pMag0[currFrame+1])/(inFac);
				if(numChannels==2){
					currMag1 = pMag1[currFrame];
					magDelta1 = (currMag1 - pMag1[currFrame+1])/(inFac);
					
				}
				for( i=0,k=0; i < currSinglePassRepeat; i++, k++){
						if(col+i >= (viewPortWidth-currSinglePassRepeat)) break;

					float c = currMag0 - magDelta0 * i;	
					pColumnGrid[row][col+i] = c;//c != 0 ? c : currMag0;
					if(numChannels==2){
						c = currMag1 - magDelta1 * i;
						pColumnGrid[row+1][col+i] = c;//c != 0 ? c : currMag1;
					}

				}
				col += i;
				currFrame++;
			}
		}
	} //end for row

	const unsigned prevCol = col;

	if(col < viewPortWidth){

			for( row = 0; row < totalRows; row += numChannels){

				for( col = prevCol; col < viewPortWidth; col++){
					pColumnGrid[row][col] = pColumnGrid[row][prevCol-1];
					if(numChannels==2) pColumnGrid[row+1][col] = pColumnGrid[row+1][prevCol-1];
					}
			   }
		} //end if col < viewPortWidth

	return TransposeDoubleArrayToVector(pColumnGrid, totalRows, viewPortWidth);

}
std::vector<float*> &SpectrogramDisplayData::CompressFftFrames(std::vector<float*> &vMag, unsigned totalFramesRepeat0,
unsigned totalFramesRepeat1,	unsigned singlePassRepeat0, unsigned singlePassRepeat1 ){


	const unsigned viewPortWidth = this->viewPortWidth;
	const unsigned fftLenHalf = sdData.frameLen / 2;

	const unsigned totalRows = fftLenHalf * numChannels;

	GenPtrAlloc<float> genPtrAlloc(totalRows, viewPortWidth);
	genPtrAlloc.TransferOwnershipToCaller(false); //this should destroy genPtrAlloc and also free pColumnGrid

	float **pColumnGrid = genPtrAlloc.GetPtrDbl();

	unsigned currFramesRepeat, currSinglePassRepeat;
//	vector<float*> vFittedMag(viewPortWidth);
	float *pMag0, *pMag1; //left / right channel mags
	float magAvg0, magAvg1, maxVal0 = -100.0f, maxVal1 = -100.0f;
	unsigned col, offset, framesUsed;
	unsigned row, i, p, m;

	const unsigned totalFrames 		 = numFrames;//*numChannels;
	currFramesRepeat = totalFramesRepeat1;
	for( row = 0; row < totalRows; row += numChannels){

		col = 0; framesUsed = 0;

		pMag0  = vMag[row]; //fft rows - not bins to speed up processing
		if(numChannels==2) pMag1 = vMag[row+1];

		for( m = 0; m <totalFrames; m += currFramesRepeat/*totalFramesRepeat*/){ //outer loop

			if( currFramesRepeat == totalFramesRepeat0 ){
				currFramesRepeat = totalFramesRepeat1;
				currSinglePassRepeat = singlePassRepeat1;
			}else{
				currFramesRepeat = totalFramesRepeat0;
				currSinglePassRepeat = singlePassRepeat0;
			}
		   //	const unsigned inFac = currSinglePassRepeat;
			for( p = 0; p < currFramesRepeat; p += currSinglePassRepeat ) { //dio koji èita samo odreðen broj frame-ova

				if(col >= viewPortWidth){
					break;  //bounds check
				}

				magAvg0 = magAvg1 = 0;
				offset = m + p;
				maxVal0 = maxVal1 = -100;
				for( i=0; i < currSinglePassRepeat; i++){
						if(offset+i >= numFrames){
							break;
						}
						float cVal = pMag0[i+offset];
						if(cVal > maxVal0) maxVal0 = cVal;
					   //	magAvg0 += cVal;
						if(numChannels==2){
							float cVal1 = pMag1[i+offset];
							if(cVal1 > maxVal1) maxVal1 = cVal1;
						 }
					  //	magAvg1 += pMag1[i+offset];
						framesUsed = i+offset+1;
				}
				pColumnGrid[row][col] = maxVal0;//magAvg0;
				if(numChannels==2){
				  //	magAvg1 /= inFac;//currSinglePassRepeat;
					pColumnGrid[row+1][col] = maxVal1;//magAvg1;
				}
				col++;
				} //end for p < currFramesRepeat
			}
		}
	//if we havent filled all columns, let's do it now
	const unsigned lastProcessedFrame = framesUsed;
	const int remFrames = numFrames - lastProcessedFrame;
	const unsigned prevCol = col;

	if(col < viewPortWidth){

		if(framesUsed < numFrames){  //use the remaining frames to populate the rem columns

			for( row = 0; row < totalRows; row += numChannels){

				pMag0  = vMag[row]; //fft rows - not bins to speed up processing
				if(numChannels==2) pMag1 = vMag[row+1];

				magAvg0 = magAvg1 = 0;
				//offset = n + p;
				for( i = lastProcessedFrame; i < numFrames; i++){
					magAvg0 += pMag0[i];
					if(numChannels==2) magAvg1 += pMag1[i];
					framesUsed = lastProcessedFrame+1;
				}
				magAvg0 /= remFrames;
				if(numChannels==2) magAvg1 /= remFrames;

				for( col = prevCol; col < viewPortWidth; col++){
					pColumnGrid[row][col] = magAvg0;
					if(numChannels==2) pColumnGrid[row+1][col] = magAvg1;
				}

			} //end for row

		}else{    //framesUsed == numFrames
			for( row = 0; row < totalRows; row += numChannels){

				for( col = prevCol; col < viewPortWidth; col++){
					pColumnGrid[row][col] = pColumnGrid[row][prevCol-1];
					if(numChannels==2) pColumnGrid[row+1][col] = pColumnGrid[row+1][prevCol-1];
					}
			   }
			}
		} //end if col < viewPortWidth

	return TransposeDoubleArrayToVector(pColumnGrid, totalRows, viewPortWidth);  //numFrames
}
std::vector<float*> &SpectrogramDisplayData::AdjustFftFrames(std::vector<float*> &vWndOverlappedFFT){

	const unsigned viewPortWidth = this->viewPortWidth;
	const unsigned fftLenHalf = sdData.frameLen / 2;
	const float framesPerPixel = numFrames >= viewPortWidth ?
			(float)numFrames/viewPortWidth : (float)viewPortWidth / numFrames; //per channel

	unsigned skip1 = framesPerPixel;
		
	unsigned skip2 = skip1 + 1 ;

	unsigned int skipFreq1, skipFreq2, rep__, correction;
   //	SetRepeatValues((float)framesPerPixel, viewPortWidth, numFrames, skipFreq1, skipFreq2, rep__, correction);

	DspUtil::SetRepeatValues_(  (float)framesPerPixel, skipFreq1, skipFreq2 );

	if(skipFreq2 == 0 && skipFreq1 == 2) skip1 = skip2;

	const unsigned totalFrameRepeat0 = skipFreq1*skip1;
	const unsigned singlePassRepeat0 = skip1;

	const unsigned totalFrameRepeat1 = skipFreq2>0 ? skipFreq2*skip2 : totalFrameRepeat0;
	const unsigned singlePassRepeat1 = skipFreq2>0 ? skip2 : singlePassRepeat0;

	vector<float*> &vMag 	 		 = TransposeVector( vWndOverlappedFFT, fftLenHalf * numChannels );

	std::vector<float*> &vFrames = numFrames >= viewPortWidth ?
			CompressFftFrames(vMag, totalFrameRepeat0, totalFrameRepeat1, singlePassRepeat0, singlePassRepeat1) :
			ExpandFftFrames	 (vMag, totalFrameRepeat0, totalFrameRepeat1, singlePassRepeat0, singlePassRepeat1) ;

	FreeVectorResources(vMag);
	return vFrames;
}
void SpectrogramDisplayData::FreeVectorResources(const std::vector<float*> &v){

	const size_t vSize = v.size();
	for(int i=0; i< vSize; i++){
		//float * p = v[i];
		delete [] v[i];
	}
	v.clear();
}
std::vector<float*> &SpectrogramDisplayData::TransposeDoubleArrayToVector(const float * const *p, unsigned numRows, unsigned numCols){

	int i, j, k, l;
	// array p interleaves right and left channel data by row
	//(0 row - left0, 1 row -right0, 2 row - left 1, 3 row -right 1 and so on)
	//but the output vector pVTransposed contains left and channel data in separate arrays (the left and right channel data are still interleave)
	//   pVTransposed[0]->left channel frame 0, pVTransposed[1]->right channel frame 0 and so on
	const unsigned totalCols = numCols * numChannels;

	const unsigned totalRows = numRows / numChannels;

	std::vector<float*> *pVTransposed = new std::vector<float*>(totalCols);

	for(i = 0; i<totalCols; i++){

		(*pVTransposed)[i]  = new float[totalRows];
	}

	float * pTrans0, *pTrans1;

	for(i = 0, l=0; i < totalCols; i+=numChannels, l++){

		pTrans0 = (*pVTransposed)[i];

		if(numChannels==2)
			pTrans1 = (*pVTransposed)[i+1];

		for(j = 0,k=0; j < numRows; j+=numChannels,k++){

			pTrans0[k] = p[j][l];

			if(numChannels==2)
				pTrans1[k] = p[j+1][l];
		}
	}
	return *pVTransposed;
}
std::vector<float*> &SpectrogramDisplayData::TransposeVector(std::vector<float*> &v, unsigned elemLen){
	/*
	v has numFrames elements (magnitude frames per channel * numChannels) and each frame has an elemLen (fftlenHalf*numChannels) points
	pVTransposed has numElem elements with each element having numFrames/numChannels points
	*/
	const unsigned vSize 	 = v.size();//totalFrames
	const unsigned vSizePerChannel = vSize / numChannels;

	std::vector<float*> *pVTransposed = new std::vector<float*>(elemLen);
	int i,j,k,l;
	for(i=0;i<elemLen;i++){
		(*pVTransposed)[i]  =   new float[vSizePerChannel];
	}
	float * pTrans0, * pTrans1;

	for( j=0, k=0; j<elemLen; j+=numChannels, k++){
		pTrans0 = (*pVTransposed)[j];
		if(numChannels==2)
			pTrans1 = (*pVTransposed)[j+1];

		for( i=0,l=0; i<vSize; i+=numChannels,l++){
		  //	const float* p = v[i];
			pTrans0[l] = v[i][k];

			if(numChannels==2)
				pTrans1[l] = v[i+1][k];
		}
	}
	return *pVTransposed;
}
unsigned SpectrogramDisplayData::GetFrameAtPos(unsigned x) const {

	unsigned frameIndex =  static_cast<float>( x ) / fFrameWidth;

	return frameIndex > numFrames-1 ? numFrames-1 : frameIndex;

}

#pragma package(smart_init)

