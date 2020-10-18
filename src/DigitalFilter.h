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

#ifndef DigitalFilterH
#define DigitalFilterH
//---------------------------------------------------------------------------
#include "Util.h"
#include "vector.h"
#include "OpCallback.h"
#include "WorkerThread.h"
#include "AsyncMsgQueue.h"
#include "GenPtrAlloc.h"
#include "DataBuffersProxy.h"

using namespace nsDigitalFilter;
using namespace WindowFunc;

typedef struct{
                EFilterType eFilterType;
				EWindow eWnd;
				unsigned len;
                float *pData;

}FILKERNEL, *PFILKERNEL;


class DigitalFilter : OperationCallback, public DataBuffersProxy {

	EFilterType eFilterType;

	EWindow eWnd ,eDefWnd;

	GenPtrAlloc<float> filData;

  // 	std::vector<float*> *dataBuffers;

	bool inverted, useBkgThread, bkgThreadFinished;//, multipleBuffers;

	static bool allThreadsFinished;

	unsigned fKernelLen, sampleRate, outSize, len, wndSum0, wndSum1, fftLen, numBuffers;

   //	unsigned bufferSize, lastBufferSize, samplesPerBuffer, samplesPerLastBuffer,
	unsigned startSample, endSample, currProgress;


	float loCutoff, hiCutoff, rollOff;

	float * pFilterKernel, * pFilterData, * pWndData, * pData;

	std::string filterName;

	static float *pSharedFilterKernel;

	static unsigned numFilters, numThreads;

	volatile static float totalProgress;



	nsWaveDisplay::PfMainWndNotify pfMainWndNotify;

	TWorkerThread *pWorkerThread;

	static AsyncMsgQueue asyncMsgQueue;

	void Free();

	void Filter();

	public:

	#if defined _NOTIFY_MAIN_WND
		void SetMainWndCallback(nsWaveDisplay::PfMainWndNotify pfMainNotify) { pfMainWndNotify = pfMainNotify; }
	#endif

	DigitalFilter(EFilterType eFilterType, EWindow eWnd, unsigned fKernelLen, float lowCutoff, float hiCutoff, unsigned sampleRate);

	~DigitalFilter(){ Free(); }

	void AddToPool();

	float *GetFilterKernelData();

	static std::vector<PFILKERNEL> vFilterPool;

	void CreateFilterKernel();

	float GetLowCutoffFreq() const { return loCutoff; }

	float GetHiCutoffFreq()  const { return hiCutoff; }

	float *GetFilterKernel() const { return pFilterKernel; }

	float *GetSharedFilterKernel() const { return pSharedFilterKernel; }

	void SetInverted(bool inverted) { this->inverted = inverted; }

	bool GetInverted() const { return inverted; }

	void ApplyFilter(float *pData, unsigned len);

	void ApplyFilter_(float *pData, unsigned len);

	void SetDefaultWindow(EWindow defWnd) { eDefWnd = defWnd; }

	EWindow GetWindow(EWindow defWnd) const { return eWnd != 0 ? eWnd : eDefWnd; }

	EFilterType GetFilterType() const { return eFilterType; }

	float *GetFilteredData ()   const { return pFilterData+fKernelLen; }

	unsigned GetDataLen() const { return outSize - fKernelLen; }

	void UseBackgroundThread(bool useThread);

	bool IsBackgroundThread() { return useBkgThread; }

	bool IsBackgroundThreadFinished(){ return bkgThreadFinished; }
	//da li maknuti ovo
	void SetProgressCallback(void (__closure *p)(void*, unsigned)) { pProgressCallback = p; } ;

	unsigned GetProgressValue() { return currProgress; }

	static unsigned GetCurrentFilter() { return asyncMsgQueue.GetValue(); }

	static unsigned GetTotalProgressValue() { return totalProgress; }



	static bool AreThreadsFinished() { return allThreadsFinished; }

	static unsigned GetNumThreads() { return numThreads; }

	static void SetNumFilters(int nF) { numFilters=nF; }

	static unsigned GetNumFilters() { return numFilters; }

	void SetFilterName(std::string &fName) { filterName = fName; }

	const std::string &GetFilterName() const { return filterName; }

	/*void SetDataBuffers(std::vector<float*> * vDataBuff, unsigned buffSize, unsigned lastBuffSize){

		dataBuffers = vDataBuff;
		numBuffers  = vDataBuff->size();
		bufferSize  = buffSize;
		lastBufferSize = lastBuffSize;
		samplesPerBuffer = buffSize/sizeof(float);
		samplesPerLastBuffer = lastBufferSize/sizeof(float);
		multipleBuffers = true;
	}  */
	void SetViewPortRange(unsigned startSample, unsigned endSample){

		this->startSample	= startSample;
		this->endSample		= endSample;
	}
};

#endif
