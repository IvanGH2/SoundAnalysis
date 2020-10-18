//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef ImportDlgH
#define ImportDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Mask.hpp>
#include "Util.h"
#include <string>
#include "SoundAnalysis.h"
using namespace std;
//---------------------------------------------------------------------------
class TRawImportDialog : public TForm
{
__published:	// IDE-managed Components
	TStaticText *StaticText1;
	TStaticText *StaticText2;
	TStaticText *StaticText3;
	TStaticText *StaticText4;
	TComboBox *cbImpEncRaw;
	TComboBox *cbImpByteRaw;
	TComboBox *cbImpChannRaw;
	TMaskEdit *meImpSrRaw;
	TButton *btnImportOk;
	TButton *btnImportCancel;
	void __fastcall btnImportCancelClick(TObject *Sender);
	void __fastcall btnImportOkClick(TObject *Sender);
private:	// User declarations
	RAW_DATA_INFO dataInfo;
	TForm1* parent;
	string fName;
public:		// User declarations

	__fastcall TRawImportDialog(TForm* Owner);

	PRAW_DATA_INFO getRawDataInfo(){ return &dataInfo;}
	
	void   SetFileName(const char* fileName)  ;
};
//---------------------------------------------------------------------------
extern PACKAGE TRawImportDialog *RawImportDialog;
//---------------------------------------------------------------------------
#endif
