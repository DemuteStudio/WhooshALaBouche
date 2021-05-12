#include "GainProcess.h"

GainProcess::GainProcess(juce::RangedAudioParameter* parameter, Analyzer* analyzer): parameter_(parameter), analyzer_(analyzer)
{
}

void GainProcess::getNextAudioBlock(juce::AudioBuffer<float>& bufferToFill)
{
	const auto gain_value = analyzer_->get_last_value();
	const Interpolation interpolator(Interpolation::point(0, previous_parameter_value),
	                                 Interpolation::point(bufferToFill.getNumSamples(), gain_value));

	for (int channel = 0; channel < bufferToFill.getNumChannels(); ++channel)
	{
		const float* input_buffer = bufferToFill.getReadPointer(channel);
		float* output_buffer = bufferToFill.getWritePointer(channel);

		for (int sample = 0; sample < bufferToFill.getNumSamples(); ++sample)
		{
			output_buffer[sample] = input_buffer[sample] * interpolator.get_value(sample);
		}
	}
	previous_parameter_value = gain_value;
	// DBG("previous_parameter_value:  " << previous_parameter_value);
}

void GainProcess::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	AudioChainElement::prepareToPlay(sampleRate, samplesPerBlock);
}

GainProcess::Interpolation::Interpolation(const point start, const point end): y_range(end.y - start.y),
                                                                               x_range(end.x - start.x),
                                                                               step(y_range / x_range),
                                                                               start(start), end(end)
{
}

float GainProcess::Interpolation::get_value(int x_position) const
{
	return start.y + step * x_position;
}

GainProcess::Interpolation::point::point(int x, float y): x(x), y(y)
{
}
