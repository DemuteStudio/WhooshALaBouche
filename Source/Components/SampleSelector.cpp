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

	g.setColour(juce::Colours::white);
	g.setFont(14.0f);
	g.drawText("SampleSelector", getLocalBounds(),
	           juce::Justification::centred, true); // draw some placeholder text
}

void SampleSelector::resized()
{
	auto rectangle = getLocalBounds();

	const int components_per_row = 5;
	const int number_of_rows = components_per_row / sample_components_.size();

	const int component_width = rectangle.getWidth() / components_per_row;
	const int delta = 5;

	for (auto& sample_component : sample_components_)
	{
		sample_component->setBounds(rectangle.removeFromLeft(component_width).reduced(delta));
	}
}

void SampleSelector::sample_clicked(SampleComponent*clicked_sample)
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
	for (const auto& sample : foley_input_->files_audio_sources_)
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
