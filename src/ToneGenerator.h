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

#ifndef tonegeneratorH
#define tonegeneratorH

#include "math.h"
#include "list"
#include "dsputil.h"
#include "GenPtrAlloc.h"

class Tone{

protected:

	unsigned int freq;

	float duration;

	float amp;

	float* data;

	GenPtrAlloc<float> tData;

	unsigned int dataLen;

	unsigned int sampleRate;

	unsigned int numChannels;

	unsigned int numPartials ;

	std::list<float> lAmps;

	std::list<float> lPhases;

	float * amps;

	unsigned int * phases;


public:

	std::list<float> GetAmplitudes() const { return lAmps; }

	void SetAmplitudes(float amp);

	std::list<float> GetPhases() const { return lPhases; }

	void SetPhases(float phase);

	unsigned GetDataLen() const { return dataLen;}

	void SetNumPartials(unsigned int numPartials);

	unsigned int GetNumPartials() const { return numPartials;}

	void SetSampleRate(unsigned int sampleRate){
		this->sampleRate = sampleRate;
    }

	unsigned int GetSampleRate()const { return sampleRate;}

	unsigned int GetFrequency()const{ return freq;}

	void SetFrequency(unsigned int freq){ this->freq = freq;}

	unsigned int GetDuration() const { return duration; }

	void SetDuration(unsigned duration) {  this->duration = duration; }

	float *GetData() const { return data; }

	void SetData(float* data){ this->data = data;}

	Tone() : numPartials(1) {  }

	~Tone(){
	  //	if(data) delete [] data;
	}

	virtual nsToneGenerator::EToneType GetToneType() const = 0;

	virtual void GenerateTone() = 0;

  //	static Tone * CreateInstance(const Tone& tone);

};


class SquareTone : public Tone{

	void DoGenerateTone();

public:
    SquareTone(unsigned sampleRate){
		SetSampleRate(sampleRate);
        numPartials = 100;
    }
    SquareTone(){
         numPartials = 100;
    }
	SquareTone(const SquareTone& other) : Tone(other) {}

	void GenerateTone();

	nsToneGenerator::EToneType GetToneType() const { return nsToneGenerator::SQUARE; }

	void Refresh();

};

class SawToothTone : public Tone{

void DoGenerateTone();

public:
	SawToothTone(unsigned sampleRate){
		SetSampleRate(sampleRate);
        numPartials = 100;
    }
	SawToothTone(){
		 numPartials = 20;
    }
	SawToothTone(const SawToothTone& other) : Tone(other) {}

	void GenerateTone();

	nsToneGenerator::EToneType GetToneType() const { return nsToneGenerator::SAWTOOTH; }

	void refresh();

};
class TriangleTone : public Tone{

void DoGenerateTone();

public:
	TriangleTone(unsigned sampleRate){
        SetSampleRate(sampleRate);
		numPartials = 100;
    }
	TriangleTone(){
		 numPartials = 20;
	}
	TriangleTone(const SawToothTone& other) : Tone(other) {}

	//void generateTone(unsigned int freq,  float amp, float duration);

	void GenerateTone();

	nsToneGenerator::EToneType GetToneType() const { return nsToneGenerator::SAWTOOTH; }

	void Refresh();

};
class SineTone : public Tone{

public:
	SineTone(unsigned sampleRate){
		SetSampleRate(sampleRate);
    }
	SineTone(){}
	//SineTone(const SineTone& other) : Tone(other){}

	void DoGenerateTone();

	void GenerateTone(unsigned freq,  float duration);

	void GenerateTone();

	void RemovePartial(unsigned int partialNum);

	void AddPartial(unsigned int partialNum);
    //CompositeTone& operator+(const Tone& tone);
	nsToneGenerator::EToneType GetToneType() const { return nsToneGenerator::SINE; }

	void Refresh(){ DoGenerateTone();}
};



#endif // TONEGENERATOR_H


