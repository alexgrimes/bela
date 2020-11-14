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
#include <libraries/Gui/Gui.h>
#include <libraries/GuiController/GuiController.h>

// Name of the sound file (in project folder)
std::string gFilename = "slow-drum-loop.wav";

// Object that handles playing sound from a buffer
MonoFilePlayer gPlayer;

// Bela slider Gui
Gui gGui;
GuiController gGuiController;

//variables for circular buffer and read pointer 
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
	
	// set up the Gui
	gGui.setup(context->projectName);
	gGuiController.setup(&gGui, "Delay Controller");
	
	//args: name, default value, minimum, maximum, increment (no fixed increment)
	gGuiController.addSlider("Delay", 0.1, 0, 0.49, 0);
	gGuiController.addSlider("Feedback", 0.5, 0, 0.95, 0);
	 
	return true;
}

void render(BelaContext *context, void *userData)
{
	
	//Read the delay in seconds
	float delay = gGuiController.getSliderValue(0);
	
	// convert delay to samples 
	int delayInSamples = delay * context->audioSampleRate;
	
	//calculate the read pointer based on the write pointer 
	gReadPointer = (gWritePointer - delayInSamples + gDelayBuffer.size()) % gDelayBuffer.size();
	
	//get Feedback level 
	float feedback = gGuiController.getSliderValue(1);
	
    for(unsigned int n = 0; n < context->audioFrames; n++) {
    	float in = gPlayer.process();
    	
    	//read the output from the write pointer (oldest sample) 
    	float out = gDelayBuffer[gReadPointer];
    	
    	//Overwrite the buffer at the write pointer, the increment and wrap pointer 
    	// do this first so we can have the read and write pointers be equal
    	gDelayBuffer[gWritePointer] = in + out * feedback; 
    	
    	
    	
    	//increment and wrap both the pointers
    	gWritePointer++;
    	if(gWritePointer >= gDelayBuffer.size())
    		gWritePointer = 0;
    	gReadPointer++;
    	if(gReadPointer >= gDelayBuffer.size())
    		gReadPointer = 0;
        
		// Write the input and output to different channels
    	audioWrite(context, n, 1, in);
    	audioWrite(context, n, 0, out);
    }
}

void cleanup(BelaContext *context, void *userData)
{

}
