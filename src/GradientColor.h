//---------------------------------------------------------------------------

#ifndef GradientColorH
#define GradientColorH
//---------------------------------------------------------------------------

/**
this class calculates gradient colours, which are used for the spectrogram display of a particular waveform.
*/
const unsigned colors[][21] = {
	{ 0xfffff8, 0xfeffd1, 0xfdffbd, 0xfcff9f, 0xfaf37b, 0xf9e064, 0xf8c948, 0xf5b640, 0xf2a037, 0xef8d2e, 0xdd6f24,
  0xd76923, 0xc25a20, 0x924b22, 0x7a4628, 0x62412e, 0x4b3c34, 0x3e3937, 0x2f323e, 0x232c45, 0x122141 },

  { 0xfffff8, 0xeeeeff, 0xddddff, 0xccccff, 0xbbbbff, 0xaaaaff, 0x9999ff, 0x8888ff, 0x7777ff, 0x6666ff,
	0x5555ff, 0x4444ff, 0x3333ff, 0x2222ff, 0x0000ff, 0x0000ee, 0x0000dd, 0x0000cc, 0x0000bb, 0x0000aa, 0x000055 },


  { 0xfcfbdd, 0xfcfbbd, 0xfcfabb, 0xfdda9c, 0xfec488, 0xfea772, 0xfc8961, 0xf4675c, 0xe55064, 0xcd4070, 0xb5367a,
	0x9b2e7f, 0x832681, 0x6a1c21, 0x6a1c80, 0x51127c, 0x36106b, 0x1d1147, 0x140e36, 0x0e0b2b,  0x010108 },


  { 0xfffff8, 0xfffed1, 0xfdffbb, 0xfcff9f, 0xfaf47c, 0xf96ed2, 0xf7c446, 0xf4ac3c, 0xf19833, 0xee8129, 0xdd6f24,
	0xc95e20, 0xb05420, 0x924b22, 0x7a4628, 0x62412e, 0x4b3c34, 0x3e3937, 0x2f323e, 0x232c45, 0x122141 }
};

enum EGradientColorMap { ORANGE_GLOW = 0, BLUE_SKY, PINK_ORANGE, EMERALD_GREEN, WHITE_ON_BLACK, BLACK_ON_WHITE };

class GradientColor{

	EGradientColorMap eGradMap;

	const unsigned cGreenShift;

	const unsigned cBlueShift;

	unsigned bkgMaxValue;

	unsigned maxValue;

	unsigned numColors;

	float gradientStep;

	unsigned gradientColor, bkgGradientColor;

	unsigned gradColorLen;

	unsigned colLen, rowLen, numColumn;

	unsigned skipBinColumn;//number that tells us which bin columns to skip (e.g. 4 means that we'll skip every 4th column

	unsigned int **pGradColors;

	unsigned char colorRed, colorBlue, colorGreen;

	void DecomposeGradientColor(unsigned color);

	void Init();

	void FreeResources();

public:
	GradientColor(unsigned );

	GradientColor();

	~GradientColor();

	unsigned **GetSpectrogramColours(unsigned **pMag, unsigned rowNum,  unsigned colNum,

 								unsigned numChannels, unsigned maxVal, unsigned minVal);

	void SetGradientContrast(unsigned gStep);

	unsigned GetGradientContrast();

	void SetGradientColor(unsigned color);

	void SetGradientColor(EGradientColorMap gColor){
		eGradMap = gColor;
	}
	void SetGradientColorMap( EGradientColorMap eColorMap) { eGradMap = eColorMap; }

	void SetGradientBkgColor();

	unsigned GetGradientBkgColor() const  { return bkgGradientColor;}

	unsigned GetGradientColor() const { return gradientColor; }

	unsigned GetSkipBinColumn() const { return skipBinColumn; }

	unsigned GetColumnNum() const { return numColumn; }
};

#endif
