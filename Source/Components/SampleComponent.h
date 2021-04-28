#pragma once

#include <JuceHeader.h>
#include "FoleyInput.h"

//==============================================================================
/*
*/
class SampleComponent  : public juce::TextButton
{
public:
    SampleComponent(const FoleyInput::FileAudioSource* sample);
    ~SampleComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void unselect();
    void select();

    const FoleyInput::FileAudioSource* file_audio_source;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleComponent)
};
