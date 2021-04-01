#include "VolumeAnalyzer.h"

VolumeAnalyzer::VolumeAnalyzer()
{
}

void VolumeAnalyzer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	const auto actual_input_channel = 0;
	const auto* inputBuffer = bufferToFill.buffer->getReadPointer(
		actual_input_channel, bufferToFill.startSample);
	auto* outputBuffer = bufferToFill.buffer->getWritePointer(actual_input_channel, bufferToFill.startSample);


	for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
	{
		samples_squares_sum += inputBuffer[sample] * inputBuffer[sample];

		(last_rms_value >= threshold_value)
			? outputBuffer[sample] = inputBuffer[sample]
			: outputBuffer[sample] = 0;
	}

	if (block_index >= rms_blocks_length)
	{

		new_rms_value = sqrt(samples_squares_sum / bufferToFill.numSamples);

		new_rms_value = (new_rms_value < threshold_value) ? 0. : new_rms_value;

		samples_squares_sum = 0.0;
		block_index = 0;

		const float variation = (new_rms_value - last_rms_value) * variation_speed;
		last_rms_value = last_rms_value + variation;
	}
	else
	{
		block_index++;
	}

	sample_index += bufferToFill.buffer->getNumSamples();
}

void VolumeAnalyzer::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	sample_rate = sampleRate;
}

float VolumeAnalyzer::get_last_rms_value_in_db() const
{
	return Decibels::gainToDecibels(last_rms_value);
}
