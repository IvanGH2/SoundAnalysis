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

#include "GradientColor.h"
#include "GenPtrAlloc.h"
#include "math.h"

//---------------------------------------------------------------------------
GradientColor::GradientColor(unsigned color)
: cGreenShift(8)
, cBlueShift(16)
, eGradMap(ORANGE_GLOW)
{ //this assumes a 32 bit color{

	Init();
}
GradientColor::GradientColor()
: cGreenShift(8)
, cBlueShift(16)                                                           
, eGradMap(ORANGE_GLOW)                                
{

	Init();
}
GradientColor::~GradientColor(){

   //	FreeResources();

   /*	if(pGradColors){

	 for(int i=0; i<rowLen;i++)  {
	 if(pGradColors[i])
		delete [] pGradColors[i];
		}

	 delete [] pGradColors;
	}  */
}
void GradientColor::FreeResources(){

	 GenPtrAlloc<unsigned>::DeallocateArray(pGradColors, rowLen);
}
void GradientColor::Init(){
	maxValue = 255;
	bkgMaxValue = 200;
	gradientStep = 3;
	gradientColor = colors[eGradMap][9];
	DecomposeGradientColor(gradientColor);
	SetGradientBkgColor();

  //	pGradientColors = NULL;
	pGradColors = NULL;
}
void GradientColor::SetGradientContrast(unsigned gStep){

	if(gStep == 0) gStep = 1;
	gradientStep = gStep;
}
void GradientColor::SetGradientColor(unsigned color){
	gradientColor = color;
	DecomposeGradientColor(color);
	SetGradientBkgColor();
	}
unsigned GradientColor::GetGradientContrast(){
	return gradientStep;
}
void GradientColor::SetGradientBkgColor() {

	bkgGradientColor = colors[eGradMap][11];
}
void GradientColor::DecomposeGradientColor(unsigned gColor){

	colorBlue =  	gColor >> cBlueShift;
	colorGreen =   (gColor >> cGreenShift) & 0xff;
	colorRed =		gColor & 0xff;
}
/*
  this method maps magnitude values to colours
*/
unsigned **GradientColor::GetSpectrogramColours(unsigned **pMag, unsigned rowNum,  unsigned colNum,
 unsigned numChannels, unsigned maxVal, unsigned minVal){

	int i, j, index;
	const unsigned totalRows = numChannels * rowNum;

	/*if(rowLen != rowNum || colLen != colNum){//dims have changed
		FreeResources(); //ako je dimens nepromijenjen možda samo izraèunati boje
		pGradColors = GenPtrAlloc<unsigned>( totalRows, colNum ).GetPtrDbl();
	}     */
   //	FreeResources();
	pGradColors = GenPtrAlloc<unsigned>( totalRows, colNum ).GetPtrDbl();
	rowLen 	= rowNum;
	colLen  = colNum;

   /*	pGradColors = new unsigned*[totalRows];
		for(i=0;i<totalRows;i++)
			pGradColors[i] = new unsigned[colNum];  */
  //	pGradColors = pMag;
	unsigned char r = 0,g = 0, b = 0;
	float normal = 0, relNorm, rStep, gStep, bStep, rFinal, gFinal, bFinal, rDelta, gDelta, bDelta;
	float rInc, gInc, bInc;
	const unsigned range = maxVal-minVal;

	switch(eGradMap){

	  case WHITE_ON_BLACK:
	  case BLACK_ON_WHITE:

		for(i=0; i<totalRows; i++){
			for(j=0; j<colNum; j++){

			normal = normal < 0 ? 0 : normal > 1 ? 1 : ( static_cast<float>(pMag[i][j]) - minVal)/range;

			r = g = b = ( eGradMap == BLACK_ON_WHITE ) ? ( 1.0f - normal ) * maxValue : normal * maxValue;

			pGradColors[i][j] = ( b << cBlueShift ) | ( g << cGreenShift ) | r;
			}
		}
		break;
	  case EMERALD_GREEN:

      	for(i=0; i<totalRows; i++){
			for(j=0; j<colNum; j++){
			g = 255;
			normal = ( static_cast<float>(pMag[i][j]) - minVal)/range;

			if (normal < 0.2f) normal = 0.2f;
			if (normal > 1.0f) normal = 1.0f;
			g *= normal;
			pGradColors[i][j] = (r << cBlueShift) | (g << cGreenShift) | b;
			}
		}
		break;
	  default:
	   for(i=0; i<totalRows; i++){
		for(j=0; j<colNum; j++){

			normal = ( static_cast<float>( pMag[i][j]) - minVal )/range;

		index = ( 1.0f - normal ) * range ;//* gradientStep;

		if(index < 2)
			index = 0;
		else if(index < 5)
			index = 1;
		else if(index < 8)
			index = 2;
		else if(index < 10)
			index = 3;
		else if(index < 12)
			index = 4;
		else if(index < 15)
			index = 5;
		else if(index < 18)
			index = 6;
		else if(index < 21)
			index = 7;
		else if(index < 24)
			index = 8;
		else if(index < 27)
			index = 9;
		else if(index < 30)
			index = 10;
		else if(index < 33)
			index = 11;
		else if(index < 36)
			index = 12;
		else if(index < 40)
			index = 13;
		else if(index < 44)
			index = 14;
		else if(index < 48)
			index = 15;
		else if(index < 53)
			index = 16;
		else if(index < 58)
			index = 17;
		else if(index < 64)
			index = 18;
		else if(index < 70)
			index = 19;
		else if(index < 78)
			index = 20;

		pGradColors[i][j] = colors[eGradMap][index];

			}
		}
	} //end switch

	return pGradColors;
}


#pragma package(smart_init)
