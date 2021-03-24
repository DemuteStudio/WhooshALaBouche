/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WhooshGeneratorAudioProcessor::WhooshGeneratorAudioProcessor(): state(std::make_unique<AudioProcessorValueTreeState>(
	                                                                *this, nullptr, "Parameters", create_parameters()))
#ifndef JucePlugin_PreferredChannelConfigurations
                                                                , AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
	                                                                .withInput("Input", juce::AudioChannelSet::stereo(),
	                                                                           true)
#endif
	                                                                .withOutput(
		                                                                "Output", juce::AudioChannelSet::stereo(), true)
#endif
                                                                )
#endif
{
}

WhooshGeneratorAudioProcessor::~WhooshGeneratorAudioProcessor()
{
}

//==============================================================================
const juce::String WhooshGeneratorAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool WhooshGeneratorAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
	return false;
#endif
}

bool WhooshGeneratorAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
	return false;
#endif
}

bool WhooshGeneratorAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
	return false;
#endif
}

double WhooshGeneratorAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int WhooshGeneratorAudioProcessor::getNumPrograms()
{
	return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int WhooshGeneratorAudioProcessor::getCurrentProgram()
{
	return 0;
}

void WhooshGeneratorAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String WhooshGeneratorAudioProcessor::getProgramName(int index)
{
	return {};
}

void WhooshGeneratorAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void WhooshGeneratorAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	sample_rate = sampleRate;
	audioSource.prepareToPlay(samplesPerBlock, sampleRate);
	for (std::list<fx_chain_element*>::value_type element : fx_chain)
	{
		element->prepareToPlay(sampleRate, samplesPerBlock);
	}
}

void WhooshGeneratorAudioProcessor::releaseResources()
{
	audioSource.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WhooshGeneratorAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}

#endif


void WhooshGeneratorAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	ScopedNoDenormals noDenormals;

	const int total_num_input_channels = getTotalNumInputChannels();
	double sample_rate_size_max = (1. / fft_size) * sample_rate;
	const int total_num_output_channels = getTotalNumOutputChannels();

	for (int channel = total_num_input_channels;
	     channel < total_num_output_channels;
	     ++channel)
	{
		buffer.clear(channel, 0, buffer.getNumSamples());
	}

	const int input_buses_count = getBusCount(true);


	for (int busIndex = 0; busIndex < input_buses_count; ++busIndex)
	{
		AudioBuffer<float> audio_buffer = getBusBuffer(buffer, true, busIndex);

		auto bufferToFill = AudioSourceChannelInfo(audio_buffer);

		for (auto channel = 0; channel < total_num_output_channels; ++channel)
		{
			const auto actual_input_channel = 0;
			const auto* inputBuffer = bufferToFill.buffer->getReadPointer(
				actual_input_channel, bufferToFill.startSample);
			auto* outputBuffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);


			for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
			{
				//Volume
				samples_squares_sum += inputBuffer[sample] * inputBuffer[sample];

				(last_rms_value >= threshold_value)
					? outputBuffer[sample] = inputBuffer[sample]
					: outputBuffer[sample] = 0;
			}
		}

		if (block_index >= rms_blocks_length)
		{
			//Volume Envelope
			temp_previous_value = last_rms_value;

			last_rms_value = sqrt(samples_squares_sum / bufferToFill.numSamples);

			last_rms_value = (last_rms_value < threshold_value) ? 0. : last_rms_value;

			samples_squares_sum = 0.0;
			block_index = 0;

			//fft
		}
		else
		{
			block_index++;
		}

		sample_index += bufferToFill.buffer->getNumSamples();
		for (std::list<fx_chain_element>::value_type* element : fx_chain)
		{
			element->getNextAudioBlock(bufferToFill);
		}
		audioSource.getNextAudioBlock(bufferToFill);
	}
}

//==============================================================================
bool WhooshGeneratorAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

void WhooshGeneratorAudioProcessor::add_element_to_fx_chain(fx_chain_element* element)
{
	fx_chain.push_back(element);
	element->prepareToPlay(sample_rate, getBlockSize());
}

juce::AudioProcessorEditor* WhooshGeneratorAudioProcessor::createEditor()
{
	return new WhooshGeneratorAudioProcessorEditor(*this);
}

//==============================================================================
void WhooshGeneratorAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void WhooshGeneratorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

my_audio_source& WhooshGeneratorAudioProcessor::getAudioSource()
{
	return audioSource;
}


AudioProcessorValueTreeState* WhooshGeneratorAudioProcessor::get_state()
{
	return state.get();
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new WhooshGeneratorAudioProcessor();
}

AudioProcessorValueTreeState::ParameterLayout WhooshGeneratorAudioProcessor::create_parameters()
{
	std::vector<std::unique_ptr<RangedAudioParameter>> parameters;

	NormalisableRange<float> frequency_range(50., 20000.);
	frequency_range.setSkewForCentre(1000.);

	parameters.push_back(std::make_unique<AudioParameterFloat>("volume", "VOLUME", 0.0f, 1.0f, 0.01f));
	parameters.push_back(std::make_unique<AudioParameterFloat>("frequency", "FREQUENCY", frequency_range, 0.,
	                                                           "FREQUENCY", AudioProcessorParameter::genericParameter));


	return {parameters.begin(), parameters.end()};
}
