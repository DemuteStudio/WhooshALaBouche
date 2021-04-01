#pragma once

#include <JuceHeader.h>

#include "../FxChainElement.h"
#include "../SpectrumAnalyzer.h"

//==============================================================================
class SpectrumAnalyserComponent : public juce::Component,
                                  private juce::Timer,
                                  public SpectrumAnalyzer

{
public:
	//! [constructor]
	SpectrumAnalyserComponent();

	~SpectrumAnalyserComponent() override
	{
	}

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
