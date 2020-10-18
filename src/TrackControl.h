//----------------------------------------------------------------------------
#ifndef TrackControlH
#define TrackControlH
//----------------------------------------------------------------------------
#include <vcl\ExtCtrls.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\Classes.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Windows.hpp>
#include <vcl\System.hpp>

#include <string>
#include <vector>
#include "Util.h"
#include "SoundAnalysis.h"

//----------------------------------------------------------------------------
using namespace nsPlayback;



class TTrackControlDlg : public TForm
{
__published:
	TCheckBox *cbPlaybackSelection;
	TScrollBox *sbTracks;
	TCheckBox *cbHiddenTracks;
	TStaticText *StaticText4;
	TStaticText *StaticText2;
	TStaticText *StaticText3;
	TStaticText *StaticText1;
	TSpeedButton *sbHideTracks;

	void __fastcall TrackControlOnShow(TObject *Sender);
	void __fastcall cbHideOnClick(TObject *Sender);
	void __fastcall cbPlayOnClick(TObject *Sender);
	void __fastcall sbHideTracksClick(TObject *Sender);
private:

	std::vector<TRACK_INFO> *pVTrackInfo;
	void RemoveOldTracks();
	void __fastcall TrackSelOnClick(TObject *Sender);
	void __fastcall TrackHideOnClick(TObject *Sender);
	TForm1 *parent;

public:
	virtual __fastcall TTrackControlDlg(TComponent* AOwner);

	void SetTrackInfo(std::vector<TRACK_INFO> *pT);

	std::vector<TRACK_INFO> *GetTrackInfo() { return pVTrackInfo; }

	void UpdateTrackInfo();

	void SetParent(TComponent *p) { parent = static_cast<TForm1*>(p); }
};
//----------------------------------------------------------------------------
extern PACKAGE TTrackControlDlg *TrackControlDlg;
//----------------------------------------------------------------------------
#endif    
