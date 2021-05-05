#include <JuceHeader.h>
#include "SelectorComponent.h"

//==============================================================================
SelectorComponent::SelectorComponent()
{
}

SelectorComponent::~SelectorComponent()
{
}

void SelectorComponent::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId)); // clear the background

	g.setColour(juce::Colours::grey);
	g.drawRect(getLocalBounds(), 1); // draw an outline around the component
}

void SelectorComponent::resized()
{
	const auto rectangle = getLocalBounds();
	const int components_per_row = 4;
	const int delta = 4;

	place_element_components(rectangle, components_per_row, delta);
}


void SelectorComponent::place_element_components(Rectangle<int> rectangle, int components_per_row, int delta)
{
	const int number_of_rows = 1 + (element_components_.size() / components_per_row);

	const auto row_height = rectangle.getHeight() / number_of_rows;
	const int component_width = rectangle.getWidth() / components_per_row;

	int component_index = 0;
	for (int row = 0; row < number_of_rows; ++row)
	{
		auto row_rectangle = rectangle.removeFromTop(row_height);
		for (int column_index = 0; column_index < components_per_row; ++column_index)
		{
			if (component_index >= element_components_.size())
			{
				break;
			}
			element_components_.at(component_index)->
			                    setBounds(row_rectangle.removeFromLeft(component_width).reduced(delta));
			component_index++;
		}
	}
}
