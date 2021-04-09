#pragma once
#include "ISpectrumAnalyzer.h"
#include <JuceHeader.h>

#include "FxChainElement.h"

class SpectrumAnalyzer : public ISpectrumAnalyzer,
                         public fx_chain_element

{
public:

	SpectrumAnalyzer();

	//==============================================================================
	void getNextAudioBlock(juce::AudioBuffer<float>& bufferToFill) override;
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	//==============================================================================
	void calculate_spectrum() override;
	void add_current_spectrum_to_accumulator_buffer() override;
	void calculate_mean_fft_data() override;
	void push_next_sample_into_fifo(float sample) noexcept override;
	void calculate_next_frame_of_spectrum() override;
	int get_fft_mean_value() override;
	void calculate_fft() override;
	int get_fft_peak() override;

	//===============================================================================
	//Accessors
	double get_frequency_interval() const;
	int get_min_frequency_fft_index() const;
	void set_min_frequency_fft_index(int min_frequency_fft_index);
	int get_max_frequency_fft_index() const;
	void set_max_frequency_fft_index(int _max_frequency_fft_index);
	float get_fft_index_upper_limit() const;
	int get_last_fft_peak() const;
	float get_speed() const;
	void set_speed(const float speed);

	//===============================================================================
	enum
	{
		fft_order = 11,
		fft_size = 1 << fft_order,
		scope_size = 512
	};

	float threshold = 0.;
protected:
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


	int min_frequency_fft_index = 0;
	int max_frequency_fft_index = 1000;

	int block_index = 0;

	int fft_sum_ = 0;
	int fft_index_ = 0;

	float variation_speed = 1.; // between 0 and 1
	int last_fft_peak = 0;

	float rms_blocks_length = 1;

	juce::dsp::WindowingFunction<float> window;
};
