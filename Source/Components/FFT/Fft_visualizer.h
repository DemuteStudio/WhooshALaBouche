/*
  ==============================================================================

    Fft_visualizer.h
    Created: 23 Mar 2021 10:42:12am
    Author:  arnau

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../PluginProcessor.h"

//==============================================================================
/*
*/
class Fft_visualizer  : public juce::Component
{
public:
    Fft_visualizer(WhooshGeneratorAudioProcessor* processor);
    ~Fft_visualizer() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void drawNextFrameOfSpectrum();
    void drawFrame(juce::Graphics& g);
private:

	static int const scope_size = 512;
	std::array<float, scope_size> scopeData;

	WhooshGeneratorAudioProcessor* processor_;
	juce::dsp::WindowingFunction<float> window;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Fft_visualizer)
};
