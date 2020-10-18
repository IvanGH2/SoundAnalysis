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


#pragma hdrstop

#include "MixedView.h"

//---------------------------------------------------------------------------
TMixedView::TMixedView(unsigned w, unsigned h, unsigned numChannels) :
TContainerView(w, h, numChannels)
{
//unsigned channelHeight =  numChannels == 1 ? h - cWaveformMargin : (h-cWaveformMargin-cWaveformSpacing)/2;
	pBmp0 = new Graphics::TBitmap;
	//pBmp0->Height = h;
	//pBmp0->Width = w;
	pBmp0->PixelFormat = pf32bit;
	if(numChannels == 2){
		pBmp1 = new Graphics::TBitmap;
		pBmp1->PixelFormat = pf32bit;
	}
}
bool TMixedView::IsRedrawNeeded(unsigned w, unsigned h, float tLevel){

	return width!=w || height!=h || transparencyLevel!=tLevel ? true : false;
}
Graphics::TBitmap *TMixedView::GetContainerViewLeft(){

			  //	if(pBmp0->Empty)
				DrawMixedView();
				return pBmp0;
}
Graphics::TBitmap *TMixedView::GetContainerViewRight(){

			 //	if(!pBmp1)
			  //	DrawMixedView();
				return pBmp1;
}
void TMixedView::DrawMixedView(){

	unsigned *pPixel, *pPixel0, *pPixel1,  *pPixel_, *pPixel0_, *pPixel1_;
	unsigned char cRed0, cGreen0, cBlue0,cRed1, cGreen1, cBlue1, bRedColor, bGreenColor, bBlueColor;
	unsigned char cRed0_, cGreen0_, cBlue0_,cRed1_, cGreen1_, cBlue1_, bRedColor_, bGreenColor_, bBlueColor_;

	const unsigned w = pMixBmp01->Width<pMixBmp02->Width ? pMixBmp01->Width : pMixBmp02->Width;
	const unsigned h = pMixBmp01->Height<pMixBmp02->Height ? pMixBmp01->Height : pMixBmp02->Height;

	/*if(pBmp0) delete pBmp0;
	pBmp0 = new Graphics::TBitmap;
	pBmp0->PixelFormat = pf32bit;*/
	pBmp0->Width = w;
	pBmp0->Height = h;
	if(numChannels==2){
		pBmp1->Width = w;
		pBmp1->Height = h;
	}

	for(int i=0;i<h;i++){

		pPixel0 = (unsigned*)pMixBmp01->ScanLine[i];
		pPixel1 = (unsigned*)pMixBmp02->ScanLine[i];
		pPixel =  (unsigned*)pBmp0->ScanLine[i];

		if(numChannels==2){
			pPixel0_ = (unsigned*)pMixBmp11->ScanLine[i];
			pPixel1_ = (unsigned*)pMixBmp12->ScanLine[i];
			pPixel_ =  (unsigned*)pBmp1->ScanLine[i];
		}

		const unsigned cGreenShift = 8;
		const unsigned cBlueShift = 16;
		const float w0 = transparencyLevel;
		const float w1 = 1.0f-transparencyLevel;

		for(int j=0;j<w;j++){

			unsigned pixel0 =  pPixel0[j];
			unsigned pixel1 =  pPixel1[j];
			cBlue0 =  	pixel0 >> cBlueShift;
			cGreen0 =  	pixel0 >> cGreenShift & 0xFF;
			cRed0 =		pixel0 & 0xFF;

			cBlue1 =  	pixel1 >> cBlueShift;
			cGreen1 =  	pixel1 >> cGreenShift & 0xFF;
			cRed1 =		pixel1 & 0xFF;
		//blend the current bitmap pixel with the overlay colour

			bRedColor = (cRed0*w0+cRed1*w1);
			bGreenColor = (cGreen0*w0+cGreen1*w1);
			bBlueColor = (cBlue0*w0+cBlue1*w1);

		if(numChannels==2){
			unsigned pixel0_ =  pPixel0_[j];
			unsigned pixel1_ =  pPixel1_[j];
			cBlue0_ =  		pixel0_ >> cBlueShift;
			cGreen0_ =  	pixel0_ >> cGreenShift & 0xFF;
			cRed0_ =		pixel0_ & 0xFF;

			cBlue1_ =  		pixel1_ >> cBlueShift;
			cGreen1_ =  	pixel1_ >> cGreenShift & 0xFF;
			cRed1_ =		pixel1_ & 0xFF;

			bRedColor_ = (cRed0_*w0+cRed1_*w1);
			bGreenColor_ = (cGreen0_*w0+cGreen1_*w1);
			bBlueColor_ = (cBlue0_*w0+cBlue1_*w1);

			pPixel_[j] =  ((bBlueColor_ << cBlueShift) | (bGreenColor_ << cGreenShift)) | bRedColor_;
			}

		pPixel[j] =  ((bBlueColor << cBlueShift) | (bGreenColor << cGreenShift)) | bRedColor;
		}
	}
 }
#pragma package(smart_init)
