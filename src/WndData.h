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

#ifndef WndDataH
#define WndDataH

#include "GenPtrAlloc.h"
#include "Util.h"

using namespace WindowFunc;
//---------------------------------------------------------------------------
class WndData{

				WindowFunc::EWindow eWnd;

                unsigned wndLen;

                GenPtrAlloc<float> wData;

                bool wndTypeChanged, wndLenChanged, calcWnd;       

                unsigned numChannels;

                float wndSum0, wndSum1;

				void SetWndFunc();

public:
				WndData() : numChannels(1), wndSum0(0), wndSum1(0){ wData.SetPtr(0); }

				WndData(EWindow eWnd, unsigned wndLen) : eWnd(eWnd),
				wndLen(wndLen), numChannels(1), wndSum0(0), wndSum1(0){ wData.SetPtr(0);}

				~WndData(){
					Free();
                }

                void Free(){

					if( wData.GetPtr() )
						delete [] wData.GetPtr();

                }

				void SetWndType(EWindow eW);

				void SetWndLen(unsigned wLen) ;

				void SetWndParams(EWindow eWnd, unsigned wLen);

				void SetNumChannels(unsigned nChannels);

				void CreateWndData(float *pRealLeft, float *pRealRight);

				float *GetWndFunc();

				void SetWndFuncSums(float &s0, float &s1);
};


#endif
