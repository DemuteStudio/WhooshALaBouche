#pragma once

#include "AudioClockComponent.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include "TenFtLookAndFeel.h"

using namespace juce;

class buttons_panel : public Component
{
public:
	buttons_panel();

	~buttons_panel();

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
	TextButton clean_envelope_button;
	TextButton fadeInButton;
	TextButton fadeOutButton;
	TextButton normalizeButton;
	AudioClockComponent clock;


	void enableButtons(std::initializer_list<Button*> buttons, bool enable);
private:


	TenFtLookAndFeel tenFtLookAndFeel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(buttons_panel)
};
