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

<<<<<<< Updated upstream
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
=======
	g.setColour(juce::Colours::grey);
	g.drawRect(getLocalBounds(), 1); // draw an outline around the component
}


void SampleSelector::place_sample_components(Rectangle<int> rectangle, const int components_per_row, const int delta)
{
	const int number_of_rows = 1 + (sample_components_.size() / components_per_row);

	const auto row_height = rectangle.getHeight() / number_of_rows;
	const int component_width = rectangle.getWidth() / components_per_row;

	int component_index = 0;
	for (int row = 0; row < number_of_rows; ++row)
	{
		auto row_rectangle = rectangle.removeFromTop(row_height);
		for (int column_index = 0; column_index < components_per_row; ++column_index)
		{
			if (component_index >= sample_components_.size())
			{
				break;
			}
			sample_components_.at(component_index)->
				setBounds(row_rectangle.removeFromLeft(component_width).reduced(delta));
			component_index++;
		}
	}
>>>>>>> Stashed changes
}

void SampleSelector::resized()
{
	const auto rectangle = getLocalBounds();

	const int components_per_row = 5;
<<<<<<< Updated upstream
	const int number_of_rows = components_per_row/sample_components_.size();

	const int component_width = rectangle.getWidth() / components_per_row;
	const int delta = 5;

	for (auto& sample_component : sample_components_)
	{
		sample_component->setBounds(rectangle.removeFromLeft(component_width).reduced(delta));
	}

=======
	const int delta = 5;

	place_sample_components(rectangle, components_per_row, delta);
}

void SampleSelector::sample_clicked(SampleComponent* clicked_sample)
{
	for (auto& sample_component : sample_components_)
	{
		if (static_cast<Button*>(sample_component.get()) != clicked_sample)
		{
			sample_component->unselect();
		}
	}
	clicked_sample->select();

	foley_input_->set_selected_sample(clicked_sample->file_audio_source->audio_source.get());
>>>>>>> Stashed changes
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
