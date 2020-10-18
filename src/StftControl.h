//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef StftControlH
#define StftControlH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "WaveDisplayContainerManager.h"
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TpStftControl : public TForm
{
__published:	// IDE-managed Components
	TStaticText *StaticText1;
	TStaticText *StaticText2;
	TComboBox *pStftWnd;
	TStaticText *StaticText3;
	TComboBox *pFftLen;
	TStaticText *StaticText4;
	TTrackBar *pStftContrast;
	TStaticText *StaticText5;
   //	TComboBox *pStftOverlap;
	TRadioGroup *rgScale;
	TComboBox *cbGradColor;
	TStaticText *StaticText6;
	TComboBox *pFftOverlap;
	void __fastcall StftWndOnChange(TObject *Sender);
   //	void __fastcall StftContrastOnSelect(TObject *Sender);
	void __fastcall SfftFLenOnChange(TObject *Sender);
	void __fastcall rgScaleClick(TObject *Sender);
	void __fastcall GradColorOnSelect(TObject *Sender);
	void __fastcall StftControlOnShow(TObject *Sender);
	void __fastcall StftControlOnClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FftOverlapOnChange(TObject *Sender);
private:	// User declarations
	  const WaveDisplayContainerManager& rWaveDisplayManager;
public:		// User declarations
	__fastcall TpStftControl(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TpStftControl *pStftControl;
//---------------------------------------------------------------------------
#endif
