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

#include "SelectionOverlay.h"

bool TSelectionOverlay::IsOverlayRedrawNeeded(const TRect &rect, EChannel eCh){

	if(rect!=selOverlayRect || (eCh==LEFT && pSelOverlay0) || (eCh==RIGHT&&pSelOverlay1))return true;

	return false;
}
Graphics::TBitmap *TSelectionOverlay::GetSelectionOverlay(const Graphics::TBitmap *pBmp,
TColor selColour, TRect &rect, EChannel eChannel){
 
 CreateOverlayBitmaps(eChannel);
 Graphics::TBitmap *pSelOverlay = eChannel==LEFT ? pSelOverlay0 : pSelOverlay1;

 selOverlayRect = rect;

 const unsigned cGreenShift = 8;
 const unsigned cBlueShift = 16;

 const unsigned overlayWidth = abs(rect.right-rect.left);//   sex-ssx;
 const unsigned overlayHeight = abs(rect.bottom-rect.top);

 const unsigned startOffsetX = rect.right>rect.left ? rect.left : rect.right;
 const unsigned startOffsetY = rect.bottom>rect.top ? rect.top : rect.bottom;

 pSelOverlay->Width = pBmp->Width;//overlayWidth;
 pSelOverlay->Height = pBmp->Height;//overlayHeight;
 pSelOverlay->Assign((Graphics::TBitmap*)pBmp);

 unsigned char cRed0, cGreen0, cBlue0,cRed1, cGreen1, cBlue1, bRedColor0, bGreenColor0, bBlueColor0;

 cBlue0 =  	selColour >> cBlueShift;
 cGreen0 =  selColour >> cGreenShift & 0xff;
 cRed0 =	selColour & 0xff;

 unsigned *pPixel0, *pPixel1;

 for(int i=0;i<overlayHeight;i++){

	pPixel0 = (unsigned*)pSelOverlay->ScanLine[startOffsetY+i];

	for(int j=0;j<overlayWidth;j++){

	unsigned pixel =  pPixel0[startOffsetX+j];
	cBlue1 =  	pixel >> cBlueShift;
	cGreen1 =  	pixel >> cGreenShift & 0xff;
	cRed1 =		pixel & 0xff;

	//blend the current bitmap pixel with the overlay colour

	const float w0 = 0.4f;
	const float w1 = 0.6f;

	bRedColor0   = (cRed0*w0+cRed1*w1);
	bGreenColor0 = (cGreen0*w0+cGreen1*w1);
	bBlueColor0  = (cBlue0*w0+cBlue1*w1);

	pPixel0[startOffsetX+j] =  ((bBlueColor0 << cBlueShift) | (bGreenColor0 << cGreenShift)) | bRedColor0;
	}
 }
  return pSelOverlay;
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
