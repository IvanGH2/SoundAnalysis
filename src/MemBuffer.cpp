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

#include "MemBuffer.h"
//---------------------------------------------------------------------------

MemBuffer::MemBuffer(FILE *pData, unsigned size,unsigned offset, EncodingType eT): buffSize(1*1024*1024), currentBuff(0) {

	InitAndFillBuffers(pData, size, offset, eT);

}
void MemBuffer::InitAndFillBuffers(FILE *pData, unsigned size, unsigned offset, EncodingType eT){

	this->pData = pData;

	dataSize = size;

	dataOffset = offset;

	numBuffers = size / buffSize+1;

	bytesPerSample = eT / 8;

	eT == BIT_32 ?   ReadEverything32() : ReadEverything();

}
  void MemBuffer::ReadEverything32(){

	vMemBuffer.clear();

	size_t bSize = buffSize;

	fseek(this->pData, dataOffset, SEEK_SET);

	for(int i=0;i<numBuffers;i++){

		if(i==numBuffers-1)
			bSize = dataSize-((numBuffers-1)*buffSize);

			currDataOffset = i * bSize;

			fseek(pData, 0, SEEK_CUR);

			char * pBuff = new char[bSize];

			fread(pBuff, sizeof(char), bSize, pData);

			vMemBuffer.push_back((float*)pBuff);
		}
			lastBuffSize = bSize;
  }

  void MemBuffer::ReadEverything(){

	size_t bSize = buffSize;

	char *pBuff;

	char *bPtr = new char[buffSize];

	fseek(pData, dataOffset, SEEK_SET);  //move the pointer to the samples

	for(int i=0;i<numBuffers;i++){

		if(i==numBuffers-1){
			bSize = dataSize-((numBuffers-1)*buffSize);
		}
		fseek(pData, 0, SEEK_CUR);

		fread(bPtr, sizeof(char), bSize, pData);

		pBuff = (char*)DspUtil::ConvertToFloat(bPtr, bSize/bytesPerSample, bytesPerSample*8);//convertToFloat will create the array so need to create it here

		vMemBuffer.push_back((float*)pBuff);

		}
		//since we've converted the samples to floats, we need to adjust the buffer size appropriately
		const unsigned buffSizeAdjust =  sizeof(float) / bytesPerSample;

		lastBuffSize = bSize*buffSizeAdjust;

		outBuffSize  = buffSize*buffSizeAdjust;

		delete [] bPtr;
  }
const float *MemBuffer::GetMemoryBuffer(unsigned which) {

		if(numBuffers <= which) return 0;

		currentBuff = which;

		return vMemBuffer.at(which);

}
const float *MemBuffer::GetNextMemoryBuffer()  {

       currentBuff++;

       if(vMemBuffer.size()<currentBuff+1) return 0;

	   return vMemBuffer.at(currentBuff);

 }

#pragma package(smart_init)
