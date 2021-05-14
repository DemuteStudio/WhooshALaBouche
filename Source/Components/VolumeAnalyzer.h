#pragma once
#include <JuceHeader.h>

#include "Analyzer.h"

using namespace juce;

class VolumeAnalyzer :  public Analyzer
{
public:
	VolumeAnalyzer(AudioParameterFloat* out_parameter, AudioProcessorValueTreeState* in_state);
	void getNextAudioBlock(AudioBuffer<float>& bufferToFill) override;
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	//=================================================================================================
	float get_last_rms_value_in_db() const;
	float get_last_value() const override;
	String get_osc_address() const override;

private:
	void apply_threshold_to_buffer(const float* inputBuffer, float* outputBuffer) const;
	void accumulate_samples_squares(const float* inputBuffer);
	void calculate_rms();
	float calculate_variation() const;
	bool end_of_integration_period() const;
	//=================================================================================================
	double sample_rate;
	float last_rms_value = 1.0;
	float samples_squares_sum = 0.0;
	int block_index = 0;
	float temp_previous_value = 0.;
	bool is_rms_different = true;

	float threshold_value = 0.;

	float new_rms_value;
	int samples_per_block = 0;
};
