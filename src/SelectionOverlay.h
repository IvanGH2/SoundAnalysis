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

#ifndef SelectionOverlayH
#define SelectionOverlayH


#include <Graphics.hpp>
#include "Util.h"

class TSelectionOverlay{

public:
  //enum EChannel { LEFT, RIGHT };

 TSelectionOverlay(){
   //	CreateOverlayBitmaps(LEFT);
	pSelOverlay0 = 0;
	pSelOverlay1 = 0;
	//CreateOverlayBitmaps(LEFT);
 }
   ~TSelectionOverlay(){
		DestroyOverlayBitmaps(LEFT);
		DestroyOverlayBitmaps(RIGHT);
 }
 Graphics::TBitmap *GetSelectionOverlay(EChannel eCh) const { return eCh==LEFT ? pSelOverlay0 : pSelOverlay1;  }

 Graphics::TBitmap *GetSelectionOverlay(
	const Graphics::TBitmap *pBmp,
	TColor selColour,
	TRect &rect,
	EChannel eChannel);

  private:
  Graphics::TBitmap *pSelOverlay0, *pSelOverlay1;
  TRect selOverlayRect;

  unsigned width;
  unsigned height;


  bool IsOverlayRedrawNeeded(const TRect &, EChannel);

  void CreateOverlayBitmaps(EChannel eChannel){

	DestroyOverlayBitmaps(eChannel);

	if(eChannel==LEFT){
		pSelOverlay0 = new Graphics::TBitmap;
		pSelOverlay0->PixelFormat = pf32bit;
	}else{
		pSelOverlay1 = new Graphics::TBitmap;
		pSelOverlay1->PixelFormat = pf32bit;
    }
  }

  void DestroyOverlayBitmaps(EChannel eChannel){

	if(eChannel==LEFT){
		if(pSelOverlay0){
			delete pSelOverlay0;
			pSelOverlay0 = 0;
		}
	}else{
		if(pSelOverlay1){
			delete pSelOverlay1;
			pSelOverlay1 = 0;
			}
		}
  }

};
//---------------------------------------------------------------------------
#endif
