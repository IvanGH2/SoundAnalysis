//---------------------------------------------------------------------------


#pragma hdrstop

#include "WaveDisplayData.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

unsigned int __fastcall WaveDisplayData::getPartialData(unsigned int viewPortPos){

	float startRelativePos = (float)viewPortPos / viewPortWidth; //viewPortPos = 1

	unsigned int startIndex = 0;

	if(startRelativePos <= 0.25f ){

		startIndex = 0;

	}else if(startRelativePos >= 0.75f ){

		startIndex = viewPortSamples * 0.5f;

	}else{
		//startIndex = totalSamples * (startRelativePos - (0.25 - (0.25 * (float)viewPortSamples/totalSamples)));
		startIndex = totalSamples * (startRelativePos - (0.25 * (float)viewPortSamples/totalSamples));
		//startIndex = viewPortSamples * (startRelativePos - 0.25);
	}
	startSample = startIndex;
	return startIndex;

}
 void __fastcall WaveDisplayData::setInput(float* in, unsigned int size)
   {
	 inputData = in;
	 totalSamples = size;
	 viewPortSamples = totalSamples;
   }
   bool __fastcall WaveDisplayData::zoomOut()
   {
	  // zoomFactor /= 2;
	   viewPortSamples *= 2;
	   samplePageSize *= 2;
	   return ( viewPortSamples >= totalSamples )? true : false;

   }
 bool __fastcall WaveDisplayData::zoomIn()
   {

	  // zoomFactor *= 2;
	   viewPortSamples /= 2;
	  if( viewPortSamples % 2 != 0 )
				  viewPortSamples++;

	   samplePageSize /= 2;
	   //samplesPerPixel = viewPortSamples / viewPortWidth;
		//if( viewPortSamples <= 2 ) zoomMax = true;
		return viewPortSamples <= 1 ? true : false;
   }

   void __fastcall WaveDisplayData::prepDataForDisplayMoreSamples(){

	//float* data = { 0,0.1, 0.15,0.3,0.4,0.6,0.9,0.65,0.5,0.4,0.2,0.04,-0.03,-0.2,-0.3,-0.4,-0.7,-0.88,-0.6,-0.4,-0.2};
	//float * calcData = new float[11];
	Form1->Caption = IntToStr(++Form1->fw);
	float*	inputData = &this->inputData[startSample];
	int i,j,k,l;
	   WavePoint point ;
	   unsigned int pixelsPerSample;
	   unsigned int samplesPerPixel;
	   unsigned int skip1, skip2, skip;
	   unsigned int length;
	   unsigned int remainder;

	   float skipRate;
	   float fSkipFraction;
	   int skipFraction;


	   /* when the number of samples is larger than the viewport size, we should either
	   average through to the samples or skip some samples
	   if, however, the viewport width exceeds the number of samples, we should
	   skip some pixels
	   */


	   samplesPerPixel = viewPortSamples / viewPortWidth;
	   remainder =	viewPortSamples % viewPortWidth;
	   /*skipRate --> tells us how many samples (or pixels), on average, we should keep (every skipRate-th sample.
	   Since skipRate is usually not an integer , we will take the integers right before and after skipRate
	   , e.g. for skipRate=2.4 we'll take 2 and 3 for skip1 and skip2 respectively. These two integers will have their
	   frequency values (that is, skip rate), denoted as skipFreq1 and skipFreq2 (i.e. the number of repetitions for
	   skip1 and skip2 respectively).
				 */
	   skipRate = (float)viewPortSamples / viewPortWidth;
	   pixelsPerSample = 1;
	   length = viewPortWidth;


	   skip1 = (unsigned int)skipRate;
	   skip2 = skip1 + 1 ;
	   fSkipFraction =  skipRate - skip1; //we're interested in the decimal part only
	   skipFraction = floor( fSkipFraction * 10 + .5) ;  //truncating to just 1 decimal place for simplicity and casting to integer


	   //depending on the value of skipFraction, the skip frequency leans more towards skipFreq1 or skipFreq2
	   //for example, if skipRate = 2.1, skipFreq1 will take 9 and skipFreq2 will take 1
	   unsigned int skipFreq[9][2] = { {9, 1}, {4, 1},{2, 1},{3, 2},{1, 1},{2, 3},{1, 2},{1, 4},{1, 9} };
	   unsigned int skipFreq1, skipFreq2;


	   if(remainder!=0 && skipFraction > 0){
	   skipFreq1 = skipFreq[skipFraction-1][0];
	   skipFreq2 = skipFreq[skipFraction-1][1];
       }else{
	   skipFreq1 = viewPortSamples / skip1;
	   skipFreq2 = 0;
	   }


	   viewPortData = new WavePoint[length];
	   viewPortPos  = new WavePoint[length];
	   //int useEveryPixel = samplesPerPixel - 1;
	   int m=0;
	   int indexZero = 1;
	   if(viewPortData)
	   {
		   skip = skip1;
		   for(j = 0,i = 0; j < length;  )
		   {
			  try{
			  int sampleValue;
			  k=0;
			  //use the first frequency
			  for( k = i+skip1-indexZero, l = 0; l < skipFreq1; k += skip1,l++, j+= pixelsPerSample) {


			  //if(fabs(inputData[k]) < 0.01f) inputData[k]=0;
			  sampleValue = inputData[k] * viewPortMidPoint;


			 // sampleValue =   (( inputData[k-3]+inputData[k-2]+inputData[k-1]+inputData[k]+inputData[k+1]+inputData[k+2]+inputData[k+3])/7)  * viewPortMidPoint;

			  point.x = k;
			  point.y = inputData[k]*32768;//viewPortMidPoint;
			  viewPortPos[j] = point;
			  point.x = j;
			  point.y = viewPortMidPoint - sampleValue;

			  viewPortData[j] = point;
			  //calcData[m++] = inputData[k];
			   //i = i + k;


			  }
			i =  k-skip1 ;

			   for( k = i+skip2, l = 0; l < skipFreq2; k += skip2,l++, j+= pixelsPerSample) {

			  sampleValue = inputData[k] * viewPortMidPoint;
			 // sampleValue =   (( inputData[k-3]+inputData[k-2]+inputData[k-1]+inputData[k]+inputData[k+1]+inputData[k+2]+inputData[k+3])/7)  * viewPortMidPoint;
			  point.x = k;
			  point.y = inputData[k]*32768;//viewPortMidPoint;
			  viewPortPos[j] = point;

			  point.x = j;
			  point.y = viewPortMidPoint - sampleValue;

			  viewPortData[j] = point;
			//  calcData[m++] = inputData[k];
			  //i = i + k;
				}
				 i =  k-skip2;
				 indexZero = 0;
			  }catch(Exception& e )
			  {
				   throw e;
			  }
			  //m++;
		   }
		  // delete [] viewPortData;
		  // delete [] viewPortPos;
		  //return calcData;
	   }
}

void __fastcall WaveDisplayData::prepDataDisplayLessSamples(/*const float* inputData, unsigned int dataLen, unsigned int viewLen*/){

	 Form1->Caption = IntToStr(--Form1->fw);
	int i,j,k,l;
    WavePoint point ;

    unsigned int pixelsPerSample;

    unsigned int samplesPerPixel;

    unsigned int skip1, skip2, skip;

    unsigned int length;

	unsigned int remainder;

    float skipRate;

    float fSkipFraction;

    int skipFraction;

	/* when the number of samples is larger than the viewport size, we should either

	  average through to the samples or skip some samples

	  if, however, the viewport width exceeds the number of samples, we should

      skip some pixels

      */
	   // samplesPerPixel = viewPortSamples / viewPortWidth;
	   remainder =	 viewPortWidth % viewPortSamples;


		pixelsPerSample =  viewPortWidth / viewPortSamples ;

		skipRate = (float)viewPortWidth / viewPortSamples ;

        samplesPerPixel = 1;

		length = viewPortSamples;



    skip1 = (unsigned int)skipRate;

    skip2 = skip1 + 1 ;

    fSkipFraction =  skipRate - skip1; //we're interested in the decimal part only

	skipFraction = floor( fSkipFraction * 10 + .5) ;  //truncating to just 1 decimal place for simplicity and casting to integer

    //depending on the value of skipFraction, the skip frequency leans more towards skipFreq1 or skipFreq2

    //for example, if skipRate = 2.1, skipFreq1 will take 9 and skipFreq2 will take 1

	unsigned int skipFreq[9][2] = { {9, 1}, {4, 1},{2, 1},{3, 2},{1, 1},{2, 3},{1, 2},{1, 4},{1, 9} };

	unsigned int skipFreq1, skipFreq2;

	if(remainder!=0){

		skipFreq1 = skipFreq[skipFraction-1][0];
		skipFreq2 = skipFreq[skipFraction-1][1];
	}else{

		skipFreq1 = viewPortSamples / skip1;

        skipFreq2 = 0;

	}
	int m=0;
	int indexZero = 1;
	skip = skip1;
	
	 viewPortData = new WavePoint[viewPortSamples];
	 viewPortPos  = new WavePoint[viewPortSamples];
	k = 0;
	for(j = 0,i = 0; k < viewPortWidth;  )
	{
		int sampleValue;
		for( k = i+skip1-indexZero, l = 0; l < skipFreq1; k += skip1,l++, j+= pixelsPerSample) {

			  sampleValue = inputData[m] * viewPortMidPoint;
			  point.x = m;//i+k;
			  point.y = inputData[m] * 32768;//viewPortMidPoint;
			  viewPortPos[m] = point;
			  point.x = k;//i+k;
			  point.y = viewPortMidPoint - sampleValue;

			  viewPortData[m++] = point;
				//calcData[m] = inputData[m];
				//pixelPos[m++] = k;

		}

        i =   k-skip1 ;

		for( k = i+skip2, l = 0; l < skipFreq2; k += skip2,l++, j+= pixelsPerSample) {

			  sampleValue = inputData[m] * viewPortMidPoint;
			  //sampleValue = inputData[i+l] * viewPortMidPoint;
			  point.x = m;// i+k;
			  point.y = inputData[m] * 32768;//viewPortMidPoint;
			  viewPortPos[m] = point;
			  point.x = k;//i+k;
			  point.y = viewPortMidPoint - sampleValue;
			  viewPortData[m++] = point;
			// calcData[m] = inputData[m];
			 //pixelPos[m++] = k;
		}
		i =   k-skip2  ;
		indexZero = 0;


		}
		//correction because the actual samples taken is fewer than viewPortSamples number
		viewPortSamples = m;

}
