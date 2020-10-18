//---------------------------------------------------------------------------

#ifndef SpectralFeatureH
#define SpectralFeatureH

#include "map.h"
#include <fstream>
//---------------------------------------------------------------------------
class SpectralFeature {

	std::string context;
	float binResolution;
	unsigned sampleRate;
	unsigned bitsPerSample;
	unsigned numMaxValues;
	map<float,float> mMag;

	void SetMaxValues(float *, int);
	
	public:

	SpectralFeature(): numMaxValues(50){}

	SpectralFeature(unsigned numMax): numMaxValues(numMax){}

	void DumpToFile(ofstream &);

	void SetBinResolution(float bRes) { binResolution = bRes;}
		
};
#endif
