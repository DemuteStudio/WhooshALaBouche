/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WhooshGeneratorAudioProcessor::WhooshGeneratorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
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

	int totalNumInputChannels = getTotalNumInputChannels();
	int totalNumOutputChannels = getTotalNumOutputChannels();

	for (int channel = totalNumInputChannels;
	     channel < totalNumOutputChannels;
	     ++channel)
	{
		buffer.clear(channel, 0, buffer.getNumSamples());
	}

	int intputBusesCount = getBusCount(true);


	for (int busIndex = 0; busIndex < intputBusesCount; ++busIndex)
	{
		AudioBuffer<float> audio_buffer = getBusBuffer(buffer, true, busIndex);

		auto bufferToFill = AudioSourceChannelInfo(audio_buffer);


		for (auto channel = 0; channel < totalNumOutputChannels; ++channel)
		{
			const auto actual_input_channel = 0;
			const auto* inputBuffer = bufferToFill.buffer->getReadPointer(
				actual_input_channel, bufferToFill.startSample);
			auto* outputBuffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);


			for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
			{
				samples_squares_sum += inputBuffer[sample] * inputBuffer[sample];

				if (last_rms_value >= threshold_value)
				{
					outputBuffer[sample] = inputBuffer[sample];
				}
				else
				{
					outputBuffer[sample] = 0;
				}
			}
		}

		if (block_index >= rms_blocks_length)
		{
			last_rms_value = sqrt(samples_squares_sum / bufferToFill.numSamples);

			if (last_rms_value < threshold_value)
			{
				last_rms_value = 0;
			}
			
			rms_envelope.list_.emplace_back(0, last_rms_value);

			samples_squares_sum = 0.0;
			block_index = 0;
		}
		else
		{
			block_index++;
		}

		audioSource.getNextAudioBlock(bufferToFill);
	}
}

//==============================================================================
bool WhooshGeneratorAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
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

envelope* WhooshGeneratorAudioProcessor::load_new_envelope()
{
	rms_envelope = envelope();
	return &rms_envelope;
}

MemoryBlock WhooshGeneratorAudioProcessor::get_envelope_memory_block()
{
	//TODO convert envelope to memory block

	MemoryBlock my_memory_block;
	for (envelope::envelope_node node : rms_envelope.list_)
	{
		my_memory_block.append(&node.value, sizeof(float));
	}

	return my_memory_block;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new WhooshGeneratorAudioProcessor();
}
