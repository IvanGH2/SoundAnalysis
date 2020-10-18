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

#ifndef SpectrogramViewH
#define SpectrogramViewH

#include <Graphics.hpp>
#include "ContainerView.h"
#include "GradientColor.h"

using namespace nsWaveDisplay;

class TSpectrogramView : public TContainerView {
 
Graphics::TBitmap *pBmp0, *pBmp1;//, *pBmp00, *pBmp11; //left and right channel bitmaps

GradientColor *pGradientColor, gradientColor;

unsigned *pData, **pGridData;

unsigned numFrames, frameWidth, len, colorMap, gradientStep;

float maxMag, minMag;

protected:

virtual void DrawSpectrogramView();

public:
  //numFrames ????
TSpectrogramView(unsigned w, unsigned h, unsigned numChannels,  unsigned numFrames,unsigned frameWidth, float maxMag,
unsigned *pData, unsigned len) ;

TSpectrogramView(unsigned w, unsigned h, unsigned numChannels, float maxMag, float minMag, unsigned **pData) ;

virtual ~TSpectrogramView();

Graphics::TBitmap *GetContainerViewLeft();

Graphics::TBitmap *GetContainerViewRight();

void SetData(unsigned *,unsigned ); //1D

void SetData(unsigned **);

void SetColorMap(unsigned cMap) { colorMap = cMap; }

void SetGradientStep(unsigned gStep) { gradientStep = gStep; }

void SetMaxMagnitude(float m) { maxMag = m; }

EContainerView GetContainerViewType() const { return SPECTROGRAM; }
};
//---------------------------------------------------------------------------
#endif
