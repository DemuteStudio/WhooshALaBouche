#include "ParametersBox.h"

#include <JuceHeader.h>


ParametersBox::ParametersBox(AudioProcessor* processor, AudioProcessorValueTreeState* parameters_state, int fft_size):
	threshold("threshold", "THRESHOLD", util::parameter_type::THRESHOLD),
	frequency_band("frequency_band", "FREQUENCY BAND", util::parameter_type::FREQUENCY_BAND,
	               Slider::TwoValueHorizontal),
	frequency_variation_speed("frequency_variation_speed", "FREQUENCY SPEED",
	                          util::parameter_type::FREQUENCY_VARIATION_SPEED),
	volume_variation_speed("volume_variation_speed", "VOLUME SPEED", util::parameter_type::VOLUME_VARIATION_SPEED),
	processor(processor), parameters_state(parameters_state)
{
	const double samples_per_block = processor->getBlockSize();
	const double sample_rate = processor->getSampleRate();

	double time_per_block = (samples_per_block / sample_rate);

	addAndMakeVisible(threshold);
	addAndMakeVisible(frequency_band);
	addAndMakeVisible(frequency_variation_speed);
	addAndMakeVisible(volume_variation_speed);

	link_sliders_to_parameters();
	set_parameters_value_to_text();
}

ParametersBox::~ParametersBox()
{
	sliders_attachment_.clear();
	two_values_sliders_attachment_.clear();
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


void ParametersBox::parameter_gui_component::resized()
{
	auto rectangle = getLocalBounds();

	label.setBounds(rectangle.removeFromLeft(150));
	slider->setBounds(rectangle);
	slider->setTextBoxStyle(Slider::TextBoxRight, true, 150, rectangle.getHeight());
}

//==============================================================================
void ParametersBox::link_sliders_to_parameters()
{
	sliders_attachment_.push_back(std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*parameters_state, parameters::threshold.id, *threshold.slider));
	sliders_attachment_.push_back(std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*parameters_state, parameters::frequency_speed.id, *frequency_variation_speed.slider));
	sliders_attachment_.push_back(std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*parameters_state, parameters::volume_speed.id, *volume_variation_speed.slider));
	two_values_sliders_attachment_.push_back(std::make_unique<TwoValueSliderAttachment>(
		*parameters_state, parameters::min_frequency.id, parameters::max_frequency.id, *frequency_band.slider));
}

void ParametersBox::set_parameters_value_to_text()
{
	threshold.slider->textFromValueFunction = [](double value)-> String
	{
		const int value_in_db = Decibels::gainToDecibels(value);
		return std::to_string(value_in_db) + " dB";
	};

	frequency_band.slider->textFromValueFunction = [this](double value)-> String
	{
		RangedAudioParameter* min_frequency_parameter = parameters_state->getParameter("min_frequency");
		const int min_frequency = min_frequency_parameter->convertFrom0to1(
			min_frequency_parameter->getValue());

		RangedAudioParameter* max_frequency_parameter = parameters_state->getParameter("max_frequency");
		const int max_frequency = min_frequency_parameter->convertFrom0to1(
			max_frequency_parameter->getValue());


		return std::to_string(min_frequency) + " / " + std::to_string(max_frequency)+ " Hz";
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
