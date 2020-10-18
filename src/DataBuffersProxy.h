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
	
#ifndef DataBuffersProxyH
#define DataBuffersProxyH

#include "vector"

class DataBuffersProxy {

	protected:

	bool multipleBuffers;

	unsigned numBuffers, bufferSize, lastBufferSize, samplesPerBuffer, samplesPerLastBuffer;

	std::vector<float*> * dataBuffers;

  	public:
     /*
   SetDataBuffers should only be invoked when we have multiple input (sound) buffers
   vDataBuff - vector containing pointers to sound buffers
   buffSize - size in bytes of an individual buffer(except the last one)
   lastBuffSize - size of the last buffer, which is usually lower then buffSize (because we often have some remaining samples that don't fit buffSize exactly)
   */
	void SetDataBuffers(std::vector<float*> * vDataBuff, unsigned buffSize, unsigned lastBuffSize){

		dataBuffers 		 = vDataBuff;
		numBuffers  		 = vDataBuff->size();
		bufferSize  		 = buffSize;
		lastBufferSize	     = lastBuffSize;
		samplesPerBuffer 	 = buffSize/sizeof(float);
		samplesPerLastBuffer = lastBufferSize/sizeof(float);
		multipleBuffers 	 = true;
	}
};

#endif