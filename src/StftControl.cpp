//$$---- Form CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "StftControl.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TpStftControl *pStftControl;
//---------------------------------------------------------------------------
__fastcall TpStftControl::TpStftControl(TComponent* Owner/*, WaveDisplayContainerManager& const rWDM*/)
	: TForm(Owner),
	rWaveDisplayManager(WaveDisplayContainerManager::GetInstance(NULL))
{

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*void __fastcall TpStftControl::StftContrastOnSelect(TObject *Sender)
{
	pStftContrast->SelEnd = pStftContrast->Position;
	rWaveDisplayManager.GetActiveContainer()->SetStftContrast(pStftContrast->Position);
	rWaveDisplayManager.GetActiveContainer()->DrawSpectrogram();
}   */
//---------------------------------------------------------------------------
void __fastcall TpStftControl::SfftFLenOnChange(TObject *Sender)
{
	rWaveDisplayManager.GetActiveContainer()->SetStftFrameLength(StrToInt(pFftLen->Items->Strings[pFftLen->ItemIndex]));
	rWaveDisplayManager.GetActiveContainer()->DrawSpectrogram();
}
//---------------------------------------------------------------------------
void __fastcall TpStftControl::StftWndOnChange(TObject *Sender){
	rWaveDisplayManager.GetActiveContainer()->SetStftFrameWindow((EWindow)pStftWnd->ItemIndex);
	rWaveDisplayManager.GetActiveContainer()->DrawSpectrogram();
}

void __fastcall TpStftControl::rgScaleClick(TObject *Sender)
{
	 if(rgScale->ItemIndex == 1) return;
	 rWaveDisplayManager.GetActiveContainer()->SetStftScale(rgScale->ItemIndex);
	 rWaveDisplayManager.GetActiveContainer()->DrawSpectrogram();
}
//---------------------------------------------------------------------------
void __fastcall TpStftControl::GradColorOnSelect(TObject *Sender)
{
	rWaveDisplayManager.GetActiveContainer()->SetStftColorMap(cbGradColor->ItemIndex);
	rWaveDisplayManager.GetActiveContainer()->DrawSpectrogram();
}

void __fastcall TpStftControl::StftControlOnShow(TObject *Sender)
{
	rWaveDisplayManager.GetActiveContainer()->DialogueShown(true);
}
//---------------------------------------------------------------------------
void __fastcall TpStftControl::StftControlOnClose(TObject *Sender,
      TCloseAction &Action)
{
	 rWaveDisplayManager.GetActiveContainer()->DialogueShown(false);
}

void __fastcall TpStftControl::FftOverlapOnChange(TObject *Sender)
{
	rWaveDisplayManager.GetActiveContainer()->SetFrameOverlap(pFftOverlap->ItemIndex);
	rWaveDisplayManager.GetActiveContainer()->DrawSpectrogram();
}
//---------------------------------------------------------------------------

