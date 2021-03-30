#include "ParametersBox.h"

#include <JuceHeader.h>

//==============================================================================
ParametersBox::ParametersBox(AudioProcessor* processor, AudioProcessorValueTreeState* parameters_state, int fft_size):
	threshold("threshold", "THRESHOLD", parameter_type::THRESHOLD),
	rms_length("rms_length","RMS LENGTH", parameter_type::RMS_LENGTH),
	fft_order("fft_order","FFT ORDER",parameter_type::FFT_ORDER ),
	frequency_band("frequency_band","FREQUENCY BAND", parameter_type::FREQUENCY_BAND,Slider::TwoValueHorizontal),
	frequency_variation_speed("frequency_variation_speed","FREQUENCY SPEED", parameter_type::FREQUENCY_VARIATION_SPEED),
	volume_variation_speed("volume_variation_speed","VOLUME SPEED", parameter_type::VOLUME_VARIATION_SPEED)

{
	const double samples_per_block = processor->getBlockSize();
	const double sample_rate = processor->getSampleRate();

	double time_per_block = (samples_per_block / sample_rate);


	addAndMakeVisible(threshold);
	addAndMakeVisible(rms_length);
	addAndMakeVisible(fft_order);
	addAndMakeVisible(frequency_band);
	addAndMakeVisible(frequency_variation_speed);
	addAndMakeVisible(volume_variation_speed);

	threshold.slider->setValue(0);
	rms_length.slider->setValue(0);

	const float frequency_step = sample_rate / fft_size;
	const int minimum_frequency = 50;
	int maximum_frequency = sample_rate / 2;
	
	const int minimum_index = minimum_frequency / frequency_step;
	const int maximum_index = maximum_frequency / frequency_step;
	
	
	frequency_band.slider->setRange(minimum_index, maximum_index);
	frequency_band.slider->setMinAndMaxValues(minimum_index, maximum_index);
	frequency_band.slider->setSkewFactorFromMidPoint(1000. / frequency_step);
	
	
	// frequency_band.value_label.setText(
	// 	std::to_string(frequency_band.slider->getMinValue()) + " / " + std::to_string(
	// 		frequency_band.slider->getMaxValue()), dontSendNotification);
	//
	//
	// //FFT Peak variation Speed========================================================================================================================================================================================================================================================================================================================
	// frequency_variation_speed_slider = std::make_unique<Slider>();
	// frequency_variation_speed_slider->setSliderStyle(Slider::LinearHorizontal);
	// addAndMakeVisible(frequency_variation_speed_slider.get());
	// frequency_variation_speed_slider->setName("fft_variation_speed");
	//
	// frequency_variation_speed_slider->setRange(0., 1.);
	// frequency_variation_speed_slider->setValue(1.);
	//
	// addAndMakeVisible(frequency_variation_speed_label);
	// frequency_variation_speed_label.setText("FFT SPEED", NotificationType::dontSendNotification);
	//
	// addAndMakeVisible(frequency_variation_speed__value_label);
	// frequency_variation_speed__value_label.setJustificationType(Justification::centred);
	// frequency_variation_speed__value_label.setText(
	// 	std::to_string((int)frequency_variation_speed_slider->getValue() * 100) + " % ", dontSendNotification);
	//
	// //Volume variation Speed========================================================================================================================================================================================================================================================================================================================
	// volume_variation_speed_slider = std::make_unique<Slider>();
	// volume_variation_speed_slider->setSliderStyle(Slider::LinearHorizontal);
	// addAndMakeVisible(volume_variation_speed_slider.get());
	// volume_variation_speed_slider->setName("volume_variation_speed");
	//
	// volume_variation_speed_slider->setRange(0., 1.);
	// volume_variation_speed_slider->setValue(1.);
	//
	// addAndMakeVisible(volume_variation_speed_label);
	// volume_variation_speed_label.setText("VOLUME SPEED", NotificationType::dontSendNotification);
	//
	// addAndMakeVisible(volume_variation_speed__value_label);
	// volume_variation_speed__value_label.setJustificationType(Justification::centred);
	// volume_variation_speed__value_label.setText(
	// 	std::to_string((int)volume_variation_speed_slider->getValue() * 100) + " % ", dontSendNotification);


	sliders_attachment_.push_back(std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*parameters_state, "threshold", *threshold.slider));
	sliders_attachment_.push_back(std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*parameters_state, "rms_length", *rms_length.slider));
	sliders_attachment_.push_back(std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*parameters_state, "fft_speed", *frequency_variation_speed.slider));
	sliders_attachment_.push_back(std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		*parameters_state, "volume_speed", *volume_variation_speed.slider));
}

ParametersBox::~ParametersBox()
{
	sliders_attachment_.clear();
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
	rms_length.setBounds(rectangle.removeFromTop(slider_height));
	frequency_band.setBounds(rectangle.removeFromTop(slider_height));
	frequency_variation_speed.setBounds(rectangle.removeFromTop(slider_height));
	volume_variation_speed.setBounds(rectangle.removeFromTop(slider_height));
}

void ParametersBox::add_listener(Slider::Listener* listener) const
{
	threshold.slider->addListener(listener);
	rms_length.slider->addListener(listener);
	frequency_band.slider->addListener(listener);
	frequency_variation_speed.slider->addListener(listener);
	volume_variation_speed.slider->addListener(listener);
}

ParametersBox::parameter_gui::parameter_gui(const String id, const String text,  parameter_type type)
{
	slider = std::make_unique<Slider>();
	slider->setSliderStyle(Slider::LinearHorizontal);
	addAndMakeVisible(slider.get());
	slider->setName(id);

	addAndMakeVisible(label);

	label.setText(text, NotificationType::dontSendNotification);

	addAndMakeVisible(value_label);
	value_label.setJustificationType(Justification::centred);

	slider->setValue(100);

	parameter_type_ = type;
}

ParametersBox::parameter_gui::parameter_gui(const String id, const String text, parameter_type type,
                                            const Slider::SliderStyle style): parameter_gui(id,text, type)
{
	slider->setSliderStyle(style);
}
void ParametersBox::parameter_gui::resized()
{
	
	auto rectangle = getLocalBounds();
	label.setBounds(rectangle.removeFromLeft(150));
	value_label.setBounds(rectangle.removeFromRight(150));
	slider->setBounds(rectangle);
	slider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
}

