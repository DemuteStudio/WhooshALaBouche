/*
  ==============================================================================

    Fft_visualizer.h
    Created: 23 Mar 2021 10:42:12am
    Author:  arnau

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class Fft_visualizer  : public juce::Component
{
public:
    Fft_visualizer();
    ~Fft_visualizer() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Fft_visualizer)
};
