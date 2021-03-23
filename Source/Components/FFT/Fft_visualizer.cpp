/*
  ==============================================================================

    Fft_visualizer.cpp
    Created: 23 Mar 2021 10:42:12am
    Author:  arnau

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Fft_visualizer.h"

//==============================================================================
Fft_visualizer::Fft_visualizer()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

Fft_visualizer::~Fft_visualizer()
{
}

void Fft_visualizer::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("Fft_visualizer", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void Fft_visualizer::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
