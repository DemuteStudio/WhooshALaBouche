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
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
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
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int WhooshGeneratorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WhooshGeneratorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String WhooshGeneratorAudioProcessor::getProgramName (int index)
{
    return {};
}

void WhooshGeneratorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void WhooshGeneratorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	audioSource.prepareToPlay(samplesPerBlock, sampleRate);
}

void WhooshGeneratorAudioProcessor::releaseResources()
{
	audioSource.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WhooshGeneratorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void WhooshGeneratorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
		AudioBuffer<float> inputBuffer = getBusBuffer(buffer, true, busIndex);
		audioSource.getNextAudioBlock(AudioSourceChannelInfo(inputBuffer));
	}
}

//==============================================================================
bool WhooshGeneratorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WhooshGeneratorAudioProcessor::createEditor()
{
    return new WhooshGeneratorAudioProcessorEditor (*this);
}

//==============================================================================
void WhooshGeneratorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void WhooshGeneratorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

TenFtAudioSource& WhooshGeneratorAudioProcessor::getAudioSource()
{
		return audioSource;

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WhooshGeneratorAudioProcessor();
}
