#include <JuceHeader.h>
#include "out_parameters_box.h"

#include "Util.h"

//==============================================================================
out_parameters_box::out_parameters_box(AudioProcessorValueTreeState* processor_state):
	volume_out("volume_out", "VOLUME", util::parameter_type::VOLUME),
	frequency_out("frequency_peak", "FREQUENCY PEAK", util::parameter_type::FREQUENCY_PEAK)
{

	addAndMakeVisible(volume_out);
	addAndMakeVisible(frequency_out);

	volume_out_attachment_ = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*processor_state, "volume", *volume_out.slider);
	frequency_out_attachment_ = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*processor_state, "frequency", *frequency_out.slider);

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
	auto rectangle = getLocalBounds();

	const int width = rectangle.getWidth();

	const int number_of_slots = 5;
	const int slot_width = width / number_of_slots;

	volume_out.setBounds(rectangle.removeFromLeft(slot_width));
	frequency_out.setBounds(rectangle.removeFromLeft(slot_width));

}

void out_parameters_box::set_slider_value(util::parameter_type parameter, float value)
{
	switch (parameter)
	{
	case util::VOLUME:
		volume_out.slider->setValue(value);
		break;
	case util::FREQUENCY_PEAK:
		frequency_out.slider->setValue(value);
		break;
	default: ;
	}

}

void out_parameters_box::parameter_gui_component::resized()
{
	auto rectangle = getLocalBounds();

	const int delta = 5;

	label.setBounds(rectangle.removeFromTop(50).reduced(delta));
	slider->setBounds(rectangle.reduced(delta));
	slider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
}
