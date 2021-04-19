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
#include "Components/OutParametersState.h"
#include "Components/InParametersState.h"
#include "Components/VolumeAnalyzer.h"
#include "Components/SpectrumAnalyzer.h"
#include "Components/OutputTimer.h"
#include "Components/GainProcess.h"


using namespace juce;


//==============================================================================
/**
*/
class WhooshGeneratorAudioProcessor : public juce::AudioProcessor, private OutputTimer
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


	// std::unique_ptr<envelope> rms_envelope;
	// std::unique_ptr<envelope> rms_envelope_clean;
	//==============================================================================
	OutParametersState out_parameters;
	InParametersState in_parameters;

	//==============================================================================
	SpectrumAnalyzer* get_spectrum_analyzer();

private:
	my_audio_source audioSource;

	std::unique_ptr<VolumeAnalyzer> volume_analyzer_;
	std::unique_ptr<SpectrumAnalyzer> spectrum_analyzer;

	std::unique_ptr<GainProcess> gain_process_;

	std::list<fx_chain_element*> sidechain_input_processing_chain;
	std::list<fx_chain_element*> input_processing_chain;

	std::vector<Analyzer*> analyzers;


	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WhooshGeneratorAudioProcessor)
};
