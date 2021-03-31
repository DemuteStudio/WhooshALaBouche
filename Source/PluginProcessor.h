/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Components/myAudioSource.h"
#include "Components/FxChainElement.h"
#include "Components/Util.h"

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
	void add_element_to_fx_chain(fx_chain_element* element);
	void remove_element_to_fx_chain(fx_chain_element* element);

	//==============================================================================
	my_audio_source& getAudioSource();
	float get_last_rms_value_in_db();

	double sample_rate;
	float last_rms_value = 1.0;
	float samples_squares_sum = 0.0;
	int block_index = 0;
	int sample_index = 0;
	float temp_previous_value = 0.;
	bool is_rms_different = true;


	float threshold_value = 0.0;
	float rms_blocks_length = 1;
	float new_rms_value;
	float variation_speed = 1.;

	// std::unique_ptr<envelope> rms_envelope;
	// std::unique_ptr<envelope> rms_envelope_clean;
	//==============================================================================
	AudioProcessorValueTreeState* get_out_state();
	AudioProcessorValueTreeState* get_in_state();
	AudioProcessorValueTreeState::ParameterLayout create_out_parameters() const;
	AudioProcessorValueTreeState::ParameterLayout create_in_parameters() const;

private:
	my_audio_source audioSource;

	std::list<fx_chain_element*> fx_chain;

	std::unique_ptr<AudioProcessorValueTreeState > out_state_;
	std::unique_ptr<AudioProcessorValueTreeState > in_state_;
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WhooshGeneratorAudioProcessor)
};
