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

#include "SoundIO.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

void SoundIO::DoCleanup(){
   //	if(pData && dataType!=BIT_32) delete [] pData;    //ne treba ako koristim GenPtrAlloc
   //	else if(pOData) delete [] pOData;
}
/**
  for INPUT we fill just the WAVEFORMATEX struct and ignore the remaining fields
  for OUTPUT we fill the entire WAVE_HDR structure
*/
void SoundIO::SetHeaderInfo(PRAW_DATA_INFO pRawDataInfo){

   if(GetDirection() == nsIODirection::OUTPUT)
   ((SoundOutput*)this)->SetFixedWaveHeader();
   //fill WAVEFORMATEX struct
   waveHdr.wfex.nSamplesPerSec 	=    pRawDataInfo->sampleRate;
   waveHdr.wfex.wBitsPerSample  =    pRawDataInfo->encoding;
   waveHdr.wfex.nChannels   	=    pRawDataInfo->numChannels;
   waveHdr.wfex.nBlockAlign 	=    waveHdr.wfex.wBitsPerSample/8 * waveHdr.wfex.nChannels;
   waveHdr.wfex.nAvgBytesPerSec =	 waveHdr.wfex.nBlockAlign * waveHdr.wfex.nSamplesPerSec;
   waveHdr.wfex.wFormatTag 		=    waveHdr.wfex.wBitsPerSample == BIT_32 ? WAVE_FORMAT_IEEE_FLOAT : WAVE_FORMAT_PCM;
   waveHdr.wfex.cbSize 			=	 0;
   pWaveFormat = &waveHdr;
}
void SoundOutput::SetFixedWaveHeader(){

 //	const unsigned cRiffChunkSize  = 8;
	waveHdr.chunkId[0] = 'R';
	waveHdr.chunkId[1] = 'I';
	waveHdr.chunkId[2] = 'F';
	waveHdr.chunkId[3] = 'F';

	//waveHdr.chunkSize = dataSize - cRiffChunkSize;
	waveHdr.format[0] = 'W';
	waveHdr.format[1] = 'A';
	waveHdr.format[2] = 'V';
	waveHdr.format[3] = 'E';

	waveHdr.subChunkId[0] = 'f';
	waveHdr.subChunkId[1] = 'm';
	waveHdr.subChunkId[2] = 't';
	waveHdr.subChunkId[3] = ' ';
	waveHdr.subChunkSize = 16;//sizeof(WAVEFORMATEX);
}
void SoundInput::CheckHeader(const char *pHdr){

	const char  riffSection[] = { 'R','I','F','F' };
	const char  waveSection[] = { 'W','A','V','E' };
	const unsigned riffOffset = 0;
	const unsigned waveOffset = 8;
	const unsigned riffLen 	  = sizeof(riffSection);
	const unsigned waveLen	  = sizeof(waveSection);
	for(int i=0;i<riffLen;i++){
		if(pHdr[riffOffset+i]!=riffSection[i])
			throw SoundIOException("Invalid file format");
	}
	for(int i=0;i<waveLen;i++){
		if(pHdr[waveOffset+i]!=waveSection[i])
			throw SoundIOException("Invalid file format");
	}
}

/**
  this mehod populates the WAVEFORMATEX structure from the information entered by a user (via the Raw Data import dialog)
*/

//unsigned int  SoundInput::OpenRaw(const char* pFileName)
unsigned int  SoundInput::OpenRaw(const std::string &fName)
{
  if(fileSize == 0){
  ifstream file (fName.c_str(), ios::in | ios::binary | ios::ate);
  if (file.is_open())
  {
	fileSize = dataSize = file.tellg();
	//this should never happen
	if(fileSize < dataOffset){

		cout << "Offset larger than file-should never happen";
        return 0;
	}

  //	pData = new char [dataSize];
	pData = soundIO.GetPtr(dataSize);
	soundIO.TransferOwnershipToCaller(false);

	file.seekg (dataOffset, ios::beg);
	file.read (pData, dataSize);
	pOData = pData;
	file.close();
	//cout << "the entire file content is in memory";
	SetTypeSizeinBytes();
	#if defined _USE_INTERNAL_FORMAT_
	 if(dataType != BIT_32) {

	 pData = ConvertToInternalFormat(pData,GetNumSamples());
	 soundIO.DeallocateArray();
	 soundIO.SetPtr(pData);
   //	 if(pOData) delete [] pOData;
	 }
	 #endif
  }
	else cout << "Unable to open file";
  }
  fileName = fName;
  return fileSize;
}
//unsigned int  SoundInput::Open(const char* fileName)
unsigned int  SoundInput::Open(const std::string &fileName)
{
  return OpenHeader(fileName);
}
/**
 the method converts sampled data to floating point values
*/
char *SoundIO::ConvertToInternalFormat(char *pData, unsigned nSamples){

	 return (char*)DspUtil::ConvertToFloat(pData, nSamples, dataType);//pIData;
}
void SoundIO::SetTypeSizeinBytes(){

	const short format = waveHdr.wfex.wFormatTag;//pWaveFormat->wfex.wFormatTag;
	const short bitsPerSample = waveHdr.wfex.wBitsPerSample;
	switch(format){
		case WAVE_FORMAT_IEEE_FLOAT://0x0003:
			dataType = BIT_32;  //32 BIT FLOAT
			break;
		case WAVE_FORMAT_PCM://0x0001:
			dataType = (bitsPerSample == 8) ? BIT_8 : BIT_16 ;
			break;
		default:
			throw SoundIOException("Unsupported wave format");
	}
}
/**
this method sets the offset to the data section (where the samples are located)
it also asssumes that pData already points to the fmt section (so yoz should call this after SetFmtOffset)
*/
void  SoundInput::SetDataOffset()
{
	const unsigned  cAdvancePtr   =  2;
	const unsigned  cDataSizePtr  =  4;
	const unsigned  cSectionSize  =  8;

	char *pEndFile = pData + fileSize - fmtOffset;
	char *pStartFile = pData;

	pData += (fmtOffset+cDataSizePtr);

	while(pData < pEndFile){
		if(IsDataChunk(pData)){
			pData += cSectionSize;
			pEndFile = pData;
			dataOffset = (unsigned int)(pEndFile-pStartFile);
			dataSize  = *(unsigned int*)(pData-cDataSizePtr); //the size in bytes of the samples
		}else{
			pData += cAdvancePtr;
		}
	}
	pData = pStartFile;
}
//offset into the file where the WAVEFORMATEX structure is located (also populates the WAVEFORMAT(EX) struct
void  SoundInput::SetFmtOffset()
{
	const unsigned  fmtSectionSize =  8;
	const unsigned  cAdvancePtr    =  2;
	char *pEndFile = pData + fileSize;
	char *pStartFile = pData;
	//this is usually the correct offset to the fmt section but if chunks like list, fact etc exist, we need to  find the location where the fmt chunk begin (it can't be hardcoded)
	pData += cWaveFormatIdOffset;

	while(pData < pEndFile){
	if(IsFmtChunk(pData)){
	   fmtOffset = pData - pStartFile ;
	   pData += fmtSectionSize;
	   pEndFile = pData;
	   FillWaveFormatEx((WAVEFORMATEX& const)*pData);
	}else{
		pData+=cAdvancePtr;
		}
	}
	pData = pStartFile;
}

void SoundIO::FillWaveFormatEx(WAVEFORMATEX& const rWfex){

  // if(!pWaveFormat)	pWaveFormat = new WAVE_HDR;
   waveHdr.wfex = rWfex;

   //ovo dolje možda neæe trebati
   wfex.nSamplesPerSec = rWfex.nSamplesPerSec;
   wfex.wBitsPerSample = rWfex.wBitsPerSample;
   wfex.nChannels = rWfex.nChannels;
   wfex.nBlockAlign = rWfex.nBlockAlign;
   wfex.nAvgBytesPerSec = rWfex.nAvgBytesPerSec;
   wfex.cbSize = rWfex.cbSize;
   wfex.wFormatTag = rWfex.wFormatTag;

   SetTypeSizeinBytes();
}
bool SoundInput::IsFmtChunk(const char *pData){

	const char fmt[3] = { 'f','m','t' };
	for(int i=0; i<3;i++){
		if(pData[i]!= fmt[i])
			return false;
	}
	return true;
}
bool SoundInput::IsDataChunk(const char *pData){

	const char data[4] = { 'd','a','t','a' };
	for(int i=0; i<4;i++){
		if(pData[i]!=data[i])
			return false;
	}
	return true;
}

#if !defined  _FAST_IO
//unsigned SoundInput::OpenHeader(const char *pFileName)
unsigned SoundInput::OpenHeader(const std::string &fName)
{
  ifstream file (fName.c_str(), ios::in|ios::binary|ios::ate);
  if (file.is_open())
  {
	file.seekg (0, ios::end);
	fileSize = file.tellg();

  //	pData = new char [fileSize];
	pData = soundIO.GetPtr(fileSize);
	soundIO.TransferOwnershipToCaller(false);

	file.seekg (0, ios::beg);
	//read header
	file.read (pData, fileSize);
	pWaveFormat = (PWAVE_HDR)pData;
	pOData = pData;     //mièe se

	CheckHeader(pData);
	//convert to internal representation
	SetFmtOffset();   //basically fills the WAVEFORMATEX
	if(fmtOffset==0)throw SoundIOException("Invalid file format or invalid file");

	SetDataOffset();  //sets the offset to the sampled data (accessible via GetData())
	//check that we have a data chunk, otherwise throw an exception
	if(dataOffset==0)throw SoundIOException("Invalid file format or invalid file");

	SetTypeSizeinBytes(); //MOŽDA MAKNUTI ODAVDE
	fileName = pFileName;
	pData +=  dataOffset;  //move the pointer to the actual sound data

	//copy the sound file header to a WAVE_HDR struct
	CopyHeader(pWaveFormat);
	pWaveFormat = &waveHdr;
	 #if defined _USE_INTERNAL_FORMAT_
	 if(dataType != BIT_32) {
	 const char *pTmp0 = pData-dataOffset;
	 const char *pTmp1 = pWaveformData;       //??????

	 pData = ConvertToInternalFormat(pData,GetNumSamples());
	 soundIO.DeallocateArray();
	 soundIO.SetPtr(pData);  //

   //	pWaveformData = ConvertToInternalFormat(pWaveformData,numWfSamples);
	  //	delete pTmp0;
	  //	delete pTmp1;
	//  delete [] pOData;     //ovo se mièe
	  }
	 #endif

	file.close();
  //	CalculateTrackTime();
	return pWaveFormat->chunkSize;
  }//
  else
	  throw SoundIOException("Can't open file. Check that the file exists!" );

  return 0;

}
#endif

#if defined _FAST_IO
/*
 unfortunately C++ file stream classes are unacceptably slow, so I'm using plain C when FAST_IO is defined
*/
//nova logika
unsigned SoundInput::OpenHeader(const std::string &fName)
{
	  // unsigned size;
	  FILE *pFile = fopen(fName.c_str(), "rb");
	  if(!pFile)throw SoundIOException("Can't open file. Check that the file exists!" );
	  fseek(pFile, 0, SEEK_END);
	  fileSize = ftell(pFile); //file size
	  fseek(pFile, 0, SEEK_SET);   //set the file pointer to the wav data

	 // pData = new char [fileSize];
	  if(fileSize <= maxBufferSize){ //put the entire file  in  a single buffer
		pData = soundIO.GetPtr(fileSize);
		soundIO.TransferOwnershipToCaller(false);
		fread(static_cast<char*>(pData), sizeof(char), fileSize/sizeof(char), pFile);
	  }else{
	  // put only the beginning of the file (up to minBufferSize) in a buffer --the rest of the file uses multiple buffers (below)
		pData = soundIO.GetPtr(minBufferSize);
		soundIO.TransferOwnershipToCaller(false);
		fread(static_cast<char*>(pData), sizeof(char), minBufferSize/sizeof(char), pFile);
	  }
	  // fclose(pFile);
	 pOData = pData;  //????
	 pWaveFormat = (PWAVE_HDR)(pData);

	 CheckHeader(pData);
	//convert to internal representation
	 SetFmtOffset();   //basically fills the WAVEFORMATEX
	 if(fmtOffset==0)throw SoundIOException("Invalid file format or invalid file");

	 SetDataOffset();  //sets the offset to the sampled data (accessible via GetData())
	 //check that we have a data chunk, otherwise throw an exception
	 if(dataOffset==0)throw SoundIOException("Invalid file format or invalid file");

	//copy the sound file header to a WAVE_HDR struct
	CopyHeader(pWaveFormat);
	pWaveFormat = &waveHdr;

	SetTypeSizeinBytes();
	fileName = fName;
	pData +=  dataOffset;  //move the pointer to the actual sound data

	 #if defined _USE_INTERNAL_FORMAT_
	 if(fileSize <= maxBufferSize){
		if(dataType != BIT_32) {
		//	const char *pTmp0 = pData-dataOffset;
 //	 convert the sound samples to floating point values
			pData = ConvertToInternalFormat(pData,GetNumSamples());
			soundIO.DeallocateArray();
			soundIO.SetPtr(pData);  //

			if(!pData)	throw SoundIOException("Insufficient memory while converting to internal format!");
		}

	 }else{
	  //  memBuffer.SetBufferSize(bufferSize);
		if(viewPortWidth>0) //????
			memBuffer.SetBufferSize(GetBufferSizeGivenViewPortWidth());
		memBuffer.InitAndFillBuffers(pFile, dataSize, dataOffset, dataType);
	 //	memBuffer.InitAndFillBuffers(pFile, dataSize, dataOffset, static_cast<EBytesPerSample>(dataType/8));
		multipleBuffers = true;
	 }
	 #endif
	fclose(pFile);
	return fileSize;
}
#endif
char* SoundIO::GetData() const {
return pData;
/*#if !defined _USE_INTERNAL_FORMAT_
	return pData ;
#else
	return pIData;
#endif  */
}
/**
this method prolly shouldn't be here
this method computes the buffer size(s) given the viewport width and is computed only
when we have multiple buffers (for large files). This is later used by WaveDisplayData class,
which prepares data for the waveform view.
*/
unsigned SoundInput::GetBufferSizeGivenViewPortWidth(){

	unsigned rep, rep_, rep__;

	int correction;

	CalculateNumSamples();

	const unsigned samplesPerChannel = numSamples / waveHdr.wfex.nChannels;

	const unsigned samplesPerPixel = static_cast<float>(samplesPerChannel)/viewPortWidth+1;

	const unsigned bS = memBuffer.GetBufferSize();// 1*1024*1024; //1MB  default

	DspUtil::SetRepeatValues(static_cast<float>(samplesPerPixel), viewPortWidth,
	samplesPerChannel, rep, rep_, rep__, correction);

	unsigned rem = bS % (( rep*samplesPerPixel + (samplesPerPixel+1)*rep_)) *  sizeof(float) ;
	return bS-rem;
}
/**
 it's easier to just set the names than o copy them
*/
void SoundIO::SetHdrSectionNames(){

	waveHdr.chunkId[0] = 'R';
	waveHdr.chunkId[1] = 'I';
	waveHdr.chunkId[2] = 'F';
	waveHdr.chunkId[3] = 'F';

	pWaveFormat->chunkSize = fileSize;//GetDataSize() + cHdrSize - cRiffChunkSize;
	waveHdr.format[0] = 'W';
	waveHdr.format[1] = 'A';
	waveHdr.format[2] = 'V';
	waveHdr.format[3] = 'E';

	waveHdr.subChunkId[0] = 'f';
	waveHdr.subChunkId[1] = 'm';
	waveHdr.subChunkId[2] = 't';
	waveHdr.subChunkId[3] = ' ';
	//pWaveFormat->subChunkSize = sizeof(WAVEFORMATEX);
}
/**
 this method copies the header data /e.g RIFF, WAVE to a WAVE_HDR structure
 the actual format (waveHdr.wfex) has already been copied so it's not copied here
*/
void SoundInput::CopyHeader(PWAVE_HDR pWF){

   SetHdrSectionNames();
   waveHdr.chunkSize = pWF->chunkSize;
   waveHdr.subChunkSize = pWF->subChunkSize;
}
 void SoundOutput::Save(const char *pFileName, EFileType eFileType){

   //	std::string fileExt = eFileType == WAVE ? "wav" : "raw";
	const unsigned int cRiffChunkSize = 8;
	dataOffset =  cHdrSize;
	//char *pData = (char*)pWaveFormat;
  //	const unsigned int factSize = 8;
  //	const unsigned int numSamples = GetNumSamples();
	fileSize = dataSize+cHdrSize;
	pWaveFormat->chunkSize = fileSize - cRiffChunkSize;
   //	fileSize +=  cHdrSize;
	try{
	ofstream outFile(pFileName,	ios::binary );
	if(outFile.is_open()){
		//if(eFileType==WAVE){
		//write header
		outFile.write((const char*)pWaveFormat, cHdrSize-cDataChunkSize);
		//data chunk
		outFile << 'd' << 'a' << 't' << 'a' ;
		outFile.write((const char*)&dataSize, sizeof(unsigned int));
	   //	}
		//write the samples
		outFile.write(pData, dataSize);
		outFile.close();
	}
	}catch(exception& e){
	// cout << e.what();
	 throw SoundIOException(e.what());
	}

 }
 unsigned SoundIO::GetNumSamples(){

  //	if(numSamples == 0)
  	CalculateNumSamples();
	return  numSamples;

 }
 void SoundIO::CalculateNumSamples(){

	 const unsigned bitsPerSample = GetDirection() == nsIODirection::OUTPUT ? BIT_32 : waveHdr.wfex.wBitsPerSample;
	 numSamples = GetDataSize()/(bitsPerSample>>3);
	 if(numSamples % 2 != 0) numSamples--;
 }

 //pSoundData->
