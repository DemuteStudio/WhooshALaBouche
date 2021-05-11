/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WhooshGeneratorAudioProcessor::WhooshGeneratorAudioProcessor() : out_parameters_(
	                                                                 std::make_unique<OutParametersState>(this)),
                                                                 in_parameters_(std::make_unique<InParametersState>(
	                                                                 this, SpectrumAnalyzer::fft_size)),
                                                                 intern_parameters_(
	                                                                 std::make_unique<InternParametersState>(this)),
                                                                 internal_foley_input_(
	                                                                 std::make_unique<FoleyInput>()),
                                                                 volume_analyzer_(
	                                                                 std::make_unique<VolumeAnalyzer>(
		                                                                 static_cast<AudioParameterFloat*>(
			                                                                 out_parameters_->
			                                                                 get_state()->getParameter(
				                                                                 parameters::volume_out.id)),
		                                                                 gain_processor_->get_gain_parameter(),
		                                                                 in_parameters_->get_state())),
                                                                 gain_processor_(
	                                                                 std::make_unique<GainProcess>(
		                                                                 out_parameters_->get_state()->getParameter(
			                                                                 parameters::volume_out.id))),
#ifndef JucePlugin_PreferredChannelConfigurations
                                                                 AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
	                                                                 .
	                                                                 withInput(


		                                                                 "Input"
		                                                                 ,
		                                                                 juce::AudioChannelSet::stereo(),


		                                                                 true
	                                                                 )
#endif
	                                                                 .
	                                                                 withOutput(


		                                                                 "Output"
		                                                                 ,
		                                                                 juce::AudioChannelSet::stereo(),


		                                                                 true
	                                                                 )
#endif
                                                                 )
#endif
{
	sidechain_input_processing_chain_ = {volume_analyzer_.get()};

	input_processing_chain_ = {gain_processor_.get()};

	analyzers_ = {volume_analyzer_.get()};

	OutputTimer::set_intern_parameters(intern_parameters_.get());
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
	return false;
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
	audioSource.prepareToPlay(samplesPerBlock, sampleRate);
	for (std::list<AudioChainElement*>::value_type element : sidechain_input_processing_chain_)
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
	const int total_num_output_channels = getTotalNumOutputChannels();

	for (int channel = total_num_input_channels;
	     channel < total_num_output_channels;
	     ++channel)
	{
		buffer.clear(channel, 0, buffer.getNumSamples());
	}
	AudioPlayHead* playHead = getPlayHead();
	AudioPlayHead::CurrentPositionInfo positionInfo{};
	is_playing_ = false;

	if (playHead != nullptr)
	{
		playHead->getCurrentPosition(positionInfo);
		is_playing_ = positionInfo.isPlaying;
	}

	const int input_buses_count = getBusCount(true);

	AudioBuffer<float> foley_input = AudioBuffer<float>(getTotalNumOutputChannels(), getBlockSize());
	AudioBuffer<float> side_chain_input = getBusBuffer(buffer, true, 0);

	internal_foley_input_->getNextAudioBlock(foley_input);

	for (std::list<AudioChainElement>::value_type* element : sidechain_input_processing_chain_)
	{
		element->getNextAudioBlock(side_chain_input);
	}

	for (std::list<AudioChainElement>::value_type* element : input_processing_chain_)
	{
		element->getNextAudioBlock(foley_input);
	}

	//TODO: change implementation
	audioSource.getNextAudioBlock(AudioSourceChannelInfo(foley_input));

	for (int channel = 0; channel < 2; ++channel)
	{
		auto* buffer_to_fill = buffer.getWritePointer(channel);
		auto* buffer_with_noise = foley_input.getWritePointer(channel);

		for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
		{
			buffer_to_fill[sample] = buffer_with_noise[sample];
		}
	}
}

//==============================================================================
bool WhooshGeneratorAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

void WhooshGeneratorAudioProcessor::add_element_to_fx_chain(AudioChainElement* element)
{
	sidechain_input_processing_chain_.push_back(element);
	element->prepareToPlay(getSampleRate(), getBlockSize());
}

void WhooshGeneratorAudioProcessor::remove_element_to_fx_chain(AudioChainElement* element)
{
	sidechain_input_processing_chain_.erase(std::find(sidechain_input_processing_chain_.begin(),
	                                                  sidechain_input_processing_chain_.end(), element));
}

AudioProcessorEditor* WhooshGeneratorAudioProcessor::createEditor()
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

RecordingBuffer& WhooshGeneratorAudioProcessor::getAudioSource()
{
	return audioSource;
}

ParametersState* WhooshGeneratorAudioProcessor::get_in_parameters() const
{
	return in_parameters_.get();
}

ParametersState* WhooshGeneratorAudioProcessor::get_intern_parameters() const
{
	return intern_parameters_.get();
}

ParametersState* WhooshGeneratorAudioProcessor::get_out_parameters() const
{
	return out_parameters_.get();
}

FoleyInput* WhooshGeneratorAudioProcessor::get_internal_foley_input() const
{
	return internal_foley_input_.get();
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new WhooshGeneratorAudioProcessor();
}
