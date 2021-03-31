#include "SpectrumAnalyserComponent.h"


SpectrumAnalyserComponent::SpectrumAnalyserComponent():
	SpectrumAnalyzer()
{
	setOpaque(true);
	startTimerHz(15);
}

void SpectrumAnalyserComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	if (bufferToFill.buffer->getNumChannels() > 0)
	{
		const auto* channelData = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);

		for (auto i = 0; i < bufferToFill.numSamples; ++i)
			push_next_sample_into_fifo(channelData[i]);
	}
}

void SpectrumAnalyserComponent::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	sample_rate = sampleRate;
	frequency_interval = (1. / fft_size) * sampleRate;
	fft_upper_limit = fft_size / 2;
}

void SpectrumAnalyserComponent::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

	g.setOpacity(1.0f);
	g.setColour(juce::Colours::white);
	draw_frame(g);
}

void SpectrumAnalyserComponent::timerCallback()
{
	calculate_spectrum();
	repaint();
}


void SpectrumAnalyserComponent::draw_frame(juce::Graphics& g)
{
	for (int i = 1; i < scope_size; ++i)
	{
		const auto width = getLocalBounds().getWidth();
		const auto height = getLocalBounds().getHeight();

		g.drawLine({
			(float)juce::jmap(i - 1, 0, scope_size - 1, 0, width),
			juce::jmap(scopeData[i - 1], 0.0f, 1.0f, (float)height, 0.0f),
			(float)juce::jmap(i, 0, scope_size - 1, 0, width),
			juce::jmap(scopeData[i], 0.0f, 1.0f, (float)height, 0.0f)
		});
	}
}
