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
class ParametersBox : public juce::Component
{
public:
	ParametersBox();
	~ParametersBox() override;

	void paint(juce::Graphics&) override;
	void resized() override;

	void add_listener(Slider::Listener* listener) const;


	std::unique_ptr<Slider> threshold_slider;
	Label threshold_label;


private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParametersBox)
};
