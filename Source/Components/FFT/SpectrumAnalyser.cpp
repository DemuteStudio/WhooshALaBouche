#include "SpectrumAnalyser.h"

void AnalyserComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	if (bufferToFill.buffer->getNumChannels() > 0)
	{
		auto* channelData = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);

		for (auto i = 0; i < bufferToFill.numSamples; ++i)
			push_next_sample_into_fifo(channelData[i]);
	}
}

void AnalyserComponent::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	sample_rate_size_max = (1. / fft_size) * sampleRate;
}

void AnalyserComponent::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId)); 

	g.setOpacity(1.0f);
	g.setColour(juce::Colours::white);
	draw_frame(g);
}

void AnalyserComponent::timerCallback()
{
	if (nextFFTBlockReady)
	{
		draw_next_frame_of_spectrum();
		nextFFTBlockReady = false;
		repaint();
	}
}

void AnalyserComponent::push_next_sample_into_fifo(float sample) noexcept
{
	if (fifoIndex == fft_size) 
	{
		if (! nextFFTBlockReady) 
		{
			std::fill(fftData.begin(), fftData.end(), 0.0f);
			std::copy(fifo.begin(), fifo.end(), fftData.begin());
			nextFFTBlockReady = true;
		}

		fifoIndex = 0;
	}

	fifo[fifoIndex++] = sample; 
}

void AnalyserComponent::draw_next_frame_of_spectrum()
{
	// first apply a windowing function to our data
	window.multiplyWithWindowingTable(&fftData[0], fft_size);
	// then render our FFT data..
	forwardFFT.performFrequencyOnlyForwardTransform(&fftData[0]);

	const auto mindB = -100.0f;
	const auto maxdB = 0.0f;

	for (int i = 0; i < scope_size; ++i) 
	{
		const auto skewed_proportion_x = 1.0f - std::exp(std::log(1.0f - (float)i / (float)scope_size) * 0.2f);
		const auto fft_data_index = juce::jlimit(0, fft_size / 2, (int)(skewed_proportion_x * (float)fft_size * 0.5f));
		const auto level = juce::jmap(juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(fftData[fft_data_index])
		                                           - juce::Decibels::gainToDecibels((float)fft_size)),
		                              mindB, maxdB, 0.0f, 1.0f);

		scopeData[i] = level; 
	}
}

void AnalyserComponent::draw_frame(juce::Graphics& g)
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

int AnalyserComponent::get_fft_mean_value()
{
	if (fft_index_ = ! 0)
	{
		const int mean_value = fft_sum_ / fft_index_;
		DBG("FREQUENCY PEAK: " <<mean_value<<"  fft_index = "<<fft_index_<< "  fft_sum = " << fft_sum_);
		fft_sum_ = 0;
		fft_index_ = 0;

		return mean_value;
	}
	return 0;
}


void AnalyserComponent::calculate_fft()
{
	if (nextFFTBlockReady)
	{
		forwardFFT.performFrequencyOnlyForwardTransform(fftData.data());
		nextFFTBlockReady = false;
	}
}

int AnalyserComponent::get_fft_peak()
{
	jassert(
		min_frequency_fft_index >= 0 && max_frequency_fft_index <= fft_size && min_frequency_fft_index <
		max_frequency_fft_index);
	const auto max_iterator = std::max_element(fftData.begin() + min_frequency_fft_index,
	                                           fftData.begin() + max_frequency_fft_index);
	if (max_iterator != fftData.begin() + max_frequency_fft_index)
	{
		const auto index = std::distance(fftData.begin(), max_iterator);
		return index * sample_rate_size_max;
	}
	DBG("max");
	return 0;
}

double AnalyserComponent::get_sample_rate_size_max() const
{
	return sample_rate_size_max;
}

int AnalyserComponent::get_min_frequency_fft_index() const
{
	return min_frequency_fft_index;
}

void AnalyserComponent::set_min_frequency_fft_index(int min_frequency_fft_index)
{
	this->min_frequency_fft_index = min_frequency_fft_index;
}

int AnalyserComponent::get_max_frequency_fft_index() const
{
	return max_frequency_fft_index;
}

void AnalyserComponent::set_max_frequency_fft_index(int _max_frequency_fft_index)
{
	max_frequency_fft_index = _max_frequency_fft_index;
}
