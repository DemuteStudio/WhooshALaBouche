#pragma once

#include <JuceHeader.h>
#include "FoleyInput.h"
#include "SampleSelector.h"
#include "LibrarySelector.h"

//==============================================================================
/*
*/
class SampleManager  : public juce::Component
{
public:
	void setup_library_name_label();
	void setup_selector_name_label();
    SampleManager(FoleyInput* foley_input);
    ~SampleManager() override;

    void paint (juce::Graphics&) override;
    void resized() override;
//==============================================================================
    void set_child_selector_bounds(const Rectangle<int>& rectangle) const;
	void setup_load_library_button();
	void display_library_name();

private:
    FoleyInput* foley_input_;

	SampleSelector sample_selector_;
	LibrarySelector library_selector_;

	SelectorComponent* visible_selector= nullptr;
	std::vector<SelectorComponent*> children_;
//==============================================================================
	void show_samples_selector();
	void show_libraries_selector();
	void set_visible_selector(SelectorComponent* new_visible_component);

//==============================================================================
	Label library_name{"", ""};
	Label selector_name{"", ""};
	TextButton load_library_{"load_library"};
//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleManager)
};
