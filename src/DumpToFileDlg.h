//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef DumpToFileDlgH
#define DumpToFileDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>

#include "Stft.h"

#include <Dialogs.hpp>
//---------------------------------------------------------------------------

class TdlgDumpToFile : public TForm
{
__published:	// IDE-managed Components
	TRadioGroup *rgDataSource;
	TRadioGroup *rgChannels;
	TGroupBox *gbLimitRange;
	TStaticText *StaticText2;
	TMaskEdit *meStartFrame;
	TStaticText *StaticText1;
	TStaticText *StaticText3;
	TStaticText *StaticText4;
	TMaskEdit *meEndFrame;
	TMaskEdit *meStartBin;
	TMaskEdit *meEndBin;
	TButton *btnChooseFile;
	TButton *btnSave;
	TSaveDialog *SaveDialog1;
	TLabel *lblSaveLoc;
	TStaticText *StaticText5;
	TGroupBox *GroupBox1;
	TCheckBox *cbFreq;
	TCheckBox *cbBasicInfo;
	TRadioGroup *rgChannelSeparation;
	TRadioGroup *rgFrameValues;
	TCheckBox *cbFrame;
	TButton *btnClose;
	void __fastcall btnChooseFileClick(TObject *Sender);
	void __fastcall btnSaveClick(TObject *Sender);
	void __fastcall btnCloseClick(TObject *Sender);

private:	// User declarations
	TComponent *parent;
	AnsiString fileName;
	SpectrogramDisplayData *pSpectralData;
	unsigned startFrame, endFrame, startBin, endBin;
public:		// User declarations
	__fastcall TdlgDumpToFile(TComponent* Owner);
	__fastcall TdlgDumpToFile(TComponent* Owner,SpectrogramDisplayData *pSD);
	void Init(unsigned sFrame, unsigned eFrame, unsigned sBin, unsigned eBin);
};
//---------------------------------------------------------------------------
extern PACKAGE TdlgDumpToFile *dlgDumpToFile;
//---------------------------------------------------------------------------
#endif
