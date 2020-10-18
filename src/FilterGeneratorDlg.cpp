//$$---- Form CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FilterGeneratorDlg.h"
#include "SoundAnalysis.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TpFilterDlg *pFilterDlg;
//---------------------------------------------------------------------------
__fastcall TpFilterDlg::TpFilterDlg(TComponent* Owner)
	: TForm(Owner)
{
	pParent = (TForm1*)Owner;
}
__fastcall TpFilterDlg::TpFilterDlg(TComponent* Owner, unsigned sampleRate)
	: TForm(Owner), eWnd(BLACKMAN)
{
	this->sampleRate = sampleRate;
	pMeFreqLo->Text = "500";
	pMeFreqHi->Text = "700";
	maxFreq =   sampleRate / 2;
	cutoffFreq = 500;
	cutoffFreqHi = 700;
	transitionBand = 30;
	filterLength = 1024;
	pParent = (TForm1*)Owner;

}
void TpFilterDlg::SetCutoffFrequency(unsigned cutoffFreq)
{
	this->cutoffFreq = cutoffFreq;
	pMeFreqLo->Text = IntToStr(cutoffFreq);
}
void TpFilterDlg::SetTransitionBand(unsigned bw)
{
	 this->transitionBand = bw;
}
//---------------------------------------------------------------------------
void __fastcall TpFilterDlg::btnFilterCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TpFilterDlg::FilterOnSelect(TObject *Sender)
{
	TComboBox *pDFilter = (TComboBox*)Sender;

	eFilterType = (EFilterType)pDFilter->ItemIndex;
	if(eFilterType == BANDPASS){
		meBandsNum->Enabled = true;
		meBandsNum->Text	= 3;
	}else{
	   meBandsNum->Enabled = false;
	   meBandsNum->Text	= 0;

	}

}
//---------------------------------------------------------------------------
void __fastcall TpFilterDlg::CuttoffLoOnExit(TObject *Sender)
{
	unsigned loFreq = StrToInt(Trim(pMeFreqLo->Text));

	if(eFilterType == BANDPASS && loFreq >= cutoffFreqHi) {
		ShowMessage("Lo cutoff frequency must be lower than the high cutoff frequency");
		pMeFreqHi->Text = IntToStr(cutoffFreqHi-10);
	}
	cutoffFreq = loFreq;
}
//---------------------------------------------------------------------------
void __fastcall TpFilterDlg::CuttoffHiOnExit(TObject *Sender)
{
	unsigned hiFreq = StrToInt(Trim(pMeFreqHi->Text));
	if(eFilterType == BANDPASS &&  hiFreq <= cutoffFreq) {
		ShowMessage("High cutoff frequency must be higher than the low cutoff frequency");
		pMeFreqHi->Text = IntToStr(cutoffFreq);
	}
	cutoffFreqHi = hiFreq;
}
//---------------------------------------------------------------------------
void __fastcall TpFilterDlg::btnFilterOkClick(TObject *Sender)
{
	 filterLength = StrToInt(Trim(pMeFilterLen->Text));
	 ((TForm1*)pParent)->DrawFilteredWave(eFilterType, eWnd,
	 cutoffFreq, cutoffFreqHi, filterLength, vBandInfo, cbInverted->Checked );
}
//---------------------------------------------------------------------------

void __fastcall TpFilterDlg::meBandsNumChange(TObject *Sender)
{
	  if( StrLen( Trim(meBandsNum->Text).c_str() ) == 0 ) return;
	  const int bandsNum = StrToInt(Trim(meBandsNum->Text));
	  const float hiFreq = StrToFloat(Trim(pMeFreqHi->Text));
	  const float diffFreq = hiFreq - StrToFloat(Trim(pMeFreqLo->Text));
	  //float harmonic = StrToFloat(meHarmAmp->Text);
	  nsDigitalFilter::PFILTERBAND_INFO pFI;
	  tcFilterBand->Tabs->Clear();
	  vBandInfo.clear();
	  for(int i=0; i<bandsNum;i++){
		tcFilterBand->Tabs->Add(IntToStr(i+1));
		pFI = new nsDigitalFilter::FILTERBAND_INFO;
		pFI->cutoffLo = hiFreq + (i*diffFreq);
		pFI->cutoffHi = pFI->cutoffLo + diffFreq;

		vBandInfo.push_back(pFI);
	  }
	  if(bandsNum > 0){

		meBandCutoffLo->Text = FloatToStr(vBandInfo.at(0)->cutoffLo);  //FloatToStr(vHarmInfo.at(0)->freq);
		meBandCutoffHi->Text = FloatToStr(vBandInfo.at(0)->cutoffHi);
	  }
}
//---------------------------------------------------------------------------

void __fastcall TpFilterDlg::TcFilterBandChange(TObject *Sender)
{
	unsigned tabIndex = tcFilterBand->TabIndex;
	meBandCutoffLo->Text =  FloatToStr(vBandInfo.at(tabIndex)->cutoffLo);
	meBandCutoffHi->Text =  FloatToStr(vBandInfo.at(tabIndex)->cutoffHi);
}
//---------------------------------------------------------------------------

void __fastcall TpFilterDlg::FilterWndOnSelect(TObject *Sender)
{
	eWnd = (WindowFunc::EWindow)pCbFilterWnd->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TpFilterDlg::BandLoHiOnExit(TObject *Sender)
{
	PFILTERBAND_INFO pFbi = vBandInfo.at(tcFilterBand->TabIndex);
	if(static_cast<TMaskEdit*>(Sender)->Tag == 1){

		pFbi->cutoffLo =  StrToFloat(meBandCutoffLo->Text);

	}else{

	   pFbi->cutoffHi  =  StrToFloat(meBandCutoffHi->Text);
	}
}
//---------------------------------------------------------------------------

