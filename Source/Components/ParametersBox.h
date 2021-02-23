/*
  ==============================================================================

    ParametersBox.h
    Created: 23 Feb 2021 3:47:28pm
    Author:  arnau

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace juce;

//==============================================================================
/*
*/
class ParametersBox : public juce::Component, public Slider::Listener
{
public:
	ParametersBox();
	~ParametersBox() override;

	void paint(juce::Graphics&) override;
	void resized() override;

	void sliderValueChanged(Slider* slider) override;

	std::unique_ptr<Slider> threshold_slider;
	Label threshold_label;
	float threshold_value;

private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParametersBox)
};
