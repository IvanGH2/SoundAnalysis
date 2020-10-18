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
//---------------------------------------------------------------------------
#pragma hdrstop

#include "SysUtils.hpp"

#include "WaveDisplayData.h"
#include "DspUtil.h"
#include "sstream.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

void WaveDisplayData::DoCleanup(){}

void  WaveDisplayData::SetStartSample(unsigned viewPortPos, bool zoomOut){

	static int prevPos = -1;
	static int prevSample = -1;

	unsigned viewPortPosSample = viewPortPos == 0 ? 0 : prevPos != viewPortPos
													  ?	GetAbsoluteSample(viewPortPos, true) : prevSample;

	const float rPos = static_cast<float>(viewPortPos)/viewPortWidth;

	int startPos = viewPortPosSample-(rPos * viewPortSamples * waveData.numChannels);

	startSample =  startPos < 0 ? 0 : startPos;

	//for 2 -channels make sure that we start at the left channel sample
	if(waveData.numChannels==2 && startSample%2!=0) startSample--;

	prevZoomType = zoomOut;
	prevPos = viewPortPos;
	prevSample = viewPortPosSample;
}
void  WaveDisplayData::SetInputData(const WAVE_DISPLAY_DATA &rWaveInputData){

		waveData = rWaveInputData;
		totalSamples =  rWaveInputData.dataLen;
		viewPortSamples = totalSamples / waveData.numChannels;  //per channel
		prevViewPortSamples = viewPortSamples;

		SetViewPortMidPoint();
		PrepareDisplayData(WaveDisplayData::ZOOM, 0, false);
		InitZoomLevels();
}
void WaveDisplayData::SetViewPortMidPoint(){
	viewPortMidPoint =  viewPortHeight / 2;
}
void WaveDisplayData::InitZoomLevels(){

	unsigned vpSamples = totalSamples / waveData.numChannels;
	const unsigned maxLevel = cZoomInMaxLevel / 2;
	do{
	   zoomLevels.push_back(vpSamples);
	   vpSamples /= 2;
	}while( vpSamples >= maxLevel);
}
bool  WaveDisplayData::ZoomInFull(unsigned pos){

	  currZoomLevel = zoomLevels.size()-2;
	  return ZoomIn(pos);
}
bool  WaveDisplayData::ZoomOutFull(unsigned pos){

	  currZoomLevel = 1;

	  return ZoomOut(0);
}
bool  WaveDisplayData::ZoomOut(unsigned int zoomOutAtPos = 0){

	   prevViewPortSamples = viewPortSamples;
	   viewPortSamples = zoomLevels.at(--currZoomLevel);

	   PrepareDisplayData(WaveDisplayData::ZOOM, zoomOutAtPos, true);
	   return viewPortSamples * waveData.numChannels >= totalSamples ? true : false;
}
bool  WaveDisplayData::ZoomIn(unsigned int zoomInAtPos = 0)
{
	   prevViewPortSamples = viewPortSamples;
	   viewPortSamples = zoomLevels.at(++currZoomLevel);

	   PrepareDisplayData(WaveDisplayData::ZOOM, zoomInAtPos, false);
	   return viewPortSamples  <= cZoomInMaxLevel ? true : false;
}
void WaveDisplayData::PrepDataForDisplayLessSamples(/*const float* inputData, unsigned int dataLen, unsigned int viewLen*/){


	int i,j, k = 0, l, m = 0;
	WAVE_POINT point ;

	unsigned  pixelsPerSample, startBuffer, length,  widthExt = 40;

	int skip1, skip2, skip;

	float skipRate, fSkipFraction;

	int skipFraction;
	 //for 2 -channels make sure that we start at the left channel sample
	//if(waveData.numChannels==2 && startSample%2!=0) startSample--;

	if(!multipleBuffers){
			pInputData = &waveData.pData[startSample];

	}else{//multiple buffers
			startBuffer = startSample / samplesPerBuffer;
			pInputData = &dataBuffers->at(startBuffer)[startSample-startBuffer*samplesPerBuffer];
	}

	/* when the number of samples is larger than the viewport size, we should either
	  average through to the samples or skip some samples
	  if, however, the viewport width exceeds the number of samples, we should
	  skip some pixels
	  */

	pixelsPerSample =  (viewPortWidth+widthExt)  / viewPortSamples ;

	skipRate = static_cast<float>(viewPortWidth+widthExt) / viewPortSamples ;

	skip1 = skipRate;

	skip2 = skip1 + 1 ;

	unsigned skipFreq1, skipFreq2;

	DspUtil::SetRepeatValues(skipRate, skipFreq1, skipFreq2);

	if(skipFreq1==0) skipFreq1 = viewPortSamples;

	int sampleValue;

	int indexZero = 1;

	const unsigned samplesNum = viewPortSamples*waveData.numChannels ;

		#if defined(_DEBUG_) && defined(_WAVE_DISPLAY_DATA)
			ss <<  "skip:	" << skip1 << " skip freq	:" << skipFreq1 << newLine;
			debugger.Log(ss.str());
		#endif

	for(j = 0,i = -skip1*2;/* k < viewPortWidth;*/ m < samplesNum; ){

		for( k = i+skip1-indexZero, l = 0; l < skipFreq1; k += skip1, l++, j+= pixelsPerSample) {

				sampleValue = pInputData[m] * viewPortMidPoint;
				point.x = k;//i+k;
				point.y = viewPortMidPoint - sampleValue;
				point.startSample = startSample + m;
				point.endSample = startSample + m;
				point.sampleValue = pInputData[m];
				pViewPortData[m++] = point;

			  if(waveData.numChannels == 2){
				sampleValue = pInputData[m] * viewPortMidPoint;
				point.x = k;//i+k;
				point.y = viewPortMidPoint - sampleValue;
				point.startSample = startSample + m;    //relative sample pos (from the start of waveData.pData[startSample])
				point.endSample = startSample + m;
				point.sampleValue = pInputData[m];
				pViewPortData[m++] = point;
			  }
			#if defined(_DEBUG_) && defined(_WAVE_DISPLAY_DATA)
			if(point.y > 180){
				ss <<  "x:" << point.x << " y:" << point.y << newLine;
				debugger.Log(ss.str());
			}
			#endif
			}
		i =   k-skip1;

		for( k = i+skip2, l = 0; l < skipFreq2; k += skip2,l++, j+= pixelsPerSample) {

				sampleValue = pInputData[m] * viewPortMidPoint;
				point.x = k;//i+k;
				point.y = viewPortMidPoint - sampleValue;
				point.startSample = startSample + m;
				point.endSample = startSample + m;
				point.sampleValue = pInputData[m];
				pViewPortData[m++] = point;

			  if(waveData.numChannels == 2){
				sampleValue = pInputData[m] * viewPortMidPoint;
				point.x = k;//i+k;
				point.y = viewPortMidPoint - sampleValue;
				point.startSample = startSample + m;
				point.endSample = startSample + m;
				point.sampleValue = pInputData[m];
				pViewPortData[m++] = point;
			  }
		#if defined(_DEBUG_) && defined(_WAVE_DISPLAY_DATA)
			if(point.y > 180){
				ss <<  "x:" << point.x << " y:" << point.y << newLine;
				debugger.Log(ss.str());
			}
		#endif

		}
		i =  k-skip2  ;
		indexZero = 0;
		
		}

		startSample = startSample==0 ? startSample : startSample-1;
		sampleValue = pInputData[0] * viewPortMidPoint;
		point.x = -skip1;//i+k;
		point.y = viewPortMidPoint - sampleValue;
		pViewPortData[0] = point;

		sampleValue = pInputData[m] * viewPortMidPoint;
		point.x = viewPortWidth+skip1;//i+k;
		point.y = viewPortMidPoint - sampleValue;
		pViewPortData[m] = point;

		samplesForDisplay = m+1;

		pInputData = !multipleBuffers ? waveData.pData : dataBuffers->at(startBuffer);   //reset the pointer to the start of the input data
}

void WaveDisplayData::PrepareDisplayData(const WaveDisplayData::DisplayMode mode, unsigned int pos, bool zoomOut){

			if (viewPortSamples > viewPortWidth){
			  SetStartSample(pos, zoomOut);
			  PrepDataForDisplayMoreSamples();
			  eWaveformMode = SAMPLE_AVERAGING;
			 // samplesForDisplay = viewPortWidth * (2*waveData.numChannels) ;
			}else{
			  SetStartSample(pos, zoomOut);
			  PrepDataForDisplayLessSamples();
			  eWaveformMode = PIXEL_AVERAGING;
		  //	  samplesForDisplay = viewPortSamples  + (2*waveData.numChannels);
			}
}
void  WaveDisplayData::AdjustViewportSize(unsigned newWidth, unsigned newHeight){

		//if the viewport width hasn't changed, let's just adjust vertically
		if( newWidth==viewPortWidth && newHeight==viewPortHeight) return;

		if( newWidth==viewPortWidth){
			viewPortHeight = newHeight;
			AdjustViewportVertically(newHeight);
		}else{

        	viewPortWidth = newWidth;
			viewPortHeight = newHeight;
			SetViewPortMidPoint();
			PrepareDisplayData();
		}
}
 void  WaveDisplayData::AdjustViewportVertically(unsigned newHeight){

			  WAVE_POINT point;
			  int sampleValue;
			 // SetViewPortMidPoint();
			  viewPortMidPoint = newHeight/2 ;
			  //viewPortSamples < (viewPortWidth * waveData.numChannels) means that the number of samples to display is fewer than the viewPortWidth * number of channels
			  unsigned step = viewPortSamples < (viewPortWidth * waveData.numChannels) ? 1 : waveData.numChannels * 2;
			  for(int i=0, j=0; i < samplesForDisplay ; i+= step, j++){    //i+=2 za pos/neg

				sampleValue = pViewPortData[i].sampleValue * viewPortMidPoint;
				point.x = pViewPortData[i].x;   //j
				point.y = viewPortMidPoint - sampleValue;
				point.startSample =  pViewPortData[i].startSample;
				point.endSample =  pViewPortData[i].endSample;
				point.sampleValue = pViewPortData[i].sampleValue;
				pViewPortData[i] = point;
				//dodano  negati
				if(step > 1){
			  //	index = pViewPortData[i+1].soundIndex;
				sampleValue = pViewPortData[i+1].sampleValue * viewPortMidPoint;
				point.x = pViewPortData[i+1].x;  //j
				point.y = viewPortMidPoint - sampleValue;
				point.startSample =  pViewPortData[i+1].startSample;
				point.endSample =  pViewPortData[i+1].endSample;
				point.sampleValue = pViewPortData[i+1].sampleValue;
				pViewPortData[i+1] = point;
				}
				 //promjenit   pInputDataLeft
				if(waveData.numChannels == 2){
			  //	index = pViewPortData[i+2].soundIndex;
				sampleValue = pViewPortData[i+2].sampleValue * viewPortMidPoint;
				point.x = pViewPortData[i+2].x;
				point.y = viewPortMidPoint - sampleValue;
				point.startSample =  pViewPortData[i+2].startSample;
				point.endSample =  pViewPortData[i+2].endSample;
				point.sampleValue = pViewPortData[i+2].sampleValue;
				pViewPortData[i+2] = point; //right channel - pos
				if(step > 1){
			   //	index = pViewPortData[i+3].soundIndex;
				sampleValue = pViewPortData[i+3].sampleValue * viewPortMidPoint;
				point.x = pViewPortData[i+3].x;
				point.y = viewPortMidPoint - sampleValue;
				point.startSample =  pViewPortData[i+3].startSample;
				point.endSample =  pViewPortData[i+3].endSample;
				point.sampleValue = pViewPortData[i+3].sampleValue;
				pViewPortData[i+3] = point; //right channel - neg
			}
		}
	}
 }
   void WaveDisplayData::PrepDataForDisplayMoreSamples(){

	 WAVE_POINT point ;
	 unsigned pixelsPerSample;
	 unsigned sampPerPixel;
	 int sampleValue;
	 sampPerPixel = viewPortSamples / viewPortWidth;   //per track , not per channel

	 float currVal0, currVal1 ;
	 float samplesPeak0, samplesPeak1, samplesNegPeak0, samplesNegPeak1;

	 int i,j=0, k=0, l=0, ii=1, jj=0;

				#if defined(_DEBUG_) && defined(_WAVE_DISPLAY_DATA)
					ss <<  "prepDataForDisplayMoreSamples"  << newLine;
					debugger.Log(ss.str());
				#endif
	 unsigned rep, rep_, rep__;
	 int correction = 0, samplesPerPixel = sampPerPixel;
	 float sPerPixel = static_cast<float>(viewPortSamples)/viewPortWidth;

	 DspUtil::SetRepeatValues(static_cast<float>(sPerPixel), viewPortWidth, viewPortSamples, rep, rep_, rep__, correction);
	 if(rep_==0){
		rep= viewPortSamples;
		samplesPerPixel = sPerPixel;
		}

		bool leave = false, leave_=true;

		unsigned maxIndex, maxIndexLast;

		unsigned relStartSample, relEndSample, diffStartSample = 0;
		unsigned startBuffer = 1;
		const unsigned numBuffers = this->numBuffers;
		unsigned lastBuffer = numBuffers;
		const unsigned endSample = startSample + viewPortSamples * waveData.numChannels;

		if(!multipleBuffers){
			pInputData = &waveData.pData[startSample];
			maxIndex = waveData.numChannels==1 ? viewPortSamples-(samplesPerPixel+1):
				viewPortSamples*2-2*(samplesPerPixel+2)+1;
		}else{//multiple buffers
			startBuffer = startSample / samplesPerBuffer + 1;
			lastBuffer  = endSample / samplesPerBuffer + 1;

			relStartSample  = startSample>0 ? startSample-((startBuffer-1) * samplesPerBuffer) : 0;
			relEndSample    = endSample-((lastBuffer-1) * samplesPerBuffer) ;
			diffStartSample = startSample - relStartSample;

            if(startBuffer == lastBuffer){

				maxIndex = waveData.numChannels==1 ?  relEndSample -(samplesPerPixel+1):
														relEndSample-(2*samplesPerPixel+2)+1;
				maxIndexLast = maxIndex;
			}else{

				if( waveData.numChannels==1 ){

					maxIndex 	 = samplesPerBuffer -(samplesPerPixel+1);
					maxIndexLast = relEndSample -(samplesPerPixel+1);

				}else{
					const int sampleOffset = relEndSample-(2*samplesPerPixel+2)+1;
					maxIndex = samplesPerBuffer-(2*samplesPerPixel+2)+1;
					maxIndexLast = sampleOffset<0 ? relEndSample : relEndSample-(2*samplesPerPixel+2)+1;
				}
			}
		}
		if(waveData.numChannels==2) samplesPerPixel *= 2;


		unsigned prevSample=0;// = startSample;
		unsigned beginSample =  multipleBuffers ? relStartSample : 0;

		for(int m=startBuffer;m<=lastBuffer;m++){

				if(multipleBuffers){
					pInputData = dataBuffers->at(m-1);
					if(m  > startBuffer) beginSample = 0;
				  //	pInputData = m == startBuffer ? &dataBuffers->at(m-1)[startSample] : dataBuffers->at(m-1);
					if(m == lastBuffer && startBuffer < lastBuffer)//handling the last buffer
						maxIndex = maxIndexLast;

					   prevSample += k;
				}else prevSample = startSample;

				if(waveData.numChannels == 1){ //mono

				for( k=beginSample;  k<maxIndex; j++, l+=2, k+=samplesPerPixel){

				samplesPeak0    = 0;
				samplesNegPeak0 = 0;

				for(i=0; i<samplesPerPixel; i++){
					currVal0 = pInputData[i+k];
					if(currVal0 >= 0){
					   if(currVal0 > samplesPeak0){
							samplesPeak0 = currVal0;
							}
					}else{
					   if(currVal0 < samplesNegPeak0){
							samplesNegPeak0 = currVal0;

					   }
					}
				}
				//populating waveform display data -- positive samples
				sampleValue = samplesPeak0 * viewPortMidPoint;
				point.x = j;
				point.y = viewPortMidPoint - sampleValue;
				point.sampleValue = samplesPeak0;
				point.startSample = k+prevSample+diffStartSample;
				point.endSample = point.startSample+i;

				pViewPortData[l] = point;

				#if defined(_DEBUG_) && defined(_WAVE_DISPLAY_DATA)
					ss <<  "k:" << i+k << " l:" << l << newLine;
					debugger.Log(ss.str());
				#endif
				//populating waveform display data -- negative samples
				sampleValue = samplesNegPeak0 * viewPortMidPoint;
				point.x = j;
				point.y = viewPortMidPoint - sampleValue;
				point.sampleValue = samplesNegPeak0;

				pViewPortData[l+1] = point;

				if(rep_){
					if((ii)%rep==0 && leave_) {
						samplesPerPixel = sampPerPixel+1;
						leave=true;
						leave_=false;
						ii=0;
					}else if (jj%rep_==0 && leave){
						samplesPerPixel = sampPerPixel;
						leave=false;
						leave_=true;
						jj=0;
					}
				 if(!leave)  ii++;
				 if(!leave_) jj++;
				 if(rep__>0 && j%rep__ ==0) samplesPerPixel += correction;
					}
				 }

				}else{

				for( k=beginSample; k<maxIndex; j++,l+=4, k+=samplesPerPixel){ //samplesPerPixel for both channels   1 kanal 10000 sampples   200 vw  50 spp   2 kanala 5000 / 200 =  25 sample per pixel per channel
					try{
					samplesPeak0 	= 0;
					samplesNegPeak0 = 0;
					samplesPeak1 	= 0;
					samplesNegPeak1 = 0;

					for(i=0; i<samplesPerPixel; i+=2) {
						currVal0 = pInputData[i+k];
						currVal1 = pInputData[i+k+1];
				//left channel
						if(currVal0 >= 0){
							if(currVal0 > samplesPeak0){
								samplesPeak0 = currVal0;
								//soundIndexPos0 = i+k;
							}
						}else{
							if(currVal0 < samplesNegPeak0){
								samplesNegPeak0 = currVal0;

							}
						}
				//right channel
				if(currVal1 >= 0){
					if(currVal1 > samplesPeak1){
						samplesPeak1 = currVal1;

						}
				}else{
					if(currVal1 < samplesNegPeak1){
						samplesNegPeak1 = currVal1;

						}
					}
				}//end inner for
				//populating waveform display data -  left channel positive
				sampleValue = samplesPeak0 * viewPortMidPoint;
				point.x = j;
				point.y = viewPortMidPoint - sampleValue;
				point.sampleValue = samplesPeak0;
				point.startSample = k+prevSample;//soundIndexPos0;
				point.endSample   = point.startSample+i;
				pViewPortData[l]  = point;

				//left channel neg
				sampleValue = samplesNegPeak0 * viewPortMidPoint;
                point.x = j;
				point.y = viewPortMidPoint - sampleValue;
				point.sampleValue = samplesNegPeak0;
				pViewPortData[l+1] = point;
               
				//right channel pos
				sampleValue = samplesPeak1 * viewPortMidPoint;
				point.x = j;
				point.y = viewPortMidPoint - sampleValue;
				point.sampleValue = samplesPeak1;
				pViewPortData[l+2] = point;
                //right channel neg
				sampleValue = samplesNegPeak1 * viewPortMidPoint;
                point.x = j;
				point.y = viewPortMidPoint - sampleValue;
				point.sampleValue = samplesNegPeak1;
				pViewPortData[l+3] = point;
				}catch(...){
						maxIndex=maxIndex;
					}

				if(rep_){
					if((ii)%rep==0 && leave_) {
						samplesPerPixel = sampPerPixel*2+2;
						leave=true;
						leave_=false;
						ii=0;
						}
					else if (jj%rep_==0 && leave){
						samplesPerPixel = sampPerPixel*2;
						leave=false;
						leave_=true;
						jj=0;
					}
					if(!leave)  ii++;
					if(!leave_) jj++;
					if(rep__>0 && j%rep__ ==0) samplesPerPixel += correction;
				 }
						}
					} //end stereo
				}//end outer loop ->numBuffers
				samplesForDisplay = waveData.numChannels ==2 ? (l-4):(l-2);

				pInputData = !multipleBuffers ? waveData.pData : dataBuffers->at(startBuffer-1);
	   }



