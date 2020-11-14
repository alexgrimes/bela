/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\

http://bela.io

C++ Real-Time Audio Programming with Bela - Lecture 11: Circular buffers
circular-buffer: template code for implementing delays
*/

#include <Bela.h>
#include <vector>
#include "MonoFilePlayer.h"

// Name of the sound file (in project folder)
std::string gFilename = "slow-drum-loop.wav";

// Object that handles playing sound from a buffer
MonoFilePlayer gPlayer;

// TODO: declare variables for circular buffer
std::vector<float> gDelayBuffer;
unsigned int gWritePointer = 0;
unsigned int gReadPointer = 0;


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

	// allocate the circular buffer to 0.5 seconds 
	gDelayBuffer.resize(0.5 * context->audioSampleRate);
	
	//calculate the offset between read and write pointers 
	gReadPointer = (gWritePointer - (int)(0.1*context->audioSampleRate) + gDelayBuffer.size()) % gDelayBuffer.size();
	 
	return true;
}

void render(BelaContext *context, void *userData)
{
    for(unsigned int n = 0; n < context->audioFrames; n++) {
    	//input could come from anywhere
        float in = gPlayer.process();
    	
    	//read the output from the write pointer (oldest sample)
    	float out = gDelayBuffer[gReadPointer];
    	
    	//Overwrite the buffer at the write pointer, the increment and wrap pointer 
    	gDelayBuffer[gWritePointer] = in; 
    	
    	//increment and wrap both the pointers
    	gWritePointer++;
    	if(gWritePointer >= gDelayBuffer.size())
    		gWritePointer = 0;
    	gReadPointer++;
    	if(gReadPointer >= gDelayBuffer.size())
    		gReadPointer = 0;
        
		// Write the input and output to different channels
    	audioWrite(context, n, 0, in);
    	audioWrite(context, n, 1, out);
    }
}

void cleanup(BelaContext *context, void *userData)
{

}
