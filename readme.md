
Sound analysis ( SA )


Sound analysis is a collection of modules for audio analysis that I've written in my free time. It is written mostly as a library but contains elements of a framework (e.g callbacks).
An application demonstrating the SA capabilities is provided as well. Since documentation is currently lacking, having an application should help developers to get started with using the library.
I intend to write documentation covering the most important API-s by the end of the year because I don't think that any software can gain traction and reach wider audience without it.



1. Technology

The library is written in c++03 using the Borland bcc32 compiler (c++ builder). The current version compiles and runs only on Windows. 
This compiler is old and it affected the way I wrote some module, primarily I wasn't able to make the FFT stuff use SIMD instructions (data parallelism), 
which could've boosted  performance by several times.
While the GUI and multithreading support depend on the VCL (Visual Component Library) library, which is not portable, all the other stuff use standard 
c++ so it should be possible to port this library to other platforms (e.g Linux) relatively easily, provided of course that you rewrite 
the GUI/multithreading stuff. There's a Linux equivalent of VCL, so that could help too if one descides to go in that direction.

2. Purpose of the library

While there are some open-source sound analysis libraries available (e.g Sonic visualizer and even Audacity has some analysis features), I think they're pretty basic and 
I wanted to see some additional features anyway. Plus, I wanted to teach myself some dsp programming as well. At the moment SA really doesn't do 
much more than the two applications I've just mentioned (and it'is more buggy) but I plan to add more features in the future and hopefully other
developers interested in this kind of thing will start contributing.
Currently, as far as sound analysis is concerned, SA supports only spectrograms but it can also compute several spectral domain features including 
spectral centroid,  spectral rolloff, flux etc. What I'd really like to see implemented in SA are advanced statistical analysis methods. 
In fact, I've already done some work on GMMs and NMFs but left them out of the current SA version because I'm still thinking of how best to incorporate and integrate these features.

3.	Main features
 
	- multitrack waveform, spectrogram and mixed view display. 
	
	- tabular display of the spectral features including the bin magnitudes, spectral centroid, roll-off, flux etc.
	
	- ability to hide tracks that are currently not being analyzed/viewed, which helps to declutter the GUI.

	- advanced filtering. It's possible to create multiple filtered tracks at once (using a bandpass filter), which speeds up the filtering dramatically. 
	
	- tone generation. Unlike most tone generators, the SA tone generator can create a fundamental tone plus as many harmonics as you want. You can also manually change the amplitudes and frequencies of the harmonics
	
	- dumping the analysis results to a file for later analysis using using other software.
	
4. SA Application (Borland/embarcadero developer project is provided and not a precompiled binary )

The SA application is written primarily for developers and is not meant to be used for end users at this point (the provided binary is not a standalone application). 
By examining the application code, developers should get acquainted  with the library and how it works internally. 

![SA application demo](<https://drive.google.com/file/d/17eeEIIMkIi6HizZqMHVq2YIw5_1B8UQ7/view?usp=sharing>)
	
4. To do

	- developers' documentation is lacking so the focus now will be on describing the most useful (or necessary) API-s in order for developers to be able to play with the existing code, add new features, fix bugs etc. This
	is arguably the most important thing, otherwise I can't really expect that anyone will want to use the library.

	-  implementing the fft stuff using simd (sse2) , which alone can improve performance considerably (several times). I've already done this but unfortunately bcc32 doesn't support sse/avx so I put it on hold 
	for now. I plan to either write a separate sse library or recompile the exisiting code in a newer c++ builder version (c++ 10.4 Sydney which can make use of the Clang compiler). 

	
	- integrating some advanced statistics into the analysis phase. I've already done some work on GMM and NMF and am now looking into ways how to best integrate that stuff with the existing code. 
		
5. Bugs
	-  yes, the SA software is quite buggy at this point
	
	- I'll try to fix bugs as soon as they're reported especially serious bugs like access violations and illegal floating point operations. Still, given 
	that the library is quite complex/complicated it may take (quite) a while before they're fixed.

If you have any questions regarding this software you can contact me at ivanbalen2@gmail.com.


	
	
	
	

	
	
