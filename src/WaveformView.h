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

#ifndef WaveformViewH
#define WaveformViewH

//#include "vcl.h"
#include <Graphics.hpp>
#include "ContainerView.h"

using namespace nsWaveDisplay;
//---------------------------------------------------------------------------
class TWaveformView : public TContainerView {

Graphics::TBitmap *pBmp0, *pBmp1; //left and right channel bitmaps

TColor brushColour, penColour, penColourDisabled, borderColour;

void *pData;

bool enabled;

unsigned len; //array length

EWaveformDisplayMode eWaveformDisplayMode;

protected:

virtual void DrawWaveformView();

public:

TWaveformView(unsigned w, unsigned h, unsigned numChannels, void *pData, unsigned len);

~TWaveformView(){
	if(pBmp0) {
		delete pBmp0;
		pBmp0=0;
		}
	if(numChannels == 2 && pBmp1) {
		delete pBmp1;
		pBmp1 = 0; }
}

void SetWaveColour(TColor );

TColor GetWaveColour() const { return penColour; }

void SetBkgColour(TColor );

TColor GetBkgColor() const { return brushColour; }

void SetInputData(void *pData,unsigned );

Graphics::TBitmap *GetContainerViewLeft();

Graphics::TBitmap *GetContainerViewRight();

void SetEnabled(bool state) { enabled = state; }

void SetWaveformDisplayMode(EWaveformDisplayMode eMode)  { eWaveformDisplayMode = eMode; }
 
EContainerView GetContainerViewType() const { return WAVEFORM; }
};
#endif
