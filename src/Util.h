/*	Copyright 2020 Ivan Balen
	This file is part of the Sound analysis library.
	The Sound analysis library is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	The Sound analysis library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the Lesser GNU General Public License
	along with the Sound analysis library.  If not, see <http://www.gnu.org/licenses/>.*/
#ifndef UtilH
#define UtilH

#include <string>

enum EContainerView { WAVEFORM, SPECTROGRAM, MIXED };                                

enum EWaveformDisplayMode { SAMPLE_AVERAGING, PIXEL_AVERAGING };

enum ESoundSource { TONE_GENERATOR=0, DIGITAL_FILTER, SPECTRUM_FILTER, SOUND_FILE};

enum EChannel { LEFT, RIGHT };
/**input struct for WaveDisplayData (i.e. input for waveform display data)
we need the following as input
- numChannels
- inputData (at the moment, this must be a pointer to an array pf floats
- number of float elements  (= number of samples =  number of float elements / numChannels )
**/

namespace SoundAnalysis {

	/*struct ActionProgres{
		unsigned currPercentage;
		unsigned totalPercentage;
	} */
	typedef void (*PfActionProgress)(unsigned*);
}
struct WAVE_DISPLAY_DATA{
 float * pData;
 unsigned dataLen;
 unsigned int numChannels;
 //unsigned int numBuffers;
 WAVE_DISPLAY_DATA(){}
 WAVE_DISPLAY_DATA(unsigned int numChannels, float* pData, unsigned int dataLen/*, unsigned int numBuffers*/)
 : numChannels(numChannels) , pData(pData), dataLen(dataLen){}
};
typedef  WAVE_DISPLAY_DATA *PWAVE_DISPLAY_DATA;

typedef struct{
	unsigned startSample;
	unsigned endSample;

} SAMPLE_RANGE, *PSAMPLE_RANGE;

struct WAVE_PLAYBACK_DATA{

 unsigned int samplesPerSec;  //sample rate   prolly not needed anymore given multiple player instances
 unsigned int bitsPerSample;  //bit depth
 SAMPLE_RANGE sampleRange; //used for selections
 PWAVE_DISPLAY_DATA pWaveData;
 WAVE_PLAYBACK_DATA(){}
 WAVE_PLAYBACK_DATA(PWAVE_DISPLAY_DATA pWaveDisplayData, unsigned int sampleRate)
 : pWaveData(pWaveDisplayData/*waveDisplayData.numChannels, waveDisplayData.pData, waveDisplayData.dataLen*/) , samplesPerSec(sampleRate){}
};

typedef  WAVE_PLAYBACK_DATA *PWAVE_PLAYBACK_DATA;

namespace WindowFunc {

	enum EWindow {  HAMMING = 0, BLACKMAN, HANN, RECTANGULAR };
}
typedef struct{
	unsigned int encoding;
	unsigned int numChannels;
	unsigned int sampleRate;
}RAW_DATA_INFO, *PRAW_DATA_INFO;

//enum EWaveMsgType { REMOVE, ADD, SELECT,  RESIZE, REPOSITION };

namespace nsPlayback {
//OPENED and CLOSED are used for enabling/disabling the Wave player controls (Play/Pause/stop) as a whole
//the other events are triggered by the WavePlayer
enum  EPlaybackEvent { PAUSED = 0, STARTED, STOPPED, POSITIONED, OPENED, CLOSED, CLOSED_ALL };

enum EPlaybackIndicatorMove { FORWARD, BACKWARD };

typedef struct{
	bool mute;
	bool solo;
}PLAYBACK_SELECTION, *PPLAYBACK_SELECTION;

typedef struct{
	unsigned waveId;   //samo jedan ???
	PWAVE_PLAYBACK_DATA pPlaybackData; //sound data info
}TRACK_SELECTION, *PTRACK_SELECTION;

//typedef void (*PfPlaybackMsgPos)( EPlaybackEvent, unsigned int, unsigned int);
typedef void (*PfPlaybackMsgPos)( EPlaybackEvent, unsigned , unsigned );
//this message is sent either by the wave player or the wave manager and used when updating the GUI (e.g when the playback has finished)
typedef void (__closure *PfPlaybackMsg)( EPlaybackEvent, unsigned);

//sent from SoundIO
//typedef void (__closure *PfFileOpenMsg)( unsigned );   //used when opening a long file
struct FILE_INFO{

	char 	*pFileName;
	unsigned numChannels;
	unsigned samplesPerSec;
	unsigned bitsPerSample;
	unsigned fileId;
	FILE_INFO(){}
	FILE_INFO(char *pFN, unsigned numChannels, unsigned samplesPerSec, unsigned bitsPerSample, unsigned fId)
	: pFileName(pFN), numChannels(numChannels),
	samplesPerSec(samplesPerSec), bitsPerSample(bitsPerSample), fileId(fId)
	{}
};
typedef FILE_INFO* PFILE_INFO;

struct TRACK_INFO{

	PFILE_INFO pFileInfo;
	bool selForPlayback;
	bool hide;
	TRACK_INFO(){}
	TRACK_INFO(PFILE_INFO pFi) : pFileInfo(pFi){}
};

typedef TRACK_INFO * PTRACK_INFO;
}
//class WaveDisplayContainerManager;

namespace nsWaveDisplay {

	const unsigned cWaveformMargin 			= 16;   //margin between the waveform and the lower edge/end of its container
	const unsigned cWaveformSpacing 		= 5;   //spacing between the waveform channels
	const unsigned cWaveformRightMargin 	= 35;
	const unsigned cWaveformVertMargin		= 25;  //used for when alligning vertically
	const unsigned cWaveformHeight 			= 220; //default waveform container height
	const unsigned cWaveformMinHeight 	  	= 40;  //minimum waveform container height
	const unsigned cNonClientCtrlHeight   	= 55;  //status bar + tool bar height
	const unsigned cToolbarHeight 		  	= 35;
	const unsigned cAmpWidth 			  	= 30;  //wave amplitude panel
	const unsigned cWaveCtrlWidth 		  	= 0; //wave control panel -- the panel holding the controls like Solo/Mute buttons etc
	const unsigned cSpectrogramIncrement  	= 40;  //this is used when drawing spectrogram bitmaps of different heights
	const unsigned cScrollBarWidth			= 17;

	enum { LEFT_CHANNEL = 1, RIGHT_CHANNEL = 2, LEFT_AMP = 3, RIGHT_AMP = 4 };
	//messages that are sent from a particular waveform container to the manager
	enum EWaveMsgType { REMOVE, ADD, SELECT, SELECT_WAVEFORM, RESIZE, REPOSITION, INIT_SELECTION,
	REMOVE_SELECTION, POSITION_INDICATOR };

	enum ESpectrogramMsgType { FRAME_SELECT, FRAME_BIN_SELECT };

	enum EScale { LINEAR = 0, LOG };

	enum EMainWndNotification { INV_TRACK_TIME, INV_TRACK_POS, CONTAINER_ADDED, CONTAINER_REMOVED,CONTAINER_RESIZED, SHOW_ACTION_PROGRESS };

	enum ESpectrogramSelectionMode { FULL_BANDWIDTH, PARTIAL_BANDWIDTH };

	struct TRACK_POSITION{
		unsigned startSample;
		unsigned endSample;
		unsigned currSample;
		TRACK_POSITION(unsigned sSample, unsigned eSample, unsigned cSample) :
		startSample(sSample), endSample(eSample), currSample(cSample) {}
		TRACK_POSITION(){}
	};

	struct TRACK_TIME{
		unsigned startTime;
		unsigned selectionTime;
		unsigned trackTime;
		TRACK_TIME(){}
	};

	struct FILTER_PROCESSING_PROGRESS{
		unsigned progressPercentage;
		unsigned progressTotalPercentage;
		std::string currProcessFile;
		FILTER_PROCESSING_PROGRESS(){}
		FILTER_PROCESSING_PROGRESS(unsigned progress, unsigned totalProgress, std::string fName) :
		progressPercentage(progress), progressTotalPercentage(totalProgress), currProcessFile(fName){}
	};

	typedef	TRACK_POSITION *PTRACK_POSITION;

	typedef	TRACK_TIME *PTRACK_TIME;

	typedef FILTER_PROCESSING_PROGRESS *PFILTER_PROCESSING_PROGRESS;

	//messages that are sent from the wave container manager to the main app window
	typedef void (__closure *PfWaveformInfo)(unsigned, unsigned);

	typedef void (__closure *PfMainWndNotify)(EMainWndNotification, const void* );


	typedef void (__closure *PfSpectrogramSelMsg)() ;

	typedef void (__closure *PfSpectrogramRemovedMsg)(bool) ;
}

namespace nsDigitalFilter{

	enum EFilterType { LOPASS, HIPASS, BANDPASS };

	typedef struct{
		EFilterType eFilterType;
		float cutoffLo;
		float cutoffHi;
		WindowFunc::EWindow eWnd;
		unsigned bandsNum;
	}FILTER_INFO, *PFILTER_INFO;

	typedef struct{
		float cutoffLo;
		float cutoffHi;
	}FILTERBAND_INFO, *PFILTERBAND_INFO;

}
namespace nsWaveTimer{

	enum EWaveTimeInterval { MILISECONDS, SECONDS, MINUTES, HOURS };
}
namespace nsToneGenerator{

	enum EToneType { SINE = 0, SQUARE, TRIANGLE, SAWTOOTH, COMPOSITE };

	typedef struct{
		nsToneGenerator::EToneType eToneType;
		float freq;
		float amp;
		float phase;
		float duration;
		float sampleRate;
		unsigned harmNum; //number of harmonics that this tone contains including the fundamental
}TONE_INFO, *PTONE_INFO;

//certain properties (e.g.frequency=  correspond to the fundamental tone properties
typedef struct{
	float freq;
	float amp;
	float phase;
}HARMONICS_INFO, *PHARMONICS_INFO;

}

namespace nsAnalysis{

	enum EAnalysisSource  { MAGNITUDE=0, CENTROID, ROLLOFF, FLUX, ZEROCROSSINGRATE, AVG_MAGNITUDE };

	enum EChannelSeparation { INTERLEAVED, SEPARATED };

	enum EChannelInclusion { BOTH, LEFT, RIGHT };

	enum EValueSeparation { FRAME_VALUE, VALUE_PER_ROW };
}

//defines
#define _USE_INTERNAL_FORMAT_   //this means that sampled data will be internally converted to float (unless they're already floating point)
//#define _DEBUG_
#define _FAST_IO

#define _NOTIFY_MAIN_WND
//enum EncodingType { BIT_8U=1, BIT_16=2, BIT_24=3, BIT_32F=4 };
#endif
