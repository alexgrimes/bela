/***** ExponentialSegment.cpp *****/

// an ExponentialSegment generator 

#include <cmath>
#include "ExponentialSegment.h"

//Constructor
ExponentialSegment::ExponentialSegment()
{
	sampleRate_ = 1;
	setValue(0);
}

// constructor specifying a sample rate
ExponentialSegment::ExponentialSegment(float sampleRate)
{
	sampleRate_ = sampleRate;
	setValue(0);
}

//set the sample rate for all calculations
void ExponentialSegment::setSampleRate(float rate)
{
	sampleRate_ = rate;
}

// jump to a value 
void ExponentialSegment::setValue(float value)
{
	currentValue_ = value;
	asymptoticValue_ = targetValue_ = value;
	expValue_ = 0;
	multiplier_ = 0;
	
}

//ramp to a value over a period of time 
void ExponentialSegment::rampTo(float value, float time, float overshootRatio)
{
	//ramp towards the target value 
	targetValue_ = value;
	
	// calculate how far beyond the target to ramp, based on value and overshoot 
	float distanceToTarget = targetValue_ - currentValue_;
	asymptoticValue_ = currentValue_ + distanceToTarget * overshootRatio;
	
	expValue_ = currentValue_ - asymptoticValue_;
	
	//calculate time constant to reach the target in the specified time 
	double tau = -1.0 * time / log(1.0 - 1.0/overshootRatio);
	
	//calculate the multiplier for each frame 
	multiplier_ = pow(exp(-1.0 / tau), 1.0 / sampleRate_);
}

//Generate and return the next ramp output 
float ExponentialSegment::process()
{
	currentValue_ = asymptoticValue_ + expValue_;
	
	if(!finished())
		expValue_ *= multiplier_;
		
		return currentValue_;
}

//return whether the ramp is finished 
bool ExponentialSegment::finished()
{
	//check if we have reached the target. need to check if we're going up or down 
	if(currentValue_ >= targetValue_ && currentValue_ <= asymptoticValue_)
		return true;
	if(currentValue_ <= targetValue_ && currentValue_ >= asymptoticValue_)
		return true;
	return false;
}

//destructor 
ExponentialSegment::~ExponentialSegment()
{
	
}
