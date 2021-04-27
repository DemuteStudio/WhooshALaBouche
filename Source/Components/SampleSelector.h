/*
  ==============================================================================

    SampleSelector.h
    Created: 26 Apr 2021 10:51:33am
    Author:  arnau

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "InternalFoleyInput.h"
#include "SampleComponent.h"

//==============================================================================
/*
*/
class SampleSelector : public juce::Component
{
public:
	SampleSelector(InternalFoleyInput* foley_input);
	~SampleSelector() override;

	void paint(juce::Graphics&) override;
	void resized() override;

private:
	std::vector<std::unique_ptr<SampleComponent>> sample_components_;
	InternalFoleyInput* foley_input_;

	void create_sample_components();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleSelector)
};
