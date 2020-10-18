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

#include "DigitalFilter.h"
#include "DspUtil.h"
#include "fft.h"


float * 		DigitalFilter::pSharedFilterKernel = 0;
volatile float  DigitalFilter::totalProgress = 0.0f;
unsigned		DigitalFilter::numFilters = 0;
unsigned		DigitalFilter::numThreads = 0;
AsyncMsgQueue 	DigitalFilter::asyncMsgQueue;
//bool 			DigitalFilter::allThreadsFinished = false;

DigitalFilter::DigitalFilter(EFilterType eFilterType, EWindow eWnd, unsigned fKernelLen, float loCutoff, float hiCutoff, unsigned sampleRate):

	eFilterType(eFilterType),
	eWnd(eWnd),
	fKernelLen(fKernelLen),
	loCutoff(loCutoff),
	hiCutoff(hiCutoff),
	sampleRate(sampleRate),
	eDefWnd(BLACKMAN),
	useBkgThread(false),
	bkgThreadFinished(false),
	pWorkerThread(0)
{
	//eDefWnd = RECTANGULAR;
	if(eWnd==0)
		eWnd = eDefWnd;
	inverted = false;
	filData.TransferOwnershipToCaller(false);

}
void DigitalFilter::UseBackgroundThread(bool useThread){

	useBkgThread = useThread;

	if(useThread){
		pWorkerThread = new TWorkerThread(true);
		pWorkerThread->FreeOnTerminate = true;
		pWorkerThread->SetWorkerMethodCallback(&Filter);
		numThreads++;
	  //pWorkerThread->SetUpdateGUIMethod(pProgressCallback);
		}
	  
}
void DigitalFilter::CreateFilterKernel(){

	 float wndSum0, wndSum1;
	 if(eWnd == 0)eWnd = eDefWnd;

   //	if(pSharedFilterKernel) return;
	const float loCutoffF = static_cast<float>( loCutoff ) / sampleRate;
	const float hiCutoffF = static_cast<float>( hiCutoff ) / sampleRate;

	switch(eFilterType){
		case LOPASS:
			pFilterKernel = DspUtil::CreateLoPassFilter(loCutoffF, fKernelLen, eDefWnd);
			break;
		case HIPASS:
			pFilterKernel = DspUtil::CreateHighPassFilter(hiCutoffF, fKernelLen, eDefWnd);
			break;
		case BANDPASS:
			pFilterKernel = DspUtil::CreateBandPassFilter(loCutoffF, hiCutoffF, fKernelLen, eDefWnd);
			break;
	}
	pSharedFilterKernel = pFilterKernel;

}
void DigitalFilter::Free(){
	if(pFilterKernel)   delete [] pFilterKernel;
 //	if(pFilterData) 	delete [] pFilterData;
   //	numFilters--;
}
void DigitalFilter::ApplyFilter(float *pData, unsigned len){

	  this->pData = pData;
	  this->len   = len;
	  if( useBkgThread ){
		if(!pWorkerThread)UseBackgroundThread( true );
		pWorkerThread->Resume();//starts the worker thread and calls Filter()
	  }else{
		Filter(); //call Filter() in the main thread
	  }

}
//this implements FFT convolution but there's somethng wrong with it because the output suffers from digital artifacts
void DigitalFilter::Filter(){

	const unsigned fftLen = DspUtil::NextPowerOfTwo(fKernelLen*2);//fKernelLen*2;
	unsigned frameLen = fftLen - fKernelLen;
	const unsigned frameOverlap = fftLen-frameLen;

	//if we have relatively many remaining samples (>100) that haven't been processed, let's handle these as well, otherwise  just ignore them

	outSize =  len+fKernelLen;//+paddingZeros;
	
	FFT fft;

	//FFT the filter kernel
	GenPtrAlloc<float> dataReal( fftLen );
	GenPtrAlloc<float> fkReal( fftLen );
	GenPtrAlloc<float> overlap( frameOverlap );

	dataReal.TransferOwnershipToCaller( false );
	fkReal.TransferOwnershipToCaller( false );
	overlap.TransferOwnershipToCaller( false );

	float * pFilterKernelReal = fkReal.GetPtr();// new float[fftLen];      ???????
	float * pDataReal 		  = dataReal.GetPtr();//new float[fftLen];
	float * pOverlap  		  = overlap.GetPtr();//new float[frameOverlap];

	pFilterData =  filData.GetPtr(outSize);// new float[outSize];
  //	memset(pFilterData, 0, sizeof(float) * outSize);
	float *pFk = pFilterKernel;// ? pFilterKernel : pSharedFilterKernel;


	memset(&pFilterKernelReal[frameOverlap], 0, sizeof(float) * frameLen);
	memcpy(&pFilterKernelReal[0], pFk, sizeof(float) * frameOverlap);
	//zero the overlap array
	memset(pOverlap, 0, sizeof(float) * frameOverlap);

 //FFT the filter kernel
	fft.ComputeRealFFT(pFilterKernelReal, fftLen, false);
	//FFT the input signal
	float * pCurrFrame;
	const unsigned numFrames =  len / frameLen;   //2 is just to account for the overlap
	unsigned offsetData = 0, offset = 0;
	int i, progress = 0, prevProgress = 0;
	int deltaProgress;
	const int lastFrame = numFrames+1;
	const float scale = 2.0f / fftLen;
	float * pData = this->pData;

	unsigned startBuffer, lastBuffer, startSample = this->startSample, endSample = this->endSample;
	unsigned relStartSample, relEndSample, currBuff, currBuffLen, paddLen;

	if( multipleBuffers ){

	//endSample = this->endSample == 0 ? startSample + sdData.numSamples : this->endSample;
			startBuffer = startSample / samplesPerBuffer;
			lastBuffer  = endSample / samplesPerBuffer;

			relStartSample  = startSample > 0 ? startSample - (startBuffer * samplesPerBuffer) : 0;
			relEndSample    = endSample-(lastBuffer * samplesPerBuffer) ;

			pData = &dataBuffers->at(startBuffer)[relStartSample];

			currBuff = startBuffer;
			currBuffLen = startBuffer==lastBuffer ? relEndSample : samplesPerBuffer;
	}

	for(i=0;i<numFrames;i++){
	//first copy the time domain samples because we if we don't they'll get overwritten (FFT uses an in-place algorithm)
		//offsetData = i * frameLen;

		if(i==lastFrame)
			frameLen = len-offsetData;
        
		if( multipleBuffers ){

			if(offset >= currBuffLen && startBuffer != lastBuffer) {//we should switch to the next buffer
				offset = 0;
				pData = dataBuffers->at(++currBuff);//currBuffer mora biti globalna zbog multithreading-a , odnosno svaki thread mora imati svoju vlastitu
				if(currBuff == lastBuffer) currBuffLen = relEndSample;
			}

			if (currBuff == lastBuffer && offset + fftLen > currBuffLen/*relEndSample*/) {//padding necessary
				paddLen = offset + fftLen - currBuffLen;//lastBuffEndSample - pCurrBufferPtr;

				memcpy(&pDataReal[0], &pData[offset], sizeof(float) * (fftLen-paddLen) );
				memset(&pDataReal[fftLen-paddLen], 0, sizeof(float) * paddLen);
			  //	offset = currBuffLen;
			}else{
				if(offset + fftLen > currBuffLen){
					paddLen = offset + fftLen - currBuffLen;//-pCurrBufferPtr;
					memcpy(&pDataReal[0], &pData[offset], sizeof(float) * (fftLen-paddLen) );
					memset(&pDataReal[fftLen-paddLen], 0, sizeof(float) * paddLen);
			  //	offset = currBuffLen;
				}else{ //we're still completely inside the current buffer, which means no padding is needed
					memcpy(&pDataReal[0], &pData[offset], sizeof(float) * frameLen);
					memset(&pDataReal[frameLen], 0, sizeof(float) * frameOverlap);
				}
			}
		}else{ //end multiple buffers

			memcpy(pDataReal, &pData[offsetData], sizeof(float) * frameLen);

			memset(&pDataReal[frameLen], 0, sizeof(float) * frameOverlap);
		}

		fft.ComputeRealFFT(pDataReal, fftLen, false);  //forward fft
		//perform FFT convolution
		fft.ComputeConvolution(pDataReal, pFilterKernelReal, fftLen);

		fft.ComputeRealFFT(pDataReal, fftLen, true); //inverse fft

		pCurrFrame = pDataReal;

		for(int ii=0;ii<frameOverlap;ii++){ //overlap and add
			pCurrFrame[ii] = (pCurrFrame[ii]+pOverlap[ii]) * 0.5f;
			pOverlap[ii] = pCurrFrame[frameLen+ii];
		}

		if(i==lastFrame)
			frameLen += frameOverlap;

		for(int k=0;k<frameLen;k++){
			pFilterData[k+offsetData] = pCurrFrame[k]*scale;
		}

	#if defined _NOTIFY_MAIN_WND
		progress = ((static_cast<float>(i+1))/numFrames)*100;

		if(progress > 0 && progress % 5 == 0){
            int deltaProgress = progress-prevProgress;
				totalProgress += static_cast<float>(deltaProgress)/numFilters;
				prevProgress = progress;
				currProgress = progress;
			   if(!useBkgThread)	pfMainWndNotify(nsWaveDisplay::SHOW_ACTION_PROGRESS,
				&nsWaveDisplay::FILTER_PROCESSING_PROGRESS(progress, totalProgress, filterName));
	  //	}
	}
	#endif
		offsetData += frameLen;
		offset 	   += frameLen;
		if(multipleBuffers && currBuff == lastBuffer && offset >= currBuffLen)
			break;
	} //end main for loop

	if(inverted){
		for(int i=0;i<outSize;i++)
			pFilterData[i] = -pFilterData[i];
	}//end inverted

	 if(useBkgThread && pWorkerThread){
		bkgThreadFinished = true;
		numThreads--;
		asyncMsgQueue.AddValue(reinterpret_cast<unsigned>( this ));
	 }

}
void DigitalFilter::AddToPool(){

	PFILKERNEL pFk = new FILKERNEL;
	pFk->eFilterType = eFilterType;
	pFk->eWnd = eWnd;
	pFk->len = fKernelLen;
	pFk->pData = pFilterKernel;

	vFilterPool.push_back(pFk);
}

float *DigitalFilter::GetFilterKernelData() {

std::vector<PFILKERNEL>::const_iterator iter;

for(iter=vFilterPool.begin();iter<vFilterPool.end();iter++){

const FILKERNEL fk = **iter;
if(fk.eFilterType == eFilterType && fk.eWnd == eWnd && fk.len == fKernelLen) return fk.pData;

}
return 0;
}
//time domain convolution
void DigitalFilter::ApplyFilter_(float *pData, unsigned len){

	outSize =  len + fKernelLen - 1;
	pFilterData = new float[outSize];

	for(int i=0;i<len;i++){
		for(int j=0;j<fKernelLen;j++){//fKernelLen+1
			pFilterData[i+j] = (pFilterData[i+j] + (float)(pData[i] * pFilterKernel[j]) * 0.5f) ;
		}
	}
   //advance the pointer so as to skip
   pFilterData +=  fKernelLen;
}
//-------------------------------------------------- )-------------------------

#pragma package(smart_init)
