#ifndef FFT_H
#define FFT_H

#include <vector>


#define PI 3.14159265359

struct CFFT{

    float r; //real part
    float i; //imaginary part
};

class FFT
{
   //CFFT *pTwiddles;
    static std::vector<CFFT*> vTwiddles;
   // static std::vector<CFFT*> vTwiddlesReal;
	static CFFT * pTwr;

	static bool  prevInverse;
	static unsigned 	 prevFftSize;

public:
	FFT(){}

	void ComputeComplexFFT(CFFT *pInOut, unsigned len, bool inverse);

	void ComputeRealFFT(float *pInOut, unsigned len, bool inverse);

	void ComputeConvolution(float * pReal, float * pFkReal, unsigned len);

    static void ComputeTwiddles(unsigned,bool);

	static void ComputeTwiddlesReal(unsigned, bool);

    static inline std::vector<CFFT*> &GetTwiddles() { return vTwiddles; }

   // static inline std::vector<CFFT*> &GetTwiddlesReal() { return vTwiddlesReal; }

    static inline CFFT* const GetTwiddlesReal() { return pTwr; }

};



#endif // FFT_H
