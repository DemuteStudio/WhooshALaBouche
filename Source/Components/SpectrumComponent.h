#pragma once

#include <JuceHeader.h>

#include "FxChainElement.h"
#include "SpectrumAnalyzer.h"

//==============================================================================
class SpectrumComponent : public juce::Component,
                                  private juce::Timer

{
public:
	//! [constructor]
	SpectrumComponent(SpectrumAnalyzer* analyzer);

	~SpectrumComponent() override
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

	SpectrumAnalyzer* analyzer;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumComponent)
};
