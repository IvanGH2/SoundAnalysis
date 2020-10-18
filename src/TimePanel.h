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

#ifndef TimePanelH
#define TimePanelH

#include <Graphics.hpp>
//---------------------------------------------------------------------------

class TTrackTimePanel {

				Graphics::TBitmap *pTimeBmp;

				unsigned trackTimeMilis, totalTrackTimeMilis, startTime, totalWidth, zoomLevel;

				int trackStartPos, keepEvery, keepEveryMark;

				unsigned numSegments, trackWidth, trackDivReps, maxNum, mainIndLength, auxIndLength;

				unsigned  textWidthSecHalf, textWidthMin;

				float  trackSegment,  auxDivLength;

				bool seconds;
			   //	void SetMaxNumberOfTimeLabels();
				void SetMainDivLength();

				unsigned SkipEvery(int divLen, int txtLen);

				std::string GetTimeLabel(unsigned min, unsigned sec, unsigned milis);

				void ConvertTimeFromMilis(unsigned segment, unsigned &min, unsigned &sec, unsigned &rest);

				void SetDivLength();

				unsigned GetEveryNthSegment(float pps);

				public:

				TTrackTimePanel()  {
					startTime		= 0;
					pTimeBmp 		= new Graphics::TBitmap;
				//	totalWidth = pTimeBmp->Width - (cWaveformRightMargin+trackStartPos);
					textWidthMin 	= pTimeBmp->Canvas->TextWidth("00.00");
					//textWidthSec = pTimeBmp->Canvas->TextWidth("00.0");;
				   //	textWidthMinHalf = textWidthMin/2;
					textWidthSecHalf = pTimeBmp->Canvas->TextWidth("00.0")/2+3;
					mainIndLength    = 4;
					auxIndLength	 = 3;
					zoomLevel 		 = 1;
					trackSegment 	 = 0.1f;
				}

				void SetTotalTrackTime(unsigned trackTime) { totalTrackTimeMilis = trackTime; }
				//track time corresponds to the track time currently visible in the viewport
				void SetTrackTime(unsigned trackTime) { trackTimeMilis = trackTime; }

				unsigned GetTrackTime() const { return trackTimeMilis; }

				void SetWidth(unsigned w)  { pTimeBmp->Width  = w; }

				void SetHeight(unsigned h) { pTimeBmp->Height = h; }

				void SetTrackWidth(unsigned w) { totalWidth = w; }

				void DrawTimeLabels();

				void SetZoomLevel(unsigned zoomLevel) { this->zoomLevel = zoomLevel; }

				void SetStartTime(unsigned startTime) { this->startTime = startTime; }

				unsigned GetStartTime()  const { return startTime; }

				unsigned GetEvery() const { return keepEvery; }

				unsigned GetLen() const { return auxDivLength; }

				Graphics::TBitmap *GetTimeBitmap() { return pTimeBmp; }

				void DrawPlaybackIndicatorAtPos(Graphics::TCanvas *pCanvas, int );

				unsigned GetPlaybackIndicatorPos(unsigned trackPos);

				void TrimTrailingZeros(std::string &s);

};
#endif
