//---------------------------------------------------------------------------


#pragma hdrstop

#include "SpectralFeature.h"

void SpectralFeature::SetMaxValues(float *pData, int len){

	const float bRes = binResolution;
	for(int i=0;i<len;i++){
		mMag[i*bRes] = pData[i];
	}
	//remove any values after
	map<float,float>::iterator mIter=mMag.begin();
	int i=numMaxValues;
	while(i--) mIter++;

	mMag.erase(mIter, mMag.end());

	/*for(mIter=mMag.begin(); mIter!=mMag.end(); mIter++){
			  if(i++<numMaxValues)continue;
			  mMag.erase(mIter, mIter.end());
	}   */
}
void SpectralFeature::DumpToFile(ofstream &out){

	 const char tab = '\t';
	 out << "FORMAT" << std::endl;
	 out << sampleRate << tab << bitsPerSample << tab << binResolution << std::endl;
	 out << "CONTEXT" << std::endl;
	 out << context.c_str() << std::endl;
	 out << "SUBCONTEXT" << std::endl;
	 out << context.c_str() << std::endl;
	 out << "MAGNITUDE VALUES" << std::endl;
	 map<float,float>::iterator mIter;
	 for(mIter=mMag.begin(); mIter!=mMag.end(); mIter++){
			  out << mIter->first << tab << mIter->second << std::endl;
	}
}
#pragma package(smart_init)
