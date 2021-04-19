#include "VolumeAnalyzer.h"

VolumeAnalyzer::VolumeAnalyzer(AudioParameterFloat* out_parameter, AudioProcessorValueTreeState* in_state): Analyzer(
	out_parameter, in_state, util::VOLUME)
{
}


void VolumeAnalyzer::getNextAudioBlock(AudioBuffer<float>& bufferToFill)
{
	threshold_value = in_parameters_state->getParameter("threshold")->getValue();

	for (int channel = 0; channel < bufferToFill.getNumChannels(); ++channel)
	{
		const auto* inputBuffer = bufferToFill.getReadPointer(
			channel);

		auto* outputBuffer = bufferToFill.getWritePointer(channel);


		accumulate_samples_squares(inputBuffer);
		apply_threshold_to_buffer(inputBuffer, outputBuffer);

		if (end_of_integration_period())
		{
			calculate_rms();
			const float variation = calculate_variation();
			last_rms_value = last_rms_value + variation;
		}
		else
		{
			block_index++;
		}

		sample_index += bufferToFill.getNumSamples();
	}
}

void VolumeAnalyzer::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	sample_rate = sampleRate;
	samples_per_block = samplesPerBlock;
}


void VolumeAnalyzer::apply_threshold_to_buffer(const float* inputBuffer, float* outputBuffer)
{
	for (auto sample = 0; sample < samples_per_block; ++sample)
	{
		(last_rms_value >= threshold_value)
			? outputBuffer[sample] = inputBuffer[sample]
			: outputBuffer[sample] = 0;
	}
}

void VolumeAnalyzer::accumulate_samples_squares(const float* inputBuffer)
{
	for (auto sample = 0; sample < samples_per_block; ++sample)
	{
		samples_squares_sum += inputBuffer[sample] * inputBuffer[sample];
	}
}

void VolumeAnalyzer::calculate_rms()
{
	new_rms_value = sqrt(samples_squares_sum / samples_per_block);

	new_rms_value = (new_rms_value < threshold_value) ? 0. : new_rms_value;

	samples_squares_sum = 0.0;
	block_index = 0;
}

float VolumeAnalyzer::calculate_variation() const
{
	const float variation_speed = in_parameters_state->getParameter("volume_speed")->getValue();
	return (new_rms_value - last_rms_value) * variation_speed;
}

bool VolumeAnalyzer::end_of_integration_period() const
{
	const int rms_blocks_length = 2/sample_rate;
	return block_index >= rms_blocks_length;
}

float VolumeAnalyzer::get_last_rms_value_in_db() const
{
	return Decibels::gainToDecibels(last_rms_value);
}

float VolumeAnalyzer::get_last_value() const
{
	return last_rms_value;
}

String VolumeAnalyzer::get_osc_address() const
{
	return "/volume";
}
