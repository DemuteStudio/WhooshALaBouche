#pragma once
#include "ISpectrumAnalyzer.h"
#include <JuceHeader.h>


#include "Analyzer.h"
#include "FxChainElement.h"

using namespace juce;

class SpectrumAnalyzer : public ISpectrumAnalyzer,
                         public Analyzer
{
public:

	SpectrumAnalyzer(AudioParameterFloat* parameter, AudioProcessorValueTreeState* in_state);

	//==============================================================================
	void getNextAudioBlock(juce::AudioBuffer<float>& bufferToFill) override;
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	//==============================================================================
	void calculate_spectrum() override;
	void add_current_spectrum_to_accumulator_buffer() override;
	void calculate_mean_fft_data() override;
	void push_next_sample_into_fifo(float sample) noexcept override;
	float get_level(float mindB, float maxdB, int point);
	void calculate_next_frame_of_spectrum() override;
	int get_fft_mean_value() override;
	void calculate_fft() override;
	float calculate_variation(int new_frequency_peak) const;
	int get_min_frequency_fft_index() const;
	int get_max_frequency_fft_index();
	int get_fft_peak() override;

	//===============================================================================
	//Accessors
	double get_frequency_interval() const;
	float get_fft_index_upper_limit() const;

	//===============================================================================
	float get_last_value() const override;
	String get_osc_address() const override;

	//===============================================================================
	enum
	{
		fft_order = 11,
		fft_size = 1 << fft_order,
		scope_size = 512
	};

	double sample_rate;
	double frequency_interval;
	float fft_upper_limit;


	float scopeData[scope_size];
private:

	juce::dsp::FFT forwardFFT;


	std::array<float, fft_size> fifo;
	std::array<float, 2 * fft_size> fftData;

	std::array<float, fft_size> accumulator_buffer;

	int fifoIndex = 0;
	bool nextFFTBlockReady = false;


	int block_index = 0;

	int fft_sum_ = 0;
	int fft_index_ = 0;

	int last_fft_peak = 0;

	int min_frequency_fft_index = 0;
	int max_frequency_fft_index= 1000;

	bool min_and_max_in_bounds() const;
	juce::dsp::WindowingFunction<float> window;
};
