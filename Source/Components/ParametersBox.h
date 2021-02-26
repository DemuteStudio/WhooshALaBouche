#pragma once

#include <JuceHeader.h>

using namespace juce;

//==============================================================================
/*
*/
class ParametersBox : public juce::Component
{
public:
	ParametersBox(double samples_per_block);
	~ParametersBox() override;

	void paint(juce::Graphics&) override;
	void resized() override;

	void add_listener(Slider::Listener* listener) const;


	std::unique_ptr<Slider> threshold_slider;
	Label threshold_label;

	std::unique_ptr<Slider> rms_length_slider;
	Label rms_length_label;

private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParametersBox)
};
