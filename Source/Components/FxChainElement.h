#pragma once

#include <JuceHeader.h>

class fx_chain_element
{
public:
	fx_chain_element() = default;
	~fx_chain_element() = default;

	virtual void getNextAudioBlock(juce::AudioBuffer<float>& bufferToFill)
	{
	}

	virtual void prepareToPlay(double sampleRate, int samplesPerBlock)
	{
		sampleRate_ = sampleRate;
		samplesPerBlock = samplesPerBlock;
	}

protected:
	double sampleRate_;
	double samplesPerBlock_;
};
