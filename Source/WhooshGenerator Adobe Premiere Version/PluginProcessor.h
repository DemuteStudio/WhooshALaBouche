/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "../Components/RecordingBuffer.h"
#include "../Components/AudioChainElement.h"
#include "../Components/Util.h"
#include "../Components/OutParametersState.h"
#include "../Components/InParametersState.h"
#include "../Components/VolumeAnalyzer.h"
#include "../Components/SpectrumAnalyzer.h"
#include "../Components/OutputTimer.h"
#include "../Components/GainProcess.h"
#include "../Components/InternParametersState.h"
#include "../Components/FoleyInput.h"


using namespace juce;


//==============================================================================
/**
*/
class WhooshGeneratorAudioProcessor : public AudioProcessor
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
	void add_element_to_fx_chain(AudioChainElement* element);
	void remove_element_to_fx_chain(AudioChainElement* element);

	//==============================================================================
	RecordingBuffer& getAudioSource();
	//==============================================================================

	//==============================================================================
	[[nodiscard]] ParametersState* get_in_parameters() const;
	[[nodiscard]] ParametersState* get_intern_parameters() const;
	[[nodiscard]] ParametersState* get_out_parameters() const;
	[[nodiscard]] FoleyInput* get_internal_foley_input() const;

private:
	std::unique_ptr<OutParametersState> out_parameters_;
	std::unique_ptr<InParametersState> in_parameters_;
	std::unique_ptr<InternParametersState> intern_parameters_;

private:
	//==============================================================================
	RecordingBuffer audioSource;
	//==============================================================================
	std::unique_ptr<VolumeAnalyzer> volume_analyzer_;

	std::unique_ptr<GainProcess> gain_processor_;

	std::unique_ptr<FoleyInput> internal_foley_input_;
	//==============================================================================
	std::list<AudioChainElement*> sidechain_input_processing_chain_;
	std::list<AudioChainElement*> input_processing_chain_;

	std::vector<Analyzer*> analyzers_;

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WhooshGeneratorAudioProcessor)
};
