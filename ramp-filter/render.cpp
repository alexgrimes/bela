/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\

http://bela.io

C++ Real-Time Audio Programming with Bela - Lecture 12: Envelopes
ramp-filter: linear envelope to control the frequency of a resonant filter
*/

#include <Bela.h>
#include <cmath>
#include "Wavetable.h"
#include "Filter.h"

// Pins for analog I/O 
const unsigned int kInputDuration = 0;
const unsigned int kInputMaxFrequency = 1;
const unsigned int kInputOscillatorFrequency = 2;

// Variables for linear envelope
float gRampDuration = 2.0;
float gFilterFrequencyMin = 200.0;
float gFilterFrequencyMax = 4000.0;
float gFilterFrequency = gFilterFrequencyMin;  
float gFilterFrequencyIncrement = 0;

// Oscillator frequency
const float gOscillatorFrequency = 110.0;

// Oscillator and filter objects
Wavetable gOscillator;
Filter gFilter;

// setup() only runs one time
bool setup(BelaContext *context, void *userData)
{
	std::vector<float> wavetable;
	const unsigned int wavetableSize = 512;
	
	//Double check that we have the expected analog sample rate 
	if(context->analogInChannels < 3 || context->analogSampleRate != context->audioSampleRate / 2) 
	{
		rt_fprintf(stderr, "This example needs at least 3 analog inputs at half the audio rate.\n");
		return false;
	}
		
	// Populate a buffer with the first 32 harmonics of a sawtooth wave
	wavetable.resize(wavetableSize);
	for(unsigned int n = 0; n < wavetable.size(); n++) {
		wavetable[n] = 0;
		for(unsigned int harmonic = 1; harmonic <= 32; harmonic++) {
			wavetable[n] += sinf(2.0 * M_PI * (float)harmonic * (float)n / 
								 (float)wavetable.size()) / (float)harmonic;
		}
	}
	
	// Initialise the wavetable, passing the sample rate and the buffer
	gOscillator.setup(context->audioSampleRate, wavetable);
	gOscillator.setFrequency(gOscillatorFrequency);
	
	// Initialise the filter
	gFilter.setSampleRate(context->audioSampleRate);
	gFilter.setFrequency(gFilterFrequencyMin);
	gFilter.setQ(4.0);
	
	// TODO: calculate ramp increment
	float rampDuration = gRampDuration * context->audioSampleRate;
	gFilterFrequencyIncrement = (gFilterFrequencyMax - gFilterFrequencyMin) / rampDuration;
	
    return true;
}

// render() is called every time there is a new block to calculate
void render(BelaContext *context, void *userData)
{
   	// This for() loop goes through all the samples in the block
	for (unsigned int n = 0; n < context->audioFrames; n++) {
		
		//read the analog inputs 
		float durationInput = analogRead(context, n/2, kInputDuration);
		float maxFrequencyInput = analogRead(context, n/2, kInputMaxFrequency);
		float oscillatorInput = analogRead(context, n/2, kInputOscillatorFrequency);
		
		// recalculate the interval based on the input controls
		gFilterFrequencyMax = map(maxFrequencyInput, 0, 3.3/4.096, 400.0, 8000.0);
		gRampDuration = map(durationInput, 0, 3.3/4.096, 0.05, 5.0);
		gFilterFrequencyIncrement = (gFilterFrequencyMax - gFilterFrequencyMin) / (gRampDuration * context->audioSampleRate);
		
		// recalculate and set the osc freq 
		float oscillatorFrequency = map(oscillatorInput, 0, 3.3/4.096, 40.0, 500.0);
		gOscillator.setFrequency(oscillatorFrequency);
		
		// increment the frequency to create a linear ramp
		gFilterFrequency += gFilterFrequencyIncrement;
		if(gFilterFrequency >= gFilterFrequencyMax)
			gFilterFrequency = gFilterFrequencyMin;
		
		// Update the filter frequency
		gFilter.setFrequency(gFilterFrequency);
		
		// Generate and filter the signal
		float in = gOscillator.process();
		float out = 0.2 * gFilter.process(in);

		// This part is done for you: store the sample in the
		// audio output buffer
		for(unsigned int channel = 0; channel <context->audioOutChannels; channel++) {
		    audioWrite(context, n, channel, out);
		}
    }
}

// cleanup() runs once when the program stops
void cleanup(BelaContext *context, void *userData)
{
	// nothing to do here
}
