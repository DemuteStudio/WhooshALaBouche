#include <JuceHeader.h>
#include "SampleManager.h"


SampleManager::SampleManager(FoleyInput* foley_input): foley_input_(foley_input), sample_selector_(foley_input), library_selector_(foley_input)
{
	setup_library_name_label();
	setup_selector_name_label();
	setup_load_library_button();

	children_ = {&sample_selector_, &library_selector_};
	show_samples_selector();

	library_selector_.on_library_loaded = [this]()
	{
		show_samples_selector();
		sample_selector_.reload_samples();
		display_library_name();
		
	};
}

void SampleManager::setup_load_library_button()
{
	addAndMakeVisible(load_library_);
	load_library_.setButtonText("LOAD LIBRARY");
	load_library_.setToggleState(true, dontSendNotification);
	load_library_.onClick = [this]()
	{
		show_libraries_selector();
	};
}

void SampleManager::display_library_name()
{
	library_name.setText(foley_input_->get_current_library_name(), dontSendNotification);
}

void SampleManager::setup_library_name_label()
{
	addAndMakeVisible(library_name);
	display_library_name();
}

void SampleManager::setup_selector_name_label()
{
	addAndMakeVisible(selector_name);
	selector_name.setJustificationType(Justification::centred);
}

//==============================================================================
SampleManager::~SampleManager()
{
}

void SampleManager::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId)); // clear the background

	g.setColour(juce::Colours::grey);
	g.drawRect(getLocalBounds(), 1); // draw an outline around the component
}

void SampleManager::resized()
{
	auto rectangle = getLocalBounds();

	const int components_per_row = 5;
	const int delta = 4;

	auto info_bar = rectangle.removeFromTop(30);
	library_name.setBounds(info_bar.removeFromLeft(75));
	load_library_.setBounds(info_bar.removeFromRight(75).reduced(delta));
	selector_name.setBounds(info_bar);

	set_child_selector_bounds(rectangle);
}

void SampleManager::set_child_selector_bounds(const Rectangle<int>& rectangle) const
{
	for (auto component : children_)
	{
		component->setBounds(rectangle);
	}
}

void SampleManager::show_samples_selector()
{
	set_visible_selector(&sample_selector_);
	selector_name.setText("Choose sample", dontSendNotification);

}

void SampleManager::show_libraries_selector()
{
	set_visible_selector(&library_selector_);
	selector_name.setText("Choose Library", dontSendNotification);
}

void SampleManager::set_visible_selector(SelectorComponent* new_visible_component)
{
	if (visible_selector != nullptr)
	{
		removeChildComponent(visible_selector);
	}
	visible_selector = new_visible_component;
	addAndMakeVisible(visible_selector);
}

