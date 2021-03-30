#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioWaveformOpenGLComponent.h"


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
	TextButton clean_envelope_button;


	void enableButtons(std::initializer_list<Button*> buttons, bool enable);
private:


	//MyLookAndFeel tenFtLookAndFeel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(buttons_panel)
};
