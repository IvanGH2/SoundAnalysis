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

#ifndef MemBufferH
#define MemBufferH

#include "vector"
#include "stdio.h"
#include "DspUtil.h"


//---------------------------------------------------------------------------
class MemBuffer
{

     FILE *pData;

	 unsigned dataSize, dataOffset, buffSize, lastBuffSize, outBuffSize;

	 unsigned bytesPerSample, currentBuff, currDataOffset, numBuffers;

	 std::vector<float*> vMemBuffer;

	 void ReadEverything32();

	 void ReadEverything();

     void SetToZero(char *pData, unsigned offset, unsigned len);

public:

	MemBuffer() : buffSize(1*1024*1024){}//empty constuctor

	MemBuffer(FILE *pData, unsigned dataSize, unsigned dataOffset, EncodingType eBps);

	void InitAndFillBuffers(FILE *pData, unsigned dataSize, unsigned dataOffset, EncodingType );

	void SetBufferSize(unsigned size) { buffSize = size; }

	unsigned GetBufferSize() const { return buffSize; }

	unsigned GetOutBufferSize() const { return outBuffSize; }

	unsigned GetLastBufferSize() const { return lastBuffSize; }

	unsigned GetNumBuffers() const { return numBuffers; }

	unsigned GetCurrentBuffer() const { return currentBuff; }

	const float *GetMemoryBuffer(unsigned which);

	const float *GetNextMemoryBuffer();

	std::vector<float*> *GetMemoryBuffers() { return  &vMemBuffer; }

};
#endif
