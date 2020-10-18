//$$---- Form CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ToneGenDlg.h"
#include "SoundAnalysis.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TToneGeneratorDlg *ToneGeneratorDlg;
//---------------------------------------------------------------------------
__fastcall TToneGeneratorDlg::TToneGeneratorDlg(TComponent* owner)
	: TForm(owner), parent(owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TToneGeneratorDlg::meToneHarNumChange(TObject *Sender)
{
	  if( StrLen( Trim(meToneHarNum->Text).c_str() ) == 0 ) return;
	  int harNum = StrToInt(meToneHarNum->Text);
	  float fundamental = StrToFloat(meToneFreq->Text);
	  //float harmonic = StrToFloat(meHarmAmp->Text);
	  nsToneGenerator::PHARMONICS_INFO pHI;
	  tcHarm->Tabs->Clear();
	  vHarmInfo.clear();
	  for(int i=0; i<harNum;i++){
		tcHarm->Tabs->Add(IntToStr(i+1));
		pHI = new nsToneGenerator::HARMONICS_INFO;
		pHI->freq = fundamental * (i+2);
		pHI->amp = 0.6f;
		pHI->phase = 0.0f;
		vHarmInfo.push_back(pHI);
	  }
	  if(harNum > 0){
		meHarmFreq->Text = FloatToStr(vHarmInfo.at(0)->freq);
		meHarmAmp->Text = "0,6";
		meHarmPhase->Text = "0,0";
	  }
}
//---------------------------------------------------------------------------
void __fastcall TToneGeneratorDlg::meHarmAmpExit(TObject *Sender)
{
   if( StrLen( Trim(meHarmAmp->Text).c_str() ) == 0 ) return;
   float val = StrToFloat(meHarmAmp->Text);

   if(val > 1){
		ShowMessage("Valid input values are in the 0 - 1 range. Please, change the value");
		meHarmAmp->Text = "0,0";
	}else{
		vHarmInfo.at(tcHarm->TabIndex)->amp = val;
	}
}
//---------------------------------------------------------------------------
void __fastcall TToneGeneratorDlg::meToneAmpExit(TObject *Sender)
{
	if( StrLen( Trim(meToneAmp->Text).c_str() ) == 0 ) return;
	float val = StrToFloat(meToneAmp->Text);

   if(val > 1){
	ShowMessage("Valid input values are in the 0-1.0 range. Please, change the value");
	meToneAmp->Text = "0,0";
	}
}
/*void __fastcall TToneGeneratorDlg::CheckAmpRange(float val){


}   */
//---------------------------------------------------------------------------
void __fastcall TToneGeneratorDlg::tcHarmChange(TObject *Sender)
{
   //	meHarmAmp->OnExit(0);//force the event
	const unsigned tabIndex = tcHarm->TabIndex;

	meHarmFreq->Text =  FloatToStr(vHarmInfo.at(tabIndex)->freq);
	meHarmAmp->Text =   FloatToStr(vHarmInfo.at(tabIndex)->amp);
	meHarmPhase->Text = FloatToStr(vHarmInfo.at(tabIndex)->phase);
}
//---------------------------------------------------------------------------
void __fastcall TToneGeneratorDlg::memeHarmFreqExit(TObject *Sender)
{
	if( StrLen( Trim(meHarmFreq->Text).c_str() ) == 0 ) return;
	float val = StrToFloat(Trim(meHarmFreq->Text));
	vHarmInfo.at(tcHarm->TabIndex)->freq = val;
}
//---------------------------------------------------------------------------
void __fastcall TToneGeneratorDlg::meHarmPhaseExit(TObject *Sender)
{
	if( StrLen( Trim(meHarmPhase->Text).c_str() ) == 0 ) return;
	float val = StrToFloat(Trim(meHarmPhase->Text));

	if(val > PI2){
		ShowMessage("Valid input values are in the 0 - 2PI range. Please, change the value");
		meHarmAmp->Text = "0,0";
	}else{
		vHarmInfo.at(tcHarm->TabIndex)->phase = val;
	}
}
//---------------------------------------------------------------------------
void __fastcall TToneGeneratorDlg::btnToneCancelClick(TObject *Sender)
{
	Close();	
}
//---------------------------------------------------------------------------
void __fastcall TToneGeneratorDlg::btnToneOKClick(TObject *Sender)
{
	const int sampleRate = StrLen( Trim(meHarmPhase->Text).c_str() ) == 0 ? 8000 : StrToInt(Trim(meToneSampleRate->Text));
	dataInfo.encoding = BIT_32;
	dataInfo.numChannels = 1;
	dataInfo.sampleRate = sampleRate;

	toneInfo.eToneType	= (nsToneGenerator::EToneType)cbToneType->ItemIndex;
	toneInfo.freq 		= StrToFloat(Trim(meToneFreq->Text));
	toneInfo.amp 		= StrToFloat(Trim(meToneAmp->Text));
	toneInfo.duration 	= StrToFloat(Trim(meToneTime->Text));
	toneInfo.sampleRate = StrToFloat(Trim(meToneSampleRate->Text));
	toneInfo.harmNum 	= vHarmInfo.size()+1;

	((TForm1*)parent)->GenerateTone(toneInfo, vHarmInfo);

	Close();

}
//---------------------------------------------------------------------------
