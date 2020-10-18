
#ifndef GenericWavePlayerH
#define GenericWavePlayerH

#include <exception>


using namespace std;


class WaveException : public exception {
	std::string exMsg ;
public:
  WaveException(const std::string& s) { exMsg = s;  }

  virtual const char* what() const    // throw()
  {
	return exMsg.c_str();
  }
};

class GenericWavePlayer {
 
	protected:
				virtual int StartPlayback() = 0;

				virtual int StopPlayback() 	= 0;

				virtual int PausePlayback() = 0;

				virtual int ResumePlayback() = 0;
				//allocate playback buffers -- should be called before StartPlayback
				virtual int PrepareForPlayback(bool) = 0;

				virtual int OpenForPlayback() = 0 ;
				/*
				An implentation of this class will typically have additional methods such as
				virtual void Position() //positions the playback at a particular time/sample
				virtual void ProcessMsg()  //callback for processing messages from the audio device
				virtual void PlaySelection(); //when only a part of the waveform is selected
				**/
				bool isPlaying;

				bool isPaused;

				bool isCompleted;

				void RaiseException(const std::string& ex){
					throw  WaveException(ex);
				}

};
#endif
