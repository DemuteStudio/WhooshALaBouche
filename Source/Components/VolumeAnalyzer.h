#pragma once
#include <JuceHeader.h>
#include "FxChainElement.h"

using namespace juce;

class VolumeAnalyzer:public fx_chain_element
{
public:
	VolumeAnalyzer();

	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;

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
	//=================================================================================================
	float get_last_rms_value_in_db() const;
};
