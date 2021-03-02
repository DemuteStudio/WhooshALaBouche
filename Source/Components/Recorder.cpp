#include "Recorder.h"

Recorder::Recorder()
{
	setLookAndFeel(&tenFtLookAndFeel);

	addAndMakeVisible(&sendEnvelopeButton);
	sendEnvelopeButton.setButtonText("Send Envelope");
	sendEnvelopeButton.setClickingTogglesState(false);
	sendEnvelopeButton.setToggleState(false,
	                            NotificationType::dontSendNotification
	);

	addAndMakeVisible(&recordButton);
	recordButton.setButtonText("Record");
	recordButton.setClickingTogglesState(false);
	recordButton.setToggleState(false,
	                            NotificationType::dontSendNotification
	);

	addAndMakeVisible(&playButton);
	playButton.setButtonText("Play");
	playButton.setEnabled(false);

	addAndMakeVisible(&stopButton);
	stopButton.setButtonText("Stop");
	stopButton.setEnabled(false);

	addAndMakeVisible(&loopButton);
	loopButton.setButtonText("Loop");
	loopButton.changeWidthToFitText();
	loopButton.setToggleState(false,
	                          NotificationType::dontSendNotification
	);
	loopButton.setEnabled(false);

	addAndMakeVisible(&muteButton);
	muteButton.setButtonText("Mute");
	muteButton.setEnabled(false);

	addAndMakeVisible(&fadeInButton);
	fadeInButton.setButtonText("Fade In");
	fadeInButton.setEnabled(false);

	addAndMakeVisible(&fadeOutButton);
	fadeOutButton.setButtonText("Fade Out");
	fadeOutButton.setEnabled(false);

	addAndMakeVisible(&normalizeButton);
	normalizeButton.setButtonText("Normalize");
	normalizeButton.setEnabled(false);

	addAndMakeVisible(&clock);
}

Recorder::~Recorder()
{
	setLookAndFeel(nullptr);
}

void Recorder::resized()
{
	juce::Rectangle<int> bounds =
		getLocalBounds().reduced(10);
	float width = bounds.getWidth(),
	      height = bounds.getHeight(),
	      delta = 5.0f;
	int button_row_height = height / 3;
	juce::Rectangle<int> row1 = bounds.removeFromTop(button_row_height),
	                     row2 = bounds.removeFromTop(button_row_height),
	                     row3 = bounds.removeFromTop(button_row_height);

	recordButton.setBounds(
		row1.removeFromLeft(width * 0.5f).reduced(delta)
	);
	sendEnvelopeButton.setBounds(
		row1.reduced(delta)
	);
	playButton.setBounds(
		row2.removeFromLeft(width * 0.42f).reduced(delta)
	);
	stopButton.setBounds(
		row2.removeFromLeft(width * 0.42f).reduced(delta)
	);
	loopButton.setBounds(
		row2.removeFromLeft(width * 0.07f).reduced(delta)
	);
	clock.setBounds(
		row2.reduced(delta)
	);
	muteButton.setBounds(
		row3.removeFromLeft(width / 4.0f).reduced(delta)
	);
	fadeInButton.setBounds(
		row3.removeFromLeft(width / 4.0f).reduced(delta)
	);
	fadeOutButton.setBounds(
		row3.removeFromLeft(width / 4.0f).reduced(delta)
	);
	normalizeButton.setBounds(
		row3.removeFromLeft(width / 4.0f).reduced(delta)
	);
}

void Recorder::paint(Graphics& g)
{
	g.fillAll(findColour(
		AudioWaveformOpenGLComponent::ColourIds::waveformBackgroundColour
	).contrasting(0.2f));
}

// ==============================================================================


void Recorder::setupButton(
	TextButton& button, std::string buttonText, bool enabled
)
{
	button.setButtonText(buttonText);
	button.setEnabled(enabled);
}

void Recorder::enableButtons(
	std::initializer_list<Button*> buttons, bool enable
)
{
	for (Button* button : buttons)
	{
		button->setEnabled(enable);
	}
}
