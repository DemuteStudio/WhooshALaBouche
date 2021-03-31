#include "SpectrumAnalyzer.h"

SpectrumAnalyzer::SpectrumAnalyzer(): forwardFFT(fft_order),
                                      window(fft_size, juce::dsp::WindowingFunction<float>::hann)

{
}

void SpectrumAnalyzer::calculate_spectrum()
{
	if (nextFFTBlockReady)
	{
		calculate_next_frame_of_spectrum();
		last_fft_peak = get_fft_peak();
		nextFFTBlockReady = false;
	}
}

void SpectrumAnalyzer::add_current_spectrum_to_accumulator_buffer()
{
	auto fft_data_iterator = fftData.begin();
	for (float element : accumulator_buffer)
	{
		element += *fft_data_iterator;
		++fft_data_iterator;
	}
}

void SpectrumAnalyzer::calculate_mean_fft_data()
{
	auto fft_data_iterator = fftData.begin();
	for (float element : accumulator_buffer)
	{
		*fft_data_iterator = element / block_index;
		++fft_data_iterator;
	}
}

void SpectrumAnalyzer::push_next_sample_into_fifo(float sample) noexcept
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

void SpectrumAnalyzer::calculate_next_frame_of_spectrum()
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

int SpectrumAnalyzer::get_fft_mean_value()
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


void SpectrumAnalyzer::calculate_fft()
{
	if (nextFFTBlockReady)
	{
		forwardFFT.performFrequencyOnlyForwardTransform(fftData.data());
		nextFFTBlockReady = false;
	}
}

int SpectrumAnalyzer::get_fft_peak()
{
	jassert(
		min_frequency_fft_index >= 0 && max_frequency_fft_index <= fft_size && min_frequency_fft_index <=
		max_frequency_fft_index);
	const auto max_iterator = std::max_element(fftData.begin() + min_frequency_fft_index,
	                                           fftData.begin() + max_frequency_fft_index);
	if (max_iterator != fftData.begin() + max_frequency_fft_index)
	{
		const auto index = std::distance(fftData.begin(), max_iterator);
		const int new_frequency_peak = index * frequency_interval;
		const int variation = (new_frequency_peak - last_fft_peak) * variation_speed;
		const int out_fft_peak = last_fft_peak + variation;
		last_fft_peak = out_fft_peak;
		DBG("FFT PEAK VOLUME  "<<*max_iterator <<"   out:  "<<out_fft_peak);
		return out_fft_peak;
	}
	DBG("max");
	return 0;
}

double SpectrumAnalyzer::get_frequency_interval() const
{
	return frequency_interval;
}

int SpectrumAnalyzer::get_min_frequency_fft_index() const
{
	return min_frequency_fft_index;
}

void SpectrumAnalyzer::set_min_frequency_fft_index(int min_frequency_fft_index)
{
	this->min_frequency_fft_index = min_frequency_fft_index;
}

int SpectrumAnalyzer::get_max_frequency_fft_index() const
{
	return max_frequency_fft_index;
}

void SpectrumAnalyzer::set_max_frequency_fft_index(int _max_frequency_fft_index)
{
	max_frequency_fft_index = _max_frequency_fft_index;
}

float SpectrumAnalyzer::get_fft_index_upper_limit() const
{
	return fft_upper_limit;
}

float SpectrumAnalyzer::get_speed() const
{
	return variation_speed;
}

void SpectrumAnalyzer::set_speed(const float speed)
{
	this->variation_speed = speed;
}

int SpectrumAnalyzer::get_last_fft_peak() const
{
	return last_fft_peak;
}
