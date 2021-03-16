#include "MyLookAndFeel.h"
#include "AudioWaveformComponent.h"
#include "AudioPlaybackPositionComponent.h"
#include "AudioWaveformSelectedRegionComponent.h"


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
		    AudioWaveformOpenGLComponent::ColourIds::waveformBackgroundColour,
	    waveformSelectedRegionBackgroundColourId =
		    AudioWaveformSelectedRegionComponent::ColourIds::waveformSelectedRegionBackgroundColour,
	    lineColourId =
		    AudioPlaybackPositionComponent::ColourIds::line_colour;

	setColour(waveformColourId, contrasting_colour);
	setColour(waveformBackgroundColourId, main_colour.contrasting(0.2f));
	setColour(waveformSelectedRegionBackgroundColourId,
	          main_colour.contrasting(1.0f));
	setColour(lineColourId,
	          Colour::contrasting(
		          Colour::contrasting(
			          findColour(waveformColourId),
			          findColour(waveformBackgroundColourId)
		          ),
		          findColour(waveformSelectedRegionBackgroundColourId)
	          ).overlaidWith(Colours::red.withAlpha(0.8f)).withAlpha(0.8f)
	);
}

MyLookAndFeel::~MyLookAndFeel()
{
}
