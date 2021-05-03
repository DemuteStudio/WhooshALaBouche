#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "../../WhooshGenerator/Source/Components/ParametersBox.h"
#include "../../WhooshGenerator/Source/Components/AudioWaveformComponent.h"
#include "../../WhooshGenerator/Source/Components/MyLookAndFeel.h"
#include "../../WhooshGenerator/Source/Components/OutParameterBox.h"
#include "../../WhooshGenerator/Source/Components/VolumeAnalyzer.h"
#include "../../WhooshGenerator/Source/Components/SpectrumComponent.h"
#include "../../WhooshGenerator/Source/Components/SampleSelector.h"

using namespace juce;

//==============================================================================
/**
*/
class WhooshGeneratorAudioProcessorEditor : public juce::AudioProcessorEditor, private Timer
{
public:
	WhooshGeneratorAudioProcessorEditor(WhooshGeneratorAudioProcessor&);
	~WhooshGeneratorAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;

	static int get_number_of_blocks_from_milliseconds(double sample_rate, float length_in_milliseconds,
	                                                  int samples_per_block);

	int number_of_samples_to_display = 0;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	WhooshGeneratorAudioProcessor& audioProcessor;

	ParametersBox in_parameters_box_;

	void enableRecording();

	void timerCallback() override;

	AudioSampleBuffer* audioBuffer;

	//==================================================
	AudioWaveformComponent waveform;
	OutParametersBox out_parameters_box_;
	SampleSelector sample_selector_;

	//==================================================

	MyAudioSource* audio_source;
	MyLookAndFeel my_look_and_feel_;

	//==================================================
	std::vector<Component*> components;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WhooshGeneratorAudioProcessorEditor)
};
