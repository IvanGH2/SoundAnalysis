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

#pragma hdrstop

#include "WaveformView.h"
#include "WaveDisplayData.h"
//---------------------------------------------------------------------------
TWaveformView::TWaveformView(unsigned w, unsigned h, unsigned numChannels, void *pData, unsigned len) :
TContainerView( w, h, numChannels), pData(pData), len(len), enabled(true)
{
//unsigned channelHeight =  numChannels == 1 ? h - cWaveformMargin : (h-cWaveformMargin-cWaveformSpacing)/2;
	pBmp0 = new Graphics::TBitmap;
	pBmp0->Height = h;
	pBmp0->Width = w;
	pBmp0->PixelFormat = pf32bit;

	if(numChannels == 2){
		pBmp1 = new Graphics::TBitmap;
		pBmp1->Height = h;
		pBmp1->Width = w;
		pBmp1->PixelFormat = pf32bit;
	}
	brushColour = clWhite;
	penColour = clGreen;
	borderColour = clBlack;
	penColourDisabled = clMoneyGreen;
	//DrawWaveformView(); //??? možda ne treba ovo
}

void TWaveformView::SetInputData(void *pData, unsigned len){
	this->pData = pData;
	this->len = len;
}             

void TWaveformView::SetWaveColour(TColor colour){
				penColour = colour;
}
void TWaveformView::SetBkgColour(TColor colour){
				brushColour = colour;
}
Graphics::TBitmap *TWaveformView::GetContainerViewLeft(){

			  //	if(pBmp0->Empty)
				DrawWaveformView();
				return pBmp0;
}
Graphics::TBitmap *TWaveformView::GetContainerViewRight(){

				if(!pBmp0)
					DrawWaveformView();
				return pBmp1;
}

void TWaveformView::DrawWaveformView(){

	PWAVE_POINT wpValues =  (PWAVE_POINT)pData;

	unsigned channelHeight = height;// numChannels == 1 ? height - cWaveformMargin : (height-cWaveformMargin-cWaveformSpacing)/2;

	TRect rect(0, 0, width, channelHeight);
 
	//drawing an offscreen bitmap for the left channel
	TColor waveColour =  enabled ? penColour : penColourDisabled;
	pBmp0->Width = width;
	pBmp0->Height = channelHeight;

	pBmp0->Canvas->Pen->Width = 1;
	pBmp0->Canvas->Brush->Color = brushColour;
	pBmp0->Canvas->Pen->Color = brushColour;//borderColour;
	pBmp0->Canvas->Rectangle(rect);
	pBmp0->Canvas->Pen->Color = waveColour;

	//right channel
	//drawing the actual samples
	if( numChannels == 1){

		pBmp0->Canvas->MoveTo(wpValues[0].x, wpValues[0].y);
		for( int i = 1; i < len; i++ ){
			pBmp0->Canvas->LineTo(wpValues[i].x, wpValues[i].y);
			if(eWaveformDisplayMode==PIXEL_AVERAGING){
				pBmp0->Canvas->MoveTo(wpValues[i].x, wpValues[i].y-2);
				pBmp0->Canvas->LineTo(wpValues[i].x, wpValues[i].y+2);
				pBmp0->Canvas->MoveTo(wpValues[i].x, wpValues[i].y);
			}
	 }
	}else{    //2-channels
		pBmp1->Canvas->Brush->Color = brushColour;
		pBmp1->Width = width;
		pBmp1->Height = channelHeight;
		pBmp1->Canvas->Pen->Color = brushColour;//borderColour;
		pBmp1->Canvas->Rectangle(rect);
		pBmp1->Canvas->Pen->Color = waveColour;
		if(eWaveformDisplayMode==PIXEL_AVERAGING){
			pBmp0->Canvas->MoveTo(wpValues[0].x, wpValues[0].y); //left pos
			pBmp0->Canvas->LineTo(wpValues[2].x, wpValues[2].y); //left neg
			pBmp1->Canvas->MoveTo(wpValues[1].x, wpValues[1].y); //right pos
			pBmp1->Canvas->LineTo(wpValues[3].x, wpValues[3].y); //right neg

		for( int i = 4; i < len; i += 4 ){
			pBmp0->Canvas->LineTo(wpValues[i].x,   wpValues[i].y); //left pos
			pBmp0->Canvas->LineTo(wpValues[i+2].x, wpValues[i+2].y); //left neg
			pBmp1->Canvas->LineTo(wpValues[i+1].x, wpValues[i+1].y); //right pos
			pBmp1->Canvas->LineTo(wpValues[i+3].x, wpValues[i+3].y); //right neg
			}
		}else{ //sample averaging
			pBmp0->Canvas->MoveTo(wpValues[0].x, wpValues[0].y); //left pos
			pBmp0->Canvas->LineTo(wpValues[2].x, wpValues[2].y); //left neg
			pBmp1->Canvas->MoveTo(wpValues[1].x, wpValues[1].y); //right pos
			pBmp1->Canvas->LineTo(wpValues[3].x, wpValues[3].y); //right neg

		for( int i = 4; i < len; i += 4 ){
			pBmp0->Canvas->MoveTo(wpValues[i].x,   wpValues[i].y); //left pos
			pBmp0->Canvas->LineTo(wpValues[i+1].x, wpValues[i+1].y); //left neg
			pBmp1->Canvas->MoveTo(wpValues[i+2].x, wpValues[i+2].y); //right pos
			pBmp1->Canvas->LineTo(wpValues[i+3].x, wpValues[i+3].y); //right neg
			}
		}
	}
}
#pragma package(smart_init)
