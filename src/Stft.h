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
//---------------------------------------------------------------------------

#ifndef StftH
#define StftH                                                         

#include <vector>
#include "Util.h"
#include "DspUtil.h"
#include "Debug.h"
#include "WorkerThread.h"
#include "fft.h"
#include <SyncObjs.hpp>
#include "WndData.h"
#include "DataBuffersProxy.h"
//---------------------------------------------------------------------------

typedef struct {
	float binFrequency;
	float binLMagnitude;
	float binRMagnitude;

}FFT_BIN_PROP, *PFFT_BIN_PROP;


struct FFT_RES{

	float *pReal;
	unsigned threadId;
};
struct FFT_RESULT{
	float *pRealLeft;
	float *pImgLeft;
	float *pRealRight;
	float *pImgRight;

	~FFT_RESULT(){
		if(pRealLeft)
			delete [] pRealLeft;
		if(pRealRight)
			delete [] pRealRight;
		if(pImgLeft)
			delete [] pImgLeft;
		if(pImgRight)
			delete [] pImgRight;
	}
	FFT_RESULT(){
	pRealLeft 	= NULL;
	pImgLeft  	= NULL;
	pRealRight 	= NULL;
	pImgRight 	= NULL;
	}
	int threadId;
};
typedef FFT_RESULT *PFFT_RESULT;

typedef struct{
  float *pLMagnitudes;
  float *pRMagnitudes;
}FRAME_MAGNITUDES, *PFRAME_MAGNITUDES;
/*typedef struct{

	unsigned *pMagnitudes;  //in pixels
	//unsigned frameWidth
	;
	unsigned binsPerFrame;

} FFT_SPECTRAL_LINES, *PFFT_SPECTRAL_LINES ; */
enum ESpectralFeature { CENTROID, ROLLOFF, FLUX };


struct SPECTROGRAM_PARAMS {

enum EFrameOverlap { FO2 = 0, FO4, FO8 };

unsigned numSamples;
unsigned numChannels;
unsigned samplesPerSec;

unsigned frameLen; //in samples
EFrameOverlap frameOverlap; //usually 0.5
float *pData;
WindowFunc::EWindow eWnd;
nsWaveDisplay::EScale eScale;


SPECTROGRAM_PARAMS(){
	
	frameLen = 1024;
	frameOverlap = FO2;
	eWnd = WindowFunc::BLACKMAN;
	eScale = nsWaveDisplay::LINEAR;
}

SPECTROGRAM_PARAMS(float *pData, unsigned numSamples, unsigned numChannels, unsigned sampleRate,
WindowFunc::EWindow eWnd, unsigned frameLen, EFrameOverlap frameOverlap, nsWaveDisplay::EScale eScale) :

 pData(pData), numChannels(numChannels), numSamples(numSamples), samplesPerSec(sampleRate),
 eWnd(eWnd), frameLen(frameLen), frameOverlap(frameOverlap), eScale(eScale)
 {}
};

class SpectrogramDisplayData : public DataBuffersProxy{

unsigned viewPortWidth, prevViewPortWidth;

unsigned viewPortHeight, prevViewPortHeight, prevTotalBinsPerFrame;

unsigned viewPortMidPoint;  //y dimension

float frameWidth; //a frame width in pixels

unsigned minFrameWidth;
//numWindows (number of overlapping windows per channel), numFrames = numWindows/oversamplingFactor
unsigned framesPerPixel, numWindows, numFrames, prevNumFrames, prevNumWindows;

//unsigned numWindows; //number of overlapping windows  per channel

unsigned numSamplesRem;

unsigned numSamplesPadding;

unsigned overlapLen, stepLen; // in samples

float frameOverlap; //in percentages converted to floats (e.g 0.5 )

SPECTROGRAM_PARAMS::EFrameOverlap eFrameOverlap;

unsigned numColumns;

unsigned fftLen, bufferLen;

int freqDelta, wndCnt;

unsigned frameLenHalf;

unsigned dispBinsRem, binsRem, binsDup,selStartFrame, selEndFrame, selStartBin, selEndBin;//used for remaining bins
	   //multiple buffers
unsigned  startSample, endSample,  lastBuffer, startBuffer;
//numBuffers, bufferSize, lastBufferSize , samplesPerLastBuffer, samplesPerBuffer;

const unsigned cMagPosDelta;//a  constant added to each magnitude value to make them positive

float maxFFTVal;

float *pData; //input

float *pWndData;

float *pWaveData;

WndData wndData;

float freqResolution;//frequency bin resolution

float maxFrequency; //Nyquist frequency = sampling rate / 2

float wndSum0, wndSum1; //used for power spectrum normalization

float maxMagnitude, minMagnitude;

unsigned maxIndex , maxFrame;

float threshHold;

float fFreqStep, fFrameWidth;

unsigned binsPerFrame, prevBinsPerFrame; //the actual number of bins that are displayed to the user--different from frameLenHalf(which corresponds to the number of bins returned by an FFT computation)

unsigned numChannels, numCores, numThreads,  wndPerThread;

bool framePadding, useBkgThreads, computePhase;

nsWaveDisplay::EScale eScale;

FFT fft;

SPECTROGRAM_PARAMS sdData;

PFFT_RESULT pFFTResOvr;

FFT_BIN_PROP binProp;

float *pFftl, *pFftr;

unsigned *pBinIndex; //currently used only when the number of bins is fewer than the number of the viewport height pixels (so we duplicate some bins)

float *pInputLeft;
float *pInputRight;
//complex array for FFT
float *pInputLeftImg;
float *pInputRightImg;

float **pInputLeftArr;
float **pInputRightArr;
//complex array for FFT
float **pInputLeftImgArr;
float **pInputRightImgArr;

float **ppMagnitudeLeft;
float **ppMagnitudeRight;

float *pMagnitudeLeft;
float *pMagnitudeRight;

float *pFFTLReal, *pFFTRReal;
float *pFFTLImg,*pFFTRImg;

float *pCfftLReal, *pCfftReal;
float *pCfftLImg,*pCfftRImg;

unsigned **pOutColumn;

std::vector<PFFT_RESULT> vWndFFT; //stores FFT data for an (overlapping) window
std::vector<float*> vFFT;    //same as vWndFFT but not sorted (used only in multithreading)
std::vector<float*> vWndOverlappedFFT; //stores FFT for overlapping windows
//std::vector<float*> vWndFittedFFT; // this basically stores vWndOverlappedFFT but is truncated viewPortWidth
std::vector<float*> vWndCombinedFFT; //stores FFT for overlapping windows
std::vector<unsigned>  vThreadsEx;
std::vector<float>vSFlux;
std::vector<float>vSCentroid;
std::vector<float>vSRolloff;
std::vector<float>vWndMagnitude;//total frame magnitude
std::vector<float>vWndTotalMagnitude;//total frame magnitude
std::vector<float*>vSWave;
std::vector<float*>vDSCentroid;
std::vector<TWorkerThread*>vWThread;

std::vector<float*> vWndPhase;
std::vector<float*> vScaledFrames;

//multiple
//std::vector<float*> *dataBuffers;

//multiple threads

struct MULTI_THREAD_PARAMS {
	unsigned startBuffer;
	unsigned endBuffer;
	unsigned startSample;
	unsigned endSample;
	MULTI_THREAD_PARAMS(){}
	MULTI_THREAD_PARAMS(unsigned sB, unsigned eB, unsigned sS, unsigned eS)
	: startBuffer(sB), endBuffer(eB), startSample(sS), endSample(eS){}
};
std::vector<MULTI_THREAD_PARAMS> vMthread;

void (__closure *pfSpectCallback)();

TCriticalSection *pLock;//together with TThread the only VCL component here to support threading because c++ didn't support threading prior to c++11

void FreeResources();

//void FreeScaledFrames();

void FreeVectorResources(const std::vector<float*> &v);

void CreateSpectrogramGridRem(unsigned startColumn, unsigned);

//void DumpInfoToFile(ofstream&);
void CreateAndInitThreads();

void CopyAndPadData(PFFT_RESULT, unsigned );

void CopyAndPadData(float *pRealLeft, unsigned offset);

void CalculateWindowFFTs();

void ComputeFFT();

void CombineOverlappedOneChannel(unsigned startWnd,unsigned endWnd, unsigned threadId);

void CombineOverlappedTwoChannel(unsigned startWnd,unsigned endWnd, unsigned threadId);


void CalculateMagnitude(float*, float *,  float *, float *,float& totalLeft, float &totalRight);

void CalculateMagnitudeAndPhase(float* pLr, float *pLi, float* pRr, float *pRi, float &totalLeft, float &totalRight);

void CopyData(float*, float*, unsigned);

void CopyData(float * pRealLeft, float * pRealRight, float * pData);

void CopyAndPadData(float *pPaddedLeft, float *pPaddedRight);
//multiple buffers
void ComputeFFTsMulBuffers(unsigned startWnd, unsigned& endWnd, unsigned tId);

void CopyAndPadData(float *pPaddedLeft, float *pPaddedRight, float *pData, unsigned numSamplesPadding);

void SetStftForFewerBins();

void SetBinIndex(unsigned f0,unsigned f1, unsigned rep0, unsigned rep1, unsigned limiter);

unsigned GetBinIndex(unsigned);

const unsigned GetBinStep(float fDelta);

void SetSpectrogramDisplayData();

void SetSpectrogramData();

void SetSpectrogramDisplayDataLog();

unsigned SetSpectrogramDisplayDataForMultipleFrames();

void CalculateFrameWidth();

void Init();

void CreateDenseSpectrogramGrid(unsigned *);

//void CreateDense2DGrid();

void CombineMultipleColumns(unsigned numColToAverage);

void SortFftFrames();

//gets a peak FFT value
void GetMaxVal( unsigned&, unsigned&);

void SetNumWndPerThread(unsigned nWnds,unsigned nThreads);

void CalculateZeroCrossingRate(unsigned frame0, unsigned frame1);

void CalculateAverageMagnitudes(unsigned frame0, unsigned frame1);

std::vector<float*> &ExpandFftFrames(std::vector<float*> &vMag, unsigned totalPixelsRepeat0,
 unsigned totalPixelsRepeat1,  unsigned singlePassRepeat0,unsigned singlePassRepeat1 );

std::vector<float*> &CompressFftFrames(std::vector<float*> &vMag, unsigned totalFramesRepeat0,
unsigned totalFramesRepeat1,	unsigned singlePassRepeat0, unsigned singlePassRepeat1 );

std::vector<float*> &AdjustFftFrames(std::vector<float*> &vWndOverlappedFFT);

std::vector<float*> &TransposeDoubleArrayToVector(const float * const *p, unsigned numRows, unsigned numCols);

std::vector<float*> &TransposeVector(std::vector<float*> &v, unsigned elemLen);

//unsigned *GetSpectralLines(unsigned &len) const { len = bufferLen; return pOut; }

//unsigned *GetSpectralLines() const {  return pOut; }

//void FreeRawFft();

void DoStft();

 #if defined _DEBUG_
   Debugger &debugger;
   std::stringstream ss;
   char newLine ;
   #endif

   void SetBandFreqStep();

   void SetWndData();

   void SetThreadParams(unsigned numThreads, unsigned numWnd);

   std::vector<float> vFBandStep;
   std::vector<float> vZcr;
   std::vector<float> vAvgMag;

public:
	int frameReached1, frameReached2, frameReached3, frameReached4, totalFrames;

	SpectrogramDisplayData(unsigned width, unsigned height);

	~SpectrogramDisplayData(){ FreeResources(); }

	void UseBkgThread(bool);

	float GetNormSum() const { return wndSum0; }

	void SetInputData( SPECTROGRAM_PARAMS& const rSpectData);

	void RecalculateFFT();

	void SetNumWindows();

	void WindowData(PFFT_RESULT);

	void WindowData(float *, float *);

	float inline GetRelativeMagnitude(float val);

	void SetDrawSpectrogramCallback(void (__closure *pf)()) { pfSpectCallback = pf; }

	unsigned GetBinsPerFrame() const {  return binsPerFrame; }

	unsigned **GetSpectralGrid() const {  return pOutColumn; }

	float * GetWndData() const { return wndData.GetWndFunc(); }

	unsigned GetFFTFrameLength() const { return sdData.frameLen; }

	unsigned GetNumChannels() const { return sdData.numChannels; }

	unsigned GetFramesNumber() const { return numWindows / 2; }

	unsigned GetEndFrame() const { return numWindows / 2 - 1; }

	float GetFrameWidth() const { return frameWidth; }

	unsigned GetFrameAtPos(unsigned x) const ; //this returns a non-overlapping frame index

	float GetMaxMagnitude() const { return maxMagnitude; }

	float GetMinMagnitude() const { return minMagnitude; }

	unsigned GetFrameLength() const  ;

	float GetFrameOverlap() const { return sdData.frameOverlap; }

	void SetFrameLength(unsigned );

	///void SetFrameOverlap(float fOverlap);

	void SetFrameOverlap(SPECTROGRAM_PARAMS::EFrameOverlap efOverlap);//{ eFrameOverlap = efOverlap; }

	void SetFrameWindow(WindowFunc::EWindow eWnd);

	void SetFrameScale(nsWaveDisplay::EScale eScale) { this->eScale = eScale; }

	nsWaveDisplay::EScale GetFrameScale() const { return eScale; }

	EWindow GetFrameWindow() const { return sdData.eWnd; }

	void AdjustViewportVertically(unsigned );

	void AdjustViewportSize(unsigned w, unsigned h );

	SPECTROGRAM_PARAMS GetSpectralInputData() const { return sdData; }

	FFT_BIN_PROP& const GetBinPropertiesAtPos(unsigned x, unsigned y);

	float *GetFrameMagnitudesLeft(unsigned frameNum) const {
	return  numChannels == 1 /*|| frameNum == 0 */ ? vWndOverlappedFFT[frameNum] : vWndOverlappedFFT[((frameNum+1)*2-1)];
	}
	float *GetFrameMagnitudesRight(unsigned frameNum) const {
	return   vWndOverlappedFFT.at((frameNum+1)*2);
	}
//void SetFrameMagnitudes(unsigned frameNum) ;

	float GetThreshHold() const { return threshHold; }

	void SetThreshHold(float threshHoldVal) { threshHold = threshHoldVal; }

	std::vector<float>  *GetSpectralFlux();

	std::vector<float>  *GetSpectralCentroid();

	std::vector<float>  *GetSpectralRolloff();

	std::vector<float>  *GetAverageMagnitudes();

	std::vector<float>  *GetZeroCrossingRate(); //normally ZCR shouldn't be a float but an integer -- still, this makes certain things somewhat cleaner (less code)

	void CalculateSpectralFlux(unsigned frame0, unsigned frame1);

	void CalculateSpectralRolloff(unsigned frame0, unsigned frame1);

	void CalculateSpectralCentroid(unsigned frame0, unsigned frame1);

	void SetSelection(unsigned xStart,unsigned xEnd, unsigned yStart, unsigned yEnd);

	unsigned GetSelStartBin() const { return selStartBin; }

	unsigned GetSelEndBin() const { return selEndBin; }

	unsigned GetSelStartFrame() const { return selStartFrame; }

	unsigned GetSelEndFrame() const { return selEndFrame; }

	void DumpMagnitudesToFile(const char *pFileName, unsigned startFrame, unsigned endFrame,unsigned startBin, unsigned endBin);

	void DumpToFile(const char *pFileName, unsigned startFrame, unsigned endFrame);

	void DumpToFile(const char *pFileName, unsigned startFrame, unsigned endFrame,unsigned startBin, unsigned endBin, int &status);

	unsigned GetWidth() const { return (numWindows/2) * frameWidth; }//this could less than the view port width

	unsigned GetViewPortWidth() const { return viewPortWidth; }

	unsigned GetViewPortHeight() const { return viewPortHeight; }

	void SetViewPortHeight(unsigned h) { viewPortHeight = h; /*binsPerFrame = viewPortHeight;*/}

	void SetViewPortWidth(unsigned w) { viewPortWidth = w; }

	unsigned GetNumColumns() const { return bufferLen;    }

	unsigned GetNumFrames() const { return numFrames; }

	unsigned GetOverlappingFramesNumber() const { return numWindows; }

	unsigned GetMagnitudeNumber() const { return bufferLen; }
//return the total number of samples -- this depends on the current zoom level
	unsigned GetNumSamples() const { return sdData.numSamples; }

	unsigned GetMaxFrequency() const { return maxFrequency; }

	void SetNumSamples(unsigned numSamples){  sdData.numSamples = numSamples; }

	void SetData(float *pData) { sdData.pData = pData; }

	std::vector<float*> *GetFFTFrames() { return &vFFT; }

	void SetBinPositions(unsigned height, unsigned &binStart, unsigned &binEnd);

	void SetData(float *pData, unsigned numSamples) {
		sdData.pData = pData;
		sdData.numSamples = numSamples;
		CalculateFrameWidth();
	}

	void SetStartAndEndFrames(unsigned startFrame, unsigned endFrame){
		selStartFrame = startFrame;
		selEndFrame = endFrame;
	}
	void SetStartAndEndBins(unsigned startBin, unsigned endBin){
		selStartBin = startBin;
		selEndBin = endBin;
	}

	void SetViewPortRange(unsigned startSample, unsigned endSample){

		this->startSample	= startSample;
		this->endSample		= endSample;
	}

};
#endif

