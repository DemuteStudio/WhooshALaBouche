#include "OutParameterBox.h"

#include "Util.h"

//==============================================================================
OutParametersBox::OutParametersBox(AudioProcessorValueTreeState* output_parameters, AudioProcessorValueTreeState* internal_parameters):
	volume_out("volume_out", "VOLUME", util::parameter_type::VOLUME),
	frequency_out("frequency_peak", "FREQUENCY PEAK", util::parameter_type::FREQUENCY_PEAK)
{
	parameters_components = {&volume_out, &frequency_out};
	for (std::vector<parameter_gui_component*>::value_type parameters_component : parameters_components)
	{
		addAndMakeVisible(parameters_component);
	}

	volume_out_attachment_ = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*output_parameters, parameters::volume_out.id, *volume_out.slider);
	frequency_out_attachment_ = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*output_parameters, parameters::frequency_out.id, *frequency_out.slider);

	analyze_on_pause_attachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(
		*internal_parameters, parameters::analyze_on_pause.id, analyze_on_pause_button);

	volume_out.slider->textFromValueFunction = [](double value)-> String
	{
		const int value_in_db = util::gain_to_decibels(value);
		return std::to_string(value_in_db) + " dB";
	};

	frequency_out.slider->textFromValueFunction = [](double value)-> String
	{
		return std::to_string(static_cast<int>(value)) + " Hz";
	};

	//TODO: Clean This
	addAndMakeVisible(show_values_button);
	show_values_button.setButtonText("SHOW VALUES");
	show_values_button.addListener(this);

	addAndMakeVisible(analyze_on_pause_button);
	analyze_on_pause_button.setButtonText("ANALYZE ON PAUSE");
	analyze_on_pause_button.addListener(this);
}

OutParametersBox::~OutParametersBox()
{
}

void OutParametersBox::paint(juce::Graphics& g)
{
	/* This demo code just fills the component's background and
	   draws some placeholder text to get you started.

	   You should replace everything in this method with your own
	   drawing code..
	*/

	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId)); // clear the background

	g.setColour(juce::Colours::grey);
	g.drawRect(getLocalBounds(), 1); // draw an outline around the component

}

void OutParametersBox::resized()
{
	auto sliders_rectangle = getLocalBounds();
	auto buttons_rectangle = sliders_rectangle.removeFromLeft(100);

	const int width = sliders_rectangle.getWidth();

	const int number_of_slots = 5;
	const int slot_width = width / number_of_slots;

	volume_out.setBounds(sliders_rectangle.removeFromLeft(slot_width));
	frequency_out.setBounds(sliders_rectangle.removeFromLeft(slot_width));

	const int delta = 5;
	show_values_button.setBounds(buttons_rectangle.removeFromBottom(30).reduced(delta));
	analyze_on_pause_button.setBounds(buttons_rectangle.removeFromBottom(30).reduced(delta));
}

void OutParametersBox::set_slider_value(util::parameter_type parameter, float value) const
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

void OutParametersBox::buttonClicked(Button* button)
{
	if (button == &show_values_button)
	{
		for (std::vector<parameter_gui_component*>::value_type parameter_component : parameters_components)
		{
			parameter_component->show_values = (parameter_component->show_values) ? false : true;
			parameter_component->resized();
		}
	}
	else if(button == &analyze_on_pause_button)
	{
		
	}
}

void OutParametersBox::parameter_gui_component::resized()
{
	auto rectangle = getLocalBounds();

	const int delta = 5;

	rectangle.removeFromTop(delta);
	label.setBounds(rectangle.removeFromTop(11).reduced(delta / 2));
	slider->setBounds(rectangle.reduced(delta));
	if (show_values)
	{
		slider->setTextBoxStyle(Slider::TextBoxBelow, true, rectangle.getWidth(), 20);
	}
	else
	{
		slider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
	}
}
