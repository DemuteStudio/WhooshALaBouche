#include <JuceHeader.h>
#include "ParametersBox.h"

//==============================================================================
ParametersBox::ParametersBox(double samples_per_block, double sample_rate)
{
	double time_per_block = (samples_per_block / sample_rate);
	threshold_slider = std::make_unique<Slider>();
	threshold_slider->setSliderStyle(Slider::LinearHorizontal);
	addAndMakeVisible(threshold_slider.get());
	threshold_slider->setName("threshold");
	threshold_slider->setRange(0.0, 0.5);
	threshold_slider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);

	addAndMakeVisible(threshold_label);
	threshold_label.setText("Threshold", NotificationType::dontSendNotification);

	addAndMakeVisible(threshold_value_label);
	threshold_value_label.setText("-Inf dB", NotificationType::dontSendNotification);


	rms_length_slider = std::make_unique<Slider>();
	rms_length_slider->setSliderStyle(Slider::LinearHorizontal);
	addAndMakeVisible(rms_length_slider.get());
	rms_length_slider->setName("rms_length");
<<<<<<< Updated upstream
	rms_length_slider->setRange(samples_per_block*2000, samples_per_block*10000);
	rms_length_slider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
=======
	rms_length_slider->setRange(20.0, 200.);
>>>>>>> Stashed changes

	addAndMakeVisible(rms_length_label);
	rms_length_label.setText("RMS Length", NotificationType::dontSendNotification);
	addAndMakeVisible(rms_length_value_label);
	rms_length_value_label.setText((String)rms_length_slider->getValue()+ " ms", NotificationType::dontSendNotification);
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

	const int slider_height = 50;

	auto threshold_rectangle = rectangle.removeFromTop(slider_height);
	threshold_label.setBounds(threshold_rectangle.removeFromLeft(150));
	threshold_value_label.setBounds(threshold_rectangle.removeFromRight(150));
	threshold_slider->setBounds(threshold_rectangle);

	
	auto rms_rectangle = rectangle.removeFromTop(slider_height);
	rms_length_label.setBounds(rms_rectangle.removeFromLeft(150));
	rms_length_value_label.setBounds(rms_rectangle.removeFromRight(150));
	rms_length_slider->setBounds(rms_rectangle);

}

void ParametersBox::add_listener(Slider::Listener* listener) const
{
	threshold_slider->addListener(listener);
	rms_length_slider->addListener(listener);
}


