/*
  ==============================================================================

    SampleComponent.h
    Created: 26 Apr 2021 3:42:57pm
    Author:  arnau

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class SampleComponent  : public juce::Component
{
public:
    SampleComponent(juce::File sample_file);
    ~SampleComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    juce::String name;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleComponent)
};
