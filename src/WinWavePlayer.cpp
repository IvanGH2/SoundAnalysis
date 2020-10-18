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

#include "WinWavePlayer.h"
#include "Mmreg.h"
#include "SoundAnalyzerExc.h"


#define PLAYBACK_POS_INDICATOR;

 //WinWavePlayer* WinWavePlayer::pWinWavePlayer;
//-------------------------------------------------------------------------
 WinWavePlayer::WinWavePlayer() :
 internalSampleBits(32),
 internalSampleSize(4){
 waveBufferSize = 1024*8; //8kB default playback buffer size, but for larger files it would make sense to make this larger (to do)
 pWaveData = &waveData;
}
int WinWavePlayer::StartPlayback(){

		MMRESULT mm;
		OpenForPlayback();


		mm = waveOutWrite(hWaveOut, &whdrFirst,  sizeof(WAVEHDR));
		mm = waveOutWrite(hWaveOut, &whdrSecond, sizeof(WAVEHDR));

		if (mm != MMSYSERR_NOERROR){
			SetWaveError(mm);
			ShowMessage(errorMsg);
			   //RaiseException(errorMsg);
		}else{
			isPlaying = true;
			isCompleted = false;
			}
		return mm;
}

int WinWavePlayer::StopPlayback(){

		MMRESULT mm = waveOutReset(hWaveOut);
		//reset variables
		isCompleted = true;
		isPlaying =   false;
		firstBuffer = false;

		waveBufferSize = 1024*8;  //4kB default buffer size
		dataOffset = 0;
		pfPlaybackMsg(nsPlayback::STOPPED, waveId);
		return mm;
}
int WinWavePlayer::PausePlayback(){

		MMRESULT mm = waveOutPause(hWaveOut);
		isPlaying = false;
		isPaused = true;
		return mm;
}
int WinWavePlayer::ResumePlayback(){

		MMRESULT mm = waveOutRestart(hWaveOut);
		isPlaying = true;
		isPaused = false;
		return mm;
}
void WinWavePlayer::ContinuePlayback(){

		MMRESULT mm;
		mm = firstBuffer ? waveOutWrite(hWaveOut, &whdrSecond, sizeof(WAVEHDR)) :  waveOutWrite(hWaveOut, &whdrFirst, sizeof(WAVEHDR));;
		if (mm != MMSYSERR_NOERROR){
			SetWaveError(mm);
			ShowMessage(errorMsg);
		}
		firstBuffer = !firstBuffer;
}

   void  WinWavePlayer::SetPlaybackData(PTRACK_SELECTION pTrack){


	PWAVE_PLAYBACK_DATA pPlaybackData = pTrack->pPlaybackData;

	pData = pPlaybackData->pWaveData->pData;

	if(multipleBuffers){
		SetPlaybackRange( pPlaybackData->sampleRange.startSample, pPlaybackData->sampleRange.endSample);
	}else{
		pWaveData->pData = (char*)pData + (pPlaybackData->sampleRange.startSample * internalSampleSize *
		pPlaybackData->pWaveData->numChannels);

		if(pPlaybackData->sampleRange.endSample > pPlaybackData->sampleRange.startSample)
			playbackSize = (pPlaybackData->sampleRange.endSample - pPlaybackData->sampleRange.startSample) * internalSampleSize;
	}
}
  void WinWavePlayer::SetPlaybackRange(unsigned startSample, unsigned endSample){  //len==viewPortSamples

	if(endSample==0) endSample = ((numBuffers-1)*bufferSize + lastBufferSize)/sizeof(float);
	startBuffer = startSample/samplesPerBuffer;
	lastBuffer = endSample/samplesPerBuffer;
	currBuffer = startBuffer;
	currBufferSize = startBuffer != lastBuffer ? bufferSize :  (endSample-lastBuffer*samplesPerBuffer)*sizeof(float);
	//dataOffset is the  offset in samples/sizeof(float) from the buffer start
	dataOffset = (startSample-startBuffer*samplesPerBuffer)*sizeof(float);
	lastBufferOffset =  (endSample-lastBuffer*samplesPerBuffer)*sizeof(float);//
   //	lastBufferOffset = startBuffer != lastBuffer ? lastBufferSize : currBufferSize;//lastBufferSizeendSample*sizeof(float) - lastBuffer*samplesPerBuffer*sizeof(float);

 }
int WinWavePlayer::PrepareForPlayback(bool fb){

		MMRESULT mm;

		if(multipleBuffers){
			mm = PrepareForPlaybackMultiple(fb);
			return mm;
		}
		if(dataOffset + waveBufferSize >= playbackSize )
			   waveBufferSize =  playbackSize - dataOffset;
		else
			dataOffset += waveBufferSize;

		WAVEHDR * const pWhdr  = fb ? &whdrFirst : &whdrSecond;

		pWhdr->dwBufferLength = waveBufferSize;
		pWhdr->lpData = pWaveData->pData + dataOffset;
		mm = waveOutPrepareHeader(hWaveOut, pWhdr, sizeof(WAVEHDR));

		if (mm != MMSYSERR_NOERROR)
			SetWaveError(mm);

		return mm;
}
int WinWavePlayer::PrepareForPlaybackMultiple(bool fb){

		MMRESULT mm;
		unsigned waveBufferSize = this->waveBufferSize;

	   //	static unsigned currBuff = startBuffer;
		const unsigned buffSize = currBufferSize;
		const unsigned lastBuffSize = lastBufferSize;

	   //	dataOffset += waveBufferSize;
		if( currBuffer == lastBuffer && dataOffset + waveBufferSize >= lastBufferOffset )
			waveBufferSize =  lastBufferOffset - dataOffset;
		else if( dataOffset + waveBufferSize >= buffSize )
			waveBufferSize =  buffSize - dataOffset;

		if(this->waveBufferSize != waveBufferSize && currBuffer<lastBuffer){
			pWaveData->pData = (char*)dataBuffers->at(++currBuffer);
			dataOffset = 0;
			waveBufferSize = this->waveBufferSize;
		}

		WAVEHDR * const pWhdr = fb ? &whdrFirst : &whdrSecond;

		pWhdr->dwBufferLength = waveBufferSize;
		pWhdr->lpData = pWaveData->pData + dataOffset;
		mm = waveOutPrepareHeader(hWaveOut, pWhdr, sizeof(WAVEHDR));

		dataOffset += waveBufferSize;

		if (mm != MMSYSERR_NOERROR)
			SetWaveError(mm);

		return mm;
}
int WinWavePlayer::PreparePlaybackBuffers(){

	unsigned wavBuffSize =  waveBufferSize;
	const unsigned buffSize = currBufferSize;

	if(multipleBuffers){

		static unsigned currBuff = startBuffer;

		pWaveData->pData = (char*)dataBuffers->at(startBuffer);

		if( dataOffset + wavBuffSize >= buffSize )
			wavBuffSize =  buffSize - dataOffset;

		whdrFirst.lpData = pWaveData->pData+dataOffset;
		whdrFirst.dwBufferLength = wavBuffSize;
		//buffer #2
		wavBuffSize = waveBufferSize;
		dataOffset += wavBuffSize;
		//reset the buffer length to default buffer length

		if(dataOffset>=buffSize){
			pWaveData->pData = (char*)dataBuffers->at(++currBuffer);
			dataOffset = 0;
			if(currBuffer == lastBuffer){
			   if( wavBuffSize >= lastBufferOffset )
					wavBuffSize =  lastBufferOffset;
			}
			}else{
				if( dataOffset + wavBuffSize >= buffSize )
					wavBuffSize =  buffSize - dataOffset;
			}
		 whdrSecond.lpData = pWaveData->pData + dataOffset;
		 whdrSecond.dwBufferLength = wavBuffSize;

		}else{

    	if(dataOffset + waveBufferSize > playbackSize )
			waveBufferSize =  playbackSize - dataOffset;

		whdrFirst.lpData = pWaveData->pData;
		whdrFirst.dwBufferLength = waveBufferSize;
		  //prepare second buffer
		dataOffset += waveBufferSize;

		if(dataOffset + waveBufferSize > playbackSize )
			waveBufferSize =  playbackSize - dataOffset;

		whdrSecond.lpData = pWaveData->pData + waveBufferSize;
		whdrSecond.dwBufferLength = waveBufferSize;
	}
}
int WinWavePlayer::OpenForPlayback() {

			MMRESULT mm;
			//open the device for playback -- the actual playback won't start until we call waveOutWrite
			mm = waveOutOpen(&hWaveOut, WAVE_MAPPER, &pWaveData->pWfex, (unsigned long)hWnd, 0, CALLBACK_WINDOW);

			if(hWaveOut){
				PreparePlaybackBuffers();

				mm = waveOutPrepareHeader(hWaveOut, &whdrFirst,  sizeof(WAVEHDR));
				mm = waveOutPrepareHeader(hWaveOut, &whdrSecond, sizeof(WAVEHDR));
			}

			if(mm != MMSYSERR_NOERROR){
				SetWaveError(mm);
				ShowMessage(errorMsg);
			}
			return mm;
}

void WinWavePlayer::ProcessWaveMsg(WPARAM wp , LPARAM lp){
	/*
	  wp =  handle to device (HWAVEOUT )
	  lp = pointer to the buffer that's been played
	*/
	waveOutUnprepareHeader((HWAVEOUT)wp, (LPWAVEHDR)lp, sizeof(WAVEHDR));

	NotifyOfPlaybackPosition();

		if( isCompleted && !isPlaying) return;   //Stop clicked

		if(isCompleted ){   //if a track has reached the end, stop the playback and set the flag to false
		   StopPlayback();
		   isCompleted = false;
		   return;
		}

		if((!multipleBuffers && dataOffset + waveBufferSize == playbackSize) ||
		( multipleBuffers && currBuffer==lastBuffer && dataOffset  >= lastBufferOffset ) ){  //playing the last buffer
			   isCompleted = true; //set the flag and return immediately so that we don't try to prepare new playback buffers
			   return;
		 }

		 //alternate between the playback buffers so that the audio device never runs out of data
		if (!firstBuffer){
			PrepareForPlayback(true);
		}else{
			PrepareForPlayback(false);
		}
	   ContinuePlayback();
}
void WinWavePlayer::NotifyOfPlaybackPosition(){

	#if defined(PLAYBACK_POS_INDICATOR)
		  if(pfPlaybackMsgPos){
			pfPlaybackMsgPos(nsPlayback::POSITIONED, waveId, GetPlaybackPosition());
		  }
	#endif
}
/**
this method is called to initialize the playback data
if _USE_INTERNAL_FORMAT_ is defined, the WAVEFORMAT structure is modified  to match that of  WAVE_FORMAT_IEEE_FLOAT
and the actual data (pData) have alreday been converted to floats (inside SoundIO)
*/
void WinWavePlayer::SetInput( WAVEFORMATEX* pWfex,  char* pData, unsigned size, unsigned int waveId){

		 //pWaveData = new WAVE_DATA;
		 this->waveId = waveId;
		 pWaveData->pWfex = *pWfex;
		 pWaveData->pData = pData;
		 pWaveData->cSize = size;

	   //	 ppWaveData->waveId = waveId;
		 playbackSize = pWaveData->cSize;
		 #if defined _USE_INTERNAL_FORMAT_
			playbackSize *= (internalSampleBits/pWaveData->pWfex.wBitsPerSample);
			pWaveData->pWfex.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
			pWaveData->pWfex.wBitsPerSample = internalSampleBits;
			pWaveData->pWfex.nAvgBytesPerSec =  pWfex->nChannels * pWfex->nSamplesPerSec * internalSampleSize;
			pWaveData->pWfex.nBlockAlign =  pWfex->nChannels * internalSampleSize;
			pWaveData->pWfex.cbSize = 0;

		 #endif
		 hmSoundTracks[waveId] = pWaveData; //ovo maknuti ???
}

void WinWavePlayer::Position(){

	//pfs->Seek(OFFSET_WAVEDATA + pwi->StartSample * pwi->BytesPerSample, soFromBeginning);
}
void WinWavePlayer::SetWaveError(MMRESULT mm)
{
		switch (mm) {
			case MMSYSERR_INVALHANDLE:
				errorMsg = "Wave exception: Invalid handle";
				break;
			case MMSYSERR_NOMEM:
				errorMsg = "Wave exception: Not enough memory";
				break;
			case MMSYSERR_NODRIVER:
				errorMsg = "Wave exception: No driver present";
				break;
			case WAVERR_UNPREPARED:
				errorMsg = "Wave exception: Unprepared buffer";
				break;
			case MMSYSERR_HANDLEBUSY:
				errorMsg = "Wave exception: Handle busy";
				break;
			case MMSYSERR_ALLOCATED:
				errorMsg = "Wave exception: Device already allocated";
				break;
			case WAVERR_BADFORMAT:
				errorMsg = "Wave exception: Unsupported wave format";
				break;
			default:
				errorMsg = "Wave exception: Unknown error: Code " + mm;
				;
		}
}
unsigned int WinWavePlayer::GetPlaybackPosition()
{
	mmTime.wType = TIME_SAMPLES;
	waveOutGetPosition(hWaveOut, &mmTime, sizeof(MMTIME));
	return mmTime.u.sample * pWaveData->pWfex.nChannels ;
}

#pragma package(smart_init)
