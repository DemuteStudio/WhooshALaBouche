#include "SpectrumAnalyzer.h"

SpectrumAnalyzer::SpectrumAnalyzer(AudioParameterFloat* parameter, AudioProcessorValueTreeState* in_state):
	forwardFFT(fft_order),
	window(fft_size,
	       juce::dsp::WindowingFunction<float>::hann),
	Analyzer(parameter, in_state, util::FREQUENCY_PEAK)

{
	scopeData.fill(0);
}

void SpectrumAnalyzer::getNextAudioBlock(juce::AudioBuffer<float>& bufferToFill)
{
	if (bufferToFill.getNumChannels() > 0)
	{
		for (int channel = 0; channel < bufferToFill.getNumChannels(); ++channel)
		{
			const auto* channelData = bufferToFill.getReadPointer(channel, 0);

			for (auto i = 0; i < bufferToFill.getNumSamples(); ++i)
				push_next_sample_into_fifo(channelData[i]);
		}
	}
}

void SpectrumAnalyzer::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	sample_rate = sampleRate;
	frequency_interval = (1. / fft_size) * sampleRate;
	fft_upper_limit = fft_size / 2;
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

float SpectrumAnalyzer::get_level(const float mindB, const float maxdB, const int point)
{
	const auto skewed_proportion_x = 1.0f - std::exp(
		std::log(1.0f - static_cast<float>(point) / static_cast<float>(scope_size)) * 0.2f);

	const auto fft_data_index = juce::jlimit<int>(0, fft_upper_limit,
	                                              static_cast<int>(skewed_proportion_x * fft_upper_limit));

	float level = 0;

	if (fft_data_index >= min_frequency_fft_index && fft_data_index <= max_frequency_fft_index)
	{
		level = juce::jmap(juce::jlimit(mindB, maxdB, Decibels::gainToDecibels(fftData[fft_data_index])
		                                - Decibels::gainToDecibels(static_cast<float>(fft_size))),
		                   mindB, maxdB, 0.0f, 1.0f);
	}
	return level;
}

void SpectrumAnalyzer::calculate_next_frame_of_spectrum()
{
	window.multiplyWithWindowingTable(&fftData[0], fft_size);

	forwardFFT.performFrequencyOnlyForwardTransform(&fftData[0]);

	const auto mindB = -100.0f;
	const auto maxdB = 0.0f;


	for (int point = 0; point < scope_size; ++point)
	{
		scopeData[point] = get_level(mindB, maxdB, point);
	}
}

int SpectrumAnalyzer::get_fft_mean_value()
{
	if (fft_index_ =! 0)
	{
		const int mean_value = fft_sum_ / fft_index_;
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

float SpectrumAnalyzer::calculate_variation(const int new_frequency_peak) const
{
	const float variation_speed = in_parameters_state->getParameter(parameters::frequency_speed.id)->getValue();
	return (new_frequency_peak - last_fft_peak) * variation_speed;
}

int SpectrumAnalyzer::get_min_frequency_fft_index() const
{
	const float min_frequency_fft_index_0to1 = in_parameters_state->getParameter(parameters::min_frequency.id)->
	                                                                getValue();
	return in_parameters_state->getParameterRange(parameters::min_frequency.id)
	                          .convertFrom0to1(
		                          min_frequency_fft_index_0to1);
}

int SpectrumAnalyzer::get_max_frequency_fft_index()
{
	const float max_frequency_fft_index_0to1 = in_parameters_state->getParameter(parameters::max_frequency.id)->
	                                                                getValue();
	return in_parameters_state->getParameterRange(parameters::max_frequency.id)
	                          .convertFrom0to1(
		                          max_frequency_fft_index_0to1);
}

bool SpectrumAnalyzer::min_and_max_in_bounds() const
{
	return min_frequency_fft_index >= 0 && max_frequency_fft_index <= fft_size && min_frequency_fft_index <=
		max_frequency_fft_index;
}

int SpectrumAnalyzer::get_fft_peak()
{
	min_frequency_fft_index = get_min_frequency_fft_index();

	max_frequency_fft_index = get_max_frequency_fft_index();

	jassert(
		min_and_max_in_bounds());


	const auto max_iterator = std::max_element(fftData.begin() + min_frequency_fft_index,
	                                           fftData.begin() + max_frequency_fft_index);

	const auto max_index_iterator = fftData.begin() + max_frequency_fft_index;

	if (max_iterator != max_index_iterator)
	{
		const float peak_amplitude = *max_iterator;

		if (peak_amplitude >= in_parameters_state->getParameter(parameters::threshold.id)->getValue())
		{
			const auto index = std::distance(fftData.begin(), max_iterator);

			const int new_frequency_peak = index * frequency_interval;

			const int variation = calculate_variation(new_frequency_peak);

			const int out_fft_peak = last_fft_peak + variation;

			last_fft_peak = out_fft_peak;

			return out_fft_peak;
		}
	}
	return last_fft_peak;
}

double SpectrumAnalyzer::get_frequency_interval() const
{
	return frequency_interval;
}

float SpectrumAnalyzer::get_fft_index_upper_limit() const
{
	return fft_upper_limit;
}

float SpectrumAnalyzer::get_last_value() const
{
	return last_fft_peak;
}

String SpectrumAnalyzer::get_osc_address() const
{
	return "/frequency";
}
