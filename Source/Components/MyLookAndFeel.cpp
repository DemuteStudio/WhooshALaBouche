#include "MyLookAndFeel.h"
#include "AudioWaveformComponent.h"


MyLookAndFeel::MyLookAndFeel()
{
	const Colour main_colour = Colour(233, 242, 249),
	             contrasting_colour = Colour(27, 50, 95),
	             text_colour = Colour(233, 242, 249);

	setColour(TextButton::ColourIds::buttonColourId, main_colour.brighter());
	setColour(TextButton::ColourIds::buttonOnColourId, main_colour.brighter());
	setColour(TextButton::ColourIds::textColourOffId, text_colour);
	setColour(TextButton::ColourIds::textColourOnId, text_colour);
	setColour(Slider::ColourIds::backgroundColourId,
	          main_colour.contrasting(0.3f));
	setColour(Slider::ColourIds::thumbColourId, main_colour.contrasting(0.8f));
	setColour(Slider::ColourIds::trackColourId, main_colour);
	setColour(Label::ColourIds::textColourId, text_colour);
	setColour(ToggleButton::ColourIds::textColourId, text_colour);
	setColour(ToggleButton::ColourIds::tickColourId, text_colour);

	int waveformColourId =
		AudioWaveformOpenGLComponent::ColourIds::waveformColour,
		waveformBackgroundColourId =
		AudioWaveformOpenGLComponent::ColourIds::waveformBackgroundColour;

	setColour(waveformColourId, contrasting_colour);
	setColour(waveformBackgroundColourId, main_colour.contrasting(0.2f));
}

MyLookAndFeel::~MyLookAndFeel()
{
}
