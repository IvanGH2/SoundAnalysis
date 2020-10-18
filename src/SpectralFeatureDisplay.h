//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef SpectralFeatureDisplayH
#define SpectralFeatureDisplayH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "Stft.h"
#include "WaveDisplayContainer.h"
#include "cspin.h"
#include "featurediff.h"
#include "featurematcher.h"
//---------------------------------------------------------------------------
class TfrmSpecFeatDisplay : public TForm
{
__published:	// IDE-managed Components
	TPaintBox *pbSpecFeatDisplay;
	TStaticText *StaticText1;
	TStaticText *StaticText2;
	TComboBox *cbSpecFeatX;
	TComboBox *cbSpecFeatY;
	TButton *btnRefreshDisplay;
	TCheckBox *cbShowAllTracks;
	TCSpinEdit *cSeTracks;
	TPanel *Panel1;
	TStaticText *stFirst;
	TStaticText *stSecond;
	TLabel *lblFirst;
	TLabel *lblSecond;
	TStaticText *StaticText3;
	TStaticText *StaticText4;
	TLabel *lblThird;
	TButton *btnMatchSound;
	TButton *btnSpEnvelope;
	TButton *btnMatchSpEnv;
	void __fastcall btnRefreshDisplayClick(TObject *Sender);
	void __fastcall SpectralFeaturesOnPaint(TObject *Sender);
	void __fastcall cbShowAllTracksClicked(TObject *Sender);
	void __fastcall cbSpectFeatXSelect(TObject *Sender);
	void __fastcall SelTrackOnChange(TObject *Sender);
	void __fastcall btnMatchSoundClick(TObject *Sender);
	void __fastcall btnSpEnvelopeClick(TObject *Sender);
	void __fastcall btnMatchSpEnvClick(TObject *Sender);

private:	// User declarations
	enum EAxis { X, Y };
	SpectrogramDisplayData *pSd;
	std::vector<PTWaveDisplayContainer> *pVsd;
	std::vector<float*> vMag0, vPhase0, vMag1, vPhase1;
	void DrawSpectralCentroidAxis(EAxis);
	void DrawSpectralFeatureAxis(EAxis, float, float);
	void DrawFeatures();
	unsigned margin, numPartitionsx, numPartitionsy;
	const unsigned maxTracks;
	FeatureDiff *pFd;
	FeatureMatcher *pFMatcher;
	TComponent *pParent;

	void   	 GetFeatureMinMax(const std::vector<float> & vF,float &fMin, float &fMax);
	void   	 MatchSpectralEnvelopes();
	void 	 Normalize(std::vector<float> *);
	unsigned GetMaxFreq() const;
	unsigned GetNumTracks() const;
	void  	 DrawLegend();
	std::vector<float>* GetSelectedFeatureX(unsigned);
	std::vector<float*> vMod;
	float *pFeatureMatch;
  // 	std::vector<float> *pCentroid, *pFlux;

public:		// User declarations
	__fastcall TfrmSpecFeatDisplay(TComponent* Owner);

	void SetSpectralFeaturesProvider(SpectrogramDisplayData* p){ pSd = p; }

	void SetSpectralFeaturesProvider(std::vector<PTWaveDisplayContainer> *v){ pVsd = v; }

};
//---------------------------------------------------------------------------
extern PACKAGE TfrmSpecFeatDisplay *frmSpecFeatDisplay;
//---------------------------------------------------------------------------
#endif
