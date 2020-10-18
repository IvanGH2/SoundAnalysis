//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef ToneGenDlgH
#define ToneGenDlgH
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>
#include <Tabs.hpp>
#include <ComCtrls.hpp>
#include <vector>
#include "Util.h"
//---------------------------------------------------------------------------
class TToneGeneratorDlg : public TForm
{
__published:	// IDE-managed Components
	TBevel *Bevel1;
	TStaticText *StaticText1;
	TStaticText *StaticText2;
	TStaticText *StaticText3;
	TStaticText *StaticText4;
	TComboBox *cbToneType;
	TMaskEdit *meToneFreq;
	TMaskEdit *meToneAmp;
	TMaskEdit *meToneTime;
	TButton *btnToneOK;
	TButton *btnToneCancel;
	TStaticText *StaticText5;
	TMaskEdit *meToneHarNum;
	TBevel *Bevel2;
	TTabControl *tcHarm;
	TStaticText *StaticText6;
	TMaskEdit *meHarmAmp;
	TStaticText *StaticText7;
	TMaskEdit *meToneSampleRate;
	TStaticText *StaticText8;
	TMaskEdit *meHarmFreq;
	TStaticText *StaticText9;
	TMaskEdit *meHarmPhase;
	void __fastcall meToneHarNumChange(TObject *Sender);
	void __fastcall meHarmAmpExit(TObject *Sender);
	void __fastcall meToneAmpExit(TObject *Sender);
	void __fastcall tcHarmChange(TObject *Sender);
	void __fastcall memeHarmFreqExit(TObject *Sender);
	void __fastcall meHarmPhaseExit(TObject *Sender);
	void __fastcall btnToneCancelClick(TObject *Sender);
	void __fastcall btnToneOKClick(TObject *Sender);
private:	// User declarations
	std::vector<nsToneGenerator::PHARMONICS_INFO>  vHarmInfo;
	RAW_DATA_INFO dataInfo;
	nsToneGenerator::TONE_INFO toneInfo;
	TComponent *parent;
public:		// User declarations
	__fastcall TToneGeneratorDlg(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TToneGeneratorDlg *ToneGeneratorDlg;
//---------------------------------------------------------------------------
#endif
