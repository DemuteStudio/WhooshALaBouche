#pragma once

#include <JuceHeader.h>

class fx_chain_element
{
public:
	fx_chain_element() = default;

	virtual void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
	{
	}

	virtual void prepareToPlay(double sampleRate, int samplesPerBlock)
	{
	}
};
