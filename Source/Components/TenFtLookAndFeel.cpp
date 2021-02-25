#include "TenFtLookAndFeel.h"
#include "AudioWaveformComponent.h"
#include "AudioPlaybackPositionComponent.h"
#include "AudioWaveformSelectedRegionComponent.h"


TenFtLookAndFeel::TenFtLookAndFeel()
{
	Colour mainColour =
		       Colour(50,62,68),
	       contrastingColour = mainColour.contrasting(1.0f);
	setColour(TextButton::ColourIds::buttonColourId, mainColour.brighter());
	setColour(TextButton::ColourIds::buttonOnColourId, mainColour.brighter());
	setColour(TextButton::ColourIds::textColourOffId, contrastingColour);
	setColour(TextButton::ColourIds::textColourOnId, contrastingColour);
	setColour(Slider::ColourIds::backgroundColourId,
	          mainColour.contrasting(0.3f));
	setColour(Slider::ColourIds::thumbColourId, mainColour.contrasting(0.8f));
	setColour(Slider::ColourIds::trackColourId, mainColour);
	setColour(Label::ColourIds::textColourId, contrastingColour);
	setColour(ToggleButton::ColourIds::textColourId, contrastingColour);
	setColour(ToggleButton::ColourIds::tickColourId, contrastingColour);

	int waveformColourId =
		    AudioWaveformOpenGLComponent::ColourIds::waveformColour,
	    waveformBackgroundColourId =
		    AudioWaveformOpenGLComponent::ColourIds::waveformBackgroundColour,
	    waveformSelectedRegionBackgroundColourId =
		    AudioWaveformSelectedRegionComponent::ColourIds::waveformSelectedRegionBackgroundColour,
	    lineColourId =
		    AudioPlaybackPositionComponent::ColourIds::line_colour;

	setColour(waveformColourId, contrastingColour);
	setColour(waveformBackgroundColourId, mainColour.contrasting(0.2f));
	setColour(waveformSelectedRegionBackgroundColourId,
	          mainColour.contrasting(1.0f));
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

TenFtLookAndFeel::~TenFtLookAndFeel()
{
}
