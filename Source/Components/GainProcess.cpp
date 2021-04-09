#include "GainProcess.h"

GainProcess::GainProcess(juce::RangedAudioParameter* parameter): parameter_(parameter)
{
}

void GainProcess::getNextAudioBlock(juce::AudioBuffer<float>& bufferToFill)
{
	const interpolation interpolator(interpolation::point(0, previous_parameter_value),
	                                 interpolation::point(bufferToFill.getNumSamples(), parameter_->getValue()));

	for (int channel = 0; channel < bufferToFill.getNumChannels(); ++channel)
	{
		auto* input_buffer = bufferToFill.getReadPointer(channel);
		auto* output_buffer = bufferToFill.getWritePointer(channel);

		for (int sample = 0; sample < bufferToFill.getNumSamples(); ++sample)
		{
			output_buffer[sample] = input_buffer[sample] * interpolator.get_value(sample);
		}
	}
	previous_parameter_value = parameter_->getValue();
}

void GainProcess::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	fx_chain_element::prepareToPlay(sampleRate, samplesPerBlock);
}

GainProcess::interpolation::interpolation(const point start, const point end): y_range(end.y - start.y),
                                                                               x_range(end.x - start.x),
                                                                               step(y_range / x_range),
                                                                               start(start), end(end)
{
}

float GainProcess::interpolation::get_value(int x_position) const
{
	return start.y + step * x_position;
}

GainProcess::interpolation::point::point(int x, float y): x(x), y(y)
{
}
