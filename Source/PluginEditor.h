#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Components/Recorder.h"
#include "Components/ParametersBox.h"

using namespace juce;

//==============================================================================
/**
*/
class WhooshGeneratorAudioProcessorEditor : public juce::AudioProcessorEditor, public Slider::Listener, private Timer
{
public:
	WhooshGeneratorAudioProcessorEditor(WhooshGeneratorAudioProcessor&);
	~WhooshGeneratorAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;

	void sliderValueChanged(Slider* slider) override;
	static int get_number_of_blocks_from_milliseconds(double sample_rate, float length_in_milliseconds, int samples_per_block);
	void onAudioSourceStateChange(my_audio_source::State state);

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	WhooshGeneratorAudioProcessor& audioProcessor;

	Recorder recorder_;
	ParametersBox parameters_box_;


	void openButtonClicked();

	void loadFile(AudioFormatReader* audioReader);

	void unloadFile();

	void recordButtonClicked();

	void enableRecording();

	void disableRecording();

	void loopButtonClicked();

	void timerCallback() override;

	AudioFormatManager formatManager;
	std::unique_ptr<AudioSampleBuffer> audioBuffer;

	AudioWaveformComponent waveform;
	AudioWaveformSelectedRegionComponent selectedRegion;
	AudioPlaybackPositionComponent playbackPosition;

	EnvelopeComponent envelope_;
	EnvelopeSelectedRegionComponent envelope_selected_region_;
	AudioPlaybackPositionComponent envelope_playback_position_;
	envelope* envelope_array_;
	

	AudioScrollerComponent scroller;

	my_audio_source* audio_source;

	TenFtLookAndFeel tenFtLookAndFeel;

	//==================================================
	OSCSender osc_sender_;

	void send_osc_message(String message);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WhooshGeneratorAudioProcessorEditor)
};
