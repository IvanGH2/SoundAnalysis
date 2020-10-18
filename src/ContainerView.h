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
	
#ifndef ContainerViewH
#define ContainerViewH
#include <Graphics.hpp>
#include "Util.h"

class TContainerView {

private:
	Graphics::TBitmap *pBmp0_, *pBmp1_, *pBmp00, *pBmp11;
	Graphics::TBitmap *pBmp2, *pBmp2_;

void StretchContainerView(bool lCh){

   TRect r(0, 0, width, height);

   if(lCh){
	if(!pBmp00){
		pBmp00 = new Graphics::TBitmap;
		pBmp00->PixelFormat = pf32bit;
	}

	pBmp00->Width  = width;
	pBmp00->Height = height;

	pBmp00->Canvas->StretchDraw(r, pBmp0_);

   }else{

	if(numChannels==2){

		if(!pBmp11){
			pBmp11 = new Graphics::TBitmap;
			pBmp11->PixelFormat = pf32bit;
		}

	pBmp11->Width = width;
	pBmp11->Height = height;

	pBmp11->Canvas->StretchDraw(r, pBmp1_);
	}//end numChannels==2
   }
}
protected:

unsigned width, height, numChannels, channelHeight; //height and width refers to the container's height and width respectively, channelHeight is basically height - margins (or height -margins)/2 for stereo

TContainerView(unsigned w, unsigned h, unsigned nChannels)
: width(w), height(h), numChannels(nChannels), pBmp0_(0), pBmp1_(0), pBmp00(0), pBmp11(0) {}

~TContainerView(){
	  if(pBmp0_)   {	delete pBmp0_;	pBmp0_ =0; }
	  if(pBmp1_)   {	delete pBmp1_;  pBmp1_ =0; }
	  if(pBmp00)   {	delete pBmp00;  pBmp00 =0; }
	  if(pBmp11)   {	delete pBmp11;  pBmp11 =0; }
}

virtual Graphics::TBitmap *GetContainerViewLeft()  = 0;

virtual Graphics::TBitmap *GetContainerViewRight() = 0;

virtual EContainerView GetContainerViewType() const = 0;

public:

void SetHeight(unsigned h) { height = h; }

void SetWidth(unsigned w)  { width = w; }

unsigned GetHeight() const { return height; }

unsigned GetWidth() const  { return width;  }

bool IsContainerViewLeft() const {return pBmp0_; }

bool IsContainerViewRight() const {return pBmp1_; }

void SetContainerViewLeft(void *pView){
	if(!pBmp0_){
		pBmp0_ = new Graphics::TBitmap;
	}
	pBmp0_->Assign((Graphics::TBitmap*)pView);
}

void SetContainerViewRight(void *pView){

	if(!pBmp1_){
		pBmp1_ = new Graphics::TBitmap;
	}
	pBmp1_->Assign((Graphics::TBitmap*)pView);
}

Graphics::TBitmap *StretchContainerViewLeft(){

				StretchContainerView(true);
				return pBmp00;
}

Graphics::TBitmap *StretchContainerViewRight(){
				StretchContainerView(false);
				return pBmp11;
}

};

#endif
