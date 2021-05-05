#include "MyLookAndFeel.h"
#include "AudioWaveformComponent.h"


MyLookAndFeel::MyLookAndFeel()
{
	const Colour off_white_blue = Colour(242, 246, 252),
	             blue = Colour(84, 176, 243),
	             light_kaki = Colour(177, 197, 197),
	             //===========================IZOTOPE COLORS==========================================================
	             dark_background = Colour(50, 50, 50),
	             slider_background = Colour(60, 60, 60),
	             white_text_on = Colour(213, 213, 213),
	             grey_text_off = Colour(106, 106, 106),
	             orange_contrast = Colour(137, 106, 93),
	             bright_meter = Colour(181, 196, 189),
	             //=====================================================================================
	             dark_green = Colour(27, 50, 5);

	const Colour main_color = Colour(dark_background),
	             contrasting_color = Colour(orange_contrast),
	             text_color = Colour(white_text_on),
	             text_color_off = Colour(grey_text_off),
	             transparent_color = Colour((uint8)0, 0, 0, 0.f);

	//=====================================================================================
	setColour(juce::ResizableWindow::backgroundColourId, main_color);
	//=====================================================================================
	setColour(TextButton::ColourIds::buttonColourId, main_color);
	setColour(TextButton::ColourIds::buttonOnColourId, main_color);
	setColour(TextButton::ColourIds::textColourOffId, text_color_off);
	setColour(TextButton::ColourIds::textColourOnId, text_color);
	//=====================================================================================
	setColour(Slider::ColourIds::backgroundColourId,
	          slider_background);
	// setColour(Slider::ColourIds::thumbColourId, main_color.contrasting(0.8f));
	setColour(Slider::ColourIds::thumbColourId, text_color);
	setColour(Slider::ColourIds::trackColourId, bright_meter);
	setColour(Slider::ColourIds::textBoxTextColourId, text_color);
	//=====================================================================================
	setColour(Label::ColourIds::textColourId, text_color);
	//=====================================================================================
	setColour(ToggleButton::ColourIds::textColourId, text_color);
	setColour(ToggleButton::ColourIds::tickColourId, text_color);

	setColour(Slider::ColourIds::textBoxOutlineColourId, transparent_color);

	int waveformColourId =
		    AudioWaveformOpenGLComponent::ColourIds::waveformColour,
	    waveformBackgroundColourId =
		    AudioWaveformOpenGLComponent::ColourIds::waveformBackgroundColour;

	setColour(waveformColourId, bright_meter);
	setColour(waveformBackgroundColourId, main_color);
}

MyLookAndFeel::~MyLookAndFeel()
{
}
