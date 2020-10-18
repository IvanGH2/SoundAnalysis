#ifndef FileLayoutH
#define FileLeyoutH

struct FileLayout {
				enum eSource {  MAGNITUDES = 0, CENTROID, ROLLOFF, FLUX };
				enum eChannels { BOTH = 0 , LEFT , RIGHT };
				enum eFrameValues { FRAME_PER_ROW = 0, FRAME_PER_COLUMN };
				enum eChannelSeparation { SEPARATED = 0, INTERLEAVED }; //for stereo only

				static bool enumFrames;
				static bool enumBins;
				static bool stftInfo;

				static eChannels eCh ;
				static eFrameValues eFv ;
				static eChannelSeparation eCs;
				static eSource eSrc ;

				private:

				FileLayout(){}
};

FileLayout::eSource FileLayout::eSrc 			= MAGNITUDES;
FileLayout::eChannelSeparation FileLayout::eCs 	= SEPARATED;
FileLayout::eChannels FileLayout::eCh 			= LEFT;
FileLayout::eFrameValues FileLayout::eFv 		= FRAME_PER_ROW;
bool FileLayout::enumFrames 					= false;
bool FileLayout::enumBins   					= false;
bool FileLayout::stftInfo   					= false;
#endif
