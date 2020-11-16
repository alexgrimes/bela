/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\

http://bela.io

C++ Real-Time Audio Programming with Bela - Lecture 13: State Machines
metronome-envelope: metronome code using an exponential envelope rather than a sample
*/

#include <Bela.h>
#include <math.h>

// Oscillator variables
float gPhase = 0;			// Current phase
float gFrequency = 1000;	// Frequency in Hz

// Envelope variables
float gAmplitude = 0.0;   
float gEnvelopeScaler = 0.997;

// Metronome state machine variables
// TODO: declare variables for which beat (state) we're 
const int kMetronomeStateOff = -1;
const int kMetronomeBeatsPerBar = 4;
int gMetronomeBeat = kMetronomeStateOff; //state var... tells us which beat we're in
int gMetronomeCounter = 0;
int gMetronomeInterval = 0;

// button vars 
const int kButtonPin = 0;
int gPreviousButtonValue = 1;
// const int kLEDPin = 0;
// int gLEDInterval = 0;

// setup() only runs one time
bool setup(BelaContext *context, void *userData)
{
	// Calculate the metronome interval based on 120 bpm
	float bpm = 120.0;
	gMetronomeInterval = 60.0 * context->audioSampleRate / bpm;
	
	//LED blink length fixed at 50ms
	// gLEDInterval = 0.05 * context->audioSampleRate;
	
	// Initialise GPIO pins 
	pinMode(context, 0, kButtonPin, INPUT);
	// pinMode(context, 0, kLEDPin, OUTPUT);
	
    return true;
}

// render() is called every time there is a new block to calculate
void render(BelaContext *context, void *userData)
{
   	// This for() loop goes through all the samples in the block
	for (unsigned int n = 0; n < context->audioFrames; n++) {
		// read the button 
		int value = digitalRead(context, n, kButtonPin);
		
		if(value == 0 && gPreviousButtonValue != 0) {
			//button clicked: is the metronome off?
			if(gMetronomeBeat == kMetronomeStateOff) {
				gMetronomeBeat = 0;
				gMetronomeCounter = 0;
				gAmplitude = 1.0;
				gFrequency = 2000;
			}
			else {
				// Turn metro off 
				gMetronomeBeat = kMetronomeStateOff;
			}
		}
		gPreviousButtonValue = value;
		
		// if the metro is not off, advance the counter and beat 
		if(gMetronomeBeat != kMetronomeStateOff) {
			if(++gMetronomeCounter >= gMetronomeInterval) {
				//metro tick elapsed; reset counter and envelope 
				gMetronomeCounter = 0;
				gAmplitude = 1.0;
				
				gMetronomeBeat++;
				if(gMetronomeBeat >= kMetronomeBeatsPerBar) {
					gMetronomeBeat = 0;
					gFrequency = 2000;
				}
				else {
					gFrequency = 1000;
				}
			}
		gAmplitude *= gEnvelopeScaler;
		}
		
	    // Calculate a sample of the sine wave, and scale by the envelope
		gPhase += 2.0 * M_PI * gFrequency / context->audioSampleRate;
		if(gPhase >= 2.0 * M_PI)
			gPhase -= 2.0 * M_PI;
		float out = gAmplitude * sin(gPhase);

		// Write the sample to the audio output buffer
		for(unsigned int channel = 0; channel <context->audioOutChannels; channel++) {
		    audioWrite(context, n, channel, out);
		}
		
		//Turn the LED on if we are early enough in the tick 
		// if(gMetronomeBeat != kMetronomeStateOff && gMetronomeCounter < gLEDInterval) {
		// 	digitalWriteOnce(context, n, kLEDPin, HIGH);
		// } 
		// else {
		// 	digitalWriteOnce(context, n, kLEDPin, LOW);
		// }
    }
}

// cleanup() runs once when the program stops
void cleanup(BelaContext *context, void *userData)
{
	// nothing to do here
}
