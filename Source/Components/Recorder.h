#pragma once

#include "AudioClockComponent.h"
#include "AudioPlaybackPositionComponent.h"
#include "AudioScrollerComponent.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioWaveformComponent.h"
#include "AudioWaveformSelectedRegionComponent.h"
#include "TenFtLookAndFeel.h"
#include "EnvelopeDrawer/Envelope.h"
#include "EnvelopeDrawer/EnvelopeComponent.h"
#include "EnvelopeDrawer/EnvelopeSelectedRegionComponent.h"
#include "EnvelopeDrawer/Envelope.h"

using namespace juce;

class Recorder : public Component
{
public:
	Recorder();

	~Recorder();

	void resized() override;

	void paint(Graphics& g) override;

	void setupButton(
		TextButton& button,
		std::string buttonText,
		bool enabled
	);

	TextButton sendEnvelopeButton;
	TextButton recordButton;
	TextButton playButton;
	TextButton stopButton;
	ToggleButton loopButton;
	TextButton muteButton;
	TextButton fadeInButton;
	TextButton fadeOutButton;
	TextButton normalizeButton;
	AudioClockComponent clock;


	void enableButtons(std::initializer_list<Button*> buttons, bool enable);
private:


	TenFtLookAndFeel tenFtLookAndFeel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Recorder)
};
