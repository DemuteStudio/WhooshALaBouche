/*
  ==============================================================================

    WaveFormDisplay.h
    Created: 8 May 2021 12:42:27pm
    Author:  arnau

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace juce;

class WaveFormDisplay : public Component,
                        public ChangeListener,
                        public Timer

{
public:
	WaveFormDisplay();
	~WaveFormDisplay() override;

	void paint(juce::Graphics&) override;
	void resized() override;

private:
	void changeListenerCallback(ChangeBroadcaster* source) override;
	void timerCallback() override;
	//==============================================================================
	AudioFormatManager formatManager;
	// AudioTransportSource& transportSource;
	AudioThumbnailCache thumbnailCache;
	std::unique_ptr<AudioThumbnail> thumbnail_;
	int source_samples_per_thumbnail_sample_ {512};
	bool enable_transport_follow_ = false;

	//==============================================================================
	Rectangle<int> bottomThumbArea;
	Rectangle<int> topThumbArea;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveFormDisplay)
};
