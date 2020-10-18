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

#ifndef WAVEDISPLAYDATA
#define WAVEDISPLAYDATA

#include <exception>
#include <vector>
#include <stack>
#include "math.h"
#include "Util.h"
#include "Debug.h"
#include "DataBuffersProxy.h"
//this defines a single wave point as used for graphical display (x, y) plus an index into the original sound array
typedef struct {
	int x;
	int y;
	float sampleValue; //sample value
	unsigned startSample; //index (relative) into the original sound array
	unsigned endSample;
} WAVE_POINT, *PWAVE_POINT;

typedef WAVE_POINT SAMPLE_POINT;


class WaveDisplayData : public DataBuffersProxy
{
   unsigned zoomFactor;   //1 , 2, 4, 8, 16....or 1/2,  1/4, 1/8, 1/16
   unsigned viewPortWidth;
   unsigned viewPortHeight;
   unsigned totalSamples;
   unsigned viewPortSamples, prevViewPortSamples;  //decreasing or increasing number of samples per channel for every zoom in/out action
   unsigned zoomSample;    //starting sample when zooming in/out
   unsigned length;      //defines the length of viewPortData/viewPortPos
   unsigned viewPortMidPoint;
   unsigned currZoomLevel;
   //input

   #if defined (_DEBUG_) && defined(_WAVE_DISPLAY_DATA)
	Debugger &debugger;
	std::stringstream ss;
	char newLine ;
   #endif
   
   float *pInputData; //pointer to the current sound sample
   //std::vector<float*> *dataBuffers;
   std::vector<unsigned int> zoomLevels;//stores viewPortSamples for each zoom level
 //  std::stack<unsigned int>  zoomLevelPos; //stores starting positions/samples

   WAVE_DISPLAY_DATA waveData;  //input information
   //output -- produced by this class and used by TWaveDisplayContainer
  // PWAVE_POINT pViewPortData;//defines Y axis coordinates (values)
   WAVE_POINT  pViewPortData[16*1000]; //8k  WAVE_POINT structures * 20bytes each

   EWaveformDisplayMode eWaveformMode;
   bool zoomMax ;
   bool zoomMin ;
   bool prevZoomType; //zoomIn or zoomOut

   unsigned samplesForDisplay;
  // unsigned bufferSize, lastBufferSize, samplesPerBuffer, samplesPerLastBuffer, numBuffers;

   float deltaY;
   enum DisplayMode { ZOOM, SCROLL };
   int showEverySample, startSample;      //shows every Nth sample when the number of sample exceeds the viewport width
   const unsigned cZoomInMaxLevel;
   //when the number of samples is greater than the viewport width
   void PrepDataForDisplayMoreSamples();
   //when the number of samples is fewer than the viewport width
   void PrepDataForDisplayLessSamples();

   void InitZoomLevels();

   void DoCleanup();

   void SetViewPortMidPoint();

   void PrepareDisplayData(const WaveDisplayData::DisplayMode, unsigned int pos, bool zoomOut);

   void WaveDisplayData::PrepareDisplayData(){

	if (viewPortSamples > viewPortWidth){
		PrepDataForDisplayMoreSamples();
		eWaveformMode = SAMPLE_AVERAGING;
	}else{
		PrepDataForDisplayLessSamples();
		eWaveformMode = PIXEL_AVERAGING;
	}
   }
   public:
   #if defined(_DEBUG_) && defined(_WAVE_DISPLAY_DATA)
   WaveDisplayData(int width, int height)
  : debugger(Debugger::GetInstance()), newLine('\n'), cZoomInMaxLevel(64)
   {

		Init(width,height);
		ss <<  "WaveDisplayData" << newLine ;
		debugger.Log(ss.str());
   }
   #else
	WaveDisplayData(int width, int height) : cZoomInMaxLevel(64)
   {

		Init(width, height);
   }
   #endif
   void Init(int width, int height){

		viewPortHeight = height;
		viewPortWidth = width;
		viewPortMidPoint = height/2 ;
		startSample = 0;
		pInputData = NULL;
		currZoomLevel = 0;
		multipleBuffers = false;
	  	numBuffers = 1;
	  //	zoomLevelPos.push(0);

   }
   ~WaveDisplayData(){
	  DoCleanup();
   }

 //  const std::vector<float*> *GetDataBuffers() const { return dataBuffers; }

   EWaveformDisplayMode GetWaveformDisplayMode() const { return eWaveformMode; }
   // returns the pointer to the samples that are currently in the viewport
   float *GetCurrentData() const { return &pInputData[startSample]; }
	// returns a pointer to the samples (at offset 0)
   float *GetData() const { return waveData.pData; }

   unsigned GetCurrentZoomLevel() const { return (totalSamples/waveData.numChannels)/viewPortSamples; }

   void  SetInputData(const WAVE_DISPLAY_DATA &rWaveInputData);

   const WAVE_DISPLAY_DATA &GetInputData() const { return waveData; }

  // PWAVE_POINT GetRecalcDisplayValues() const { return pRecalcViewPortDataY; }

   const PWAVE_POINT  GetSampleValues() const { return (const PWAVE_POINT)pViewPortData; }

   int GetStartSample() const { return startSample; }

   int GetEndSample() const { return startSample + GetViewPortSamplesTotal(); }

   void SetStartSample(unsigned int viewPortPos, bool zoomOut);

   unsigned GetAbsoluteSample(unsigned pos, bool startSample) const {

		 const unsigned rPos = static_cast<float>(pos)/viewPortWidth * samplesForDisplay;
		 const unsigned index = viewPortSamples <= viewPortWidth ?  GetSamplePosition(pos) :
																	rPos ;//* waveData.numChannels*2; //numSamples;
		 return startSample ? pViewPortData[index].startSample : pViewPortData[index].endSample;
   }
   unsigned GetSamplePosition(int pos){

	int minDistance = abs((int)(pos-pViewPortData[0].x));
	unsigned minIndex = 0;
	for(int i=1;i<samplesForDisplay;i++){
		unsigned distance =  abs((int)(pos-pViewPortData[i].x));
		  if(distance<minDistance){
			minDistance = distance;
			minIndex = i;
		  }
	}
	return minIndex;
   }
   bool  ZoomIn(unsigned int );   //zoomFactor is increased by a factor of 2, and as a consequnce, viewportSamples is decrease by the same factor

   bool  ZoomOut(unsigned int);

   bool  ZoomOutFull(unsigned int zoomOutAtPos = 0);

   bool  ZoomInFull(unsigned int zoomInAtPos = 0);

   unsigned GetViewPortWidth() const { return viewPortWidth;}

   void SetViewPortWidth(unsigned v) {  viewPortWidth = v; }

   unsigned GetViewPortHeight() const { return viewPortHeight; }

   void SetViewPortHeight(unsigned h) {  viewPortHeight = h; /*SetViewPortMidPoint();*/ }

   unsigned GetViewPortSamples() const { return viewPortSamples;}  //returns samples per channel

   unsigned GetViewPortSamplesTotal()const { return viewPortSamples * waveData.numChannels;}  //returns the total number of samples

   //returns the total number of samples for display /the approximate formula is viewportWidth*numChannels*linePerPixel
   //linePerPixel is set to 2 when when the number of samples is greater than the viewport width because we separately average positive and negative samples
   unsigned int  GetSamplesForDisplay() const { return samplesForDisplay; }

   void  AdjustViewportVertically(unsigned newHeight);

   void  AdjustViewportSize(unsigned newWidth, unsigned newHeight);

};

#endif

