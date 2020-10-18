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
	
#include "fft.h"
#include "math.h"

std::vector<CFFT*> FFT::vTwiddles;
CFFT *FFT::pTwr = 0;
unsigned  FFT::prevFftSize = 1024;
bool  FFT::prevInverse = false;

/**
 * @brief FFT::ComputeComplexFFT - computes the complex FFT
 * @param cFft - real and imaginary parts
 * @param fftSize
 */

void FFT::ComputeComplexFFT(CFFT *cFft, unsigned fftSize, bool inverse){

	int i, j, k, m;
    float rxt, ixt;
	const unsigned twLen = vTwiddles.size();

	m = log(fftSize)/log(2);
    int fftSizeHalf = fftSize/2;
	j = k = fftSizeHalf;

	if(/*twLen == 0 ||*/ twLen < m || inverse != prevInverse) ComputeTwiddles(fftSize, inverse);

	for (i = 1; i < (fftSize-1); i++) {
			if (i < j) {

            rxt = cFft[j].r;
            ixt = cFft[j].i;
			cFft[j].r = cFft[i].r;
			cFft[j].i = cFft[i].i;
            cFft[i].r = rxt;
            cFft[i].i = ixt;
            }

            k = fftSizeHalf;

            while (k <= j){
				j = j - k;
				k = k>>1;
				}
            j = j + k;

		}    //end for

    int le, le2, l, ip;
    float sr, si, ur, ui;

    int ii;
	CFFT *pTwiddles;

	for (k = 1; k <= m; k++){
        le = pow(2, k);
        le2 = le/2;
		pTwiddles = vTwiddles.at(k-1);
        for (j = 1; j <= le2; j++) {
			l = j - 1;

			for (i = l; i < fftSize; i += le) {
				ip = i + le2;

				rxt = cFft[ip].r * pTwiddles[l].r - cFft[ip].i * pTwiddles[l].i;
				ixt = cFft[ip].r * pTwiddles[l].i + cFft[ip].i * pTwiddles[l].r;
				cFft[ip].r = cFft[i].r - rxt;
				cFft[ip].i = cFft[i].i - ixt;
				cFft[i].r  = cFft[i].r + rxt;
				cFft[i].i  = cFft[i].i + ixt;
			}    //end for
		}
	}
	prevFftSize = fftSize;
	prevInverse = inverse;
}

	void FFT::ComputeRealFFT(float * r, unsigned fftSize, bool inverse){

		int k,l;
		const unsigned halfSize = fftSize / 2;
		const unsigned qrtSize  = fftSize / 4;

		CFFT *cFFT = reinterpret_cast<CFFT*>( r );

		float h1r,h2r,h1i,h2i;

		if( !inverse ) ComputeComplexFFT( cFFT, halfSize, inverse );

		CFFT tdc;
        tdc.r = cFFT[0].r;
        tdc.i = cFFT[0].i;

		CFFT fpk, fpnk, f1k, f2k, tw;
		if( !pTwr || fftSize != prevFftSize || inverse != prevInverse )
			ComputeTwiddlesReal(fftSize, inverse);

	  //	CFFT *twiddles = pTwr;//new CFFT[halfSize/2];

		const float hS = inverse ?  0.5f : -0.5f;

		for (k=1, l=0; k <= qrtSize; ++k, ++l ){

			h1r = 0.5f * (cFFT[k].r+cFFT[halfSize-k].r);

			h1i = 0.5f * (cFFT[k].i-cFFT[halfSize-k].i);

			h2r = -hS *(cFFT[k].i+cFFT[halfSize-k].i);

			h2i = hS *(cFFT[k].r-cFFT[halfSize-k].r);

			cFFT[k].r = h1r+pTwr[l].r*h2r-pTwr[l].i*h2i;

			cFFT[k].i = h1i+pTwr[l].r*h2i+pTwr[l].i*h2r;

			cFFT[halfSize-k].r = h1r-pTwr[l].r*h2r+pTwr[l].i*h2i;

			cFFT[halfSize-k].i = -h1i+pTwr[l].r*h2i+pTwr[l].i*h2r;
		}
		if( !inverse ){
			h2r = cFFT[0].r;
			h2i = cFFT[0].i;
			cFFT[0].r = h2r + h2i;
			cFFT[halfSize].r = h2r - h2i;
			cFFT[0].i = cFFT[halfSize].i = 0;
		}else {
			ComputeComplexFFT(cFFT, halfSize, inverse);

			for(k=1;k<=qrtSize;k++){
				CFFT tmp = cFFT[halfSize-k];
				cFFT[halfSize-k].r = cFFT[k].r;
				cFFT[halfSize-k].i = cFFT[k].i;
				cFFT[k].r = tmp.r;
				cFFT[k].i = tmp.i;
			}
		}
}


void FFT::ComputeTwiddles(unsigned fftSize, bool inverse){


	const unsigned p2[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192 };
    int pwr, pwrHalf, i, j;
	float ur,ui, sr, si, tmp;
  //  if(fftSize>p2[sizeof(p2)/sizeof(unsigned)]) throw
	 int m = log(fftSize)/log(2);

	const unsigned start = vTwiddles.size()>0 ? vTwiddles.size()+1 : 1;
	const int sign = inverse ? -1 : 1;
	for (i = start; i <= m; i++) {
		pwr = p2[i];
		pwrHalf = pwr / 2;

		float phase = static_cast<float>(sign*2*PI/pwr);
		float wtemp = sin(0.5f*phase);

		float sr = -2.0 * wtemp * wtemp;
		float si = sin(phase);

		ur = 1;
		ui = 0;

		CFFT *p = new CFFT[pwrHalf+1];

        vTwiddles.push_back(p);

		p[0].r = 1;
		p[0].i = 0;

		for (j = 1; j <= pwrHalf;j++){
			tmp = ur;
			p[j].r = tmp * sr - ui * si +ur;
		  //	p[j].i = (tmp * si + ui * sr+ui);
			p[j].i = (ui + tmp * si + ui * sr);
			ur = p[j].r;
			ui = p[j].i;
		}
	}
}
void FFT::ComputeTwiddlesReal(unsigned fftSize, bool inverse){

   // const unsigned p2[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192 };
		int  i;
		float ur,ui, sr, si, tmp;

	//	const unsigned fftHalf = fftSize/2;
		const unsigned fftQrt  = fftSize>>2;

		const float phase = inverse ? -PI / (fftSize>>1) : PI / (fftSize>>1);

		float wtemp = sin(0.5f*phase);

		const float wpr = -2.0f*wtemp*wtemp;
		const float wpi = sin(phase);

		float wr = 1.0f+wpr;
		float wi = wpi;

		pTwr = new CFFT[fftQrt+1];
		pTwr[0].r = wr;
		pTwr[0].i = wi;

		for (i = 1; i <= fftQrt;i++){
			wtemp = wr;
			pTwr[i].r = wtemp*wpr - wi*wpi + wr;
			pTwr[i].i = wi*wpr + wtemp*wpi + wi;
			wi = pTwr[i].i;
			wr = pTwr[i].r;
		}

}
void FFT::ComputeConvolution(float * pReal, float * pFkReal, unsigned len){

	for(int i=0; i<len;i+=2){
		   float tmp 	= pReal[i] * pFkReal[i] - pReal[i+1] * pFkReal[i+1];
		   pReal[i+1] 	= pReal[i] * pFkReal[i+1] + pReal[i+1] * pFkReal[i];
		   pReal[i]  	= tmp;
	  }
}



