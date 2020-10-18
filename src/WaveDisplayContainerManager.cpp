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

#include "WaveDisplayContainerManager.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

PWaveDisplayContainerManager WaveDisplayContainerManager::pWaveContainerManager = 0;

WaveDisplayContainerManager::WaveDisplayContainerManager(void* Owner){

	 parent 		 = Owner;
	 containersCount = 0;
	 containerId 	 = 1111;
	 layoutPos 		 = 0;
	 totalModHeight  = 0;
}
const WaveDisplayContainerManager& WaveDisplayContainerManager::GetInstance(void* Owner){

	if(!pWaveContainerManager)
		pWaveContainerManager = new WaveDisplayContainerManager(Owner);

	return *pWaveContainerManager;
}

unsigned WaveDisplayContainerManager::GetContainersHeight(){

		int totalHeight = 0, pos = 0;
		hash_map<unsigned, PTWaveDisplayContainer>::iterator hmWaveIter;
		for ( hmWaveIter = hmWaveContainer.begin(); hmWaveIter != hmWaveContainer.end();  hmWaveIter++) {
			PTWaveDisplayContainer pCurrentContainer =  hmWaveIter->second;
			if( !pCurrentContainer->IsHidden() ){
				totalHeight +=  pCurrentContainer->Height + 2 * pos;
				pos++;
			}

		}
		return totalHeight;
}
std::vector<PFILE_INFO> *WaveDisplayContainerManager::GetContainersTrackInfo(){

		vContainersFileInfo.clear();
		hash_map<unsigned, PTWaveDisplayContainer>::iterator hmWaveIter;
		for ( hmWaveIter = hmWaveContainer.begin(); hmWaveIter != hmWaveContainer.end();  hmWaveIter++) {
			PTWaveDisplayContainer pCurrentContainer =  hmWaveIter->second;
			vContainersFileInfo.push_back( pCurrentContainer->GetFileInfo() );

		}
		return &vContainersFileInfo;
}
void WaveDisplayContainerManager::ShowSpectrogram(){

	GetActiveContainer()->DoDrawSfft();
}
unsigned int  WaveDisplayContainerManager::AddContainer(){

	pWaveContainer = new TWaveDisplayContainer((TComponent*)parent, containerId, pSoundData);
	pWaveContainer->SetWaveMessageCallback(&OnWaveMessage);
	pWaveContainer->SetLayoutPosition(layoutPos++, totalModHeight);

	pWaveContainer->Position(GetContainersHeight()/*-(containersCount * cWaveformHeight)*/);
	hmWaveContainer[containerId] = pWaveContainer;
	pWaveContainer->DrawWaveform();
	pWaveContainer->SetActive();
	pWaveContainer->SetContextMenu(pContextMenu);
	//if there are more containers, unselect all the others
	if(++containersCount > 1) Unselect(containerId);

	CalculateTotalHeight();
	pfMainWndNotify(nsWaveDisplay::CONTAINER_ADDED, &totalContainerHeight);
	pfPlaybackMsg(nsPlayback::OPENED, containerId);
	return containerId++;
}
std::vector<PTWaveDisplayContainer> *WaveDisplayContainerManager::GetContainers(){

	hash_map<unsigned, PTWaveDisplayContainer>::iterator hmWaveIter;
	vContainers.clear();
	for ( hmWaveIter = hmWaveContainer.begin(); hmWaveIter != hmWaveContainer.end();  hmWaveIter++) {

		vContainers.push_back(hmWaveIter->second);
	}
	return &vContainers;
}
/**
 this method will reposition all remaining containers to their new positions (e.g. when a container has been destroyed)
*/
void  WaveDisplayContainerManager::RepositionContainers(){   //možda ne treba ovo jer imamo dolje

		int pos = 0, prevPos = 0;
	  //	CalculateModifiedHeight();
		hash_map<unsigned int, PTWaveDisplayContainer>::iterator hmWaveIter;
		for ( hmWaveIter = hmWaveContainer.begin(); hmWaveIter != hmWaveContainer.end();  hmWaveIter++) {
			PTWaveDisplayContainer pCurrentContainer =  hmWaveIter->second;
			if( !pCurrentContainer->IsHidden() ){
				pCurrentContainer->SetLayoutPosition(pos++, totalModHeight);
			  //	pCurrentContainer->Position(totalModHeight-(pCurrentContainer->Height - cWaveformHeight));
				pCurrentContainer->Position(prevPos);
				prevPos += pCurrentContainer->Height;
			}
		}
		layoutPos = pos;
}
//after a container has been deleted, this method removes the container from the hash_map and repositions the remaining containers
void  WaveDisplayContainerManager::RemoveContainer(unsigned containerId){

	 /*PTWaveDisplayContainer pCurrentContainer  =  hmWaveContainer.find(containerId)->second;
	 if(pCurrentContainer) delete pCurrentContainer;*/
	 hmWaveContainer.erase(containerId);
	 if(hmWaveContainer.size()>0){
		containersCount--;
	  //	CalculateModifiedHeight();
		RepositionContainers();
		PTWaveDisplayContainer pCurrentContainer =  hmWaveContainer.begin()->second;
		pCurrentContainer->ActivateContainer();
		pfPlaybackMsg(nsPlayback::CLOSED, containerId);
	 }else{
		containersCount = 0;
		layoutPos = 0;
		totalModHeight  = 0;
		pfPlaybackMsg(nsPlayback::CLOSED_ALL, containerId);
	 }
		CalculateTotalHeight();
		pfMainWndNotify(nsWaveDisplay::CONTAINER_REMOVED, &totalContainerHeight);
}
void  WaveDisplayContainerManager::CalculateModifiedHeight(){

	   totalModHeight  = 0;
	   hash_map<unsigned int, PTWaveDisplayContainer>::iterator hmWaveIter;
		for ( hmWaveIter = hmWaveContainer.begin(); hmWaveIter != hmWaveContainer.end();  hmWaveIter++) {
			PTWaveDisplayContainer pCurrentContainer =  hmWaveIter->second;
			if( !pCurrentContainer->IsHidden() )
				totalModHeight += ( pCurrentContainer->Height - cWaveformHeight ) ;
		}
}
unsigned  WaveDisplayContainerManager::CalculateTotalHeight(){

	   totalContainerHeight  = 0;
	   hash_map<unsigned int, PTWaveDisplayContainer>::iterator hmWaveIter;
		for ( hmWaveIter = hmWaveContainer.begin(); hmWaveIter != hmWaveContainer.end();  hmWaveIter++) {
			PTWaveDisplayContainer pCurrentContainer =  hmWaveIter->second;
			if( !pCurrentContainer->IsHidden() )
				totalContainerHeight +=  pCurrentContainer->Height;
		}
		return  totalContainerHeight;
}
void  WaveDisplayContainerManager::RemoveAllContainers(){

		hash_map<unsigned int, PTWaveDisplayContainer>::reverse_iterator hmWaveIter;
		for ( hmWaveIter = hmWaveContainer.rbegin(); hmWaveIter != hmWaveContainer.rend();  hmWaveIter++) {
			PTWaveDisplayContainer pCurrentContainer =  hmWaveIter->second;
			delete pCurrentContainer;
		}
		 hmWaveContainer.clear();
		 containersCount = 0;
		 layoutPos = 0;
		 totalContainerHeight = 0;
		 pfMainWndNotify(nsWaveDisplay::CONTAINER_REMOVED, &totalContainerHeight);
}
//this method changes only the width of a container
void  WaveDisplayContainerManager::ResizeContainers(unsigned int newWidth, unsigned int newHeight){

	  // this->getActiveContainer()->resize(newWidth, newHeight);
	   hash_map<unsigned int, PTWaveDisplayContainer>::iterator hmWaveIter;
		for ( hmWaveIter = hmWaveContainer.begin(); hmWaveIter != hmWaveContainer.end();  hmWaveIter++) {
			PTWaveDisplayContainer pCurrentContainer =  hmWaveIter->second;
			pCurrentContainer->ParentResized(newWidth, newHeight);
		}
}
/**
   this method is called in response to a REPOSITION message
   this message is triggered by a TWaveDisplayContainer that is in the process of changing its height,
   so the remaining containers must be repositioned
*/
void  WaveDisplayContainerManager::RepositionContainers(unsigned containerId){
	  // this->getActiveContainer()->resize(newWidth, newHeight);
	  PTWaveDisplayContainer pCurrentContainer;
	   hash_map<unsigned int, PTWaveDisplayContainer>::iterator hmWaveIter;
	   unsigned accHeight = 0;
		for ( hmWaveIter = hmWaveContainer.begin(); hmWaveIter != hmWaveContainer.end();  hmWaveIter++){
			if( hmWaveIter->first <= containerId ){
				pCurrentContainer = hmWaveIter->second;
				if(!pCurrentContainer->IsHidden()){
					accHeight += pCurrentContainer->Height;//+pCurrentContainer->Top+height+1;
				}
				continue;
			}
			if(!pCurrentContainer->IsHidden()){
				pCurrentContainer =  hmWaveIter->second;
				pCurrentContainer->Position(accHeight);
				accHeight += pCurrentContainer->Height;
			}
		}
}
void  WaveDisplayContainerManager::AdjustContainersVertically(){

		 hash_map<unsigned int, PTWaveDisplayContainer>::iterator hmWaveIter;
		for ( hmWaveIter = hmWaveContainer.begin(); hmWaveIter != hmWaveContainer.end();  hmWaveIter++) {
			PTWaveDisplayContainer pCurrentContainer =  hmWaveIter->second;
		   //	pCurrentContainer->Position(totalModHeight);
		   if(!pCurrentContainer->IsHidden()){
		   //	pCurrentContainer->Position( GetContainersHeight() );
			pCurrentContainer->AdjustVertically();
		   }
		}
		CalculateTotalHeight();
		pfMainWndNotify(nsWaveDisplay::CONTAINER_RESIZED, &totalContainerHeight);
}
void  WaveDisplayContainerManager::AdjustContainersNormal(){

		hash_map<unsigned int, PTWaveDisplayContainer>::iterator hmWaveIter;
		for ( hmWaveIter = hmWaveContainer.begin(); hmWaveIter != hmWaveContainer.end();  hmWaveIter++) {
			PTWaveDisplayContainer pCurrentContainer =  hmWaveIter->second;
			if(!pCurrentContainer->IsHidden()){
			  //	pCurrentContainer->Position( GetContainersHeight() );
				pCurrentContainer->AdjustNormal();
			}
		}
		CalculateTotalHeight();
		pfMainWndNotify(nsWaveDisplay::CONTAINER_RESIZED, &totalContainerHeight);
}
void  WaveDisplayContainerManager::AdjustContainersMinimum(){
		 hash_map<unsigned int, PTWaveDisplayContainer>::iterator hmWaveIter;
		for ( hmWaveIter = hmWaveContainer.begin(); hmWaveIter != hmWaveContainer.end();  hmWaveIter++) {
			PTWaveDisplayContainer pCurrentContainer =  hmWaveIter->second;
			if(!pCurrentContainer->IsHidden()){
			 //	pCurrentContainer->Position( GetContainersHeight() );
				pCurrentContainer->AdjustMinimum();
			}
		}
		CalculateTotalHeight();
		pfMainWndNotify(nsWaveDisplay::CONTAINER_RESIZED, &totalContainerHeight);
}
void WaveDisplayContainerManager::SetActiveContainer(PTWaveDisplayContainer pContainer) {

	TRACK_TIME tTime;
	SoundIO *pSoundData = pContainer->GetSoundData();
	tTime.trackTime     = pSoundData->GetTrackTime();
	tTime.startTime     = pSoundData->GetTimeForSample(pContainer->GetDisplayData()->GetStartSample());
	tTime.selectionTime = pSoundData->GetTimeForSamples(pContainer->GetDisplayData()->GetViewPortSamples());
	pWaveContainer = pContainer;
	pfMainWndNotify(nsWaveDisplay::INV_TRACK_TIME, &tTime);
}
/**
this method is called in response to a SELECT message-- when a container is selected (usually by
clicking on it), all the other containers get unselected
*/
void  WaveDisplayContainerManager::Unselect(unsigned int containerId){

		 hash_map<unsigned int, PTWaveDisplayContainer>::iterator hmWaveIter;
		for ( hmWaveIter = hmWaveContainer.begin(); hmWaveIter != hmWaveContainer.end();  hmWaveIter++) {

			if(hmWaveIter->first == containerId)   //this one is an active container so skip it
				continue;
			PTWaveDisplayContainer pCurrentContainer =  hmWaveIter->second;
			pCurrentContainer->Unselect();
		}
}
/**
 this method is called in response to a RESIZED message (e.g. when a container has been resized)
 Typically, the manager will calculate totalModHeight (i.e. the change in height of all the containers)
*/
void  WaveDisplayContainerManager::ContainerResized(unsigned int containerId){

	hash_map<unsigned int, PTWaveDisplayContainer>::iterator hmWaveIter;
	hmWaveIter =  hmWaveContainer.find(containerId) ;
	totalModHeight +=    (((PTWaveDisplayContainer)hmWaveIter->second)->Height - cWaveformHeight) ;
	//if(hmWaveIter !=  hmWaveContainer.end())
}

void  WaveDisplayContainerManager::SetSpectrogramSelectionMode(ESpectrogramSelectionMode selMode){

	hash_map<unsigned int, PTWaveDisplayContainer>::iterator hmWaveIter;

	for ( hmWaveIter = hmWaveContainer.begin(); hmWaveIter != hmWaveContainer.end();  hmWaveIter++) {
		//PTWaveDisplayContainer pCurrentContainer =  hmWaveIter->second;
			((PTWaveDisplayContainer)hmWaveIter->second)->SetSpectrogramSelectionMode(selMode);
	}
}
//positions as given by the WavePlayer (via POSITIONED msg) is used by the waveform containers/tracks to reposition the playback indicator
void  WaveDisplayContainerManager::PositionPlaybackIndicator(unsigned containerId, unsigned position ){

	hash_map<unsigned int, PTWaveDisplayContainer>::iterator hmWaveIter;
	hmWaveIter =  hmWaveContainer.find(containerId);
   //	PTWaveDisplayContainer pCurrentContainer =  hmWaveIter->second;
	hmWaveIter->second->PositionPlaybackIndicator(position);
	//pfWaveformInfo(position, position);
}
/**
  this method iterates through the containers and selects (green color) or unselects (grey color)
  particular waveforms / tracks for playback based on the values of the solo/mute flags.
  This information is passed on to and used by the Wave player instance.
*/
void  WaveDisplayContainerManager::SelectWaveformForPlayback(unsigned int containerId){

	 bool soloExists;
	 PTWaveDisplayContainer pCurrentContainer;
	 //first pass - just check if there's at least a single solo track
	 hash_map<unsigned int, PTWaveDisplayContainer>::iterator hmWaveIter;
		for ( hmWaveIter = hmWaveContainer.begin(); hmWaveIter != hmWaveContainer.end();  hmWaveIter++) {
			pCurrentContainer =  hmWaveIter->second;
			if( soloExists = pCurrentContainer->GetPlaybackSelection()->solo )
				break;
		}

		for ( hmWaveIter = hmWaveContainer.begin(); hmWaveIter != hmWaveContainer.end();  hmWaveIter++) {
			pCurrentContainer =  hmWaveIter->second;
		 //	if(pCurrentContainer->GetContainerView()!=WAVEFORM) continue;
			if(pCurrentContainer->GetPlaybackSelection()->mute)
				pCurrentContainer->SelectForPlayback(false);
				//solo not pressed and
			else if(!pCurrentContainer->GetPlaybackSelection()->solo && soloExists == true)
				pCurrentContainer->SelectForPlayback(false);
			else
				pCurrentContainer->SelectForPlayback(true);
		}

}
vector<nsPlayback::PTRACK_SELECTION>*  WaveDisplayContainerManager::GetSelectedForPlayback(){
		nsPlayback::TRACK_SELECTION *pTrackSel = 0;

		hash_map<unsigned int, PTWaveDisplayContainer>::iterator hmWaveIter;

		ClearPlaybackSelection();

		for ( hmWaveIter = hmWaveContainer.begin(); hmWaveIter != hmWaveContainer.end();  hmWaveIter++) {
			PTWaveDisplayContainer pCurrentContainer =  hmWaveIter->second;

			if( !pCurrentContainer->IsHidden() ){                        //pCurrentContainer->GetSelectedForPlayback()
				pTrackSel = new nsPlayback::TRACK_SELECTION;
				pTrackSel->waveId   = pCurrentContainer->GetContainerId();
				pTrackSel->pPlaybackData = (PWAVE_PLAYBACK_DATA) &pCurrentContainer->GetWavePlaybackData();
		   //	trackSel.selected = true;
				vPlaybackContainers.push_back(pTrackSel);
		   }
		}
		return  &vPlaybackContainers;
}
void  WaveDisplayContainerManager::InitSelection(unsigned int containerId){

	bool selExtExtend = false;
	hash_map<unsigned int, PTWaveDisplayContainer>::iterator hmWaveIter;
	PTWaveDisplayContainer pCurrentContainer  =  hmWaveContainer.find(containerId)->second;
	const SELECTION_RANGE& rSelRange = pCurrentContainer->GetSelectionRange();
	unsigned absPos =  pCurrentContainer->Top + rSelRange.selEnd.y;
	for ( hmWaveIter = hmWaveContainer.begin(); hmWaveIter != hmWaveContainer.end();  hmWaveIter++) {
		pCurrentContainer =  hmWaveIter->second;
		if( pCurrentContainer->GetContainerId() == containerId)
			continue;

		if(pCurrentContainer->GetInitSelection()){
			pCurrentContainer->DrawSelection(rSelRange.selStart.x, rSelRange.selEnd.x);
			selExtExtend = true;
		}else{
		if(pCurrentContainer->Top <= absPos /*&&  (pCurrentContainer->Top + pCurrentContainer->Height >= absPos*/){

			pCurrentContainer->InitSelection();
			pCurrentContainer->DrawSelection(rSelRange.selStart.x, rSelRange.selEnd.x);
			selExtExtend = true;
		}
	}
	}//end for
	TWaveDisplayContainer::SetExternalExtend( selExtExtend );
}
/**
  This method is called in response to a nsWaveDisplay::REMOVE_SELECTION message.
  Only one waveform container can have a region selected unless in multiple selection mode so
  this method clears the selection on all conatiners except the one identified by the argument containerId.
*/
void  WaveDisplayContainerManager::RemoveSelection(unsigned int containerId){

		hash_map<unsigned int, PTWaveDisplayContainer>::iterator hmWaveIter;
		for ( hmWaveIter = hmWaveContainer.begin(); hmWaveIter != hmWaveContainer.end();  hmWaveIter++){
			   PTWaveDisplayContainer	pCurrentContainer =  hmWaveIter->second;
				if( pCurrentContainer->GetContainerId() == containerId)
					continue;
				pCurrentContainer->ClearSelection();
		}
}
//this method is called in response to nsWaveDisplay::POSITION_INDICATOR
void  WaveDisplayContainerManager::IndicatorPositionChanged(unsigned int containerId){

	if(pWaveContainer->GetContainerId()!=containerId)	return;

	PTWaveDisplayContainer pCurrentContainer  =  hmWaveContainer.find(containerId)->second;
	WaveDisplayData *pWData = pCurrentContainer->GetDisplayData();
	/*since PWAVE_DATA array is interleaved
	mono(pos0, neg0, pos1, neg1 etc) we multiply by 2
	stereo(leftPos0, leftNeg0, rightPos0, rightNeg0, leftPos1, leftNeg1, rightPos1, rightNeg1 etc)
	*/
	unsigned selStart, selEnd, currPos;
	const unsigned numChannels = pCurrentContainer->GetNumChannels();
	const SELECTION_RANGE &selRange =  pCurrentContainer->GetSelectionRange();
	const currRelSample = pCurrentContainer->GetPlaybackSample();

	//currPos = pCurrentContainer->GetPlaybackIndicatorPosition();
	WAVE_PLAYBACK_DATA &rWpd =  pCurrentContainer->GetWavePlaybackData();
	const int startSample = rWpd.sampleRange.startSample;// pWData->GetAbsoluteSample(selRange.selStart.x, true);
	const int endSample   = rWpd.sampleRange.endSample;//selRange.selStart.x == selRange.selEnd.x ?
						  //	pWData->GetEndSample() : pWData->GetAbsoluteSample(selRange.selEnd.x, false);

	const int currSample  = startSample + currRelSample > endSample ? endSample : startSample + currRelSample;//pWData->GetAbsoluteSample(currPos, true);
	//notify the main window
	pfMainWndNotify(nsWaveDisplay::INV_TRACK_POS, &TRACK_POSITION(startSample, endSample, currSample));
}
/**
 this method sets the active container in response to a SELECT message
*/
/*void WaveDisplayContainerManager::SetActiveContainer(PTWaveDisplayContainer pWaveContainer ){

	this->pWaveContainer =  pWaveContainer;
}   */
void  WaveDisplayContainerManager::OnWaveMessage(nsWaveDisplay::EWaveMsgType msgType, unsigned int containerId){

	switch(msgType){
		case nsWaveDisplay::ADD:
			break;
		case nsWaveDisplay::REMOVE:
			GetInstance()->RemoveContainer(containerId);
			break;
		case nsWaveDisplay::SELECT:
			GetInstance()->SetActiveContainer( GetInstance()->hmWaveContainer.find(containerId)->second );
			GetInstance()->Unselect(containerId);
			break;
		case nsWaveDisplay::SELECT_WAVEFORM:
			GetInstance()->SelectWaveformForPlayback(containerId);
			break;
		case nsWaveDisplay::RESIZE:
			GetInstance()->ContainerResized(containerId);
			break;
		case nsWaveDisplay::REPOSITION:
			GetInstance()->RepositionContainers(containerId);
			break;
		case nsWaveDisplay::INIT_SELECTION:
			GetInstance()->InitSelection(containerId);
			break;
		case nsWaveDisplay::REMOVE_SELECTION:
			GetInstance()->RemoveSelection(containerId);
			break;
		case nsWaveDisplay::POSITION_INDICATOR:
			GetInstance()->IndicatorPositionChanged(containerId);
			break;
		}
	}
	void  WaveDisplayContainerManager::OnPlaybackMessage(nsPlayback::EPlaybackEvent msgType, unsigned containerId, unsigned position){

	switch(msgType){
		case nsPlayback::POSITIONED:
			GetInstance()->PositionPlaybackIndicator(containerId, position);
			break;
		case nsPlayback::STARTED:
		case nsPlayback::STOPPED:
		case nsPlayback::PAUSED:
			break;
		}
	}
void  WaveDisplayContainerManager::ClearPlaybackSelection(){

	int size = vPlaybackContainers.size();
	for(int i=0; i<size; i++){
		nsPlayback::TRACK_SELECTION *pTrackSel =  vPlaybackContainers.at(i);
		delete	pTrackSel;
	}
	vPlaybackContainers.clear();
}


