/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\

http://bela.io

C++ Real-Time Audio Programming with Bela - Lecture 17: Block-based processing
fft-circular-buffer: show spectrum of signal from overlapping FFT windows
*/

#include <Bela.h>
#include <libraries/Fft/Fft.h>
#include <libraries/Gui/Gui.h>
#include <cmath>
#include <cstring>
#include <vector>
#include <algorithm>
#include "MonoFilePlayer.h"

// FFT-related variables
Fft gFft;					// FFT processing object
const int gFftSize = 1024;	// FFT window size in samples
const int gHopSize = 512;	// How often we calculate a window

// Circular buffer and pointer for assembling a window of samples
const int gBufferSize = 16384;
std::vector<float> gInputBuffer;
int gInputBufferPointer = 0;
int gHopCounter = 0;

// Name of the sound file (in project folder)
std::string gFilename = "drumloop.wav"; 

// Object that handles playing sound from a buffer
MonoFilePlayer gPlayer;

// GUI to display the spectrum
Gui gSpectrumGui;

bool setup(BelaContext *context, void *userData)
{
	// Load the audio file
	if(!gPlayer.setup(gFilename)) {
    	rt_printf("Error loading audio file '%s'\n", gFilename.c_str());
    	return false;
	}

	// Print some useful info
    rt_printf("Loaded the audio file '%s' with %d frames (%.1f seconds)\n", 
    			gFilename.c_str(), gPlayer.size(),
    			gPlayer.size() / context->audioSampleRate);
	
	// Set up the FFT and its input buffer
	gFft.setup(gFftSize);
	gInputBuffer.resize(gBufferSize);
	
	// GUI to emulate the LEDs
	gSpectrumGui.setup(context->projectName);

	return true;
}

// This function handles the FFT processing in this example once the buffer has
// been assembled.

void process_fft(std::vector<float> const& buffer, unsigned int starting_loc)
{
	static std::vector<float> fftCurrentOut(gFftSize / 2);	// Current FFT energy
	static std::vector<float> unwrappedBuffer(gFftSize);	// Container to hold the unwrapped values
	const float decayRate = 0.98;							// Decay rate of peak value 
	static std::vector<float> fftPeakOut(gFftSize / 2);
	// copy/unwrap buffer into FFT input (buffer --> unwrappedBuffer, with the right indexing)
	
	
	
		
	// Copy the lower half of the FFT bins to a buffer,
	// and also calculate a recent peak value for each bin
	for(int n = 0; n < gFftSize; n++) {
		// fftCurrentOut[n] = gFft.fda(n);
		
		int circularBufferIndex = (starting_loc + n- gFftSize + gBufferSize) % gBufferSize;
		unwrappedBuffer[n] = buffer[circularBufferIndex];
		
		// TODO: optionally, calculate the recent peak of each frequency bin,
		//       using smoothing by averaging over adjacent frequency bins
	}
	
	// Process the FFT based on the time domain input
	gFft.fft(unwrappedBuffer);
	
	//cpy the lower half of the fft bins to a buffer 
	// Calculate a recent peak value for each bin 
	for(int n = 0; n < gFftSize / 2; n++) {
		fftCurrentOut[n] = gFft.fda(n);
		
		// Calculate a local average of 11 points 
		float localAvg = 0;
		int dataPoints = 0;
		for(int k = std::max(0, n - 5); k <= std::min(n + 5, gFftSize / 2); k++) {
			localAvg += fftCurrentOut[k];
			dataPoints++;
		}
		
		localAvg /= (float)dataPoints;
		
		fftPeakOut[n] *= decayRate;
		if(localAvg >= fftPeakOut[n])
			fftPeakOut[n] = localAvg;
	}
	
	// Send the current values to the GUI
	gSpectrumGui.sendBuffer(0, fftCurrentOut);
	// send the peak values to the GUI
	gSpectrumGui.sendBuffer(1, fftPeakOut); 
	
}

void render(BelaContext *context, void *userData)
{
	for(unsigned int n = 0; n < context->audioFrames; n++) {
        // Read the next sample from the buffer
        float in = gPlayer.process();

		// TODO: store the sample ("in") in a circular buffer for the FFT
		// Be sure to wrap the pointer around when it reaches the end of the
		// buffer, but this is not the time to call process_fft()
		gInputBuffer[gInputBufferPointer++] = in;
		if(gInputBufferPointer >= gBufferSize) {
			gInputBufferPointer = 0;
		}
		
		// TODO: increment the hop counter and start a new FFT if we've reached the hop size
		// To calculate the FFT call process_fft(gInputBuffer, gInputBufferPointer);
	
		if(++gHopCounter >= gHopSize) {
			gHopCounter = 0;
			process_fft(gInputBuffer, gInputBufferPointer);
		}
		
		// Write the audio to the output
		for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
			audioWrite(context, n, channel, in);
		}
	}
}

void cleanup(BelaContext *context, void *userData)
{

}
