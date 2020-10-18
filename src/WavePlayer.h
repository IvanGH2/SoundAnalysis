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
	
#ifndef WavePlayerH
#define WavePlayerH

#include "windows.h"
#include "mmsystem.h"
#include "Util.h"
#include "GenericWavePlayer.h"

using namespace nsPlayback;
//---------------------------------------------------------------------------
template <class T>
class TWavePlayer {

static  TWavePlayer<T> *pWavePlayer;

GenericWavePlayer * pWavePlayerImpl;

std::vector<T*> vWavePlayerImpl;
std::vector<PTRACK_SELECTION> *vSelTracks;

HWND callbackWnd;

PfPlaybackMsgPos pfPlaybackPosCallback;
PfPlaybackMsg pfPlaybackMsg;

PWAVEFORMATEX pWfex;
char* pData;
unsigned size, waveId, numSelected, numTracks;

TWavePlayer( HWND hWnd ) :  callbackWnd( hWnd ) {}

~TWavePlayer(){

   const t_size = vWavePlayerImpl.size();
   for(int i=0;i<size;i++){
	   GenericWavePlayer * pGenWavePlayer = vWavePlayerImpl.at(i);
	   if(pGenWavePlayer)
		   delete pGenWavePlayer;
   }
}

public:

static const TWavePlayer& GetInstance( HWND hWnd){
	  if(!pWavePlayer) {
		pWavePlayer = new TWavePlayer(hWnd);
	  }
	  return *pWavePlayer;
}
void AddTrack(T *rPlayerImpl, WAVEFORMATEX* wfex, char* pData, unsigned int size, unsigned int waveId){

		pWavePlayerImpl = rPlayerImpl;
		vWavePlayerImpl.push_back(rPlayerImpl);
		
		rPlayerImpl->SetCallbackWindow(callbackWnd);
				//does not need to be called for every track
		rPlayerImpl->SetPlaybackMsgPosCallback(pfPlaybackPosCallback);
		rPlayerImpl->SetPlaybackMsgCallback(pfPlaybackMsg);
		rPlayerImpl->SetInput(wfex, pData, size, waveId);
		numTracks = vWavePlayerImpl.size();

}
void RemoveTrack(unsigned waveId){

	std::vector<T*>::iterator wIter;
	for(wIter=vWavePlayerImpl.begin();wIter!=vWavePlayerImpl.end();wIter++){
		T * pWavePlayerImpl = *wIter;
		if(pWavePlayerImpl->GetWaveId() == waveId){
			vWavePlayerImpl.erase(wIter);
			numTracks--;
			break;
		}
	}
}

void FreeResources(){
   pWavePlayerImpl->FreeResources();
}
//these methods below are solely for the purpose of controlling the implementation instances (in this case WinWavePlayer instances)
void StartPlayback(){
	
	for(int i=0;i<numTracks;i++){
		T * rWavePlayerImpl = vWavePlayerImpl.at(i);
		for(int j=0;j<numSelected;j++){
				PTRACK_SELECTION ps = vSelTracks->at(j);
				if(ps->waveId == rWavePlayerImpl->GetWaveId()){
				//kad nije multiple buffers treba i pPlaybackData gdje je pData (selekcija)
					rWavePlayerImpl->SetPlaybackData(ps);
					rWavePlayerImpl->StartPlayback();
			}
		}
	}
}
void StopPlayback(){
	for(int i=0;i<numTracks;i++){
		T &rWavePlayerImpl = *vWavePlayerImpl.at(i);
		for(int j=0;j<numSelected;j++){
				PTRACK_SELECTION ps = vSelTracks->at(j);
				if(ps->waveId == rWavePlayerImpl.GetWaveId())
					rWavePlayerImpl.StopPlayback();
		}
	}
}
void PausePlayback(){
	for(int i=0;i<numTracks;i++){
		T &rWavePlayerImpl = *vWavePlayerImpl.at(i);
		for(int j=0;j<numSelected;j++){
				PTRACK_SELECTION ps = vSelTracks->at(j);
				if(ps->waveId == rWavePlayerImpl.GetWaveId())
					rWavePlayerImpl.PausePlayback();
		}
	}
}
void ResumePlayback(){
	for(int i=0;i<numTracks;i++){
		T &rWavePlayerImpl = *vWavePlayerImpl.at(i);
		for(int j=0;j<numSelected;j++){
				PTRACK_SELECTION ps = vSelTracks->at(j);
				if(ps->waveId == rWavePlayerImpl.GetWaveId())
					rWavePlayerImpl.ResumePlayback();
		}
	}
}
void SetPlaybackPosCallback(PfPlaybackMsgPos pf){
	   pfPlaybackPosCallback =  pf;
}
void SetPlaybackMsgCallback(PfPlaybackMsg pf){
	   pfPlaybackMsg =  pf;
}

 /*
void SetInputData(char *pData, unsigned size) {

   static_cast<T*>(pWavePlayerImpl)->SetInputData(pData, size);
}*/
void SetDataBuffers_(std::vector<float*> * vDataBuff, unsigned buffSize, unsigned lastBuffSize){

	 static_cast<T*>(pWavePlayerImpl)->SetDataBuffers( vDataBuff, buffSize, lastBuffSize );
}
void SetPlaybackTracks(vector<PTRACK_SELECTION> *pSelTracks){
	numSelected =	pSelTracks->size();
	vSelTracks = pSelTracks;
	//rWavePlayerImpl.SetPlaybackTracks(pSelTracks);
}
void ProcessWaveMsg(TMessage& msg){

	for(int i=0;i<numTracks;i++){
		T *rWavePlayerImpl = vWavePlayerImpl.at(i);
		if( (int)(static_cast<T*>(rWavePlayerImpl)->GetWaveHandle()) == msg.WParam ) {
			rWavePlayerImpl->ProcessWaveMsg(msg.WParam, msg.LParam);
			break;
        }
	}
}
};
template <class T>
TWavePlayer<T>* TWavePlayer<T>::pWavePlayer = 0;
#endif
