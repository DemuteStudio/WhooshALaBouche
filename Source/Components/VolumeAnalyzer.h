#pragma once
#include <JuceHeader.h>

#include "Analyzer.h"
#include "FxChainElement.h"

using namespace juce;

class VolumeAnalyzer:public fx_chain_element, public Analyzer
{
public:
	VolumeAnalyzer(AudioParameterFloat* parameter);
	void getNextAudioBlock(AudioBuffer<float>& bufferToFill) override;
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	//=================================================================================================
	void apply_threshold_to_buffer(const float* inputBuffer, float* outputBuffer);
	void accumulate_samples(const float* inputBuffer);
	void calculate_rms();
	float calculate_variation() const;
	bool end_of_integration_period() const;
	//=================================================================================================

	//=================================================================================================
	struct zero_crossing_detector
	{
		zero_crossing_detector() = default;
		bool soundEnabled = false;
	};
	//=================================================================================================
	float get_last_rms_value_in_db() const;
	float get_last_value() const override ;
	String get_osc_address()const override;

	double sample_rate;
	float last_rms_value = 1.0;
	float samples_squares_sum = 0.0;
	int block_index = 0;
	int sample_index = 0;
	float temp_previous_value = 0.;
	bool is_rms_different = true;

	float threshold_value = 0.0;
	float rms_blocks_length = 1;
	float new_rms_value;
	float variation_speed = 1.;
	int samples_per_block = 0;
};
