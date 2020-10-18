//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef ProgressIndicatorH
#define ProgressIndicatorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmProgressDlg : public TForm
{
__published:	// IDE-managed Components
	TLabel *lblProcessing;
	TProgressBar *pbProcessing;
	TStaticText *stTotalProcess;
	TStaticText *stProcess;
	TProgressBar *pbTotalProcessing;
private:	// User declarations
public:		// User declarations
	__fastcall TfrmProgressDlg(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmProgressDlg *frmProgressDlg;
//---------------------------------------------------------------------------
#endif
