//$$---- Form CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ImportDlg.h"
#include "Util.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TRawImportDialog *RawImportDialog;
//TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TRawImportDialog::TRawImportDialog(TForm* Owner)
	: TForm(Owner)
{
	parent = (TForm1*)Owner;
	meImpSrRaw->Text = "8000";
}
//---------------------------------------------------------------------------
void __fastcall TRawImportDialog::btnImportCancelClick(TObject *Sender)
{
 	Close();	
}
//---------------------------------------------------------------------------
void __fastcall TRawImportDialog::btnImportOkClick(TObject *Sender)
{
	const unsigned  encodingValues [4] = { 8, 16, 24, 32};
	const unsigned byteOrderValues [3] = { 1, 2, 3 };
	const unsigned channelCountValues [2] = { 1, 2 };
	unsigned encodingVal = encodingValues[cbImpEncRaw->ItemIndex];
	//unsigned byteOrderVal = byteOrderValues[cbImpByteRaw->ItemIndex];
	unsigned channelVal = channelCountValues[cbImpChannRaw->ItemIndex];
	unsigned sampleRate = StrToInt(meImpSrRaw->Text);

	dataInfo.encoding =  encodingVal;
	dataInfo.numChannels = channelVal;
	dataInfo.sampleRate = sampleRate;

  // ((TForm1*)parent)->setImportData(&dataInfo);
 //  ((TForm1*)parent)->OpenRawFile(&dataInfo);
   parent->OpenRawFile(&dataInfo);

	Close();

}
void   TRawImportDialog::SetFileName(const char* fileName)
{
	   fName =    fileName;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

