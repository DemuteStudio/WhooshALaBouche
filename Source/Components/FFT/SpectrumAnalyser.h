#pragma once

#include <JuceHeader.h>

#include "../FxChainElement.h"
#include "../../PluginProcessor.h"

//==============================================================================
class SpectrumAnalyserComponent : public juce::Component,
                          private juce::Timer,
                          public fx_chain_element
{
public:
	//! [constructor]
	SpectrumAnalyserComponent();

	~SpectrumAnalyserComponent() override
	{
	}

	//==============================================================================

	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	//==============================================================================
	void paint(juce::Graphics& g) override;
	//==============================================================================
	void timerCallback() override;
	void calculate_spectrum();
	void add_current_spectrum_to_accumulator_buffer();
	void calculate_mean_fft_data();
	//==============================================================================
	void push_next_sample_into_fifo(float sample) noexcept;
	void calculate_next_frame_of_spectrum();
	void draw_frame(juce::Graphics& g);
	//==============================================================================
	int get_fft_mean_value();
	void calculate_fft();
	int get_fft_peak();
	//==============================================================================
	double get_frequency_interval() const;
	int get_min_frequency_fft_index() const;
	void set_min_frequency_fft_index(int min_frequency_fft_index);
	int get_max_frequency_fft_index() const;
	void set_max_frequency_fft_index(int _max_frequency_fft_index);
	float get_fft_index_upper_limit() const;
	int get_last_fft_peak() const;

	enum
	{
		fft_order = 11,
		fft_size = 1 << fft_order,
		scope_size = 512
	};

	float rms_blocks_length = 1;


	float get_speed() const;
	void set_speed(const float speed);
private:
	juce::dsp::FFT forwardFFT;
	juce::dsp::WindowingFunction<float> window;

	double sample_rate;

	std::array<float, fft_size> fifo;
	std::array<float, 2 * fft_size> fftData;

	std::array<float, fft_size> accumulator_buffer;

	int fifoIndex = 0;
	bool nextFFTBlockReady = false;
	float scopeData[scope_size];

	double frequency_interval;
	float fft_upper_limit ;

	int min_frequency_fft_index = 0;
	int max_frequency_fft_index;

	int block_index = 0;

	int fft_sum_ = 0;
	int fft_index_ = 0;

	float variation_speed = 1.; // between 0 and 1
	int last_fft_peak = 0;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumAnalyserComponent)
};



