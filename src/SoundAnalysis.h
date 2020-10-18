//$$---- Form HDR ----
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
#ifndef SoundAnalysisH
#define SoundAnalysisH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>

#include "SoundIO.h"
#include "Util.h"
#include "WaveDisplayData.h"
#include "DspUtil.h"
#include "Tonegenerator.h"

#include "Stft.h"
#include "DigitalFilter.h"
#include "TimePanel.h"
#include <Buttons.hpp>
#include <ToolWin.hpp>
#include "map.h"
#include "hash_map.h"
#include "set.h"
#include "WaveDisplayContainer.h"
#include "WaveDisplayContainerManager.h"
#include "WavePlayer.h"
#include "WinWavePlayer.h"
#include <ButtonGroup.hpp>
#include <Mask.hpp>

#include "FeatureMatcher.h"

//---------------------------------------------------------------------------

class TForm1 : public TForm
{
  //private
  PRAW_DATA_INFO pRawDataInfo;
  WaveDisplayData* waveDisplay;
  Graphics::TBitmap* bmp1;
  bool maxZoom, minZoom;
  unsigned int startSample;
  TWaveDisplayContainer* pWaveContainer;

  //PWaveDisplayContainerManager pWaveDisplayManager;
  const WaveDisplayContainerManager& rWaveDisplayManager;
  const TWavePlayer<WinWavePlayer>& rWavePlayer;
  const TTrackTimePanel trackTimePanel;


  void __fastcall DisplayFileInfo();

  void  DoGenerateTone(nsToneGenerator::TONE_INFO& const rToneInfo, std::vector<nsToneGenerator::PHARMONICS_INFO>& pHarmonicsInfo);

__published:	// IDE-managed Components
	TMainMenu *MainMenu1;
	TMenuItem *File1;
	TMenuItem *Open1;
	TMenuItem *Save1;
	TMenuItem *Saveas1;
	TMenuItem *Close1;                     
	TStatusBar *StatusBar1;
	TMenuItem *Import1;
	TMenuItem *View1;
	TMenuItem *Zoomin1;
	TMenuItem *Zoomout1;
	TMenuItem *Zoomnormal1;
	TMenuItem *Adjustvertically1;
	TMenuItem *Adjust1;
	TPanel *Panel1;
	TPaintBox *pTrackTimer;
	TOpenDialog *OpenDialog1;
	TScrollBox *pWaveScroller;
	TMenuItem *Generate1;
	TMenuItem *CompositeTone1;
	TMenuItem *Whitenoise1;
	TPopupMenu *pmContainerMenu;
	TMenuItem *Export1;
	TMenuItem *Analyze1;
	TMenuItem *Filter1;
	TMenuItem *STFT1;
	TMenuItem *Spectral1;
	TMenuItem *Shrink1;
	TMenuItem *IFFT1;
	TMenuItem *Resize1;
	TMenuItem *Adjustvertic1;
	TMenuItem *Shrink2;
	TMenuItem *Adjustvertically2;
	TMenuItem *Spectrogram1;
	TMenuItem *Waveform1;
	TMenuItem *Applyfilter1;
	TMenuItem *Closeall1;
	TMenuItem *Openrecent1;
	TPanel *Panel2;
	TStaticText *StaticText1;
	TMaskEdit *pStartPos;
	TStaticText *StaticText2;
	TMaskEdit *pEndPos;
	TStaticText *StaticText4;
	TMaskEdit *pPlaybackPos;
	TPanel *panMainCtrls;
	TPanel *pPlayerCtrls;
	TSpeedButton *pBtnPause;
	TSpeedButton *pBtnStop;
	TSpeedButton *pBtnPlay;
	TSpeedButton *btnSelNormal;
	TSpeedButton *btnSelSpect;
	TStaticText *StaticText5;
	TTrackBar *tbTransparency;
	TStaticText *StaticText3;
	TSaveDialog *SaveDialog1;
	TButton *Button2;
	TButton *Button1;
	TPanel *panZoomCtrls;
	TSpeedButton *ZoomInBtn;
	TSpeedButton *ZoomOutBtn;
	TSpeedButton *ZoomInFullBtn;
	TSpeedButton *ZoomOutFullBtn;
	TSpeedButton *zoomNormalBtn;
	TSpeedButton *zoomVertBtn;
	TPanel *Panel3;
	TCheckBox *cbTrackControl;
	TCheckBox *cbSpectDlg;
	TCheckBox *cbEnableThreads;
	void __fastcall OpenSoundFile(TObject *Sender);
	void __fastcall Import1Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall ZoomInBtnClick(TObject *Sender);
	void __fastcall ZoomOutBtnClick(TObject *Sender);
	void __fastcall FormOnResize(TObject *Sender);
	void __fastcall Close1Click(TObject *Sender);
	void __fastcall Adjustvertically1Click(TObject *Sender);
	void __fastcall Zoomnormal1Click(TObject *Sender);
  //	void __fastcall PlaybackCtrlClicked(TObject *Sender);
   //	void __fastcall PlayerButtonOnClicked(TObject *Sender, int Index);
	void __fastcall TrackTimerOnPaint(TObject *Sender);
	void __fastcall SpeedButton1Click(TObject *Sender);
	void __fastcall PlayerCtrlsOnClick(TObject *Sender);
	void __fastcall ZoomOutFullBtnClick(TObject *Sender);
	void __fastcall ZoomInFullBtnClick(TObject *Sender);
	void __fastcall zoomVertBtnClick(TObject *Sender);
	void __fastcall zoomNormalBtnClick(TObject *Sender);
	void __fastcall Tone1Click(TObject *Sender);
	void __fastcall Save1Click(TObject *Sender);
	void __fastcall Applyfilter1Click(TObject *Sender);
	void __fastcall Filter1Click(TObject *Sender);
  //	void __fastcall ShowSpctrogram1Click(TObject *Sender);
	void __fastcall STFT1Click(TObject *Sender);
	void __fastcall Spectral1Click(TObject *Sender);
	void __fastcall Shrink1Click(TObject *Sender);
	void __fastcall FormOnShortCut(TWMKey &Msg, bool &Handled);
	void __fastcall btnSelNormalClick(TObject *Sender);
	void __fastcall btnSelSpectClick(TObject *Sender);
	void __fastcall Shrink2Click(TObject *Sender);
	void __fastcall Adjustvertic1Click(TObject *Sender);
	void __fastcall tbTransparencyOnChange(TObject *Sender);
	void __fastcall cbTrackControlOnClick(TObject *Sender);
	void __fastcall Closeall1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall FormOnClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormOnShow(TObject *Sender);
	void __fastcall Zoomin1Click(TObject *Sender);
	void __fastcall Zoomout1Click(TObject *Sender);
	void __fastcall cbTrackControlClick(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
   //	void __fastcall FormOnKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);


private:	// User declarations

   /*	SoundInput *soundInput;
	SoundOutput *pSoundOutput; */
	unsigned maxRecentFiles, origWidth;
	bool closingApp;
	std::set<std::string> vRecentFiles;
	
	AnsiString filterName;

	SoundIO *pSoundData;

	Tone* pTone;
	//track timer
	unsigned trackTimeMilis, totalTrackTimeMilis, startTime, numToProcess, totalProgress;

  //	Graphics::TBitmap *pTrackTimerBmp;

  //	void DrawTrackTimer( );

	void  SaveFile();

	void ReadFileRecent();

	void WriteFileRecent();

	void  OpenFile(std::string fName);

	void AddMenuItem(std::string fileName, int);

	//ovo maknuti kad se obriše kontejner
	void SetSoundInput(){ pSoundData = new SoundInput(); }

	void SetSoundOutput(){ pSoundData = new SoundOutput(); }

	void SetPlayerCtrlsState(nsPlayback::EPlaybackEvent, unsigned );
	//callback to update the waveform position
	void SetWaveformInfo(unsigned startSample, unsigned endSample, unsigned currSample);

	void ShowScrollbarsIfNeeded(unsigned totalHeight);

	AnsiString& CreateFilterName(nsDigitalFilter::EFilterType eFilterType, unsigned cutoffLo, unsigned cutoffHi);
	//hash_map<const char*, SoundInput*> soundObjs;
	void MainWndCallback(EMainWndNotification eNotification, const void *p);

	void EnableActions();

	bool AllowZoomIn(const TWaveDisplayContainer *pContainer );

	void ChangeZoomBtnsState(bool maxZoom, bool minZoom);

	void UpdateActionProgress(PFILTER_PROCESSING_PROGRESS );

	void DrawTrackTimePanel(unsigned totalTime, unsigned selTime, unsigned startTime, unsigned zoomLevel);

	void UpdateProgressFromThread();

	void SwitchContainerView();

	void AddFilter( DigitalFilter  * p);

	int GetDesktopClientHeight();

	void ZoomIn();

	void ZoomOut();

	void AddTimePanel();

	void DrawTimePanel();

  //	void DrawFilteredWave(DigitalFilter *pDf, EFilterType eFilterType, unsigned cutoffFreqLo, unsigned cutoffFreqHi,
	//WAVEFORMATEX const &wfex, unsigned numSamples, unsigned numChannels);

public:		// User declarations
	__fastcall TForm1(TComponent* Owner);

	void __fastcall setImportData(PRAW_DATA_INFO pRdi) {
		pSoundData->SetHeaderInfo(pRdi);
	}

	unsigned DrawWave(float* data, unsigned int numSamples, unsigned int numChannels);

	void DrawFilteredWave(nsDigitalFilter::EFilterType eFilterType, EWindow eWnd, unsigned cutoffFreq, unsigned cutoffFreqHi, unsigned filterLength, vector<nsDigitalFilter::PFILTERBAND_INFO>&, bool );

	unsigned DoDraw();

	void __fastcall OpenRawFile(PRAW_DATA_INFO pRdi);

	void __fastcall OpenRecentSoundFile(TObject *Sender);

   // const WaveDisplayContainerManager& GetContainerManager() const { return rWaveDisplayManager; }

	SoundIO* getSoundInput(){return pSoundData;}

	void GenerateTone(nsToneGenerator::TONE_INFO& const rToneInfo, std::vector<nsToneGenerator::PHARMONICS_INFO>& pHarmonicsInfo);

	void PlaySoundPreview(char *, unsigned);

	void HideTracks( std::vector<TRACK_INFO> *pVTrackInfo );

	vector<nsPlayback::PTRACK_SELECTION>* GetPlaybackData();
	
	void __fastcall ProcessWaveDoneMsg(TMessage& done);
	
	BEGIN_MESSAGE_MAP
		MESSAGE_HANDLER(MM_WOM_DONE, TMessage, ProcessWaveDoneMsg)
	END_MESSAGE_MAP(TForm)

};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif

