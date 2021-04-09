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

	parameters_components = {&volume_out, &frequency_out};

	volume_out_attachment_ = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*processor_state, "volume", *volume_out.slider);
	frequency_out_attachment_ = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*processor_state, "frequency", *frequency_out.slider);

	volume_out.slider->textFromValueFunction = [](double value)-> String
	{
		const int value_in_db = Decibels::gainToDecibels(value);
		return std::to_string(value_in_db) +  " dB";
	};

	frequency_out.slider->textFromValueFunction = [](double value)-> String
	{
		return std::to_string((int)value) +  " Hz";
	};
	addAndMakeVisible(show_values_button);
	show_values_button.setButtonText("SHOW VALUES");
	show_values_button.addListener(this);
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
	auto sliders_rectangle = getLocalBounds();
	auto buttons_rectangle = sliders_rectangle.removeFromLeft(100);

	const int width = sliders_rectangle.getWidth();

	const int number_of_slots = 5;
	const int slot_width = width / number_of_slots;

	volume_out.setBounds(sliders_rectangle.removeFromLeft(slot_width));
	frequency_out.setBounds(sliders_rectangle.removeFromLeft(slot_width));

	const int delta = 5;
	show_values_button.setBounds(buttons_rectangle.removeFromBottom(50).reduced(delta));
}

void out_parameters_box::set_slider_value(util::parameter_type parameter, float value) const
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

void out_parameters_box::buttonClicked(Button*)
{
	for (std::vector<parameter_gui_component*>::value_type parameter_component : parameters_components)
	{
		parameter_component->show_values = (parameter_component->show_values) ? false : true;
		parameter_component->resized();
	}
}

void out_parameters_box::parameter_gui_component::resized()
{
	auto rectangle = getLocalBounds();

	const int delta = 5;

	label.setBounds(rectangle.removeFromTop(50).reduced(delta));
	slider->setBounds(rectangle.reduced(delta));
	if (show_values)
	{
		slider->setTextBoxStyle(Slider::TextBoxBelow, true, rectangle.getWidth(), 30);
	}
	else
	{
		slider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
	}
}
