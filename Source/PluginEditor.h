#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Components/Recorder.h"

//==============================================================================
/**
*/
class WhooshGeneratorAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    WhooshGeneratorAudioProcessorEditor (WhooshGeneratorAudioProcessor&);
    ~WhooshGeneratorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WhooshGeneratorAudioProcessor& audioProcessor;


	Slider slider_noise_level_;
	Label label_level_;

	Recorder recorder_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WhooshGeneratorAudioProcessorEditor)
};
