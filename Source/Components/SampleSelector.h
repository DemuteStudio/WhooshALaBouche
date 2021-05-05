#pragma once

#include <JuceHeader.h>
#include "SelectorComponent.h"
#include "FoleyInput.h"
#include "SampleComponent.h"

//==============================================================================
/*
*/
class SampleSelector : public SelectorComponent
{
public:
	SampleSelector(FoleyInput* foley_input);
	~SampleSelector() override;
	void enable_default_element();

	void reload_samples();
private:
	void element_clicked(SelectorElement*) override ;
	void create_element_components() override;

//==============================================================================
	FoleyInput* foley_input_;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleSelector)
};
