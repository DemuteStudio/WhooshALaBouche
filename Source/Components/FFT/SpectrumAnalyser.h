#pragma once

#include <JuceHeader.h>

#include "../FxChainElement.h"
#include "../../PluginProcessor.h"

//==============================================================================
class AnalyserComponent : public juce::Component,
                          private juce::Timer,
                          public fx_chain_element
{
public:
	//! [constructor]
	AnalyserComponent()
		: forwardFFT(fft_order),
		  window(fft_size, juce::dsp::WindowingFunction<float>::hann)
	{
		setOpaque(true);
		startTimerHz(15);
	}

	~AnalyserComponent() override
	{
	}


	int fft_sum_ = 0;
	int fft_index_ = 0;
	//==============================================================================

	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	//==============================================================================
	void paint(juce::Graphics& g) override;
	//==============================================================================
	void timerCallback() override;
	//==============================================================================
	void push_next_sample_into_fifo(float sample) noexcept;
	void draw_next_frame_of_spectrum();
	void draw_frame(juce::Graphics& g);
	//==============================================================================
	int get_fft_mean_value();
	void calculate_fft();
	int get_fft_peak();
	//==============================================================================
	double get_sample_rate_size_max() const;
	int get_min_frequency_fft_index() const;
	void set_min_frequency_fft_index(int min_frequency_fft_index);
	int get_max_frequency_fft_index() const;
	void set_max_frequency_fft_index(int _max_frequency_fft_index);

	enum
	{
		fft_order = 11,
		fft_size = 1 << fft_order,
		scope_size = 512
	};

private:
	juce::dsp::FFT forwardFFT;
	juce::dsp::WindowingFunction<float> window;

	std::array<float, fft_size> fifo;
	std::array<float, 2 * fft_size> fftData;
	int fifoIndex = 0;
	bool nextFFTBlockReady = false;
	float scopeData[scope_size];

	double sample_rate_size_max;

	int min_frequency_fft_index = 0;
	int max_frequency_fft_index;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyserComponent)
};
