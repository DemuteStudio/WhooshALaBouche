#include <JuceHeader.h>
#include "WaveFormDisplay.h"

//==============================================================================
WaveFormDisplay::WaveFormDisplay():
	thumbnailCache(2)
{
	formatManager.registerBasicFormats();
	// transportPosition.setStart(0.0);

	thumbnail_ = std::make_unique<AudioThumbnail>(source_samples_per_thumbnail_sample_, formatManager, thumbnailCache);

	thumbnail_->addChangeListener(this);

	startTimerHz(120);
}

WaveFormDisplay::~WaveFormDisplay()
{
}

void WaveFormDisplay::paint(juce::Graphics& g)
{
	if (thumbnail_->getTotalLength() > 0.0)
	{
		g.setGradientFill(ColourGradient(Colour::fromRGBA(255, 255, 255, 255), topThumbArea.getX(),
		                                 topThumbArea.getHeight() / 2, Colour::fromRGBA(255, 255, 255, 101),
		                                 topThumbArea.getWidth(), topThumbArea.getHeight() / 2, false));

		// if (transportPosition.getStart() < transportSource.getLengthInSeconds())
		// {
		// 	thumbnail_total_->drawChannel(g, topThumbArea.reduced(2),
		// 	                             0.0, thumbnail_total_->getTotalLength(), 0, 1.0f);
		//
		// 	thumbnail_zoomed->drawChannel(g, bottomThumbArea.reduced(2),
		// 	                              transportPosition.getStart(), transportPosition.getEnd(), 0, 1.0f);
		// }
		// else
		// {
		// 	thumbnail_total_->drawChannel(g, topThumbArea.reduced(2),
		// 	                             0.0, thumbnail_total_->getTotalLength(), 0, 1.0f);
		//
		// 	thumbnail_zoomed->drawChannel(g, bottomThumbArea.reduced(2),
		// 	                              transportPosition.getStart(), transportPosition.getEnd(), 0, 1.0f);
		// }
	}
	else
	{
		g.setColour(Colours::black);
		g.setFont(28.0f);
		g.drawFittedText("No audio file selected...", getLocalBounds(), Justification::centred, 2);
	}
}

void WaveFormDisplay::resized()
{
	auto r = getLocalBounds();
	auto transportBounds = r.removeFromBottom(30);
	int transportButtonWidth = transportBounds.getWidth() / 5;
	int reduceAmount = 3;

	bottomThumbArea = getLocalBounds();
	topThumbArea = bottomThumbArea.removeFromTop(getLocalBounds().getHeight() / 2);
}

void WaveFormDisplay::changeListenerCallback(ChangeBroadcaster* source)
{
	if (source == thumbnail_.get())
	{
		if (thumbnail_->getTotalLength() > 0.0)
			repaint();
	}
}

void WaveFormDisplay::timerCallback()
{
	// double currentTime = transportSource.getCurrentPosition();
	// transportPosition.setStart(currentTime);
	// transportPosition.setEnd(currentTime + 8.0);
	// repaint();
}
