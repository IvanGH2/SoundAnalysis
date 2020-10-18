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

#ifndef WinWavePlayerH
#define WinWavePlayerH


//#include "vfw.h"
#include "windows.h"
#include "mmsystem.h"
#include <string>
#include "hash_map.h"
#include "GenericWavePlayer.h"
#include "Util.h"
#include "DspUtil.h"
#include "GenPtrAlloc.h"
#include "DataBuffersProxy.h"

using namespace nsPlayback;
using namespace std;
//---------------------------------------------------------------------------

/**
 this class implements the Windows WAVE API
*/

typedef struct{

 WAVEFORMATEX pWfex;  //Win32 API struct needed for playback
 char *pData;   //pointer to the actual wave data
 unsigned int cSize;
 //bool selected; //selected for playback
}WAVE_DATA, *PWAVE_DATA;

typedef WAVE_DATA CONVERTED_WAVE_DATA;
typedef PWAVE_DATA PCONVERTED_WAVE_DATA;

class WinWavePlayer :  public GenericWavePlayer, public DataBuffersProxy {

	//vars for WIN32 API
	WAVEHDR whdrFirst;

	WAVEHDR whdrSecond;

	HWAVEOUT hWaveOut;

	MMTIME mmTime;

	HWND hWnd;

	WAVE_DATA *pWaveData, waveData;

	const unsigned internalSampleBits;  //32 bits

	const unsigned internalSampleSize; //float-4 bytes

	unsigned dataOffset, waveBufferSize, playbackSize, startBuffer, lastBuffer, currBuffer, waveId;

	unsigned currBufferSize, lastBufferOffset;

  //  unsigned lastBufferSize, samplesPerBuffer, samplesPerLastBuffer, bufferSize, numBuffers;

	bool firstBuffer;  //if true prepare the first buffer otherwise prep second

	//if true, the sound file is stored in multiple buffers so we need to keep track of the playback
	//indicator position as well as which buffer should be played
   //	bool  multipleBuffers;

	float *pData;

	float *pMix;

	hash_map<unsigned, PWAVE_DATA> hmSoundTracks;

	//vector<float*> *dataBuffers;

	std::string errorMsg;

	GenPtrAlloc<float> mixData;

	//callback pointers
	PfPlaybackMsgPos pfPlaybackMsgPos;

	PfPlaybackMsg pfPlaybackMsg;

	void ContinuePlayback();

	void Position();

	void SetWaveError(MMRESULT mm);

public:
				//static  WinWavePlayer& GetInstance();

				WinWavePlayer();

				~WinWavePlayer(){ FreeResources(); }
				/*playback control functions     */
				int StartPlayback();

				int StopPlayback();

				int PausePlayback();

				int ResumePlayback();
				/* end control functions */
				int PrepareForPlayback(bool);

				int PrepareForPlaybackMultiple(bool);

				int OpenForPlayback();

			  	int PreparePlaybackBuffers();

		  //		void SetDataBuffers(std::vector<float*> * vDataBuff, unsigned buffSize, unsigned lastBuffSize);

				void SetPlaybackRange(unsigned startSample, unsigned len);

				void SetPlaybackData(PTRACK_SELECTION pTrack);

				unsigned int GetPlaybackPosition();
				//možda ne treba ova metoda jer se pMix briše kod svakog playback-a
				void FreeResources(){ if(pMix) mixData.DeallocateArray(); }   //delete [] pMix;

				void NotifyOfPlaybackPosition();
				 //change method name
				void SetCallbackWindow(HWND hWindow) { hWnd = hWindow; }

				void SetInput( PWAVEFORMATEX pWfex,  char* pData, unsigned int size, unsigned int waveId);

				void SetInput(char *pData, unsigned int size, unsigned int waveId);

				//da li se ovo koristi uopæe   -ida
				void SetInputData(char *pData, unsigned size) {
					pWaveData->pData = pData;
					pWaveData->cSize = size;
					playbackSize = size;
				}
			   	//waveform message callback
				void ProcessWaveMsg(WPARAM  , LPARAM);

			   //callback mechanism for the player to notify the waveform container to update the playback position
				void SetPlaybackMsgPosCallback(PfPlaybackMsgPos  pfPlayerMsg) { pfPlaybackMsgPos = pfPlayerMsg; }
				//this callback notifies the GUI so that GUI controls (buttons etc) can be updated
				void SetPlaybackMsgCallback(PfPlaybackMsg pfMsg) { pfPlaybackMsg = pfMsg; }

				unsigned GetWaveId() const { return waveId; }

				HWAVEOUT GetWaveHandle() const { return hWaveOut; }
				//bool CheckSampleRatesForMultipleTracks() const ;

				void ShowMessage(const std::string& msg){
				   MessageBox( (HWND)NULL, msg.c_str(), (LPTSTR)"Sound analyzer", MB_OK | MB_ICONSTOP );
				}
};
#endif
