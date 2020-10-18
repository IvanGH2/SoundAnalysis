//---------------------------------------------------------------------------

#ifndef WaveDisplayInfoH
#define WaveDisplayInfoH

#include <StdCtrls.hpp>
#include <Controls.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Classes.hpp>
#include <Buttons.hpp>
#include "Util.h"

using namespace nsPlayback;
  /*
struct FILE_INFO{

	char 	*pFileName;
	unsigned numChannels;
	unsigned samplesPerSec;
	unsigned bitsPerSample;
	FILE_INFO(){}
	FILE_INFO(char *pFN, unsigned numChannels, unsigned samplesPerSec, unsigned bitsPerSample)
	: pFileName(pFN), numChannels(numChannels), samplesPerSec(samplesPerSec), bitsPerSample(bitsPerSample)
	{}
};
typedef FILE_INFO* PFILE_INFO;*/
//---------------------------------------------------------------------------
class TWaveDisplayInfo : public TPanel {

TSpeedButton* pBtnClose;
TSpeedButton* pBtnMute;
TSpeedButton* pBtnSolo;
AnsiString fName;
AnsiString prop;
TStaticText *pFileName;
//TLabel *pFileName;
TLabel *pFileProp; //sound file properties
unsigned int start;

PLAYBACK_SELECTION playbackSelection;
FILE_INFO  fileInfo;

void Init();
const AnsiString& GetFileProp();

public:

__fastcall  TWaveDisplayInfo(TComponent* owner,AnsiString& fName);
__fastcall  TWaveDisplayInfo(TComponent* owner, FILE_INFO const &fileInfo);
//void __fastcall setFileName(AnsiString& fName){  this->fName = fName; pFileName->Caption = fName; }
void __fastcall  BtnCloseOnClick(TObject* );
void __fastcall  WaveDisplayInfoOnResize(TObject* sender);
void __fastcall  WaveDisplayInfoOnClick(TObject* sender);
void __fastcall  ChangeColor(TColor color);
void __fastcall  BtnMuteSoloOnClick(TObject *sender);


/*TLabel *testLbl;
void IncTestLbl(){
	testLbl->Caption = IntToStr(++start);
} */

void  ChangeVisibility(bool visible);

const PPLAYBACK_SELECTION  GetPlaybackSelection(){ return &playbackSelection; }
const PFILE_INFO GetFileInfo()  { return &fileInfo; }
};
#endif
