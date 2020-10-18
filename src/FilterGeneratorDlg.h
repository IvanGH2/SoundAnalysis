//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef FilterGeneratorDlgH
#define FilterGeneratorDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>


#include "Util.h"
#include <ComCtrls.hpp>
#include <vector>

using namespace nsDigitalFilter;
//---------------------------------------------------------------------------
//enum EDigitalFilter { LO_PASS = 0, HI_PASS, BAND_PASS };
enum EFilterParams { TRANSITION_BAND, FILTER_LENGTH };

class TpFilterDlg : public TForm
{

	TComponent *pParent;
__published:	// IDE-managed Components
	TBevel *Bevel1;
	TStaticText *StaticText1;
	TStaticText *StaticText2;
	TStaticText *StaticText4;
	TComboBox 	*pCbFilterType;
	TMaskEdit 	*pMeFreqLo;
	TButton 	*btnFilterOk;
	TStaticText *StaticText5;
	TMaskEdit 	*pMeFreqHi;
	TStaticText *StaticText6;
	TComboBox 	*pCbFilterWnd;
	TBevel 	*Bevel2;
	TStaticText *StaticText7;
	TMaskEdit 	*meBandsNum;
	TTabControl *tcFilterBand;
	TStaticText *StaticText8;
	TMaskEdit 	*meBandCutoffLo;
	TStaticText *StaticText9;
	TMaskEdit 	*meBandCutoffHi;
	TButton 	*btnFilterClose;
	TCheckBox 	*cbInverted;
	TMaskEdit 	*pMeFilterLen;
	void __fastcall btnFilterCloseClick(TObject *Sender);
	void __fastcall FilterOnSelect(TObject *Sender);
	void __fastcall CuttoffLoOnExit(TObject *Sender);
	void __fastcall CuttoffHiOnExit(TObject *Sender);
	void __fastcall btnFilterOkClick(TObject *Sender);
	void __fastcall meBandsNumChange(TObject *Sender);
	void __fastcall TcFilterBandChange(TObject *Sender);
	void __fastcall FilterWndOnSelect(TObject *Sender);
	void __fastcall BandLoHiOnExit(TObject *Sender);
private:	// User declarations

	unsigned sampleRate;
	unsigned cutoffFreq;
	unsigned cutoffFreqHi;
	unsigned transitionBand;
	unsigned maxFreq;
	unsigned filterLength;
	nsDigitalFilter::EFilterType eFilterType;
	WindowFunc::EWindow eWnd;
	std::vector<nsDigitalFilter::PFILTERBAND_INFO>  vBandInfo;

	void SetCutoffFrequency(unsigned );
	void SetTransitionBand(unsigned );
	unsigned GetFilterLength(unsigned filLen, unsigned&);
	unsigned CalculateRolloff(EFilterParams eFParam, unsigned&);
public:		// User declarations
	__fastcall TpFilterDlg(TComponent* Owner);
	__fastcall TpFilterDlg(TComponent* Owner, unsigned );

};
//---------------------------------------------------------------------------
extern PACKAGE TpFilterDlg *pFilterDlg;
//---------------------------------------------------------------------------
#endif
