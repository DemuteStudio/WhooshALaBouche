/*
  ==============================================================================

    SampleSelector.cpp
    Created: 26 Apr 2021 10:51:33am
    Author:  arnau

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SampleSelector.h"

//==============================================================================
SampleSelector::SampleSelector(InternalFoleyInput* foley_input): foley_input_(foley_input)
{
	create_sample_components();
}

SampleSelector::~SampleSelector()
{
}

void SampleSelector::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("SampleSelector", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void SampleSelector::resized()
{
	auto rectangle = getLocalBounds();

	const int components_per_row = 5;
	const int number_of_rows = components_per_row/sample_components_.size();

	const int component_width = rectangle.getWidth() / components_per_row;
	const int delta = 5;

	for (auto& sample_component : sample_components_)
	{
		sample_component->setBounds(rectangle.removeFromLeft(component_width).reduced(delta));
	}

}

void SampleSelector::create_sample_components()
{
	for (auto sample_file : foley_input_->get_samples_files())
	{
		sample_components_.emplace_back(std::make_unique<SampleComponent>(sample_file));
	}
	for (auto& component : sample_components_)
	{
		addAndMakeVisible(component.get());
	}
}
