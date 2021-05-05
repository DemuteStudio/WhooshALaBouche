#pragma once

#include <JuceHeader.h>

#include "SelectorElement.h"

using namespace  juce;
//==============================================================================
/*
*/

class SelectorComponent  : public juce::Component
{
public:
    SelectorComponent();
    ~SelectorComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

protected:
	virtual void create_element_components(){};
	virtual void element_clicked(SelectorElement*)= 0;

	std::vector<std::unique_ptr<SelectorElement>> element_components_;
private:
	void place_element_components(Rectangle<int> rectangle, int components_per_row, int delta);

//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SelectorComponent)
};
