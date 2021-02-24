#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WhooshGeneratorAudioProcessorEditor::WhooshGeneratorAudioProcessorEditor(WhooshGeneratorAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p), recorder_(p.getAudioSource()), parameters_box_(p.sample_rate)
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
void WhooshGeneratorAudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

	g.setColour(juce::Colours::white);
	g.setFont(15.0f);
}

void WhooshGeneratorAudioProcessorEditor::resized()
{
	Rectangle<int> rectangle = getLocalBounds();
	const int height = rectangle.getHeight();
	int width = rectangle.getWidth();

	recorder_.setBounds(rectangle.removeFromTop((height / 5) * 4));
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
		audioProcessor.rms_blocks_length = get_number_of_blocks_from_milliseconds(processor.getSampleRate(), slider->getValue(), processor.getBlockSize());
	}
}


int WhooshGeneratorAudioProcessorEditor::get_number_of_blocks_from_milliseconds(
	double sample_rate, const float length_in_milliseconds, int samples_per_block)
{
	return (int)(((sample_rate / 1000) * length_in_milliseconds)/samples_per_block);
}
