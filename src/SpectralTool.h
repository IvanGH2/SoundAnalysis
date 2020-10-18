//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef SpectralToolH
#define SpectralToolH
//---------------------------------------------------------------------------
#include "SoundAnalysis.h"
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>
#include "WaveDisplayContainerManager.h"
#include <Mask.hpp>
#include <Buttons.hpp>

#include "DumpToFileDlg.h"
#include "FeatureMatcher.h"
//---------------------------------------------------------------------------
class TpSpectralForm : public TForm
{
__published:	// IDE-managed Components
	TPageControl *pcSpectralTool;
	TTabSheet *tsPowerSpectrum;
	TTabSheet *tsCentroid;
	TTabSheet *tsFlux;
	TTabSheet *tsRolloff;
	TStringGrid *sgSpectralData;
	TStaticText *StaticText5;
	TMaskEdit *meFrame;
	TButton *btnFrameRefresh;
	TStringGrid *sgSpectralFlux;
	TStringGrid *sgSpectralCentroid;
	TStaticText *StaticText6;
	TMaskEdit *meCentroidThreshold;
	TButton *btnCentroidRefresh;
	TStringGrid *sgSpectralRolloff;
	TButton *btnDumpToFile;
	TGroupBox *GroupBox1;
	TStaticText *StaticText1;
	TLabel *lblStartFrame;
	TStaticText *StaticText2;
	TLabel *lblEndFrame;
	TStaticText *StaticText3;
	TLabel *lblStartBin;
	TStaticText *StaticText4;
	TLabel *lblEndBin;
	TTabSheet *tsAvgPowerSpectrum;
	TStringGrid *sgAvgSpectralData;
	void __fastcall SpectralFormOnShow(TObject *Sender);
	void __fastcall meFrameOnExit(TObject *Sender);
	void __fastcall btnFrameRefreshClick(TObject *Sender);
	void __fastcall SpectralToolOnChange(TObject *Sender);
	void __fastcall btnThresholdRefresh(TObject *Sender);
	void __fastcall meCentroidOnExit(TObject *Sender);
//	void __fastcall btnStftPreviewClick(TObject *Sender);
	void __fastcall btnDumpToFileClick(TObject *Sender);
	void __fastcall SpectralFormOnClose(TObject *Sender, TCloseAction &Action);

private:	// User declarations

	std::vector<float*> vMod, vMag, vPhase;
	float *pFeatureMatch;
	TComponent *pParent;
	TTabSheet *pCurrTab;
	//SELECTION_RANGE stftSel;
	SpectrogramDisplayData *pSpectralData;
	unsigned startFrame, endFrame, startBin, endBin, binsNum;
	const WaveDisplayContainerManager& rWaveDisplayManager;
	bool refreshCentroid, refreshRolloff, refreshFlux,
	 refreshSpectrogram, refreshAvgMag;

	void PopulateSpectralData(unsigned);
	void DrawLabels();
	void ShowSelectionInfo();
	void PopulateSpectralFluxData();
	void PopulateSpectralCentroidData();
	void PopulateSpectralRolloffData();
	//void PopulateZeroCrossingRateData();
	void SetStartAndEndFrame();
	void LabelColumns(TStringGrid *pSg);
	void SelectFramesToProcess();
	void PopulateAverageMagnitudes();

public:		// User declarations
	__fastcall TpSpectralForm(TComponent* Owner);
	void SelectionChanged();
	void FeatureChange(nsAnalysis::EAnalysisSource);
	void DelegateMainWnd(const FeatureMatcher &fM);
	void ContainerChanged();

};
//---------------------------------------------------------------------------
extern PACKAGE TpSpectralForm *pSpectralForm;
//---------------------------------------------------------------------------
#endif
