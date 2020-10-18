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

#include "WndData.h"
#include "DspUtil.h"

	void WndData::SetWndFunc(){

		calcWnd = false;

		if(!wData.GetPtr() && eWnd != RECTANGULAR){

			wData.SetPtr( DspUtil::CreateWindowFunc(wndLen, eWnd, wndSum0, wndSum1) );

			calcWnd = true;

		}else{
			if(eWnd != RECTANGULAR){
				if(wndTypeChanged || wndLenChanged){
					Free();
					wData.SetPtr(DspUtil::CreateWindowFunc(wndLen, eWnd, wndSum0, wndSum1));
					calcWnd = true;
							}
						}
				}
	}

	void WndData::SetWndType(EWindow eW) {

		if(eW!=eWnd){
			eWnd = eW;
			wndTypeChanged = true;
		}else
			wndTypeChanged = false;
		}

	void WndData::SetWndLen(unsigned wLen) {

		if(wLen!=wndLen){

			wndLen = wLen;
			wndLenChanged = true;

			}else
				wndLenChanged = false;
			}

	void WndData::SetWndParams(EWindow eWnd, unsigned wLen){

		SetWndLen(wLen);

		SetWndType(eWnd);
		}


	void WndData::SetNumChannels(unsigned nChannels) { numChannels=nChannels; }

	void WndData::CreateWndData(float * pRealLeft, float * pRealRight){

		if(!wData.GetPtr()) SetWndFunc();

		float * pWndData = wData.GetPtr();

		if(calcWnd){
			for(int i=0; i < wndLen; i++){

				pRealLeft[i] *= pWndData[i];

				if(numChannels == 2)
					pRealRight[i] *= pWndData[i];

			}
		}
	}

	float *WndData::GetWndFunc()  {

				if(!wData.GetPtr()) SetWndFunc();

				return wData.GetPtr();
	}

	void WndData::SetWndFuncSums(float &s0, float &s1){

		SetWndFunc();
		s0 = wndSum0;
		s1 = wndSum1;

	}

#pragma package(smart_init)
