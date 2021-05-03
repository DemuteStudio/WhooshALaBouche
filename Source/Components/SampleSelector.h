#pragma once

#include <JuceHeader.h>
#include "FoleyInput.h"
#include "SampleComponent.h"

using namespace  juce;
//==============================================================================
/*
*/
class SampleSelector : public juce::Component
{
public:
	SampleSelector(FoleyInput* foley_input);
	~SampleSelector() override;

	void paint(juce::Graphics&) override;
	void resized() override;

private:
	void place_sample_components(Rectangle<int> rectangle, int components_per_row, int delta);
	void sample_clicked(SampleComponent*) ;
//==============================================================================
	std::vector<std::unique_ptr<SampleComponent>> sample_components_;
	FoleyInput* foley_input_;

	void create_sample_components();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleSelector)
};
