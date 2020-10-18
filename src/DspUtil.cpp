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

#include "dsputil.h"

void DspUtil::SetRepeatValues_(float repeatValue, unsigned &rep, unsigned &rep_ ){

//this is the interpolation/decimation LUT - the reason for using this LUT rather than doing a regular interpolation/decimation is improved speed and less memory
const unsigned repeatVec[31][2] = { {19, 1}, {14, 1}, {9, 1}, {8, 1}, {7, 1}, {6, 1}, {5, 1}, {4, 1}, {7,  2},  {3, 1},
{ 5, 2}, { 2, 1}, {7, 4}, {3, 2}, {5, 4}, {1, 1}, {4, 5}, {2, 3}, {4,  7},  {1, 2}, {2, 5},  
{ 1, 3}, { 2, 7}, {1, 4}, {1, 5}, {1, 6}, {1, 7}, {1, 8}, {1, 9}, {1, 14},  {1, 19} };

  const unsigned repeatVal  = repeatValue; 
  const float fRepDecimal   = repeatValue - repeatVal; //we're interested in the decimal part only 
 // unsigned repDecimal = floor( fRepDecimal * 1000 + 0.5f) ; 
   
  //selecting the appropriate LUT value
  int lutIndex = -1;
  if(fRepDecimal <= 0.03f ){
	rep = 1;
	rep_= 0;
   //	lutIndex = -1;
  }else if(fRepDecimal > 0.03f && fRepDecimal < 0.06f){
	lutIndex = 0;
  }else if(fRepDecimal < 0.075f){
	lutIndex = 1;
  }else if(fRepDecimal < 0.11f){
	lutIndex = 2;
  }else if(fRepDecimal < 0.125f){
	lutIndex = 3;
  }else if(fRepDecimal < 0.145f){
	lutIndex = 4;
  }else if(fRepDecimal < 0.165f){
	lutIndex = 5;
  }else if(fRepDecimal < 0.185f){
	lutIndex = 6;
  }else if(fRepDecimal < 0.215f){ //{4,1}
	lutIndex = 7;
  }else if(fRepDecimal < 0.235f){
	lutIndex = 8;
  }else if(fRepDecimal < 0.275f){ //{3,1}
	lutIndex = 9;
  }else if(fRepDecimal < 0.315f){
	lutIndex = 10;
  }else if(fRepDecimal < 0.35f){ // {2,1}
	lutIndex = 11;
  }else if(fRepDecimal < 0.38f){
	lutIndex = 12;
  }else if(fRepDecimal < 0.425f){ // {3,2}
	lutIndex = 13;
  }else if(fRepDecimal < 0.475f){
	lutIndex = 14;
  }else if(fRepDecimal < 0.525f){ // {1,1}
	lutIndex = 15;
  }else if(fRepDecimal < 0.575f){
	lutIndex = 16;
  }else if(fRepDecimal < 0.615f){ // {2,3}
	lutIndex = 17;
  }else if(fRepDecimal < 0.655f){
	lutIndex = 18;
  }else if(fRepDecimal < 0.69f){ // {1,2}
	lutIndex = 19;
  }else if(fRepDecimal < 0.725f){
	lutIndex = 20;
  }else if(fRepDecimal < 0.765f){ // {1,3}
	lutIndex = 21;
  }else if(fRepDecimal < 0.785f){
	lutIndex = 22;
  }else if(fRepDecimal < 0.81f){ //{1,4}
	lutIndex = 23;
  }else if(fRepDecimal < 0.825f){
	lutIndex = 24;
  }else if(fRepDecimal < 0.845f){
	lutIndex = 25;
  }else if(fRepDecimal < 0.865f){
	lutIndex = 26;
  }else if(fRepDecimal < 0.885f){ // {1,8}
	lutIndex = 27;
  }else if(fRepDecimal < 0.915f){ // {1,9}
	lutIndex = 28;
  }else if(fRepDecimal < 0.935f){ // {1,14}
	lutIndex = 29;
  }else if(fRepDecimal < 0.97f){ // {1, 19}
	lutIndex = 30;
  }else{
	//lutIndex = -1;
	rep = 2;
	rep_= 0;
  }
   
  if(lutIndex >= 0){
	rep  = repeatVec[lutIndex][0];
	rep_ = repeatVec[lutIndex][1];
  }

}

void DspUtil::SetRepeatValues(float repeatValue, unsigned &rep, unsigned &rep_ ){
 // const float repeatValue = ;
  const unsigned repeatVec[11][2] = { {19, 1}, {9, 1}, {4, 1},{2, 1},{3, 2},{1, 1},{2, 3},{1, 2},{1, 4},{1, 9}, {1, 19} };

 // const float realIntRatio[] = { 0.05f, 0.1f, 0.2f, 0.33f, 0.4f, 0.5f, 0.6f, 0.67f, 0.8f, 0.9f, 0.95f };

  const unsigned repeatVal = repeatValue;
//  const unsigned repeatVal_ = repeatValue+1;
  const float fRepDecimal =  repeatValue - repeatVal; //we're interested in the decimal part only
  unsigned repDecimal = floor( fRepDecimal * 10 + .5f) ;


 if(repDecimal > 0 && repDecimal <= sizeof(repeatVec)/(sizeof(unsigned)*2)){
	rep  = repeatVec[repDecimal][0];
	rep_ = repeatVec[repDecimal][1];
 }else{
	rep  = 1;//numSamples;
	rep_ = 0;
 }
}

void DspUtil::SetRepeatValues(float &repeatValue, unsigned numRepeats, unsigned totalRepeat, unsigned &rep0, unsigned &rep1, unsigned &rep2, int &correction ){

  const unsigned repeatVec[][2] = {  {19, 1}, {9, 1}, {4, 1},{2, 1},{3, 2},{1, 1},{2, 3},{1, 2},{1, 4},{1, 9}, {1, 19} };
  const float realIntRatio[] 	= { 0.05f, 0.1f, 0.2f, 0.33f, 0.4f, 0.5f, 0.6f, 0.67f, 0.8f, 0.9f, 0.95f };


  const unsigned repeatVal = (unsigned)repeatValue;

  //const unsigned repeatVal_ = (unsigned)repeatValue+1;

  const float fRepDecimal =  repeatValue - repeatVal; //we're interested in the decimal part only

  int repDecimal = (fRepDecimal>0.97f)  ? -1 : (fRepDecimal<0.03f) ? -2 : floor( fRepDecimal * 10 + .5f) ;


  if(repDecimal > 0 && repDecimal < sizeof(repeatVec)/(sizeof(unsigned)*2)){

		rep0 = repeatVec[repDecimal][0];

		rep1 = repeatVec[repDecimal][1];
	 //correction part

		const float realValue = repeatVal+realIntRatio[repDecimal];

		const unsigned calcValue = realValue * numRepeats;

		const unsigned diff = std::abs((int)(totalRepeat-calcValue));

		const float greater = fRepDecimal-realIntRatio[repDecimal];

		correction = greater>0 ? ++correction : --correction;

		rep2 = diff>0 ? totalRepeat/diff : 0;

	}else{

		rep0 = 1;

		rep1 = 0;

		if(repDecimal == -1)
			repeatValue +=1;
	}
}
unsigned DspUtil::NextPowerOfTwo(unsigned val){

	const unsigned powerTwo[] = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192 };
	const unsigned len = sizeof(powerTwo) / sizeof(unsigned);
	unsigned powOfTwo = 0;
	for(int i=0; i<len; i++){
		if(powerTwo[i]<val) continue;
		powOfTwo = powerTwo[i];
		break;
	}
	return powOfTwo;
}

/**
this method multiplies the input data by a particular window (e.g. Blackman ect)
and stores the result in Rx and Ix arrays respectively (the 2 arrays are needed for FFT)
*/
void DspUtil::WindowData(float *pData, float *Rx, float *Ix, unsigned len, EWindow eWnd){

	float *pWnd;

	pWnd = DspUtil::CreateWindowFunc(len, eWnd);
	for(int i=0; i<len;i++){

		 Rx[i] = pData[i] * pWnd[i];
		 Ix[i] = 0;
	}
}
float *DspUtil::CreateWindowFunc(unsigned len, EWindow eWnd, float &s0, float &s1){

   float *pWnd;

   switch(eWnd){
	case BLACKMAN:
		pWnd = DspUtil::CreateBlackmannWindow(len,s0,s1);
		break;
	case HAMMING:
		pWnd = DspUtil::CreateHammingWindow(len,s0,s1);
		break;
	case HANN:
		pWnd = DspUtil::CreateHannWindow(len,s0,s1);
		break;
	case RECTANGULAR:
		pWnd = 0;
   
   }
   return pWnd;
}
float *DspUtil::CreateWindowFunc(unsigned len, EWindow eWnd){


   float s0, s1;
   return DspUtil::CreateWindowFunc(len, eWnd, s0, s1);
}
float *DspUtil::CreateHannWindow(unsigned len, float &s0, float &s1){
	s0 = 1;
	s1 = 1;
	float *pWnd = new float[len];
	unsigned lenHalf = len>>1;
	//set
	pWnd[0] = 0;
	pWnd[lenHalf] = 1;
	for(int i=1,j=len-1; i<lenHalf; i++,j--){
		pWnd[i] = 0.5 * (1 - cos(PI2 * i/len));
		pWnd[j] = pWnd[i];

		s0 +=  (2 * pWnd[i]);
		s1 +=  (2 * (pWnd[i] * pWnd[i])) ;
	}
	return pWnd;
}
float *DspUtil::CreateHammingWindow(unsigned len, float &s0, float &s1){

	s0 = s1 = 1;
	float *pWnd = new float[len];
	unsigned lenHalf = len>>1;
	//set
	pWnd[0] = 0;
	pWnd[lenHalf] = 1;
	for(int i=1,j=len-1; i<lenHalf; i++,j--){
		pWnd[i] = 0.54 - 0.46 * cos(PI2 * i/len);
		pWnd[j] = pWnd[i];

		s0 +=  (2 * pWnd[i]);
		s1 +=  (2 * (pWnd[i] * pWnd[i])) ;
	}
	return pWnd;
}
float *DspUtil::CreateBlackmannWindow(unsigned len,float &s0, float &s1){

	s0 = s1 = 1;
	float *pWnd = new float[len];
	unsigned lenHalf = len>>1;
	//set
	pWnd[0] = 0;
	pWnd[lenHalf] = 1;
	for(int i=1,j=len-1; i<lenHalf; i++,j--){
		pWnd[i] = 1.0f * (0.42f - 0.5f * cos(PI2 * i/len) + 0.08 * cos( PI4 * i/len));
		pWnd[j] = pWnd[i];
		s0 +=  (2 * pWnd[i]);
		s1 +=  (2 * (pWnd[i] * pWnd[i]));
	}
	return pWnd;
}

float *DspUtil::CreateLoPassFilter(float cutoffFreq, unsigned fkLength, EWindow eWnd){

	float k = 0.6f;
	float filterVal, wndVal;
	const float fcc = PI2 * cutoffFreq;//cutoff is given as a fraction of the sampling rate
	int fkLenHalf =  fkLength>>1;
	float *pFilterKernel = new float[fkLength];
	float *pWnd = DspUtil::CreateWindowFunc(fkLength,eWnd);

	filterVal = sin(fcc * -fkLenHalf)/-fkLenHalf ;
   //	wndVal = pWnd[0] ;
	pFilterKernel[0] = filterVal * pWnd[0];
	filterVal = fcc ;
	pFilterKernel[fkLenHalf] = k * filterVal * pWnd[fkLenHalf];
   //	float sumTotal = pFilterKernel[0]+pFilterKernel[fkLenHalf];
	for(int i=1; i<fkLength; i++) {
		if(i!=fkLenHalf){
		  //	filterVal = sin(fcc * (i - fkLenHalf));
			filterVal = sin(fcc * (i - fkLenHalf)) /(i - fkLenHalf);
			pFilterKernel[i] = k * filterVal * pWnd[i];
		}

	}

	if(pWnd) delete [] pWnd;
	return pFilterKernel;
}
/*
  normally a low pass filter can be easily converted to a high pass using either
  spectral inversion or spectral reversal but this didn't work for me
*/
float *DspUtil::CreateHiPassFilter(float cutoffFreq, unsigned fkLength, EWindow eWnd){

	float filterVal, wndVal;
	unsigned fkLenHalf =  fkLength/2;

	float *pFilterKernel = DspUtil::CreateLoPassFilter( cutoffFreq, fkLength, eWnd);

	for(int i=0; i<fkLength;i++){
	   pFilterKernel[i] = -pFilterKernel[i];
	}
	pFilterKernel[fkLenHalf] += 1;
	return pFilterKernel;
}  
/**
  hi pass filter implementation using a band pass filter because spectral inversion /reversal didn't work
*/
float *DspUtil::CreateHighPassFilter( float cutoffFreq, unsigned fkLength, EWindow eWnd){

	float filterVal, wndVal;
	unsigned fkLenHalf =  fkLength/2;

	float *pFilterKernelLo = DspUtil::CreateLoPassFilter( cutoffFreq, fkLength, eWnd);
	float *pFilterKernelHi = DspUtil::CreateHiPassFilter( 0.5f, fkLength, eWnd);

	for (int i = 0; i < fkLength+1; i++)
		pFilterKernelLo[i] += pFilterKernelHi[i];
	for (int i = 0; i < fkLength+1; i++)
		pFilterKernelLo[i] = -pFilterKernelLo[i] ;//* 0.5;
	pFilterKernelLo[fkLenHalf] += 1;

	if(pFilterKernelHi) delete [] pFilterKernelHi;
	
	return  pFilterKernelLo;
}
float *DspUtil::CreateBandPassFilter( float cutoffFreqLo,float cutoffFreqHi, unsigned fkLength, EWindow eWnd){

	float filterVal, wndVal;
	unsigned fkLenHalf =  fkLength/2;

	float *pFilterKernelLo = DspUtil::CreateLoPassFilter( cutoffFreqLo, fkLength, eWnd);
	float *pFilterKernelHi = DspUtil::CreateHiPassFilter( cutoffFreqHi, fkLength, eWnd);

	for (int i = 0; i < fkLength+1; i++)
		pFilterKernelLo[i] += pFilterKernelHi[i];
	for (int i = 0; i < fkLength+1; i++)
		pFilterKernelLo[i] = -pFilterKernelLo[i] ;//* 0.5;
	pFilterKernelLo[fkLenHalf] += 1;

	if(pFilterKernelHi) delete [] pFilterKernelHi;
	
	return  pFilterKernelLo;
}
void DspUtil::ComputeMagnitudeAndPhase(const float* pLc, float *pMag, float *pPhase, unsigned len){

   //	const float fftNorm = wndSum0*wndSum0 != 0 ? wndSum0*wndSum0 : 1 ;
   //	const unsigned lenHalf = len >> 1;
	const float zeroCorrection = 0.000001f;

	float valAmp;

	for(int i=0,j=0; i<len; i+=2,j++){

		valAmp =  pLc[i]*pLc[i] + pLc[i+1]*pLc[i+1];
		if(valAmp==0) valAmp = zeroCorrection;
		pMag[j] 	= sqrt( valAmp );
		pPhase[j] 	= atan(pLc[i+1]/pLc[i]);
	}
}
float *DspUtil::GetAverageMagnitudes(std::vector<float*> &vMag, unsigned fftLen){

	const unsigned len = vMag.size(); //number of frames
	const unsigned halfFftLen = fftLen>>1;

	float *pAvg = new float[halfFftLen+1];
   //	std::vector<float> *vAvg = new ;

	memset(pAvg, sizeof(float)*(halfFftLen+1), 0);
	for(int i=0;i<len;i++){
		float *pCurrMag = vMag[i];
		for(int j=0;j<=halfFftLen;j++){
			pAvg[j] += pCurrMag[j];
		   }
	}

	for(int j=0;j<=halfFftLen;j++){
		pAvg[j] /= len;
	}
	return pAvg;
}

float DspUtil::GetPeakValue(float *pData, unsigned int len, unsigned &pos){
	float max = pData[0];
    SamplePoint soundSample;
    soundSample.position = 0;
	soundSample.value = 0;
    for(int i=0; i<len;i++){
		float currVal = pData[i];
        if(currVal>max){
			max = currVal;
			pos = i;
        }
	}
	return max;
}

/**
 this method normalizes a data series to a decibel value
*/
void DspUtil::NormalizeData(float *pData, unsigned int len, float decibelValue)
{
	if(decibelValue > 0)
		decibelValue = -decibelValue;
	unsigned pos;
	const float max = GetPeakValue(pData, len, pos);
	float normVal;

	float tmp = decibelValue / 20;
	tmp = pow(10, tmp);
	normVal = tmp * max;

     for(int i = 0; i < len; i++ )
     {
		pData[i] = pData[i] * normVal;
	 }
}
 /**
 this method converts integer PCM data (WAVE_FORMAT_PCM) to float (WAVE_FORMAT_IEEE_FLOAT)
 supported resolutions are 8 bit unsigned and 16 bit as specified in the WAVE format specification
 */
 float *DspUtil::ConvertToFloat(void *pData, int len, EncodingType encType)
{
	  const unsigned int cByteMax  =   255;
	  const unsigned int cByteHalf =   128;
	  const unsigned int cShortMax = 32768;

	  short *pSigned16;       //16 bit
	  unsigned char *pUnsigned8;
	  // NORMALLY WE WON'T CALL THIS METHOD IF WE ALREADY HAVE FLOATS SO JUST RETURN THE ORIGINAL
	  if (encType == BIT_32) return reinterpret_cast<float*>(pData);

	  float * pConverted;

	  pConverted = new (std::nothrow) float[len];
	  if( !pConverted ) return 0;

	  switch (encType) {
		 case BIT_8:   //UNSIGNED BYTE
			pUnsigned8 = reinterpret_cast<unsigned char*>(pData);
			for (int i = 0; i < len; i++) {
			pConverted[i] = pUnsigned8[i] == cByteMax  ? 1
						  : pUnsigned8[i] == cByteHalf ? 0
						  : ((float)(char)(pUnsigned8[i] - cByteHalf))/cByteHalf;
			}
			break;
		 case BIT_16:
			pSigned16 = reinterpret_cast<short*>(pData);
			for (int i = 0; i < len; i++) {
				pConverted[i] = static_cast<float>(pSigned16[i])/cShortMax;
			}
			break;

		 default:
		 break;
	  }
	  return pConverted;
}



