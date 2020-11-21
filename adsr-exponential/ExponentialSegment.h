/***** ExponentialSegment.h *****/


// an exponential segment generator 

#pragma once 

class ExponentialSegment {
	
public: 
	//constructor
	ExponentialSegment();
	
	//a constructor specifying sample rate
	ExponentialSegment(float sampleRate);
	
	// set the sample rate, used for all calculations
	void setSampleRate(float rate);
	
	// jump to a value 
	void setValue(float value);
	
	// ramp to a value over a period of time, with a given percept overshoot
	void rampTo(float value, float time, float overshootRatio = 1.001);
	
	//generate and return the next ramp output 
	float process();
	
	//return whether the ramp is finished 
	bool finished();
	
	// destructor 
	~ExponentialSegment();
	
private:
	// state variables
	double sampleRate_;
	double currentValue_;
	double targetValue_;
	double asymptoticValue_;
	double expValue_;
	double multiplier_;
};