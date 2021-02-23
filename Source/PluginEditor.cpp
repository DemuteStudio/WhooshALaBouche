#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WhooshGeneratorAudioProcessorEditor::WhooshGeneratorAudioProcessorEditor (WhooshGeneratorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), recorder_(p.getAudioSource())
{
	slider_noise_level_.setRange(0.0, 1.0, 0.1);
	slider_noise_level_.setTextBoxStyle(Slider::TextBoxRight, false, 100, 50);
	addAndMakeVisible(slider_noise_level_);

	label_level_.setText("Noise Level", NotificationType::dontSendNotification);
	addAndMakeVisible(label_level_);

	addAndMakeVisible(recorder_);

	// Make sure you set the size of the component after
	// you add any child components.
	setSize(800, 600);
}

WhooshGeneratorAudioProcessorEditor::~WhooshGeneratorAudioProcessorEditor()
{
}

//==============================================================================
void WhooshGeneratorAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void WhooshGeneratorAudioProcessorEditor::resized()
{
	Rectangle<int> rectangle = getLocalBounds();
	slider_noise_level_.setBounds(10, 30, getWidth() - 60, 90);

	recorder_.setBounds(rectangle.removeFromBottom(500));
}
