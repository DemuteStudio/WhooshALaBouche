/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Components/myAudioSource.h"
#include "Components/FxChainElement.h"


using namespace juce;


//==============================================================================
/**
*/
class WhooshGeneratorAudioProcessor : public juce::AudioProcessor
{
public:
	//==============================================================================
	WhooshGeneratorAudioProcessor();
	~WhooshGeneratorAudioProcessor() override;

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void push_next_sample_into_fifo(const float x);
	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

	//==============================================================================
	juce::AudioProcessorEditor* createEditor() override;
	// bool hasEditor() const override;

	//==============================================================================
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String& newName) override;

	//==============================================================================
	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	bool hasEditor() const override;
	//==============================================================================
	my_audio_source& getAudioSource();
	int get_fft_mean_value();

	double sample_rate;
	float last_rms_value = 1.0;
	float samples_squares_sum = 0.0;
	int block_index = 0;
	int sample_index = 0;
	float temp_previous_value = 0.;
	bool is_rms_different = true;


	float threshold_value = 0.0;
	float rms_blocks_length = 1;

	// std::unique_ptr<envelope> rms_envelope;
	// std::unique_ptr<envelope> rms_envelope_clean;
	//==============================================================================
	void calculate_fft();
	int get_fft_peak();

	int get_min_frequency_fft_index() const;
	void set_min_frequency_fft_index(int min_frequency_fft_index);
	int get_max_frequency_fft_index() const;
	void set_max_frequency_fft_index(int max_frequency_fft_index);

	AudioProcessorValueTreeState* get_state();
	AudioProcessorValueTreeState::ParameterLayout create_parameters();

	static const int fft_order = 10;
	static const int fft_size = 1 << fft_order;

	std::list<fx_chain_element*> fx_chain;

private:
	my_audio_source audioSource;


	std::unique_ptr<AudioProcessorValueTreeState > state;
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WhooshGeneratorAudioProcessor)
};
