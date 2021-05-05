#include <JuceHeader.h>
#include "SelectorElement.h"

//==============================================================================
SelectorElement::SelectorElement(String name): TextButton(name)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
}

SelectorElement::~SelectorElement()
{
}

void SelectorElement::paint (juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId)); 

	g.setColour(juce::Colours::grey);
	g.drawRect(getLocalBounds(), 1); 

	const juce::Colour text_colour = getToggleState() ? getLookAndFeel().findColour(TextButton::ColourIds::textColourOnId) : getLookAndFeel().findColour(TextButton::ColourIds::textColourOffId);
	g.setColour(text_colour);
	g.setFont(14.0f);
	g.drawText(getName(), getLocalBounds(),
		juce::Justification::centred, true); 
}

void SelectorElement::unselect()
{
	setToggleState(false, juce::dontSendNotification);
	repaint();
}

void SelectorElement::select()
{
	setToggleState(true, juce::dontSendNotification);
	repaint();
}
