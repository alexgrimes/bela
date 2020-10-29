/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\

http://bela.io

C++ Real-Time Audio Programming with Bela - Lecture 3: Wavetables
wavetable: a partially complete example implementing a wavetable
           oscillator.
*/

#include <Bela.h>
#include <libraries/Scope/Scope.h>
#include <cmath>

const int gWavetableLength = 512;		// The length of the buffer in frames
float gWavetable[gWavetableLength];		// Buffer that holds the wavetable
float gReadPointer = 0;					// Position of the last frame we played 

float gAmplitude = 0.2;					// Amplitude of the playback
float gFrequency = 220.0;				// Frequency (TODO: not implemented yet)

Scope gScope;

bool setup(BelaContext *context, void *userData)
{
	// Generate a triangle waveform (ramp from -1 to 1, then 1 to -1)
	// and store it in the buffer. Notice: generating the wavetable does 
	// not depend on the audio sample rate (why not?)
	
	//sine implementation
	// for(unsigned int n = 0; n < gWavetableLength/2; n++) {
	// 	gWavetable[n] = sin(2.0 * M_PI * (float)n / (float)gWavetableLength);
	// }
	
	//triangle implementation
	// for(unsigned int n = gWavetableLength/2; n < gWavetableLength; n++) {
	// 	gWavetable[n] = 1.0 - 4.0 * (float)(n - gWavetableLength/2) / (float)gWavetableLength;
	// }
	
	// for(unsigned int n = gWavetableLength/2; n < gWavetableLength; n++) {
	// 	gWavetable[n] = 1.0 - 4.0 * (float)(n - gWavetableLength/2) / (float)gWavetableLength;
	// }
	
	//sawtooth implementation
	for(unsigned int n = 0; n < gWavetableLength; n++) {
		gWavetable[n] = 0.5 * sin(2.0 * M_PI * (float)n / (float)gWavetableLength) +
						0.25 * sin(2.0 * 2.0 * M_PI * (float)n / (float)gWavetableLength) +
						0.125 * sin(3.0 * 2.0 * M_PI * (float)n / (float)gWavetableLength) +
						0.625 * sin(4.0 * 2.0 * M_PI * (float)n / (float)gWavetableLength);
	}
	
	

	// Initialise the Bela oscilloscope
	gScope.setup(1, context->audioSampleRate);

	return true;
}

void render(BelaContext *context, void *userData)
{
    for(unsigned int n = 0; n < context->audioFrames; n++) {
    	//gReadPointer has an integer (indexBelow) and fractional component (n + f)
    	//ensure index above hasn't fallen off the end of the wave table and if it has set it to 
    	// (n)
    	int indexBelow = floorf(gReadPointer);
    	int indexAbove = indexBelow + 1;
    	if(indexAbove >= gWavetableLength)
    	indexAbove = 0;
    	
    	
    	//extract just the value of f - the fractional apart from the integer component
    	// (f)
    	float fractionAbove = gReadPointer - indexBelow;
    	float fractionBelow = 1.0 - fractionAbove;
    	
    	
    	// Load a sample from the buffer
    	// our formula x(n + f) = (1 - f)x[n] + fx[n+1]
        float out = gAmplitude * (fractionBelow * gWavetable[indexBelow] + fractionAbove * gWavetable[indexAbove]);

        // Increment read pointer and reset to 0 when end of table is reached
        // TODO: update this code to handle different frequencies!
        gReadPointer += gWavetableLength * gFrequency / context->audioSampleRate;
        while(gReadPointer >= gWavetableLength)
        	gReadPointer -= gWavetableLength;
            
    	for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
			// Write the sample to every audio output channel
    		audioWrite(context, n, channel, out);
    	}
    	
    	// Write the sample to the oscilloscope
    	gScope.log(out);
    }
}

void cleanup(BelaContext *context, void *userData)
{

}
