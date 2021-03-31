#pragma once

#include <JuceHeader.h>

#include "../FxChainElement.h"
#include "../SpectrumAnalyzer.h"

//==============================================================================
class SpectrumAnalyserComponent : public juce::Component,
                                  private juce::Timer,
                                  public fx_chain_element,
                                  public SpectrumAnalyzer

{
public:
	//! [constructor]
	SpectrumAnalyserComponent();

	~SpectrumAnalyserComponent() override
	{
	}

	//==============================================================================
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	//==============================================================================
	void paint(juce::Graphics& g) override;
	//==============================================================================
	void timerCallback() override;
	//==============================================================================
	//==============================================================================
	void draw_frame(juce::Graphics& g);
	//==============================================================================
private:


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumAnalyserComponent)
};
