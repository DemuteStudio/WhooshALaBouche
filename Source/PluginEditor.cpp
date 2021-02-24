#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WhooshGeneratorAudioProcessorEditor::WhooshGeneratorAudioProcessorEditor (WhooshGeneratorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), recorder_(p.getAudioSource()), parameters_box_()
{
	addAndMakeVisible(recorder_);
	addAndMakeVisible(parameters_box_);

	parameters_box_.add_listener(this);


	// Make sure you set the size of the component after
	// you add any child components.
	setSize(1000, 900);
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
}

void WhooshGeneratorAudioProcessorEditor::resized()
{
	Rectangle<int> rectangle = getLocalBounds();
	const int height = rectangle.getHeight();
	int width = rectangle.getWidth();

	recorder_.setBounds(rectangle.removeFromTop((height/5)*4));
	parameters_box_.setBounds(rectangle);

}

void WhooshGeneratorAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	if (slider->getName() == "threshold")
	{
		audioProcessor.threshold_value = slider->getValue();
	}
	if (slider->getName() == "rms_length")
	{
		audioProcessor.rms_length_value = slider->getValue();
	}
}
