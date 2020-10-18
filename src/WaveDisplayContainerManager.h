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


#ifndef WaveDisplayContainerManagerH
#define WaveDisplayContainerManagerH

#include "hash_map.h"
#include "WaveDisplayContainer.h"


//---------------------------------------------------------------------------
class WaveDisplayContainerManager;
typedef   WaveDisplayContainerManager* PWaveDisplayContainerManager;
//typedef   WaveDisplayContainerManager& RWaveDisplayContainerManager;

class WaveDisplayContainerManager{

unsigned int containersCount;
unsigned int layoutPos;
unsigned int containerId;
unsigned int containerWithFocus;
unsigned int totalContainerHeight;


static PWaveDisplayContainerManager pWaveContainerManager;
static WaveDisplayContainerManager& rWaveContainerManager;

hash_map<unsigned int, PTWaveDisplayContainer> hmWaveContainer;
vector<nsPlayback::PTRACK_SELECTION> vPlaybackContainers;
vector<PTWaveDisplayContainer> vContainers;
vector<PFILE_INFO> vContainersFileInfo;
#if defined _NOTIFY_MAIN_WND
//PfWaveformInfo pfWaveformInfo;
PfMainWndNotify pfMainWndNotify ;
#endif

	WaveDisplayContainerManager(void* );    //pointer to parent

	void *parent, *pContextMenu;

	PfPlaybackMsg pfPlaybackMsg;

	PTWaveDisplayContainer pWaveContainer;

   //	WaveDisplayData* pWaveDisplayData;

	SoundIO *pSoundData;

	std::string currFileName; //možda ne treba ovdje

	int totalModHeight; //total modified height;

	static PWaveDisplayContainerManager GetInstance(){ return pWaveContainerManager; }

	void  CalculateModifiedHeight();

	void  ClearPlaybackSelection();

	unsigned CalculateTotalHeight();
public:
	#if defined _NOTIFY_MAIN_WND
 //	void SetWaveformInfoCallback(PfWaveformInfo pfWaveformInfo) { this->pfWaveformInfo = pfWaveformInfo; }
	void SetMainWndCallback(PfMainWndNotify pfMainNotify) { pfMainWndNotify = pfMainNotify; }
	#endif

	static const WaveDisplayContainerManager& GetInstance(void* owner);

	static void OnWaveMessage(nsWaveDisplay::EWaveMsgType msgType, unsigned int waveId);

	static void OnPlaybackMessage(nsPlayback::EPlaybackEvent, unsigned conId, unsigned int pos);
	   //	static PWaveDisplayContainerManager getInstance(void* owner);
	unsigned AddContainer();

	void SetSpectrogramSelectionMode(ESpectrogramSelectionMode selMode);

	void RemoveAllContainers();

	void RemoveContainer(unsigned containerId);

	void RepositionContainers();

 //	void SetDisplayData(WaveDisplayData* pWD) { pWaveDisplayData = pWD; }

	void SetSoundData(SoundIO *pSD) {  pSoundData = pSD; }

	void SetPlaybackMsg(PfPlaybackMsg pfMsg) { pfPlaybackMsg = pfMsg; }

	PTWaveDisplayContainer GetActiveContainer(){ return  pWaveContainer; }

    PTWaveDisplayContainer GetContainerById(unsigned id) { return hmWaveContainer[id]; }

	std::vector<PTWaveDisplayContainer> *GetContainers();

	//void SetActiveContainer(PTWaveDisplayContainer pWaveContainer){ this->pWaveContainer =  pWaveContainer; }

	void SetActiveContainer(PTWaveDisplayContainer pWContainer);

	//void SetFileName(const char *fName) { fileName = fName; }

	void SetFileName(std::string &fName) {
		currFileName = fName;
	}

	unsigned GetContainersCount(){ return containersCount; }

	void  ResizeContainers(unsigned int newWidth, unsigned int newHeight);

	void  AdjustContainersVertically();

	void  AdjustContainersNormal();

	void  AdjustContainersMinimum();

	void  RepositionContainers(unsigned int containerId) ;

	void  Unselect(unsigned int containerId);

	void  ContainerResized(unsigned int containerId);

	void  IndicatorPositionChanged(unsigned int containerId);

	unsigned GetContainersHeight();

	void  PositionPlaybackIndicator(unsigned conId, unsigned pos);
	//void  PositionPlaybackIndicator(unsigned int containerId, unsigned int pos);

	void  SelectWaveformForPlayback(unsigned int containerId);

	void  InitSelection(unsigned int containerId);

	void  RemoveSelection(unsigned int containerId);

	void ShowSpectrogram();

	vector<nsPlayback::PTRACK_SELECTION>* GetSelectedForPlayback();

	std::vector<nsPlayback::PFILE_INFO> *GetContainersTrackInfo();

	void SetContextMenu(void *pMenu){ pContextMenu = pMenu; }

	unsigned GetTotalContainerHeight() { return totalContainerHeight = CalculateTotalHeight(); }

};
//extern PACKAGE PWaveDisplayContainerManager pWaveDisplayManager;
#endif
