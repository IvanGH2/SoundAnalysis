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

#ifndef SoundIOH
#define SoundIOH

#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <stdexcept>
#include "stdio.h"
#include <iostream>
#include <fstream>
#include "dsputil.h"
#include "Util.h"
#include "windows.h"
#include "mmsystem.h"
#include "Mmreg.h"
#include "GenPtrAlloc.h"
#include "MemBuffer.h"
using namespace std;
//---------------------------------------------------------------------------
//enum DataType { BIT_8U=1, BIT_16=2, BIT_24=3, BIT_32F=4 };

enum EFileType { WAVE, RAW };

const unsigned cWaveFormatExOffset = 20;
const unsigned cWaveFormatIdOffset = 12;

namespace	nsIODirection {

	enum EDirection { INPUT, OUTPUT, BOTH };
}

typedef struct  {
	char chunkId[4];   //RIFF
	unsigned int chunkSize; //fSize
	char format[4];  //WAVE
	char subChunkId[4]; //fmt
	unsigned int subChunkSize; //sizeof(WAVEFORMATEX)
	//the fields below correspond to WAVEFORMATEX
	WAVEFORMATEX wfex;

} WAVE_HDR, *PWAVE_HDR;

class SoundIOException : public exception {
	std::string exMsg ;
public:
  SoundIOException(const std::string &s) { exMsg = s;  }
  virtual const char* what() const
  {
	return exMsg.c_str();
  }
};

class SoundIO{

	protected:

	//bool direction;

	char *pData; //not created but assigned

	char *pIData, *pOData;

	char *pWaveformData; //this points to the actual samples but most are skipped for performance

	unsigned dataSize;

	unsigned fileSize ;

	unsigned bufferSize;

	unsigned defaultBufferSize, maxBufferSize, minBufferSize;

	std::string fileName;

	unsigned dataOffset, fmtOffset ;

	unsigned dataTime;

	unsigned numSamples;   //total number of samples

	unsigned numWfSamples;   //number of waveform samples

	EncodingType dataType;

	PWAVE_HDR pWaveFormat;

	WAVE_HDR waveHdr;

	WAVEFORMATEX wfex;

	GenPtrAlloc<char> soundIO;

	void CalculateNumSamples();

	char *ConvertToInternalFormat(char*,unsigned);

    void SetHdrSectionNames();
	public:

	SoundIO(){
		numSamples = 0;
		dataOffset = 0;
		dataTime   = 0;
		defaultBufferSize = 5 * 1024 * 1024;//5 MB
		bufferSize = 2 * 1024 * 1024;   //2 MB
		maxBufferSize = 1 * 1024 * 1024;
		minBufferSize = 10 * 1024;//10 kB
		pWaveFormat = NULL;
	}
	~SoundIO(){	DoCleanup(); }

	//void SetDirection(bool input){  direction = input; }

	virtual nsIODirection::EDirection GetDirection() const = 0;

	void FillWaveFormatEx(WAVEFORMATEX& const rWfex);

   void DoCleanup();

   void SaveAsStringValues(const char* fileName,int size, int typeSize);

   void SaveAsType(const char* fileName,int size, int typeSize);

   void SetHeaderInfo(PRAW_DATA_INFO pRawDataInfo);

   void SetHeaderInfo(PWAVE_HDR pWaveHdr) { pWaveFormat = pWaveHdr; }

   void SetWaveFormat(WAVEFORMATEX const &wf) {
	FillWaveFormatEx(wf);
	waveHdr.wfex.wBitsPerSample = dataType = BIT_32; //internally we use 32 bits (floating point)
   }

   WAVEFORMATEX const &GetWaveFormat() const { return wfex; }

   PWAVE_HDR GetHeaderInfo() { return  &waveHdr; }  //pWaveFormat;

   void SetData(char *pData) { this->pData = pData; }

   void SetTypeSizeinBytes();

   void SetBufferSize(unsigned bSize){ bufferSize = bSize; }

   unsigned GetBufferSize() const { return bufferSize; }

   char *GetData() const ;

   char *GetWaveformData() const { return pWaveformData; }

   //char* GetData() const { return pData /*+ dataOffset*/; }

	unsigned int GetDataOffset()const {
		return dataOffset;
	}

	unsigned int GetDataSize() const {
		return dataSize;// = fileSize - dataOffset doesn't always work because a wave could have data beyond the samples
	}

	unsigned GetNumSamples();

	unsigned GetWaveformNumSamples() const { return numWfSamples; }

	void SetNumSamples(unsigned nSamples){ numSamples = nSamples; }

	void SetDataSize(unsigned int size) { dataSize = size;  }

   //	void SetFileSize(unsigned int size){ fileSize = size; }

	void SetFileName(const std::string &fName) {

	   fileName = fName;
	}

	const std::string &getFileName() const {
		return fileName;
	}

	unsigned int GetTrackTime() const {
		if(dataTime==0)
			CalculateTrackTime();
		return dataTime;
	}

	void CalculateTrackTime(){  //in ms

		 unsigned numSamples = (dataSize/(waveHdr.wfex.wBitsPerSample/8)) / waveHdr.wfex.nChannels; //per channel
		 dataTime = static_cast<float>(numSamples) / waveHdr.wfex.nSamplesPerSec * 1000;
	}
	/**
	returns the number of miliseconds (time duration) for the given number of samples
	*/
	unsigned GetTimeForSamples(unsigned numSamples){
		   if( numSamples % 2 != 0) numSamples--;
		   return (static_cast<float>(numSamples)/waveHdr.wfex.nChannels) / waveHdr.wfex.nSamplesPerSec * 1000;
	}
	/*
	return the number of miliseconds into the file for a particular sample
	*/
	unsigned GetTimeForSample(unsigned currSample){
		   return static_cast<float>(currSample) / (waveHdr.wfex.nChannels*waveHdr.wfex.nSamplesPerSec) * 1000;
	}
   /*	void SetDataInternal(char *pData){
		pIData = pData;
	}*/

  //	char* GetDataInternal() const { return pIData; } //the actual sound data converted to float (internal format)
};

class SoundInput : public SoundIO{

	//void setTypeSizeinBytes();

	unsigned previewSize, viewPortWidth;

	unsigned reducedFactor;

	bool multipleBuffers;

	MemBuffer memBuffer;

	unsigned int OpenHeader(const std::string &fileName);       //const std::string &fileName

 //	unsigned int OpenHeader(const char* fileName);

	void SetDataOffset();

	bool IsDataChunk(const char* );

	bool IsFmtChunk(const char* pData);

	void SetFmtOffset();

	void CopyHeader(PWAVE_HDR pWF);

	void CheckHeader(const char *pHdr);

	unsigned GetBufferSizeGivenViewPortWidth();

public:
	SoundInput() //: dataOffset(0),fmtOffset(0),fileSize(0),previewSize(0),reducedFactor(0)*/ //{ }
	   {

		fileSize = 0;
		dataOffset = 0;
		fmtOffset = 0;
		previewSize = 0;
		reducedFactor = 5;
		multipleBuffers = false;
		}

	unsigned int OpenRaw(const std::string &fileName);

	unsigned int OpenRaw(){ return OpenRaw(fileName.c_str()); }

	unsigned int Open(const std::string &fileName);

	nsIODirection::EDirection GetDirection() const { return nsIODirection::INPUT; }

	vector<float*> *GetMemoryBuffers() const { return memBuffer.GetMemoryBuffers(); }

	unsigned GetMemoryBuffersNum() const { return memBuffer.GetMemoryBuffers()->size(); }

	unsigned GetMemoryBufferSize() const { return memBuffer.GetOutBufferSize(); }

	unsigned GetLastMemoryBufferSize() const { return memBuffer.GetLastBufferSize(); }

	bool IsMultipleBuffers() const { return multipleBuffers; }

	void SetViewPortWidth(unsigned vW){ viewPortWidth = vW; };

};
class SoundOutput : public SoundIO{


   const unsigned cHdrSize, cDataChunkSize ;

public:

	SoundOutput() : cHdrSize(44), cDataChunkSize(8) {}

	void Save(const char *pName, EFileType eFileType);

	void SetFixedWaveHeader();

    nsIODirection::EDirection GetDirection() const { return nsIODirection::OUTPUT; }
};

#endif
