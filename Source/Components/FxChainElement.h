#pragma once

#include <JuceHeader.h>

class FxChainElement
{
public:
	FxChainElement() = default;
	~FxChainElement() = default;

	virtual void getNextAudioBlock(juce::AudioBuffer<float>& bufferToFill) = 0;
	
	virtual void prepareToPlay(double sampleRate, int samplesPerBlock)
	{
		sampleRate_ = sampleRate;
		samplesPerBlock = samplesPerBlock;
	}


protected:
	double sampleRate_;
	double samplesPerBlock_;
};
