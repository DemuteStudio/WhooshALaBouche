#include <JuceHeader.h>
#include "SampleComponent.h"

//==============================================================================
SampleComponent::SampleComponent(const FoleyInput::FileAudioSource* sample) :
	TextButton(sample->file.getFileNameWithoutExtension()), file_audio_source(sample)
{
}

SampleComponent::~SampleComponent()
{
}

void SampleComponent::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId)); // clear the background

	g.setColour(juce::Colours::grey);
	g.drawRect(getLocalBounds(), 1); // draw an outline around the component

	const juce::Colour text_colour = getToggleState() ? juce::Colours::red : juce::Colours::white;
	g.setColour(text_colour);
	g.setFont(14.0f);
	g.drawText(getName(), getLocalBounds(),
		juce::Justification::centred, true); // draw some placeholder text
}

void SampleComponent::resized()
{
	// This method is where you should set the bounds of any child
	// components that your component contains..
}

void SampleComponent::unselect()
{
	setToggleState(false, juce::dontSendNotification);
	repaint();
}

void SampleComponent::select()
{
	setToggleState(true, juce::dontSendNotification);
	repaint();
}