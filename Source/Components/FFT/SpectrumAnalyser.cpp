#include "SpectrumAnalyser.h"


SpectrumAnalyserComponent::SpectrumAnalyserComponent(): forwardFFT(fft_order),
                                                        window(fft_size, juce::dsp::WindowingFunction<float>::hann)
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

		// if (block_index >= rms_blocks_length)
		// {
		// 	calculate_spectrum();
		// 	add_current_spectrum_to_accumulator_buffer();
		// 	block_index = 0;
		// }
		// else
		// {
		// 	block_index++;
		// }

		// calculate_spectrum();
		// add_current_spectrum_to_accumulator_buffer();
	}
}

void SpectrumAnalyserComponent::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	sample_rate = sampleRate;
	frequency_interval = (1. / fft_size) * sampleRate;
	fft_upper_limit = fft_size /2;
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

void SpectrumAnalyserComponent::calculate_spectrum()
{
	if (nextFFTBlockReady)
	{
		calculate_next_frame_of_spectrum();
		nextFFTBlockReady = false;
	}
}

void SpectrumAnalyserComponent::add_current_spectrum_to_accumulator_buffer()
{
	auto fft_data_iterator = fftData.begin();
	for (float element : accumulator_buffer)
	{
		element += *fft_data_iterator;
		++fft_data_iterator;
	}
}

void SpectrumAnalyserComponent::calculate_mean_fft_data()
{
	auto fft_data_iterator = fftData.begin();
	for (float element : accumulator_buffer)
	{
		*fft_data_iterator = element / block_index;
		++fft_data_iterator;
	}
}

void SpectrumAnalyserComponent::push_next_sample_into_fifo(float sample) noexcept
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

void SpectrumAnalyserComponent::calculate_next_frame_of_spectrum()
{
	window.multiplyWithWindowingTable(&fftData[0], fft_size);

	forwardFFT.performFrequencyOnlyForwardTransform(&fftData[0]);

	const auto mindB = -100.0f;
	const auto maxdB = 0.0f;


	for (int i = 0; i < scope_size; ++i)
	{
		const auto skewed_proportion_x = 1.0f - std::exp(std::log(1.0f - (float)i / (float)scope_size) * 0.2f);
		const auto fft_data_index = juce::jlimit<int>(0, fft_upper_limit, (int)(skewed_proportion_x * fft_upper_limit));

		float level = 0;
		if (fft_data_index >= min_frequency_fft_index && fft_data_index <= max_frequency_fft_index)
		{
			level = juce::jmap(juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(fftData[fft_data_index])
			                                - juce::Decibels::gainToDecibels((float)fft_size)),
			                   mindB, maxdB, 0.0f, 1.0f);
		}

		scopeData[i] = level;
	}
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

int SpectrumAnalyserComponent::get_fft_mean_value()
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


void SpectrumAnalyserComponent::calculate_fft()
{
	if (nextFFTBlockReady)
	{
		forwardFFT.performFrequencyOnlyForwardTransform(fftData.data());
		nextFFTBlockReady = false;
	}
}

int SpectrumAnalyserComponent::get_fft_peak()
{
	jassert(
		min_frequency_fft_index >= 0 && max_frequency_fft_index <= fft_size && min_frequency_fft_index <
		max_frequency_fft_index);
	const auto max_iterator = std::max_element(fftData.begin() + min_frequency_fft_index,
	                                           fftData.begin() + max_frequency_fft_index);
	if (max_iterator != fftData.begin() + max_frequency_fft_index)
	{
		const auto index = std::distance(fftData.begin(), max_iterator);
		return index * frequency_interval;
	}
	DBG("max");
	return 0;
}

double SpectrumAnalyserComponent::get_frequency_interval() const
{
	return frequency_interval;
}

int SpectrumAnalyserComponent::get_min_frequency_fft_index() const
{
	return min_frequency_fft_index;
}

void SpectrumAnalyserComponent::set_min_frequency_fft_index(int min_frequency_fft_index)
{
	this->min_frequency_fft_index = min_frequency_fft_index;
}

int SpectrumAnalyserComponent::get_max_frequency_fft_index() const
{
	return max_frequency_fft_index;
}

void SpectrumAnalyserComponent::set_max_frequency_fft_index(int _max_frequency_fft_index)
{
	max_frequency_fft_index = _max_frequency_fft_index;
}

float SpectrumAnalyserComponent::get_fft_index_upper_limit() const
{
	return fft_upper_limit;
}
