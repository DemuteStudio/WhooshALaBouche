#include <JuceHeader.h>
#include "out_parameters_box.h"

//==============================================================================
out_parameters_box::out_parameters_box(AudioProcessorValueTreeState* processor_state)
{
	addAndMakeVisible(volume_out_slider_);
	volume_out_slider_.setSliderStyle(Slider::LinearVertical);
	volume_out_slider_.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
	volume_out_slider_.setRange(0., 1., 0.);


	addAndMakeVisible(volume_out_label_);
	volume_out_label_.setText("VOLUME", dontSendNotification);
	volume_out_label_.setJustificationType(Justification::centred);

	addAndMakeVisible(frequency_out_slider_);
	frequency_out_slider_.setSliderStyle(Slider::LinearVertical);
	// frequency_out_slider_.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
	frequency_out_slider_.setRange(50, 20000, 0.1);
	frequency_out_slider_.setSkewFactorFromMidPoint(1000.);


	addAndMakeVisible(frequency_out_label_);
	frequency_out_label_.setText("FREQUENCY", dontSendNotification);
	frequency_out_label_.setJustificationType(Justification::centred);

	volume_out_attachment_ = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*processor_state, "volume", volume_out_slider_);
	frequency_out_attachment_ = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*processor_state, "frequency", frequency_out_slider_);

}

out_parameters_box::~out_parameters_box()
{
}

void out_parameters_box::paint(juce::Graphics& g)
{
	/* This demo code just fills the component's background and
	   draws some placeholder text to get you started.

	   You should replace everything in this method with your own
	   drawing code..
	*/

	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId)); // clear the background

	g.setColour(juce::Colours::grey);
	g.drawRect(getLocalBounds(), 1); // draw an outline around the component

	// g.setColour(juce::Colours::white);
	// g.setFont(14.0f);
}

void out_parameters_box::resized()
{
	const int delta = 5;
	auto rectangle = getLocalBounds();

	auto labels_slice = rectangle.removeFromTop(50).reduced(delta);

	// int height = rectangle.getHeight();
	const int width = rectangle.getWidth();

	const int number_of_slots = 5;
	const int slot_width = width / number_of_slots;

	volume_out_slider_.setBounds(rectangle.removeFromLeft(slot_width).reduced(delta));
	volume_out_label_.setBounds(labels_slice.removeFromLeft(slot_width).reduced(delta));

	frequency_out_slider_.setBounds(rectangle.removeFromLeft(slot_width).reduced(delta));
	frequency_out_label_.setBounds(labels_slice.removeFromLeft(slot_width).reduced(delta));
	frequency_out_slider_.setTextBoxStyle(Slider::TextBoxBelow, true, slot_width, 20);
}

void out_parameters_box::set_slider_value(parameter_type parameter, float value)
{
	switch (parameter)
	{
	case volume:
		volume_out_slider_.setValue(value);
		break;
	case frequency:
		frequency_out_slider_.setValue(value);
		break;
	default: ;
	}

}

void out_parameters_box::set_slider_range(parameter_type parameter, int new_min, int new_max)
{
	switch (parameter)
	{
	case volume:
		break;
	case frequency:
		frequency_out_slider_.setRange(new_min, new_max);
		break;
	default: ;
	}
}
