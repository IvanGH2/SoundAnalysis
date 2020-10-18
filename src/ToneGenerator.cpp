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


//---------------------------------------------------------------------------


#include "ToneGenerator.h"

/*
Tone* Tone::CreateInstance(const Tone& tone){
	nsToneGenerator::EToneType toneType = tone.GetToneType();
	switch(toneType){
	case nsToneGenerator::SINE:
		return new SineTone((const SineTone&)tone);
	case nsToneGenerator::SQUARE:
		return new SquareTone((const SquareTone&)tone);

	}
	return NULL;
} */

void Tone::SetAmplitudes(float amp){

   lAmps.push_back( amp );
}
void Tone::SetPhases(float phase){

   lPhases.push_back( phase );
}

 void Tone::SetNumPartials(unsigned int numPartials){

	 this->numPartials = numPartials;

 }
 void SineTone::RemovePartial(unsigned int partialFreq){

	 float partialAmp = amps[partialFreq / freq - 1];

	 float partialPhase = phases[partialFreq / freq - 1];

	 for(int i = 0; i < dataLen; i++ ){
			 data[i] -= partialAmp * sin(PI2*(partialFreq) * ((float)i)/sampleRate + partialPhase)/numPartials;
     }
 }
 void SineTone::AddPartial(unsigned int partialFreq){

	 float partialAmp = amps[partialFreq / freq - 1];

	 float partialPhase = phases[partialFreq / freq - 1];

	 for(int i = 0; i < dataLen; i++ ){
             data[i] += partialAmp * sin(PI2*(partialFreq) * ((float)i)/sampleRate + partialPhase)/numPartials;
     }
 }

 void SineTone::GenerateTone(unsigned freq,  float duration){

	 this->freq = freq;
	 this->duration = duration;
	 dataLen = duration * sampleRate;
	 tData.TransferOwnershipToCaller(false);
	 data = tData.GetPtr(dataLen);

	 DoGenerateTone();

 }
 void SineTone::GenerateTone(){

	 dataLen = (duration/1000) * sampleRate;
	 tData.TransferOwnershipToCaller(false);
	 data = tData.GetPtr(dataLen);
	 DoGenerateTone();

 }
  void SquareTone::GenerateTone(){

	 dataLen = (duration/1000) * sampleRate;
	 tData.TransferOwnershipToCaller(false);
	 data = tData.GetPtr(dataLen);

	 DoGenerateTone();

 }
 void SineTone::DoGenerateTone(){
	 const unsigned int numEffectivePartials = numPartials;

	 std::list<float>::const_iterator iterAmp;
	 std::list<float>::const_iterator iterPhase;
	 int i, j;
	 for(i = 0; i < dataLen; i++ ){

		 data[i]=0;
		 for(j=0,iterAmp = lAmps.begin(), iterPhase = lPhases.begin(); iterAmp != lAmps.end(), j < numPartials; j++, iterAmp++, iterPhase++){
			 float valAmp = *iterAmp;
			 float valPhase = *iterPhase;
			 data[i] += valAmp * sin(PI2*(freq * (j+1)) * ((float)i)/sampleRate + valPhase);
		 }
		 data[i] /= numEffectivePartials;
	 }
 }
 void SquareTone::DoGenerateTone(){

	int i, j;
	amp = lAmps.front();    //Square wave has only a single freq (no harmonics)

	unsigned samplesPerCycle = sampleRate / freq;
	if(samplesPerCycle%2 != 0)	samplesPerCycle++;

	const unsigned samplesPerCycleHalf = samplesPerCycle / 2;

	for(i = 0; i < dataLen - samplesPerCycle; i+= samplesPerCycle ){
		 for(j=0;j<samplesPerCycleHalf;j++)
			  data[i+j] = amp;
		 for(j=samplesPerCycleHalf;j<samplesPerCycle;j++)
			 data[i+j] = -amp;
		}
}
void SawToothTone::GenerateTone(){

	 dataLen = (duration/1000) * sampleRate;
	 tData.TransferOwnershipToCaller(false);
	 data = tData.GetPtr(dataLen);

	 DoGenerateTone();
}

 void SawToothTone::DoGenerateTone(){

	amp = lAmps.front();    //Square wave has only a single freq (no harmonics)
	unsigned int samplesPerCycle = sampleRate / freq;

	float vertStep = (2.0f*amp)/samplesPerCycle;
	 for(int i = 0; i < dataLen-samplesPerCycle; i+=samplesPerCycle){
		for(int j = 0; j < samplesPerCycle; j++)
			data[i+j] = -amp + j * vertStep;

 }
}
void TriangleTone::GenerateTone(){

	 dataLen = (duration/1000) * sampleRate;
	 tData.TransferOwnershipToCaller(false);
	 data = tData.GetPtr(dataLen);

	 DoGenerateTone();

 }
 void TriangleTone::DoGenerateTone(){

	amp = lAmps.front();    //Square wave has only a single freq (no harmonics)
	unsigned int samplesPerCycle = sampleRate / freq;
	unsigned int samplesPerCycleHalf = samplesPerCycle / 2;
	unsigned int samplesPerCycleQuarter = samplesPerCycle / 4;
	float vertStep = (2.0f*amp)/samplesPerCycleQuarter  ;
	 for(int i = 0; i < dataLen-samplesPerCycleHalf; i+=samplesPerCycleHalf){
		for(int j = 0; j < samplesPerCycleQuarter; j++){
			data[i+j] = -amp + j * vertStep;
			data[i+j+samplesPerCycleQuarter] = amp - j * vertStep;
		}
	}
}


 



