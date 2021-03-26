#include "ParametersBox.h"

#include <JuceHeader.h>

//==============================================================================
ParametersBox::ParametersBox(WhooshGeneratorAudioProcessor* processor, int fft_size)
{
	const double samples_per_block = processor->getBlockSize();
	const double sample_rate = processor->sample_rate;

	double time_per_block = (samples_per_block / sample_rate);

	//threshold
	threshold_slider = std::make_unique<Slider>();
	threshold_slider->setSliderStyle(Slider::LinearHorizontal);
	addAndMakeVisible(threshold_slider.get());
	threshold_slider->setName("threshold");
	// threshold_slider->setRange(0.0, 0.5);

	addAndMakeVisible(threshold_label);
	threshold_label.setText("Threshold", NotificationType::dontSendNotification);

	addAndMakeVisible(threshold_value_label);
	threshold_value_label.setJustificationType(Justification::centred);
	const int threshold_value_db = Decibels::gainToDecibels(0.);
	threshold_value_label.setText(std::to_string(threshold_value_db) + " dB", dontSendNotification);

	//rms_length
	rms_length_slider = std::make_unique<Slider>();
	rms_length_slider->setSliderStyle(Slider::LinearHorizontal);
	addAndMakeVisible(rms_length_slider.get());
	rms_length_slider->setName("rms_length");
	// rms_length_slider->setRange(1.0, 10.);

	addAndMakeVisible(rms_length_label);
	rms_length_label.setText("RMS Length", NotificationType::dontSendNotification);

	addAndMakeVisible(rms_length_value_label);
	rms_length_value_label.setJustificationType(Justification::centred);
	const int rms_length_value = ((samples_per_block / sample_rate) * 1000.);
	rms_length_value_label.setText(std::to_string(rms_length_value) + " ms", dontSendNotification);

	//fft_order
	// fft_order_slider = std::make_unique<Slider>();
	// fft_order_slider->setSliderStyle(Slider::LinearHorizontal);
	// addAndMakeVisible(fft_order_slider.get());
	// fft_order_slider->setName("fft");
	// fft_order_slider->setRange(1.0, 10.);
	//
	// addAndMakeVisible(fft_order_label);
	// fft_order_label.setText("FFT", NotificationType::dontSendNotification);
	//
	// addAndMakeVisible(fft_order_value_label);
	// fft_order_value_label.setJustificationType(Justification::centred);
	// fft_order_value_label.setText(std::to_string(fft_size), dontSendNotification);

	//frequency band ========================================================================================================================================================================================================================================================================================================================
	frequency_band_slider = std::make_unique<Slider>();
	frequency_band_slider->setSliderStyle(Slider::TwoValueHorizontal);
	addAndMakeVisible(frequency_band_slider.get());
	frequency_band_slider->setName("frequency_band");

	const float frequency_step = processor->sample_rate / fft_size;
	const int minimum_frequency = 50;
	int maximum_frequency = processor->sample_rate / 2;

	const int minimum_index = minimum_frequency / frequency_step;
	const int maximum_index = maximum_frequency / frequency_step;


	frequency_band_slider->setRange(minimum_index, maximum_index);
	frequency_band_slider->setMinAndMaxValues(minimum_index, maximum_index);
	frequency_band_slider->setSkewFactorFromMidPoint(1000. / frequency_step);

	addAndMakeVisible(frequency_band_label);
	frequency_band_label.setText("FREQUENCY BAND", NotificationType::dontSendNotification);

	addAndMakeVisible(frequency_band_value_label);
	frequency_band_value_label.setJustificationType(Justification::centred);
	frequency_band_value_label.setText(
		std::to_string(frequency_band_slider->getMinValue()) + " / " + std::to_string(
			frequency_band_slider->getMaxValue()), dontSendNotification);


	//FFT Peak variation Speed========================================================================================================================================================================================================================================================================================================================
	frequency_variation_speed_slider = std::make_unique<Slider>();
	frequency_variation_speed_slider->setSliderStyle(Slider::LinearHorizontal);
	addAndMakeVisible(frequency_variation_speed_slider.get());
	frequency_variation_speed_slider->setName("fft_variation_speed");

	frequency_variation_speed_slider->setRange(0., 1.);
	frequency_variation_speed_slider->setValue(1.);

	addAndMakeVisible(frequency_variation_speed_label);
	frequency_variation_speed_label.setText("FFT SPEED", NotificationType::dontSendNotification);

	addAndMakeVisible(frequency_variation_speed__value_label);
	frequency_variation_speed__value_label.setJustificationType(Justification::centred);
	frequency_variation_speed__value_label.setText(
		std::to_string((int)frequency_variation_speed_slider->getValue()*100) + " % ", dontSendNotification);

	//Volume variation Speed========================================================================================================================================================================================================================================================================================================================
	volume_variation_speed_slider = std::make_unique<Slider>();
	volume_variation_speed_slider->setSliderStyle(Slider::LinearHorizontal);
	addAndMakeVisible(volume_variation_speed_slider.get());
	volume_variation_speed_slider->setName("volume_variation_speed");

	volume_variation_speed_slider->setRange(0., 1.);
	volume_variation_speed_slider->setValue(1.);

	addAndMakeVisible(volume_variation_speed_label);
	volume_variation_speed_label.setText("VOLUME SPEED", NotificationType::dontSendNotification);

	addAndMakeVisible(volume_variation_speed__value_label);
	volume_variation_speed__value_label.setJustificationType(Justification::centred);
	volume_variation_speed__value_label.setText(
		std::to_string((int)volume_variation_speed_slider->getValue()*100) + " % ", dontSendNotification);



	sliders_attachment_.push_back( std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*processor->get_in_state(), "threshold", *threshold_slider));
	sliders_attachment_.push_back( std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*processor->get_in_state(), "rms_length", *rms_length_slider));
	sliders_attachment_.push_back( std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*processor->get_in_state(), "fft_speed", *frequency_variation_speed_slider));
	sliders_attachment_.push_back( std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*processor->get_in_state(), "volume_speed", *volume_variation_speed_slider));

}

ParametersBox::~ParametersBox()
{
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

	auto threshold_rectangle = rectangle.removeFromTop(slider_height);
	threshold_label.setBounds(threshold_rectangle.removeFromLeft(150));
	threshold_value_label.setBounds(threshold_rectangle.removeFromRight(150));
	threshold_slider->setBounds(threshold_rectangle);
	// threshold_slider->setTextBoxStyle(Slider::TextBoxRight, true, 150, slider_height);
	threshold_slider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);

	auto rms_rectangle = rectangle.removeFromTop(slider_height);
	rms_length_label.setBounds(rms_rectangle.removeFromLeft(150));
	rms_length_value_label.setBounds(rms_rectangle.removeFromRight(150));
	rms_length_slider->setBounds(rms_rectangle);
	// rms_length_slider->setTextBoxStyle(Slider::TextBoxRight, true, 150, slider_height);
	rms_length_slider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);


	// auto fft_rectangle = rectangle.removeFromTop(slider_height);
	// fft_order_label.setBounds(fft_rectangle.removeFromLeft(150));
	// fft_order_value_label.setBounds(fft_rectangle.removeFromRight(150));
	// fft_order_slider->setBounds(fft_rectangle);
	// fft_order_slider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);


	auto frequency_band_rectangle = rectangle.removeFromTop(slider_height);
	frequency_band_label.setBounds(frequency_band_rectangle.removeFromLeft(150));
	frequency_band_value_label.setBounds(frequency_band_rectangle.removeFromRight(150));
	frequency_band_slider->setBounds(frequency_band_rectangle);
	frequency_band_slider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);


	auto fft_speed_rectangle = rectangle.removeFromTop(slider_height);
	frequency_variation_speed_label.setBounds(fft_speed_rectangle.removeFromLeft(150));
	frequency_variation_speed__value_label.setBounds(fft_speed_rectangle.removeFromRight(150));
	frequency_variation_speed_slider->setBounds(fft_speed_rectangle);
	frequency_variation_speed_slider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);

	auto volume_speed_rectangle = rectangle.removeFromTop(slider_height);
	volume_variation_speed_label.setBounds(volume_speed_rectangle.removeFromLeft(150));
	volume_variation_speed__value_label.setBounds(volume_speed_rectangle.removeFromRight(150));
	volume_variation_speed_slider->setBounds(volume_speed_rectangle);
	volume_variation_speed_slider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
}

void ParametersBox::add_listener(Slider::Listener* listener) const
{
	threshold_slider->addListener(listener);
	rms_length_slider->addListener(listener);
	frequency_band_slider->addListener(listener);
	frequency_variation_speed_slider->addListener(listener);
	volume_variation_speed_slider->addListener(listener);
}
