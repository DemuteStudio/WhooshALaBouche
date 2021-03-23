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

void AnalyserComponent::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colours::black);

	g.setOpacity(1.0f);
	g.setColour(juce::Colours::white);
	drawFrame(g);
}

void AnalyserComponent::timerCallback()
{
	if (nextFFTBlockReady)
	{
		drawNextFrameOfSpectrum();
		nextFFTBlockReady = false;
		repaint();
	}
}

void AnalyserComponent::push_next_sample_into_fifo(float sample) noexcept
{
	// if the fifo contains enough data, set a flag to say
	// that the next frame should now be rendered..
	if (fifoIndex == fft_size) // [11]
	{
		if (! nextFFTBlockReady) // [12]
		{
			juce::zeromem(fftData, sizeof (fftData));
			memcpy(fftData, fifo, sizeof (fifo));
			nextFFTBlockReady = true;
		}

		fifoIndex = 0;
	}

	fifo[fifoIndex++] = sample; // [12]
}

void AnalyserComponent::drawNextFrameOfSpectrum()
{
	// first apply a windowing function to our data
	window.multiplyWithWindowingTable(fftData, fft_size); // [1]

	// then render our FFT data..
	forwardFFT.performFrequencyOnlyForwardTransform(fftData); // [2]

	auto mindB = -100.0f;
	auto maxdB = 0.0f;

	for (int i = 0; i < scope_size; ++i) // [3]
	{
		auto skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)scope_size) * 0.2f);
		auto fftDataIndex = juce::jlimit(0, fft_size / 2, (int)(skewedProportionX * (float)fft_size * 0.5f));
		auto level = juce::jmap(juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(fftData[fftDataIndex])
		                                     - juce::Decibels::gainToDecibels((float)fft_size)),
		                        mindB, maxdB, 0.0f, 1.0f);

		scopeData[i] = level; // [4]
	}
}

void AnalyserComponent::drawFrame(juce::Graphics& g)
{
	for (int i = 1; i < scope_size; ++i)
	{
		auto width = getLocalBounds().getWidth();
		auto height = getLocalBounds().getHeight();

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
		forward_fft.performFrequencyOnlyForwardTransform(fft_data.data());
		nextFFTBlockReady = false;
	}
}

int AnalyserComponent::get_fft_peak()
{
	jassert(
		min_frequency_fft_index >= 0 && max_frequency_fft_index <= fft_size && min_frequency_fft_index <
		max_frequency_fft_index);
	const auto max_iterator = std::max_element(fft_data.begin() + min_frequency_fft_index,
	                                           fft_data.begin() + max_frequency_fft_index);
		if (max_iterator != fft_data.begin() + max_frequency_fft_index)
	{
		const auto index = std::distance(fft_data_.begin(), max_iterator);
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
