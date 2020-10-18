//$$---- EXE CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("SoundAnalysis.cpp", Form1);
USEFORM("ImportDlg.cpp", RawImportDialog);
USEFORM("ToneGenDlg.cpp", ToneGeneratorDlg);
USEFORM("StftControl.cpp", pStftControl);
USEFORM("SpectralTool.cpp", pSpectralForm);
USEFORM("FilterGeneratorDlg.cpp", pFilterDlg);
USEFORM("ProgressIndicator.cpp", frmProgressDlg);
USEFORM("TrackControl.cpp", TrackControlDlg);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->CreateForm(__classid(TForm1), &Form1);
		Application->CreateForm(__classid(TRawImportDialog), &RawImportDialog);
		Application->CreateForm(__classid(TToneGeneratorDlg), &ToneGeneratorDlg);
		Application->CreateForm(__classid(TpFilterDlg), &pFilterDlg);
		Application->CreateForm(__classid(TpStftControl), &pStftControl);
		Application->CreateForm(__classid(TpSpectralForm), &pSpectralForm);
		Application->CreateForm(__classid(TpFilterDlg), &pFilterDlg);
		Application->CreateForm(__classid(TfrmProgressDlg), &frmProgressDlg);
		Application->CreateForm(__classid(TTrackControlDlg), &TrackControlDlg);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
