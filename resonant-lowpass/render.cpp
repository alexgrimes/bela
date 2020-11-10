/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\

http://bela.io

C++ Real-Time Audio Programming with Bela - Lecture 8: Filters
resonant-lowpass: adjustable 2nd-order lowpass filter example (to complete in lecture)
*/

#include <Bela.h>
#include <cmath>
#include <libraries/Gui/Gui.h>
#include <libraries/GuiController/GuiController.h>
#include "MonoFilePlayer.h"

// Name of the sound file (in project folder)
std::string gFilename = "guitar-loop.wav";

// Object that handles playing sound from a buffer
MonoFilePlayer gPlayer;

// Browser-based GUI to adjust params
Gui gGui;
GuiController gGuiController;

// Filter state and coefficients
// TODO: declare additional global variable(s) to hold filter state.
// to keep track of previous inputs and previous outputs 
float gLastX1 = 0, gLastX2 = 0;
float gLastY1 = 0, gLastY2 = 0;
float gA1 = 0, gA2 = 0;
float gB0 = 1, gB1 = 0, gB2 = 0;

// Calculate the filter coefficients based on the given parameters
// Borrows code from the Bela Biquad library, itself based on code by
// Nigel Redmon
void calculate_coefficients(float sampleRate, float frequency, float q)
{
    float k = tanf(M_PI * frequency / sampleRate);
    float norm = 1.0 / (1 + k / q + k * k);
    
    gB0 = k * k * norm;
    gB1 = 2.0 * gB0;
    gB2 = gB0;
    gA1 = 2 * (k * k - 1) * norm;
    gA2 = (1 - k / q + k * k) * norm;	
}

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
	
	// Calculate initial coefficients
	calculate_coefficients(context->audioSampleRate, 1000, 0.707);
	
	// set up the GUI
	gGui.setup(context->projectName);
	gGuiController.setup(&gGui, "Filter Controller");
	
	// arguments: name, default value, minimum, max, increment
	gGuiController.addSlider("Frequency", 1000, 100, 5000, 0);
	gGuiController.addSlider("Q", 0.707, 0.5, 10, 0);

	return true;
}

void render(BelaContext *context, void *userData)
{
	// TODO (step 2): use GUI or analog inputs to recalculate coefficients
	// get params from GUI
	
	float frequency = gGuiController.getSliderValue(0); 
	float q = gGuiController.getSliderValue(1);
	
	calculate_coefficients(context->audioSampleRate, frequency, q);
	
    for(unsigned int n = 0; n < context->audioFrames; n++) {
        float in = 0.5 * gPlayer.process();
      
    	// TODO: implement filter equation
    	// implements output as weighted sum of current and previous inputs (b coefficients) and then
    	// subtracting the previous outputs weighted by the a coefficients.
		
        float out = gB0*in + gB1*gLastX1 + gB2*gLastX2 - gA1*gLastY1 - gA2*gLastY2;
        
        // Update state vars
        // it doesnt matter whether we update the X or the Y first but it matters whether X2 comes 
        // before X1 here. 
        gLastX2 = gLastX1;
        gLastX1 = in;
        gLastY2 = gLastY1;
        gLastY1 = out;
        
    	for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
			// Write the sample to every audio output channel
    		audioWrite(context, n, channel, out);
    	}
    }
}

void cleanup(BelaContext *context, void *userData)
{

}
