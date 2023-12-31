#include "ParametersBox.h"

#include <JuceHeader.h>


ParametersBox::ParametersBox(AudioProcessor* processor, AudioProcessorValueTreeState* parameters_state, int fft_size):
	threshold(util::Parameter{parameters::threshold, parameters_state}),
	frequency_band({
		               util::Parameter{parameters::min_frequency, parameters_state},
		               util::Parameter{parameters::max_frequency, parameters_state}
	               }, parameters::frequency_band),
	frequency_variation_speed(util::Parameter(parameters::frequency_speed, parameters_state)),
	volume_variation_speed(util::Parameter(parameters::volume_speed, parameters_state)),
	processor(processor), parameters_state(parameters_state)
{
	const double samples_per_block = processor->getBlockSize();
	const double sample_rate = processor->getSampleRate();
	frequency_interval = (1. / fft_size) * sample_rate;

	double time_per_block = (samples_per_block / sample_rate);

	parameter_guis = {
		&threshold, &frequency_band, &frequency_variation_speed, &volume_variation_speed
	};
	one_parameter_guis = {
		&threshold, &frequency_variation_speed, &volume_variation_speed
	};

	for (auto* parameter : parameter_guis)
	{
		addAndMakeVisible(parameter);
	}
	link_sliders_to_parameters();
	set_parameters_value_to_text();

	frequency_band.slider->onValueChange = [this, parameters_state]()-> void
	{
		RangedAudioParameter* min_frequency_parameter = parameters_state->getParameter(parameters::min_frequency.id);
		const int min_frequency = min_frequency_parameter->convertFrom0to1(
			min_frequency_parameter->getValue()) * frequency_interval;

		RangedAudioParameter* max_frequency_parameter = parameters_state->getParameter(parameters::max_frequency.id);
		const int max_frequency = max_frequency_parameter->convertFrom0to1(
			max_frequency_parameter->getValue()) * frequency_interval;

		frequency_band.value.setText(std::to_string(min_frequency) + " / " + std::to_string(max_frequency) + " Hz",
		                             NotificationType::dontSendNotification);
	};
}

ParametersBox::~ParametersBox()
{
	sliders_attachment.clear();
	two_values_sliders_attachment.clear();
}

void ParametersBox::paint(juce::Graphics& g)
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

void ParametersBox::resized()
{
	Rectangle<int> rectangle = getLocalBounds();
	const int height = rectangle.getHeight();
	int width = rectangle.getWidth();

	const int slider_height = height / 5;

	threshold.setBounds(rectangle.removeFromTop(slider_height));
	frequency_band.setBounds(rectangle.removeFromTop(slider_height));
	frequency_variation_speed.setBounds(rectangle.removeFromTop(slider_height));
	volume_variation_speed.setBounds(rectangle.removeFromTop(slider_height));
}

void ParametersBox::add_sliders_listener(Slider::Listener* listener) const
{
	threshold.slider->addListener(listener);
	frequency_band.slider->addListener(listener);
	frequency_variation_speed.slider->addListener(listener);
	volume_variation_speed.slider->addListener(listener);
}


void ParametersBox::ParameterGuiComponent::resized()
{
	auto rectangle = getLocalBounds();

	label.setBounds(rectangle.removeFromLeft(150));
	slider->setBounds(rectangle);
	slider->setTextBoxStyle(Slider::TextBoxRight, true, 150, rectangle.getHeight());
}

void ParametersBox::TwoValuesParameterGuiComponent::resized()
{
	auto rectangle = getLocalBounds();

	label.setBounds(rectangle.removeFromLeft(150));
	value.setBounds(rectangle.removeFromRight(150));
	slider->setBounds(rectangle);
	slider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
}

//==============================================================================
void ParametersBox::link_sliders_to_parameters()
{
	sliders_attachment.push_back(std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*parameters_state, parameters::threshold.id, *threshold.slider));
	sliders_attachment.push_back(std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*parameters_state, parameters::frequency_speed.id, *frequency_variation_speed.slider));
	sliders_attachment.push_back(std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*parameters_state, parameters::volume_speed.id, *volume_variation_speed.slider));
	two_values_sliders_attachment.push_back(std::make_unique<TwoValueSliderAttachment>(
		*parameters_state, parameters::min_frequency.id, parameters::max_frequency.id, *frequency_band.slider));
}

void ParametersBox::set_parameters_value_to_text() const
{
	threshold.slider->textFromValueFunction = [](double value)-> String
	{
		const int value_in_db = util::gain_to_decibels(value);
		return std::to_string(value_in_db) + " dB";
	};

	frequency_variation_speed.slider->textFromValueFunction = [](double value)-> String
	{
		return std::to_string((int)(value * 100)) + " %";
	};
	volume_variation_speed.slider->textFromValueFunction = [](double value)-> String
	{
		return std::to_string((int)(value * 100)) + " %";
	};
}

