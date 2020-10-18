//$$---- Form CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DumpToFileDlg.h"
#include "FileLayout.h"
#include "SpectralTool.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TdlgDumpToFile *dlgDumpToFile;
//---------------------------------------------------------------------------
__fastcall TdlgDumpToFile::TdlgDumpToFile(TComponent* Owner)
	: TForm(Owner)
{
}
__fastcall TdlgDumpToFile::TdlgDumpToFile(TComponent* pOwner,SpectrogramDisplayData *pSD)
	: TForm(pOwner), pSpectralData(pSD), parent((TComponent*)pOwner)
{

}
void TdlgDumpToFile::Init(unsigned sFrame, unsigned eFrame, unsigned sBin, unsigned eBin){

	meStartFrame->Text = IntToStr(sFrame);
	meEndFrame->Text   = IntToStr(eFrame);
	meStartBin->Text   = IntToStr(sBin);
	meEndBin->Text 	   = IntToStr(eBin);
}
//---------------------------------------------------------------------------
void __fastcall TdlgDumpToFile::btnChooseFileClick(TObject *Sender)
{
	if(SaveDialog1->Execute() && SaveDialog1->FileName !=0){
		btnSave->Enabled = true;
		fileName = SaveDialog1->FileName;
	}else{
		 btnSave->Enabled = false;
		 fileName = SaveDialog1->FileName ;
	}
	lblSaveLoc->Caption = fileName;
}
//---------------------------------------------------------------------------
void __fastcall TdlgDumpToFile::btnSaveClick(TObject *Sender)
{
	int status;
	FileLayout::eSrc = (FileLayout::eSource)rgDataSource->ItemIndex;
	FileLayout::eCs  = (FileLayout::eChannelSeparation)rgChannelSeparation->ItemIndex;
	FileLayout::eFv  = (FileLayout::eFrameValues)rgFrameValues->ItemIndex;
	FileLayout::eCh  = (FileLayout::eChannels)rgChannels->ItemIndex;

	FileLayout::enumFrames = cbFrame->Checked;
	FileLayout::stftInfo   = cbBasicInfo->Checked;

	((TpSpectralForm*)parent)->FeatureChange((nsAnalysis::EAnalysisSource)rgDataSource->ItemIndex);
	pSpectralData->DumpToFile(lblSaveLoc->Caption.c_str(), StrToInt(Trim(meStartFrame->Text)),
	StrToInt(Trim(meEndFrame->Text)), StrToInt(Trim(meStartBin->Text)), StrToInt(Trim(meEndBin->Text)), status);

	if(status==0){
		ShowMessage("File saved to " + fileName);
	}else{
	   ShowMessage("THere's been an error saving the file");
	}
}
//---------------------------------------------------------------------------

void __fastcall TdlgDumpToFile::btnCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------

