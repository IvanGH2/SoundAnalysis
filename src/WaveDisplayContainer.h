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
	
#ifndef WaveDisplayContainerH
#define WaveDisplayContainerH
//---------------------------------------------------------------------------
#include <Forms.hpp>
#include "WaveDisplayData.h"
#include "Stft.h"
#include "SoundIO.h"
#include "WaveDisplayinfo.h"
#include "WaveformView.h"
#include "SpectrogramView.h"
#include "MixedView.h"
#include "SelectionOverlay.h"
#include "SoundAnalyzerExc.h"

using namespace nsWaveDisplay;

//typedef WAVE_POINT SELECTION_POINT;
struct SELECTION_POINT{
	int x;
	int y;
};

typedef struct {
   SELECTION_POINT selStart;
   SELECTION_POINT selEnd;
}  SELECTION_RANGE, *PSELECTION_RANGE;

//enum EContainerView { WAVEFORM, SPECTROGRAM, BOTH };

class TWaveDisplayContainer : public TPanel
{
 TWaveDisplayInfo* pWaveInfo;  //some information about the file + controls like Mute , Solo
 TPaintBox *pWaveLeft;
 TPaintBox *pWaveRight;
 TPaintBox *pbTime;
 TPaintBox *pbAmpLeft;
 TPaintBox *pbAmpRight;

 TPanel *pCloseTrack;
 TLabel *pFileNameLbl;

 TWaveformView *pWaveformView;
 TSpectrogramView *pSpectrogramView;
 TMixedView *pMixedView;
 TSelectionOverlay *pSelOverlay;

 Graphics::TBitmap *pWaveBmpLeft, *pWaveBmpRight, *pMixedBmpLeft, *pMixedBmpRight, *pMixedBmpLeft0, *pMixedBmpRight0;
 Graphics::TBitmap *pWaveBmpLeft0, *pWaveBmpRight0; //
 Graphics::TBitmap *pBmpAmpLeft;
 Graphics::TBitmap *pBmpAmpRight;

  Graphics::TBitmap *pBmpOverlay;

 Graphics::TBitmap *pSfftBmpLeft, *pSfftBmpLeft0;
 Graphics::TBitmap *pSfftBmpRight, *pSfftBmpRight0; //if stereo

 Graphics::TBitmap* pFFTBinInfo;

 void * pContextMenu, * pSoundSource; //in VCL,this will be an instance of TPopupMenu

 ESoundSource eSoundSource;

 FILE_INFO fInfo;

 PRAW_DATA_INFO pRawDataInfo;

 WAVE_PLAYBACK_DATA wavePlaybackData;

 WaveDisplayData *pWaveDisplay; //pointer to the waveform data

 SpectrogramDisplayData *pSpectralData;//pointer to the spectrogram data

 SoundIO *pSoundData;

  AnsiString fName;

 unsigned int heightCorrection, channelHeight, paintCalled;

 bool managed, snapToSample, parentResized, completeViewRepaint, hidden, drawSelection;

 unsigned prevPos, currSample;

 bool active, selectedForPlayback, selectedExternal, shiftPressed, stretching, spectralSelectionNotification;

 static bool selectionExtExtend, scrollBarVisible;  //this flag is used when selection multiple tracks
	
 unsigned stftSelStartBin, stftSelEndBin, viewHeight;

 unsigned int containerId;  //this container's ID

 static unsigned int containersNum, containersVisibleNum; //total number of containers--used for calculating height when resizing

 unsigned int layoutPos;

 int modifiedHeight, prevHeight, vScrollBarWidth;

 unsigned gColor,gGradStep, gColorMap;

 unsigned frameLen, numChannels, adjustEvery, playbackPosition; //FFT length

 float  transparencyLevel;  //frameOverlap

 SPECTROGRAM_PARAMS::EFrameOverlap frameOverlap;

 bool repositionComplete, stftRegionSelection,
 useMultiThreading, allowPainting;

 enum EWaveStretchMode { NORMAL, MIN, MAX, CUSTOM };

 EWaveStretchMode eStretchMode;

 EContainerView eContainerView;

 EWindow eWnd; //window function for stft

 EScale eScale; //LOG or LINEAR

 nsWaveDisplay::EWaveMsgType  eWaveMsgType;
 
 bool leftBtnDown;

 typedef void (*PfWaveMsg)(nsWaveDisplay::EWaveMsgType, unsigned int) ;

 PfWaveMsg pfWaveMsg;

 PfSpectrogramSelMsg pfSpectrogramSelMsg;

 unsigned int waveHeight, startTime, endTime, opTime;
 //bool mono; //true = mono, false = stereo

 SELECTION_RANGE selRange ;  //waveform selection

 SELECTION_RANGE sfftRange; //sfft selection (in the spectogram view)

 SAMPLE_RANGE sampleRange;

 TColor origColor, foregroundColour, backgroundColour;

 void  Init();

 void  SetSize(unsigned int w, unsigned int h);

 void  AdjustWaveformSize(unsigned newWidth, unsigned newHeight);

 void  AdjustWaveformVertically(unsigned );

 void  AdjustSpectrogramVertically(unsigned );

 void  AdjustSpectrogramSize(unsigned newWidth, unsigned newHeight);

 void  AdjustContainerHeight(unsigned height, EWaveStretchMode eWsm);

 void ResizeContainer();

 void  CopyDisplayData();

 void  FreeResources();

 void DrawSelectedRegion();

 void  DrawSelectedSfftRegion();

 void DrawCentralLine(TPaintBox *pView);

 void DrawCursorPosition(int curPos);

 void  DrawAmpLabels();

 void  DrawFreqLabels();

 void DrawFreqLabelsLog();

 void ClearPrevCursorPosition( int prevCurPos);

 void ClearPreviousSelection(TRect& const );

 void SpectrogramMouseDown(TPaintBox*, int x, int y, bool);

 void SpectrogramMouseMove(TPaintBox*, int x, int y);

 void SpectrogramMouseUp(TPaintBox*, int x, int y);

 void ZoomWave();

 void ShowControls(int h);

 void ShowContainerView(TPaintBox *pView, EChannel eCh);

 void SetViewToStretch();

 void SetViewToStretchClear();

 void StretchBitmapToFit(unsigned height);

 int CalculateSnapToSampleOffset(EPlaybackIndicatorMove eMove);

 void RecalculateStftRegionPos();

 void RecomputeSelection(unsigned newWidth);

 void SetWaveDisplayData();

 void InitCtrls();

 void DrawSpectrogramCallback();

 unsigned GetTopPosition(unsigned height);

 unsigned GetChannelHeight();

 public:
	  __fastcall TWaveDisplayContainer(TComponent* ,/* const std::string& fName,*/ unsigned int containerNum,
	   SoundIO* const pSD  );
	  __fastcall ~TWaveDisplayContainer();

	  void  DoDraw();

	  void DoDrawSfft();

	  void DoDrawMixed();

	  void DrawSpectrogram();

	  void DrawWaveform();

	  void DrawMixedView();

	  void ThreadPoint() {
		pFileNameLbl->Caption = IntToStr(pSpectralData->frameReached1) + "	"+ IntToStr(pSpectralData->frameReached2)
		+"	"+IntToStr(pSpectralData->frameReached3)+"	"+IntToStr(pSpectralData->frameReached4);

		}
	  void TotalFrames() {
		pFileNameLbl->Caption = IntToStr(pSpectralData->totalFrames);

		}
	  void SetSoundSourceObject(void *pSoundSrc, ESoundSource eSrc) {
		eSoundSource = eSrc;
	  	pSoundSource = pSoundSrc;
	  }

	  void SetForegroundColour(TColor fCol){
		if(foregroundColour ==  fCol) return;
		foregroundColour =  fCol;
		DoDraw();
	  }

	  void SetBackgroundColour(TColor bCol){
		if(backgroundColour == bCol) return;
		backgroundColour =  bCol;
		DoDraw();
	  }

	  std::string GetTrackName() const { return std::string( fName.c_str() ); }

	  void EnableMultiThreading(bool enabled) { useMultiThreading = enabled; }

	  bool IsMultiThreadingEnabled() const {return useMultiThreading; }

	  int GetOperationTime() const { return opTime; }

	  void ActivateContainer();
	  
	  void SetScrollBarWidth(unsigned w) {
	  	vScrollBarWidth = w;
	  }

	  static void SetScrollBarVisible(bool v) { scrollBarVisible = v; }

	  void SetShiftState(bool shState) { shiftPressed = true; }

	  void SetSpectralData(SpectrogramDisplayData *pSD){ pSpectralData = pSD; }

	  SpectrogramDisplayData *GetSpectralData() const { return pSpectralData; }

	  void SetDisplayData(WaveDisplayData const*  pWD )  { pWaveDisplay = (WaveDisplayData*)pWD;}

	  WaveDisplayData* GetDisplayData() const { return pWaveDisplay; }

	  void SetSoundData(SoundIO *pSD) { pSoundData = pSD ; }

	  SoundIO *GetSoundData() const { return pSoundData;  }

	  const WAVE_DISPLAY_DATA &GetWaveDisplayData() const { return  pWaveDisplay->GetInputData(); }

	  float *GetWaveDisplayDataCurrentPointer() const { return pWaveDisplay->GetCurrentData(); }

	  const WAVE_PLAYBACK_DATA& GetWavePlaybackData();

	  unsigned int GetContainerId() const { return containerId; }

	  unsigned GetNumChannels() const { return numChannels; }

	  void SetFileName(AnsiString& fileName );

	  void SetFileName(const std::string &fileName );

	   void  __fastcall WaveCloseTrack(TObject *sender){

			delete this;

		}
	  //AnsiString& getFileName(){ return fileName; }
	  void __fastcall WaveOnPaint(TObject *Sender);

	  void __fastcall WaveAmpOnClick(TObject *sender);

	  void __fastcall WaveContainerOnClick(TObject *Sender);//

	  bool GetActive() const { return active; }

	  void SetActive() { Click(); } //simulate a click event , which will activate the container

	  void SetHidden(bool hide) {

		if(hide && !hidden){
			Visible = false;
			containersVisibleNum--;
		}else if(!hide && hidden){
			Visible = true;
			containersVisibleNum++;
		}
		hidden = hide;

	   }

	  bool IsHidden() const { return hidden; }

	  void  Resize(unsigned  w, unsigned  h);

	  void  ParentResized(unsigned  w, unsigned  h);

	  void  RemoveContainer() { delete this; }

	  void  SetSpectrogramSelectionMessageCallback(PfSpectrogramSelMsg pfMsg){ pfSpectrogramSelMsg = pfMsg; }

	  void SetWaveMessageCallback(PfWaveMsg pfMsg){ pfWaveMsg = pfMsg; }

	  void SetLayoutPosition(unsigned pos, int modHeight) {
		layoutPos = pos;
		modifiedHeight = modHeight;
	  }

	  void  AdjustVertically();

	  void  AdjustMinimum();

	  void  AdjustToMinimumHeight();

	  void  CustomAdjustVertically(unsigned int);

	  void  AdjustNormal();

	  void  AdjustToNormalHeight();

	  void DrawContainerFrame() {

		Canvas->Brush->Style = bsClear;
		Canvas->Rectangle( TRect(0,0, Width-1, Height-1) );
	  }

	  static void SetContainersNumber(unsigned int num = 1){
		containersNum = num;
		containersVisibleNum++;
		}

	  static void SetExternalExtend(bool selExExt) { selectionExtExtend = selExExt; }

	  void __fastcall WaveContainerKeyDown(TObject *sender, WORD &key, TShiftState Shift);
	  //wave (TPaintBox) events
	  void __fastcall WaveMouseDown(TObject *Sender, TMouseButton Button,TShiftState Shift, int X, int Y) ;

	  void __fastcall WaveMouseUp(TObject *Sender, TMouseButton Button,TShiftState Shift, int X, int Y) ;

	  void __fastcall WaveMouseMove(TObject *Sender, TShiftState Shift, int X, int Y) ;

	  void __fastcall WaveContainerMouseDown(TObject *Sender, TMouseButton Button,TShiftState Shift, int X, int Y) ;

	  void __fastcall WaveContainerMouseUp(TObject *Sender, TMouseButton Button,TShiftState Shift, int X, int Y) ;

	  void __fastcall WaveContainerMouseMove(TObject *Sender, TShiftState Shift, int X, int Y) ;

	  //container events
	  void __fastcall WaveContainerOnResize(TObject *Sender);

	  bool ZoomIn();

	  bool ZoomOut();

	  bool ZoomOutFull();

	  bool ZoomInFull();

	  void Position(int deltaY);

	  void  Unselect();   //unselects a particular container

	  unsigned  GetSelectionStartSample() const {

        //*2 is to account for both positive and negative samples
		return  pWaveDisplay->GetSampleValues()[selRange.selStart.x * numChannels * 2].startSample;
	  }
	  unsigned GetSelectionEndSample() const {

		return  pWaveDisplay->GetSampleValues()[selRange.selStart.x * numChannels * 2].endSample;
	  }

	  unsigned GetAbsoluteSamplePos(unsigned int pos) const {

		return  pWaveDisplay->GetAbsoluteSample(pos, true);
	  }

	  void SelectForPlayback(bool selected);

	  bool GetSelectedForPlayback() const { return selectedForPlayback; }

	  void PlaybackSelectionChanged(); //selects a particular waveform (chanhes its color

	  const PPLAYBACK_SELECTION  GetPlaybackSelection(){ return pWaveInfo->GetPlaybackSelection(); }

      const PFILE_INFO GetFileInfo() { return &fInfo; }

	  unsigned int GetCurrentPosition() const { return selRange.selStart.x; }

	  unsigned int GetCurrentPositionEnd() const { return selRange.selEnd.x; }

	  const SELECTION_RANGE& GetSelectionRange() const { return selRange; }

	  const SELECTION_RANGE& GetSelectionStftRange() const {  return sfftRange; }

	  void SetCurrentPosition(unsigned int cPos) { selRange.selStart.x = cPos; }

	  void InitSelection() {
		selectedExternal = true;
	  }

	  bool GetInitSelection() const { return selectedExternal; }

	  void DrawSelection(unsigned int cPosStart, unsigned int cPosEnd);

	  void ClearSelection();

	  const SAMPLE_RANGE& GetSampleRange() const;

	  void ShowFFTBinData(int x, int y);

	  void SetSampleRange();
	  //playback indicator
	  void PositionPlaybackIndicator(unsigned int);

	  void PositionPlaybackIndicatorInSpectrogramView(unsigned int currSample);

	  void MovePlaybackIndicator(EPlaybackIndicatorMove );

	  unsigned GetPlaybackIndicatorPosition() const { return playbackPosition; }

	  unsigned GetPlaybackSample() const { return currSample; }

	  void RedrawSpectrogram();

	  void SetStftColor(unsigned color){
		if(gColor!=color){
			completeViewRepaint = true;
			gColor = color;
		}
	  }
	  void SetStftColorMap(unsigned colorMap){

		if(gColorMap!=colorMap){
		completeViewRepaint = true;
		gColorMap = colorMap;
		}
	  }
	  void SetStftContrast(unsigned step){
		gGradStep = step;
	  }

	  void SetStftScale(EScale eScale){
		this->eScale = eScale;
	  }

	  void SetStftFrameWindow(WindowFunc::EWindow eWndFunc){
      	eWnd = eWndFunc;
	  }
	  void SetStftFrameLength(unsigned fLen) { frameLen = fLen; }

	 // void SetStftFrameOverlap(float fOverlap) { frameOverlap = fOverlap ; }

	  void SetFrameOverlap(SPECTROGRAM_PARAMS::EFrameOverlap fOverlap) { frameOverlap = fOverlap ; }

	  void SetContextMenu(void *pMenu) {
		pContextMenu = pMenu;
		PopupMenu =   (TPopupMenu*)pMenu;
	  }

	  EContainerView GetContainerView() const { return eContainerView; }

	  void SetStftRegionSelection(bool enabled) { stftRegionSelection = enabled; }

	  void SetSpectralTracking(bool spectTracking){ spectralSelectionNotification = spectTracking; }

	  void SetSpectrogramSelectionMode(ESpectrogramSelectionMode selMode){

		SetStftRegionSelection(selMode==PARTIAL_BANDWIDTH ? true : false);
	  }
	  unsigned GetAdjustVertically() const { return adjustEvery; }

	  void SetAdjustVertically(unsigned every=4){ adjustEvery = every; }

	  void ShowBmp(){
		if(eContainerView == SPECTROGRAM ){
			pWaveLeft->Canvas->Draw(0, 0, pSfftBmpLeft);
			if(numChannels==2)pWaveRight->Canvas->Draw(0, 0, pSfftBmpRight);
		}else
			pWaveLeft->Canvas->Draw(0, 0, pWaveBmpLeft);
			if(numChannels==2)pWaveRight->Canvas->Draw(0, 0, pWaveBmpRight);
	  //	pWaveLeft->Canvas->Draw(0, 0, pWaveBmpLeft);//pWaveBmpLeft
	  //	pWaveLeft->Canvas->Draw(0, 0, pSelOverlay->GetSelectionOverlay(LEFT));   //pSelOverlay->GetSelectionOverlay(LEFT)
	  }

	  void SetTransparencyLevel(float tLevel=0.5f) {
		transparencyLevel = (tLevel<0 || tLevel>1) ? 0.5f : tLevel;
	  }
	  const std::string GetTrackName() { return std::string(fName.c_str()); }

	  void DialogueShown(bool shown){
		 completeViewRepaint = shown ? true : false;
	  }
	  unsigned GetContainerWidth() const { return Width; }

	  unsigned GetTrackWidth() const { return pWaveLeft->Width; }
	  /**
click event handler for TPaintBox components
*/
void   __fastcall WaveOnClick(TObject *Sender){ Click(); }

	 //the following message map as well as the the empty EraseBkg method helped reduce the flicker so it's still here
	 BEGIN_MESSAGE_MAP
	 MESSAGE_HANDLER( WM_ERASEBKGND,TMessage,EraseBkg )
	 END_MESSAGE_MAP(TPanel)

	void __fastcall TWaveDisplayContainer::EraseBkg (TMessage &Message){}
};
typedef  TWaveDisplayContainer* PTWaveDisplayContainer;
extern PACKAGE  TWaveDisplayContainer* waveContainer;
#endif
