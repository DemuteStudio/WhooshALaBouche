#include <JuceHeader.h>
#include "ParametersBox.h"

//==============================================================================
ParametersBox::ParametersBox(double samples_per_block)
{
	threshold_slider = std::make_unique<Slider>();
	threshold_slider->setSliderStyle(Slider::LinearHorizontal);
	addAndMakeVisible(threshold_slider.get());
	threshold_slider->setName("threshold");
	threshold_slider->setRange(0.0, 0.5);

	addAndMakeVisible(threshold_label);
	threshold_label.setText("Threshold", NotificationType::dontSendNotification);


	rms_length_slider = std::make_unique<Slider>();
	rms_length_slider->setSliderStyle(Slider::LinearHorizontal);
	addAndMakeVisible(rms_length_slider.get());
	rms_length_slider->setName("rms_length");
	rms_length_slider->setRange(20.0, samples_per_block*10);

	addAndMakeVisible(rms_length_label);
	rms_length_label.setText("RMS Length", NotificationType::dontSendNotification);
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
	threshold_slider->setBounds(threshold_rectangle);
	threshold_slider->setTextBoxStyle(Slider::TextBoxRight, true, 150, slider_height);

	
	auto rms_rectangle = rectangle.removeFromTop(slider_height);
	rms_length_label.setBounds(rms_rectangle.removeFromLeft(150));
	rms_length_slider->setBounds(rms_rectangle);
	rms_length_slider->setTextBoxStyle(Slider::TextBoxRight, true, 150, slider_height);

}

void ParametersBox::add_listener(Slider::Listener* listener) const
{
	threshold_slider->addListener(listener);
	rms_length_slider->addListener(listener);
}


