#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
	for (juce::AudioIODeviceType* t : deviceManager.getAvailableDeviceTypes())
	{
		t->scanForDevices();
		auto lst = t->getDeviceNames();
	};

	levelSlider.setRange(0.0, 0.25);
	levelSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
	levelLabel.setText("Volume", juce::dontSendNotification);

	addAndMakeVisible(levelSlider);
	addAndMakeVisible(levelLabel);

	setSize(600, 100);

	// Make sure you set the size of the component after
	// you add any child components.
	setSize(800, 600);

	// Some platforms require permissions to open input channels so request that here
	if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
		&& ! juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
	{
		juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
		                                  [&](bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
	}
	else
	{
		// Specify the number of input and output channels that we want to open
		setAudioChannels(2, 2);
	}
}

MainComponent::~MainComponent()
{
	// This shuts down the audio device and clears the audio source.
	shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	auto t = deviceManager.getCurrentAudioDevice()->getName();
	juce::String message;
	message << "Preparing to play audio..." << juce::newLine;
	message << " samplesPerBlockExpected = " << samplesPerBlockExpected << juce::newLine;
	message << " sampleRate = " << sampleRate;
	juce::Logger::getCurrentLogger()->writeToLog(message);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	bufferToFill.clearActiveBufferRegion();
	auto* device = deviceManager.getCurrentAudioDevice();
	auto activeInputChannels = device->getActiveInputChannels();
	auto activeOutputChannels = device->getActiveOutputChannels();
	auto maxInputChannels = activeInputChannels.getHighestBit() + 1;
	auto maxOutputChannels = activeOutputChannels.getHighestBit() + 1;

	auto level = (float)levelSlider.getValue();

	for (auto channel = 0; channel < maxOutputChannels; ++channel)
	{
		if ((!activeOutputChannels[channel]) || maxInputChannels == 0)
		{
			bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
		}
		else
		{
			auto actualInputChannel = channel % maxInputChannels; // [1]

			if (!activeInputChannels[channel]) // [2]
			{
				bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
			}
			else // [3]
			{
				auto *inBuffer = bufferToFill.buffer->getReadPointer(actualInputChannel, bufferToFill.startSample);
				auto *outBuffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);

				for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
				{
					float next_random = random.nextFloat();
						outBuffer[sample] = inBuffer[sample] * level;
					int x = 5;
				}
			}
		}
	}
}

void MainComponent::releaseResources()
{
	// This will be called when the audio device stops, or when it is being
	// restarted due to a setting change.

	// For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

	// You can add your drawing code here!
}

void MainComponent::resized()
{
	levelLabel.setBounds(10, 10, 90, 20);
	levelSlider.setBounds(100, 10, getWidth() - 110, 20);
}
