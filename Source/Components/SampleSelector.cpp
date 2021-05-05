#include <JuceHeader.h>
#include "SampleSelector.h"


//==============================================================================
SampleSelector::SampleSelector(FoleyInput* foley_input): foley_input_(foley_input)
{
	SampleSelector::create_element_components();
	enable_default_element();
}

SampleSelector::~SampleSelector()
{
}

void SampleSelector::enable_default_element()
{
	if (!element_components_.empty())
	{
		element_clicked(element_components_.at(0).get());
	}
}

void SampleSelector::reload_samples()
{
	element_components_.clear();
	SampleSelector::create_element_components();
	resized();
	enable_default_element();
}


void SampleSelector::element_clicked(SelectorElement* clicked_element)
{
	for (auto& sample_component : element_components_)
	{
		if (static_cast<Button*>(sample_component.get()) != clicked_element)
		{
			sample_component->unselect();
		}
	}
	clicked_element->select();

	foley_input_->set_selected_sample(
		static_cast<SampleComponent*>(clicked_element)->file_audio_source->audio_source.get());
}

void SampleSelector::create_element_components()
{
	for (const auto& sample : foley_input_->files_audio_sources)
	{
		element_components_.emplace_back(std::make_unique<SampleComponent>(sample.get()));
	}
	for (auto& component : element_components_)
	{
		addAndMakeVisible(component.get());
		component->onClick = [this, &component]()
		{
			element_clicked(component.get());
		};
	}
}
