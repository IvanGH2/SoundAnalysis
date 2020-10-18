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

#ifndef MixedViewH
#define MixedViewH

#include <Graphics.hpp>
#include "ContainerView.h"
//---------------------------------------------------------------------------
class TMixedView : public TContainerView{

Graphics::TBitmap *pBmp0, *pBmp1; //left and right channel bitmaps
const Graphics::TBitmap *pMixBmp01,*pMixBmp02, *pMixBmp11, *pMixBmp12;

float transparencyLevel;

bool redrawNeeded;

bool IsRedrawNeeded(unsigned, unsigned, float);

protected:

 virtual void DrawMixedView();

public:

TMixedView(unsigned w, unsigned h, unsigned numChannels);

TMixedView::~TMixedView(){
	if(pBmp0) delete pBmp0;
	if(numChannels==2){
	  if(pBmp1) delete pBmp1;
	}
}

EContainerView GetContainerViewType() const { return MIXED; }

Graphics::TBitmap *GetContainerViewLeft();

Graphics::TBitmap *GetContainerViewRight();

void SetViewsToMix(const Graphics::TBitmap *pView0, const Graphics::TBitmap *pView1, EChannel eCh){

	  //redrawNeeded = IsRedrawNeeded(pView0->Width, pView0->Height, tLevel);
	  if(eCh==LEFT){
		pMixBmp01 = pView0;
		pMixBmp02 = pView1;
      }else{
		pMixBmp11 = pView0;
		pMixBmp12 = pView1;
	  }
	  //transparencyLevel = tLevel;
}

void SetTransparencyLevel(float tLevel=0) { transparencyLevel = tLevel; }


};
#endif
