#include <JuceHeader.h>
#include "SampleSelector.h"

//==============================================================================
SampleSelector::SampleSelector(FoleyInput* foley_input): foley_input_(foley_input)
{
	create_sample_components();
}

SampleSelector::~SampleSelector()
{
}

void SampleSelector::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId)); // clear the background

	g.setColour(juce::Colours::grey);
	g.drawRect(getLocalBounds(), 1); // draw an outline around the component

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
}

void SampleSelector::resized()
{
	const auto rectangle = getLocalBounds();

	const int components_per_row = 5;
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
}

void SampleSelector::create_sample_components()
{
	for (const auto& sample : foley_input_->files_audio_sources)
	{
		sample_components_.emplace_back(std::make_unique<SampleComponent>(sample.get()));
	}
	for (auto& component : sample_components_)
	{
		addAndMakeVisible(component.get());
		component->onClick = [this, &component]()
		{
			sample_clicked(component.get());
		};
	}
}
