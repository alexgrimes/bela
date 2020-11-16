/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\

http://bela.io

C++ Real-Time Audio Programming with Bela - Lecture 13: State Machines
hysteresis-comparator: implement a comparator with variable threshold (hysteresis)
*/

#include <Bela.h>
#include <libraries/Scope/Scope.h>

// Oscilloscope
Scope gScope;

// Pin declarations
const unsigned int kLEDPin = 0;					// Pin number of the LED
const unsigned int kInputPin = 0;				// Pin number of the analog input

// state machine 
enum {
	kStateLow = 0,
	kStateHigh
};

// declare states and high and low thresholds here
int gComparatorState = kStateLow;
float gThresholdHigh = 0.75;
float gThresholdLow = 0.25;

bool setup(BelaContext *context, void *userData)
{
	// Check that we have the expected analog and digital I/O
	if(context->digitalFrames != 2*context->analogFrames) {
		rt_fprintf(stderr, "This example expects analog inputs at half the digital sample rate.\n");
		return false;
	}	
	
	// Set LED pin to an output
	pinMode(context, 0, kLEDPin, OUTPUT);
	
	// Initialise the scope
	gScope.setup(2, context->digitalSampleRate);
	
	return true;
}

void render(BelaContext *context, void *userData)
{
   for(unsigned int n = 0; n < context->digitalFrames; n++) {
   		// Read the analog pin
   		float reading = analogRead(context, n/2, kInputPin);
   		int output = 0;
   	
		// hysteresis comparator: different threshold in each state
   		if(gComparatorState == kStateLow) {
   			if(reading > gThresholdHigh) {
   				rt_printf("LOW --> HIGH\n");
   				gComparatorState = kStateHigh;
   			}
   		}
   		else if (gComparatorState == kStateHigh) {
   			output = 1;
   			if(reading < gThresholdLow) {
   				rt_printf("HIGH --> LOW\n");
   				gComparatorState = kStateLow;
   			}
   		}

   		// Write the state to the scope and LED
   		digitalWriteOnce(context, n, kLEDPin, output);
   		gScope.log(reading, (float)output);
    }
}

void cleanup(BelaContext *context, void *userData)
{

}