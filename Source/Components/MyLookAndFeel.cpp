#include "MyLookAndFeel.h"
#include "AudioWaveformComponent.h"


MyLookAndFeel::MyLookAndFeel()
{
	const Colour main_color = Colour(233, 242, 249),
		contrasting_color = Colour(27, 50, 5),
		text_color = Colour(233, 242, 249),
		transparent_color = Colour((uint8)0, 0, 0, 0.f);

	setColour(TextButton::ColourIds::buttonColourId, main_color.brighter());
	setColour(TextButton::ColourIds::buttonOnColourId, main_color.brighter());
	setColour(TextButton::ColourIds::textColourOffId, text_color);
	setColour(TextButton::ColourIds::textColourOnId, text_color);
	setColour(Slider::ColourIds::backgroundColourId,
	          main_color.contrasting(0.3f));
	setColour(Slider::ColourIds::thumbColourId, main_color.contrasting(0.8f));
	setColour(Slider::ColourIds::trackColourId, main_color);
	setColour(Label::ColourIds::textColourId, text_color);
	setColour(ToggleButton::ColourIds::textColourId, text_color);
	setColour(ToggleButton::ColourIds::tickColourId, text_color);

	setColour(Slider::ColourIds::textBoxOutlineColourId, transparent_color);

	int waveformColourId =
		AudioWaveformOpenGLComponent::ColourIds::waveformColour,
		waveformBackgroundColourId =
		AudioWaveformOpenGLComponent::ColourIds::waveformBackgroundColour;

	setColour(waveformColourId, contrasting_color);
	setColour(waveformBackgroundColourId, main_color.contrasting(0.2f));
}

MyLookAndFeel::~MyLookAndFeel()
{
}
