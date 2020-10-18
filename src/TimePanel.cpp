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

#include <string>
#include <sstream>
#include <iomanip>
#include "TimePanel.h"
#include "Util.h"

using namespace nsWaveDisplay;


unsigned TTrackTimePanel::GetPlaybackIndicatorPos(unsigned trackPos) {
	return trackPos + cAmpWidth;
}
std::string TTrackTimePanel::GetTimeLabel(unsigned min, unsigned sec, unsigned milis){

	std::stringstream timeStream;
	const char *minMark = sec < 10 ? ":0" : ":";
	float rest;
	if(min>0){
		rest = milis / 1;
		if(milis < 100)
			timeStream << min  << minMark << sec << ".0" << rest;
		else
			timeStream << min  << minMark << sec << "." << rest;

	}else{
		rest = milis / 1;//millis / 10 when trackSecond is 10 or 50
		if (milis < 100) {
			timeStream << sec  << ".0" << rest;
		}else
			timeStream << sec  << "." << rest;
	}
	
	return timeStream.str();
}
void TTrackTimePanel::ConvertTimeFromMilis(unsigned segment, unsigned &min, unsigned &sec, unsigned &rest){


		const unsigned minMilis	 = 60 * 1000;
		rest = 0;
		min  =  (trackSegment * segment) / ( minMilis );
		unsigned t =	trackSegment * segment;
		if(min == 0 ){
				sec = (t) / 1000 ;
				rest = (t) % 1000 ;
		}else{
				sec  = ( t - minMilis * min ) / 1000;
				rest = ( t - minMilis * min ) % 1000;
		}
}
void TTrackTimePanel::SetDivLength(){

	numSegments = totalTrackTimeMilis / trackSegment;
	trackStartPos = cAmpWidth;
  //	totalWidth = pTimeBmp->Width - (cWaveformRightMargin+trackStartPos+15);
	trackWidth = totalWidth * zoomLevel;

	const float pixelsPerSegment = static_cast<float>(trackWidth) / numSegments;

	keepEvery = GetEveryNthSegment(pixelsPerSegment);
	auxDivLength = pixelsPerSegment;
	keepEveryMark = keepEvery * pixelsPerSegment < 60 ? keepEvery*2 : keepEvery;
}
unsigned TTrackTimePanel::GetEveryNthSegment(float pps){

	int i = 0;
	while(true){
		if( ++i * pps >= textWidthMin )
			break;
	}
	return i;
}
void TTrackTimePanel::DrawTimeLabels(){

	int timeStart;
	unsigned min, sec, rest;
	const int  yOff = 7;

	SetDivLength();
	//the position in pixels corresponding to the current track time position (seconds)
	if(startTime>0){      //
		trackStartPos =   cAmpWidth - trackWidth * static_cast<float>(startTime) / totalTrackTimeMilis;
	}
 //	lblStart = trackStartPos;

	pTimeBmp->Canvas->Rectangle(0, 0, pTimeBmp->Width, pTimeBmp->Height);

		const int runTo = numSegments + keepEveryMark;
		for(int i=0; i<runTo; i++){
			int cPos = trackStartPos + i*auxDivLength;

			if(cPos < 10 || cPos > pTimeBmp->Width-10) continue;
				if(i%keepEvery==0){
					pTimeBmp->Canvas->MoveTo(cPos, 0);
					pTimeBmp->Canvas->LineTo(cPos, mainIndLength);
				}
			if(i%keepEveryMark==0){
				ConvertTimeFromMilis(i, min, sec, rest);
				std::string& time = GetTimeLabel(min, sec, rest);
				TrimTrailingZeros(time);
				pTimeBmp->Canvas->TextOut(cPos-textWidthSecHalf, yOff, time.data());
			}
   }
}
void TTrackTimePanel::DrawPlaybackIndicatorAtPos(Graphics::TCanvas * pCanvas, int pos){

	const unsigned len 		= 6;
	const unsigned h		= pTimeBmp->Height-1;

	TPoint points[3];

	points[0] = Point(pos-len, h-len);
	points[1] = Point(pos+len, h-len);
	points[2] = Point(pos, h);

	pCanvas->Brush->Color = clGreen;
	pCanvas->Polygon(points, 3);
}

void TTrackTimePanel::TrimTrailingZeros(std::string &s){

	int i = 1;

	int len = s.length();
	char * ss = (char *)(s.c_str());

	if(ss[len-i] == '0' && ( ss[len-(i+1)] != '.' || ss[len-(i+1)] != ':' ) )
			ss[len-i] = '\0';

}
#pragma package(smart_init)
