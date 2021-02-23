#pragma once

#include "AudioClockComponent.h"
#include "AudioPlaybackPositionComponent.h"
#include "AudioScrollerComponent.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioWaveformComponent.h"
#include "AudioWaveformSelectedRegionComponent.h"
#include "TenFtLookAndFeel.h"

using namespace juce;

class Recorder : public Component,
	private Timer
{
public:
	Recorder();

	~Recorder();

	void resized() override;

	void paint(Graphics& g) override;

private:
	void openButtonClicked();

	void loadFile(AudioFormatReader* audioReader);

	void unloadFile();

	void recordButtonClicked();

	void enableRecording();

	void disableRecording();

	void loopButtonClicked();

	void setupButton(
		TextButton& button,
		std::string buttonText,
		bool enabled
	);

	void enableButtons(std::initializer_list<Button*> buttons, bool enable);

	void timerCallback() override;

private:
	TextButton openButton;
	TextButton recordButton;
	TextButton playButton;
	TextButton stopButton;
	ToggleButton loopButton;
	TextButton muteButton;
	TextButton fadeInButton;
	TextButton fadeOutButton;
	TextButton normalizeButton;

	AudioFormatManager formatManager;
	std::unique_ptr<AudioSampleBuffer> audioBuffer;

	AudioWaveformComponent waveform;
	AudioWaveformSelectedRegionComponent selectedRegion;
	AudioPlaybackPositionComponent playbackPosition;
	AudioClockComponent clock;
	AudioScrollerComponent scroller;
	TenFtAudioSource& audioSource;


	TenFtLookAndFeel tenFtLookAndFeel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Recorder)
};

