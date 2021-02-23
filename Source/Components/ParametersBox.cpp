/*
  ==============================================================================

    ParametersBox.cpp
    Created: 23 Feb 2021 3:47:28pm
    Author:  arnau

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ParametersBox.h"

//==============================================================================
ParametersBox::ParametersBox()
{
	threshold_slider = std::make_unique<Slider>();
	threshold_slider->setSliderStyle(Slider::LinearHorizontal);
	addAndMakeVisible(threshold_slider.get());
	threshold_slider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
	threshold_slider->setName("threshold");
	threshold_slider->addListener(this);

	addAndMakeVisible(threshold_label);
	threshold_label.setText("Threshold", NotificationType::dontSendNotification);
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

	int slider_height = 50;
	int label_height = 30;

	auto threshold_rectangle = rectangle.removeFromTop(slider_height);
	threshold_label.setBounds(threshold_rectangle.removeFromLeft(150));
	threshold_slider->setBounds(threshold_rectangle);

}

void ParametersBox::sliderValueChanged(Slider* slider)
{
	if (slider->getName() == "threshold")
	{
		threshold_value = slider->getValue();
	}
}
