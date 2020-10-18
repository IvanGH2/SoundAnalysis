//$$---- Form CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SpectralFeatureDisplay.h"
#include "SpectralTool.h"
#include "featuregrouper.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cspin"
#pragma resource "*.dfm"
TfrmSpecFeatDisplay *frmSpecFeatDisplay;
//---------------------------------------------------------------------------
__fastcall TfrmSpecFeatDisplay::TfrmSpecFeatDisplay(TComponent* Owner)
	: TForm(Owner), maxTracks(4), pParent((TForm1*)Owner)//pParent((TpSpectralForm*)Owner)
{
   //	pbSpecFeatDisplay->Canvas->Rectangle(TRect(pbSpecFeatDisplay->ClientRect));
   margin = 40;
   numPartitionsx = 18;
   numPartitionsy = 4;
}
//---------------------------------------------------------------------------
void TfrmSpecFeatDisplay::DrawSpectralCentroidAxis(EAxis eAxis){

	unsigned len = pbSpecFeatDisplay->Width-(2*margin);

	const unsigned lenHalf = len/2;

	const unsigned startOffset = margin;

	const unsigned indLength = 4;

	const unsigned maxFreq = pSd->GetMaxFrequency();

	unsigned deltaValue;

	unsigned partitionLen;
				//y axis
	unsigned xStart, yStart, xEnd, yEnd ;

	if(eAxis==Y){  //for now we're only using the Y axis for this

		deltaValue = maxFreq/numPartitionsy;

		len = pbSpecFeatDisplay->Height-(2*margin);

		partitionLen = len/numPartitionsy;

		xStart = margin-indLength;

		yStart = pbSpecFeatDisplay->Height-margin;

		xEnd   = margin;

		pbSpecFeatDisplay->Canvas->MoveTo(margin, margin);

		pbSpecFeatDisplay->Canvas->LineTo(margin, yStart);

		float maxValLog = log10(maxFreq);

		for(int i=0;i<numPartitionsy+1;i++){
		 //	if (eScale==LOG && i>0)
			//partitionLen = i==0 ? 0 : len*(log10(deltaValue*i)/maxValLog);

		   //	unsigned yPos = yStart- partitionLen; //yStart-i*partitionLen

			unsigned yPos = yStart- i*partitionLen; //yStart-i*partitionLen

			pbSpecFeatDisplay->Canvas->MoveTo(xStart, yPos);

			pbSpecFeatDisplay->Canvas->LineTo(margin, yPos);

			pbSpecFeatDisplay->Canvas->TextOut(5,yPos-5, IntToStr(i*deltaValue));

		/*	pbSpecFeatDisplay->Canvas->MoveTo(xStart, yPos);
			pbSpecFeatDisplay->Canvas->LineTo(margin, yPos);
			pbSpecFeatDisplay->Canvas->TextOut(5,yPos-5, IntToStr(i*deltaValue)); */
			}
			//yStart = pbSpectralFeatures->Height-margin;

			//yEnd = 10;
		}else{//X axis  not fully implemented

			yStart = pbSpecFeatDisplay->Height-margin;

			yEnd = yStart+indLength;

			pbSpecFeatDisplay->Canvas->MoveTo(margin, yStart);

			pbSpecFeatDisplay->Canvas->LineTo(margin, len);

			for(int i=0;i<numPartitionsx;i++){

				unsigned pos = margin+i*partitionLen;

				pbSpecFeatDisplay->Canvas->MoveTo(pos, yStart);

				pbSpecFeatDisplay->Canvas->LineTo(pos, yEnd);

				}
		}
}
void TfrmSpecFeatDisplay::DrawSpectralFeatureAxis(EAxis eAxis, float minVal, float maxVal){

				const unsigned indLength = 4;
				const unsigned len = pbSpecFeatDisplay->Width-(2*margin);
				const unsigned lenHalf = len/2;

				const unsigned startOffset = margin;
				unsigned partitionLen;

				const unsigned yStart = pbSpecFeatDisplay->Height-margin;   //for aux
				const unsigned yEnd = yStart+indLength;      //aux

				const int minValue = minVal;
				const int maxValue = maxVal;

				int deltaValue;

				if(eAxis==X){ //since we're only using the X axis for this feature but

					partitionLen = len/numPartitionsx;

					deltaValue = (maxValue-minValue)/numPartitionsx;

					pbSpecFeatDisplay->Canvas->MoveTo(startOffset, yStart);

					pbSpecFeatDisplay->Canvas->LineTo(startOffset+len, yStart);

					for(int i=0;i<numPartitionsx;i++){

						unsigned pos = startOffset+i*partitionLen;

						pbSpecFeatDisplay->Canvas->MoveTo(pos, yStart);

						pbSpecFeatDisplay->Canvas->LineTo(pos, yEnd);

						pbSpecFeatDisplay->Canvas->TextOut(pos-8,yStart+5, IntToStr(i*deltaValue));
				}
		}
}
unsigned TfrmSpecFeatDisplay::GetMaxFreq() const {

	const unsigned numTracks = cbShowAllTracks->Checked ? pVsd->size() : 1;

	unsigned maxFreq = pVsd->at(0)->GetSpectralData()->GetMaxFrequency();

	for(int i=1;i<numTracks;i++){
		if(maxFreq<pVsd->at(i)->GetSpectralData()->GetMaxFrequency())
			maxFreq = pVsd->at(i)->GetSpectralData()->GetMaxFrequency();
	}
	return maxFreq;
}
void TfrmSpecFeatDisplay::Normalize(std::vector<float> * v){

	const unsigned size = v->size();

	float max = (*v)[0];
	float currVal;
	int i;
	for(i=0;i<size;i++){
		currVal = (*v)[i];
		if(currVal>max)	max = currVal;
	}
   for(i=0;i<size;i++)(*v)[i]=(*v)[i]/max;
}
unsigned  TfrmSpecFeatDisplay::GetNumTracks() const{

	const unsigned nTracks = pVsd->size();

	return cbShowAllTracks->Checked ?
			nTracks <= maxTracks ? nTracks : maxTracks : 1;
}
void  TfrmSpecFeatDisplay::DrawFeatures(){

		 const TColor colour[]  = { clTeal, clRed, clBlue, clGreen };
		 const unsigned dimRed = 60;
 //		 const unsigned nTracks = pVsd->size();
	//	 const unsigned nColours = sizeof(colour)/sizeof(TColor);
		 const unsigned numTracks = GetNumTracks();
		// const unsigned numFrames = vFlux.size();
		 const unsigned len 	= pbSpecFeatDisplay->Width-dimRed;
		 const unsigned height  = pbSpecFeatDisplay->Height-dimRed;
		 int i;
		 float featureMin, featureMax;
	//	 featureMin = featureMax = vFlux[0];
		const unsigned maxFreq = GetMaxFreq();
		const unsigned startTrack = !cbShowAllTracks->Checked ? cSeTracks->Value-1 : 0;
		//pVsd->at(j)->GetSpectralData()->GetMaxFrequency();
		for(int j=startTrack;j<numTracks+startTrack;j++){
			Normalize(pVsd->at(j)->GetSpectralData()->GetSpectralFlux());
		}

		 pbSpecFeatDisplay->Canvas->Brush->Color = clWhite;
		 pbSpecFeatDisplay->Canvas->FillRect(TRect(pbSpecFeatDisplay->ClientRect));

		 for(int j=startTrack;j<startTrack+numTracks;j++){
		  //	pSd = pVsd->at(j)->GetSpectralData();
			std::vector<float> &vCentroid = *pVsd->at(j)->GetSpectralData()->GetSpectralCentroid();
			std::vector<float> &vFeature  = *GetSelectedFeatureX(j);//*pVsd->at(j)->GetSpectralData()->GetSpectralFlux();

		 unsigned numFrames = vFeature.size();
		 if(j==0) featureMin = featureMax = vFeature[0];
			for(i=1;i<numFrames;i++){
				float fluxVal = vFeature[i];
				if(fluxVal>featureMax) featureMax = fluxVal;
				else if(fluxVal<featureMin) featureMin = fluxVal;
		 }

		 const unsigned lenX   =  pbSpecFeatDisplay->Width-(2*margin);
		 const unsigned lenY   =  pbSpecFeatDisplay->Height-(2*margin);
		 const unsigned yStart =  pbSpecFeatDisplay->Height-margin;

		 const unsigned featureRange = featureMax-featureMin;

		 for(i=1;i<numFrames;i++){
			float x = (vFeature[i]-featureMin)/featureRange * lenX;
			float y = vCentroid[i]/maxFreq * lenY;

		   //	TRect r(margin+x-2, yStart-y-2, margin+x+2, yStart-y+2);
			pbSpecFeatDisplay->Canvas->Pen->Color = colour[j];
			pbSpecFeatDisplay->Canvas->Rectangle(TRect(margin+x-2, yStart-y-2, margin+x+2, yStart-y+2));

		  }
	}
	pbSpecFeatDisplay->Canvas->Pen->Color = clBlack;
	DrawSpectralFeatureAxis(X,featureMin, featureMax);
	DrawSpectralCentroidAxis(Y);
}
std::vector<float>*  TfrmSpecFeatDisplay::GetSelectedFeatureX(unsigned whichTrack){

	 std::vector<float> *vFeature = 0;

	 switch(cbSpecFeatX->ItemIndex){

		case 0:
			vFeature = pVsd->at(whichTrack)->GetSpectralData()->GetSpectralFlux();
			break;
		case 1:
			vFeature = pVsd->at(whichTrack)->GetSpectralData()->GetSpectralCentroid();
			break;
		case 2:
			vFeature = pVsd->at(whichTrack)->GetSpectralData()->GetSpectralRolloff();
			break;
		default:
			break;
	 }
	 return vFeature;
}
void __fastcall TfrmSpecFeatDisplay::btnRefreshDisplayClick(TObject *Sender)
{
	   DrawFeatures();
}
//---------------------------------------------------------------------------
void  TfrmSpecFeatDisplay::GetFeatureMinMax(const std::vector<float> & vF,float &fMin, float &fMax){

	const unsigned numFrames = vF.size();

	fMin = fMax = vF[0];

	for(int i=1;i<numFrames;i++){

		float fVal = vF[i];

		if(fVal>fMax) fMax = fVal;

		else if(fVal<fMin) fMin = fVal;

		}
}
void __fastcall TfrmSpecFeatDisplay::SpectralFeaturesOnPaint(TObject *Sender)
{
		cSeTracks->MaxValue = GetNumTracks();
		DrawFeatures();
		DrawLegend();
}
void  TfrmSpecFeatDisplay::DrawLegend(){

	  TLabel *lblTracks[] = { lblFirst, lblSecond, lblThird };
	  const unsigned size = pVsd->size();
	  for(int i=0; i<size;i++){
		lblTracks[i]->Caption = pVsd->at(i)->GetTrackName().c_str();
	  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmSpecFeatDisplay::cbShowAllTracksClicked(TObject *Sender)
{
	cSeTracks->Enabled = cbShowAllTracks->Checked ? false : true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSpecFeatDisplay::cbSpectFeatXSelect(TObject *Sender)
{
	DrawFeatures();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSpecFeatDisplay::SelTrackOnChange(TObject *Sender)
{
	if( cSeTracks->MaxValue==1 ){
		if(cSeTracks->Value > cSeTracks->MaxValue) cSeTracks->Value = cSeTracks->MaxValue;
		else if(cSeTracks->Value < cSeTracks->MinValue) cSeTracks->Value = cSeTracks->MinValue;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmSpecFeatDisplay::btnMatchSoundClick(TObject *Sender)
{
   FEATURE_2D_IN fIn;
   fIn.featureMaxVal0   = 8000;
   fIn.featureMinVal0   = 100;
   fIn.featureStep0 	= 40;


   fIn.featureMaxVal1 	= 600;
   fIn.featureMinVal1 	= 40;
   fIn.featureStep1 	= 10;
   pFMatcher = new FeatureMatcher();
   SpectrogramDisplayData *pS = pVsd->at(0)->GetSpectralData();

   float *  pWndData =  pS->GetWndData();

   pFMatcher->SetWndFuncData(pWndData);

 //  std::vector<float> *vv  = pVsd->at(0)->GetSpectralData()->GetSpectralCentroid();
 //  std::vector<float> *vv1  = pVsd->at(1)->GetSpectralData()->GetSpectralFlux();
   pFMatcher->MatchFeatures(*pVsd->at(0)->GetSpectralData()->GetSpectralCentroid(),
							*GetSelectedFeatureX(0),
							*pVsd->at(1)->GetSpectralData()->GetSpectralCentroid(),
							*GetSelectedFeatureX(1),
							*pVsd->at(0)->GetSpectralData()->GetFFTFrames(),
							 pVsd->at(0)->GetSpectralData()->GetOverlappingFramesNumber(),
							 pVsd->at(0)->GetSpectralData()->GetNumSamples(),
							 fIn);

  // (static_cast<TForm1*>(pParent))->DrawMatched(*pFMatcher);

   Close();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSpecFeatDisplay::btnSpEnvelopeClick(TObject *Sender)
{
		const TColor colour[]  = { clTeal, clRed, clBlue, clGreen };

		pbSpecFeatDisplay->Canvas->Brush->Color = clWhite;
		pbSpecFeatDisplay->Canvas->FillRect(TRect(pbSpecFeatDisplay->ClientRect));
		
		const unsigned halfLen = pVsd->at(0)->GetSpectralData()->GetFrameLength()>>1;
		const unsigned numTracks = pVsd->size();
		int i;
		float featureMin, featureMax;

		for(int j=0;j<numTracks;j++){

			const unsigned lenX   =  pbSpecFeatDisplay->Width-(2*margin);
			const unsigned lenY   =  pbSpecFeatDisplay->Height-(2*margin);
			const unsigned yStart =  pbSpecFeatDisplay->Height-margin;

			std::vector<float> &vAm = *pVsd->at(j)->GetSpectralData()->GetAverageMagnitudes();

			if(j==0) featureMin = featureMax = fabs(vAm[1]);
			for(i=2;i<=halfLen;i++){
				float featVal = fabs(vAm[i]);
				if(featVal>featureMax) featureMax = featVal;
				else if(featVal<featureMin) featureMin = featVal;
			}
			const unsigned featureRange = featureMax-featureMin;

			for(i=1;i<=halfLen;i++){
				float x = static_cast<float>(i)/halfLen * lenX;
				float y = fabs(vAm[i])/featureRange * lenY;

			pbSpecFeatDisplay->Canvas->Pen->Color = colour[j];
			pbSpecFeatDisplay->Canvas->Rectangle(TRect(margin+x-2, yStart-y-2, margin+x+2, yStart-y+2));

		}
	}
}

//---------------------------------------------------------------------------
void TfrmSpecFeatDisplay::MatchSpectralEnvelopes(){

		const unsigned fftLen = pVsd->at(0)->GetSpectralData()->GetFrameLength();
		const unsigned halfLen = fftLen>>1;
		const unsigned numFrames = pVsd->at(0)->GetSpectralData()->GetOverlappingFramesNumber();
		const unsigned numTracks = pVsd->size();
	   //	if(numTracks<2)

		std::vector<float> vMagRatio;
	   //	std::vector<float*> &vFft = *pVsd->at(0)->GetSpectralData()->GetFFTFrames();
		float *pMag, *pPhase;
		//processing track 1
		std::vector<float*> &vFft0 = *pVsd->at(0)->GetSpectralData()->GetFFTFrames();
		for(int i=0;i<numFrames;i++){
			pMag 	= new float[halfLen];
			pPhase 	= new float[halfLen];
			DspUtil::ComputeMagnitudeAndPhase(vFft0[i], pMag, pPhase, fftLen);
			vMag0.push_back(pMag);
			vPhase0.push_back(pPhase);
		}
		float *vAm0 = DspUtil::GetAverageMagnitudes(vMag0, fftLen);

		//processing track 2
		std::vector<float*> &vFft1 = *pVsd->at(1)->GetSpectralData()->GetFFTFrames();
		const unsigned numFrames1 = pVsd->at(1)->GetSpectralData()->GetOverlappingFramesNumber();
		for(int i=0;i<numFrames1;i++){
			pMag = new float[halfLen];
			pPhase = new float[halfLen];
			DspUtil::ComputeMagnitudeAndPhase(vFft1[i], pMag, pPhase, fftLen);
			vMag1.push_back(pMag);
			vPhase1.push_back(pPhase);
		}
		float *vAm1 = DspUtil::GetAverageMagnitudes(vMag1, fftLen);

		float *pWndFunc =  pVsd->at(0)->GetSpectralData()->GetWndData();

		vMagRatio.push_back(1.0f);
		int cnt=0;

		for(int i=1;i<halfLen;i++){
		   //	float ratio =  vAm0[i]/vAm1[i];
		//	float diff =  vAm0[i]- vAm1[i];  //+ means that vAm0[i]> vAm1[i]

			float ratio = vAm0[i] / vAm1[i];

			vMagRatio.push_back(ratio);
		}
		vMagRatio.push_back(1.0f);
		FFT fft;
		float *pMod0, *pMod1;
		const unsigned cpySize =  sizeof(float)*fftLen;
		for(int i=0;i<numFrames;i++){

			float *pMag   = vMag0[i];
			float *pPhase = vPhase0[i];
			pMod0 = new float[fftLen];

		  //	memcpy(pMod0,pFft, cpySize);
		   //	pMod0[halfLen] *= vMagRatio[halfLen];

			for(int j=0,k=1;j<fftLen;j+=2,k++){

				float cMag = pMag[k] * vMagRatio[k];
				pMod0[j]   = cMag*cos(pPhase[k]);
				pMod0[j+1] = cMag*sin(pPhase[k]);
			}
			fft.ComputeRealFFT(pMod0, fftLen, true);

			for(int j=0;j<fftLen;j++){

				pMod0[j] *= pWndFunc[j];
			}
			vMod.push_back(pMod0);
		}

		const unsigned outLen = (vMod.size()>>1)*fftLen-halfLen;

		pFeatureMatch = new float[outLen];
		float *pWndSqSum = new float[outLen];

		int k;
		const float c = 1.0f/halfLen;
		float wSquared = 0;

		for(int i=0; i<numFrames-1; i++){

		pMod0 =  vMod[i];
		pMod1 =  vMod[i+1];
		k = (i)*halfLen;

		for(int j=0;j<halfLen;j++){
			float val = (pMod1[j]+pMod0[halfLen+j]);
			wSquared = pWndFunc[j]+pWndFunc[halfLen+j];
			wSquared *=  wSquared;
			pWndSqSum[k+j]=  wSquared;

			pFeatureMatch[k+j] = val*c;
		}
	}
	for(int i=0; i<outLen-fftLen; i++){
	   pFeatureMatch[i] /= pWndSqSum[i];
	}

   //	(static_cast<TForm1*>(pParent))->DrawMatchedWave(pFeatureMatch, outLen);

}
void __fastcall TfrmSpecFeatDisplay::btnMatchSpEnvClick(TObject *Sender)
{
		MatchSpectralEnvelopes();
		if(Sender) return;
		const unsigned fftLen = pVsd->at(0)->GetSpectralData()->GetFrameLength();
		const unsigned halfLen = fftLen>>1;
		const unsigned numFrames = pVsd->at(0)->GetSpectralData()->GetOverlappingFramesNumber();
		const unsigned numTracks = pVsd->size();
	   //	if(numTracks<2)
		int i;

		std::vector<float> vMagRatio;

		std::vector<float> &vAm0 = *pVsd->at(0)->GetSpectralData()->GetAverageMagnitudes();
		std::vector<float> &vAm1 = *pVsd->at(1)->GetSpectralData()->GetAverageMagnitudes();

		std::vector<float*> &vFft = *pVsd->at(0)->GetSpectralData()->GetFFTFrames();
		float *pWndFunc =  pVsd->at(0)->GetSpectralData()->GetWndData();

		vMagRatio.push_back(1.0f);
		int cnt=0;

		for(i=1;i<halfLen;i++){
		   //	float ratio =  vAm0[i]/vAm1[i];
			float diff =  vAm0[i]- vAm1[i];  //+ means that vAm0[i]> vAm1[i]
			/*if(ratio < 1) {
				ratio =  static_cast<float>(1)/ratio;
				cnt++;
			} */



			float ratio = pow(10, diff/20 );
			if(diff < 0) ratio = 1.0f /ratio;

		   //	ratio = ratio < 1 ? ratio-0.2f : ratio + 0.2f;

			vMagRatio.push_back(ratio);
		}
		vMagRatio.push_back(1.0f);
		FFT fft;
		float *pMod0, *pMod1;
		const unsigned cpySize =  sizeof(float)*fftLen;
		for(int i=0;i<numFrames;i++){

			float *pFft = vFft[i];
			pMod0 = new float[fftLen];
			memcpy(pMod0,pFft, cpySize);
			pMod0[halfLen] *= vMagRatio[halfLen];
			for(int j=halfLen-1,k=1;j>0;j--,k++){
				unsigned ii=halfLen+k;
				pMod0[j] *= vMagRatio[j];
				pMod0[ii] = -pMod0[ii]*vMagRatio[j];//(-pMod0[j]);
			}
			fft.ComputeRealFFT(pMod0, fftLen, true);

			for(int j=0;j<fftLen;j++){

				pMod0[j] *= pWndFunc[j];
			}
			vMod.push_back(pMod0);
		}

		const unsigned outLen = (vMod.size()>>1)*fftLen-halfLen;

		pFeatureMatch = new float[outLen];
		float *pWndSqSum = new float[outLen];

		int k;
		const float c = 1.0f/halfLen;
		float wSquared = 0;

		for(int i=0; i<numFrames-1; i++){

		pMod0 =  vMod[i];
		pMod1 =  vMod[i+1];
		k = (i)*halfLen;

		for(int j=0;j<halfLen;j++){
			float val = (pMod1[j]+pMod0[halfLen+j]);
			wSquared = pWndFunc[j]+pWndFunc[halfLen+j];
			wSquared *=  wSquared;
			pWndSqSum[k+j]=  wSquared;

			pFeatureMatch[k+j] = val*c;
		}
	}
	for(int i=0; i<outLen-fftLen; i++){
	   pFeatureMatch[i] /= pWndSqSum[i];
	}

   //	(static_cast<TForm1*>(pParent))->DrawMatchedWave(pFeatureMatch, outLen);
}
//---------------------------------------------------------------------------

