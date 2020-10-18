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

#include "SpectrogramView.h"

//---------------------------------------------------------------------------

TSpectrogramView::TSpectrogramView(unsigned w, unsigned h, unsigned numChannels, unsigned numFrames,
unsigned frameWidth, float maxMag,  unsigned *pData, unsigned len) :
TContainerView( w, h, numChannels),
pGradientColor(&gradientColor), pData(pData),
numFrames(numFrames),
frameWidth(frameWidth),
len(len),
maxMag(maxMag)
 {
pBmp0 = 0;
pBmp1 = 0;
//pBmp00 = NULL;
//pBmp11 = NULL;
}

TSpectrogramView::TSpectrogramView(unsigned w, unsigned h, unsigned numChannels, float maxMag,float minMag,
unsigned **pData) : TContainerView( w, h, numChannels),
	pGradientColor(&gradientColor),
	pGridData(pData),
	maxMag(maxMag),
	minMag(minMag),
	pBmp0(0),
	pBmp1(0){}

TSpectrogramView::~TSpectrogramView(){

	if(pBmp0){
		delete pBmp0;
		pBmp0 = NULL;
	}

	if(numChannels== 2 && pBmp1){
		delete pBmp1;
		pBmp1 = NULL;
	}

   //	if(&gradientColor)
	 //	delete &gradientColor;

   /*	if(pBmp00)
		delete pBmp00;

	if(pBmp11)
		delete pBmp11;

	if(pGradientColor)
		delete pGradientColor;  */
}
//void FreeResources(){}
void TSpectrogramView::SetData(unsigned *pData, unsigned len){

	this->pData = pData;
	this->len = len;
}
void TSpectrogramView::SetData(unsigned **pData){
	this->pGridData = pData;
}
Graphics::TBitmap *TSpectrogramView::GetContainerViewLeft(){

				DrawSpectrogramView();
				return pBmp0;
}
Graphics::TBitmap *TSpectrogramView::GetContainerViewRight(){
				//if(pBmp1->Empty)
			   	if(!pBmp0) DrawSpectrogramView();
				return pBmp1;
}
//nova logika
void TSpectrogramView::DrawSpectrogramView(){

	int *pPixel0, *pPixel1;

   pGradientColor->SetGradientColorMap((EGradientColorMap)colorMap);
   pGradientColor->SetGradientContrast(gradientStep);

   unsigned **pGradColors = pGradientColor->GetSpectrogramColours(pGridData, height,
   width, numChannels, maxMag, minMag);

   unsigned bkgColor =  pGradientColor->GetGradientBkgColor();

   const unsigned w = width;//-50;//pGradientColor->GetColumnNum();

   TRect r(0, 0, width, height);

	if(pBmp0){
		delete pBmp0;
		pBmp0 = 0;
	}

	if(numChannels==2 && pBmp1){
		delete pBmp1;
		pBmp1 = 0;
	}
   pBmp0 = new Graphics::TBitmap;
   pBmp0->Height = height;
   pBmp0->Width = width;
   pBmp0->PixelFormat = pf32bit;
  // pBmp0->Canvas->Brush->Color = clYellow;
   pBmp0->Dormant();
   pBmp0->FreeImage();

   if(numChannels == 2){
	pBmp1 = new Graphics::TBitmap;
	pBmp1->Height = height;
	pBmp1->Width = width;
	pBmp1->PixelFormat = pf32bit;
	pBmp1->Dormant();
	pBmp1->FreeImage();
	}

  // pBmp0->Width = width;
  // pBmp0->Height = height;
	pBmp0->Canvas->Rectangle(r);
	try{
	if(numChannels==1){
		for(int i=0; i<height; i++){
			unsigned h = height-i-1;
			pPixel0 = (int*)pBmp0->ScanLine[h];

		for(int j=0,k=0; j<w; j++) {
			*pPixel0++ = pGradColors[i][j];
			}
		}
   }else{ //2-channels
	 pBmp1->Canvas->Brush->Color = clBlue;
	// pBmp1->Width = width;
	// pBmp1->Height = height;
	 pBmp1->Canvas->Rectangle(r);

	 for(int i=0, k=0; i<height; i++, k+=2){
			unsigned h = height-i-1;
			pPixel0 = (int*)pBmp0->ScanLine[h];
			pPixel1 = (int*)pBmp1->ScanLine[h];

		for(int j=0; j<w; j++) {
			*pPixel0++ = pGradColors[k][j];
			*pPixel1++ = pGradColors[k+1][j];
			}
		}
	}
   }catch(...){
		width=width;
   }
}
#pragma package(smart_init)
