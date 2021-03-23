#include "ButtonsPanel.h"

buttons_panel::buttons_panel()
{
	// setLookAndFeel(&tenFtLookAndFeel);

	addAndMakeVisible(&sendEnvelopeButton);
	sendEnvelopeButton.setButtonText("Send Envelope");
	sendEnvelopeButton.setClickingTogglesState(false);
	sendEnvelopeButton.setToggleState(false,
	                                  NotificationType::dontSendNotification
	);


	addAndMakeVisible(&clean_envelope_button);
	clean_envelope_button.setButtonText("Clean Envelope");
	clean_envelope_button.setEnabled(false);

}

buttons_panel::~buttons_panel()
{
	setLookAndFeel(nullptr);
}

void buttons_panel::resized()
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

	sendEnvelopeButton.setBounds(
		row1.reduced(delta)
	);
	clean_envelope_button.setBounds(
		row3.removeFromLeft(width / 4.0f).reduced(delta)
	);
}

void buttons_panel::paint(Graphics& g)
{
	g.fillAll(findColour(
		AudioWaveformOpenGLComponent::ColourIds::waveformBackgroundColour
	).contrasting(0.2f));
}

// ==============================================================================


void buttons_panel::setupButton(
	TextButton& button, std::string buttonText, bool enabled
)
{
	button.setButtonText(buttonText);
	button.setEnabled(enabled);
}

void buttons_panel::enableButtons(
	std::initializer_list<Button*> buttons, bool enable
)
{
	for (Button* button : buttons)
	{
		button->setEnabled(enable);
	}
}
