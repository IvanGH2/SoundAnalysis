//---------------------------------------------------------------------------

#ifndef WAVEDISPLAYDATA
#define WAVEDISPLAYDATA

//this defines a single wave point as used for graphical display
typedef struct {
unsigned int x;
unsigned int y;
} WAVE_POINT, *PWAVE_POINT;

typedef WAVE_POINT SAMPLE_POINT;

class WaveDisplayData
{
   unsigned int zoomFactor;   //1 , 2, 4, 8, 16....or 1/2,  1/4, 1/8, 1/16
   unsigned int viewPortWidth;
   unsigned int viewPortHeight;
   unsigned int totalSamples;
   unsigned int viewPortSamples;  //decreased or increased number of samples for every zoom in/out action
   unsigned int zoomSample;    //starting sample when zooming in/out
   unsigned int length;      //defines the length of viewPortData/viewPortPos
   unsigned int viewPortMidPoint;
   float* inputData;
   WavePoint* viewPortData;//defines Y axis coordinates (values)
   WavePoint* viewPortPos; //defines X axis coordinates

   bool zoomMax ;
   bool zoomMin ;

   unsigned int samplePageSize;
   unsigned int samplesForDisplay;

   int showEverySample;      //shows every Nth sample when the number of sample exceeds the viewport width
   int startSample ; //depending on where the cursor position is
   //when the number of samples is greater than the viewport width
   void __fastcall prepDataForDisplayMoreSamples();
   //when the number of samples is fewer than the viewport width
   void __fastcall prepDataForDisplayLessSamples();


   public:
   __fastcall WaveDisplayData(int Width, int Height)

   {
		this->viewPortHeight = Height;
		this->viewPortWidth = Width;
		this->viewPortMidPoint = Height/2 - 2;
		startSample = 1;
		samplesForDisplay = viewPortSamples;
   }
    unsigned int __fastcall getSamplePageSize(){ return samplePageSize; }
   void __fastcall setSamplePageSize(unsigned int samplePageSize) { this->samplePageSize = samplePageSize; }
   //float* __fastcall prepData(const float* input, unsigned int) ;
   void __fastcall setInput(float* in, unsigned int);  //input, file samples number
   WavePoint* __fastcall getSampleValues()
   {
		return viewPortData;
   }
   WavePoint* __fastcall getSamplePositions()
   {
		return viewPortPos;
   }
	enum DisplayMode { ZOOM, SCROLL };
	
   unsigned int __fastcall getPartialData(unsigned int viewPortPos);
   bool __fastcall zoomIn();   //zoomFactor is increased by a factor of 2, and as a consequnce, viewportSamples is decrease by the same factor
   bool __fastcall zoomOut();
   void __fastcall zoomPosition();
   //void __fastcall prepareSamplesForDisplay();
   //void __fastcall prepareSamplesForDisplayNew();
   unsigned int getViewPortWidth(){ return viewPortWidth;}
   unsigned int getViewPortSamples(){ return viewPortSamples;}

   void __fastcall prepDisplayData(DisplayMode);
   //void __fastcall scrollRight(unsigned int);
   unsigned int inline getSamplesForDisplay(){ return samplesForDisplay; }




};

#endif
