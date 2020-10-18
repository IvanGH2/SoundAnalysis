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

#ifndef DSPUTIL_H
#define DSPUTIL_H


#include <typeinfo>
#include <stdexcept>
#include "stdio.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "math.h"
#include "Util.h"
using namespace std;
using namespace WindowFunc;



#define PI 3.14159265359
#define PI2 2 * PI //6.2831853072
#define PI4 4 * PI //12.566370614


enum EncodingType { BIT_8 = 8, BIT_16 = 16, BIT_24 = 24, BIT_32 = 32 };


struct SamplePoint{
    float value;
    unsigned int position;
};
typedef vector<SamplePoint> vector_sample_point,*pvector_sample_point;


class BadConversion : public std::runtime_error {
public:
  BadConversion(const std::string& s)
    : runtime_error(s)
    { }
};
template<class T>
inline std::string to_string(const T& x)
{
  std::ostringstream o;
  if (!(o << x))
    throw BadConversion(std::string("to_string(")
                        + typeid(x).name() + ")");
  return o.str();
}
class DspUtil
{


	 void static SwapBytes(void *pv, size_t n);
public:

	 static void WindowData(float *pData, float *Rx, float *Ix, unsigned len, WindowFunc::EWindow eWnd);

	 static float *CreateWindowFunc(unsigned len, WindowFunc::EWindow eWnd);

	 static float *CreateWindowFunc(unsigned len, EWindow eWnd, float &s0, float &s1);

	 static float *CreateHammingWindow(unsigned len, float&, float&);

	 static float *CreateBlackmannWindow(unsigned len,float&, float&);

	 static float *CreateHannWindow(unsigned len, float &s0, float &s1);

	 static float *CreateLoPassFilter(float cutoffFreq, unsigned fkLength, WindowFunc::EWindow eWnd);

	 static float *CreateHiPassFilter(float cutoffFreq, unsigned fkLength, WindowFunc::EWindow eWnd);

	 static float *CreateHighPassFilter( float cutoffFreq, unsigned fkLength, EWindow eWnd);

	 static float *CreateBandPassFilter(float cutoffFreq,float cutoffFreqHi, unsigned fkLength, WindowFunc::EWindow eWnd) ;

	 static float *FFTConvolution(float *pReal, float *pImg, float *pFkReal, float *pFkImg, int fftSize);

	 static void FFTConvolution(float *pReal, float *pFkReal, int fftSize);

	 static float *InverseFFT(float *Rx, float *Ix, int fftSize);

	 static void FFT(float *Rx, float *Ix, int fftSize);

	 static void ComputeMagnitudeAndPhase(const float* pLc, float *pMag, float *pPhase, unsigned len);

	 static float *GetAverageMagnitudes(std::vector<float*> &vMag, unsigned fftLen);

	 static float* DecimateSequence( float* seq, unsigned int decFactor, unsigned int len );

	 static float* InterpolateSequence( float* seq, unsigned int overSampling, unsigned int len );

	 static float* ConvertToFloat(void* data, int len, EncodingType encType);

	 static unsigned NextPowerOfTwo(unsigned val);

	 static void movingAverageSequence(float* seq,  unsigned int len, unsigned int avgDataCount){

         for(int i=0; i<len-avgDataCount;i++){
			 float runningAvg=0.0f;
			 for(int j=0;j<avgDataCount;j++){
                 runningAvg += seq[i+j];
             }
             seq[i] = runningAvg / avgDataCount;
         }
     }

    template<class K,class L> static std::string convert_to_string(K* buffer, L bufferLength, L typeSize, bool swapBytes)
    {
            std::string outStr;
            int len=bufferLength/typeSize;

            for(int i=0; i<len;i++){
                if(swapBytes)
                    swap_bytes(&buffer[i],typeSize);

				outStr += (to_string(buffer[i])+"\n");
            }
            return outStr;
    }


	//static void   SwapBytes(void *pv, size_t n)
	static float*  CalculateMagnitude(float *fftRx, float *fftIx, unsigned int length);
    static void  CalculateMagnitudeInPlace(float *fftRx, float *fftIx, unsigned int length);
	static void FilterData(float *pData, unsigned len, float *pFilter, unsigned fkLen, unsigned fftLen, EWindow);

	static float GetPeakValue(float*, unsigned , unsigned& pos);
    static std::vector<SamplePoint> getThreasholdSamples(float* data, unsigned int len);
	static pvector_sample_point getThreasholdSamples(float* data, unsigned int len, float thresholdVal);
	static std::vector<float>* getAutocorrelationData(float* data, unsigned int len);
	static void NormalizeData(float* , unsigned int , float );
	static float* MixTracks(float **ppData, unsigned int numTracks, unsigned int len);
	static float* MixTracks(vector<nsPlayback::PTRACK_SELECTION> *pvData,  unsigned int len);
	static void SetRepeatValues(float ratio, unsigned &rep, unsigned &rep_ );
	static void SetRepeatValues(float &repeatValue, unsigned numRepeats, unsigned totalRepeat, unsigned &rep0, unsigned &rep1, unsigned &rep2, int &correction );
    static void SetRepeatValues_(float repeatValue, unsigned &rep, unsigned &rep_ );
};

class Output{
public:
    void static doOutput();
     template<typename K,typename L>  static  std::string convert_to_string(K* buffer, L bufferLength, L typeSize, bool swapBytes);
     void  static swap_bytes(void *pv, size_t n);
};
template<typename K,typename L>  static   std::string convert_to_string(K* buffer, L bufferLength, L typeSize, bool swapBytes);

#endif // DSPUTIL_H

