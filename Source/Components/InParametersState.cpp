#include "InParametersState.h"

InParametersState::InParametersState(AudioProcessor* processor, int fft_size): fft_size_(fft_size)
{
	state_ =
		std::make_unique<AudioProcessorValueTreeState>(
			*processor, nullptr, "IN PARAMETERS",
			create_parameters());
}

AudioProcessorValueTreeState::ParameterLayout InParametersState::create_parameters()
{
	std::vector<std::unique_ptr<RangedAudioParameter>> parameters;

	NormalisableRange<float> frequency_range = util::log_range<float>(1.,(float)fft_size_/2);

	parameters.push_back(std::make_unique<AudioParameterFloat>("threshold", "THRESHOLD", 0.0f, .5f, 0.f));
	parameters.push_back(std::make_unique<AudioParameterFloat>("rms_length", "RMS LENGTH", 0.0f, 10.0f, 0.f));
	parameters.push_back(std::make_unique<AudioParameterFloat>("min_frequency", "MIN FREQUENCY", frequency_range, 0.,
	                                                           "MIN FREQUENCY", AudioProcessorParameter::genericParameter));
	parameters.push_back(std::make_unique<AudioParameterFloat>("max_frequency", "MAX FREQUENCY", frequency_range, fft_size_,
	                                                           "MAX FREQUENCY", AudioProcessorParameter::genericParameter));
	parameters.push_back(std::make_unique<AudioParameterFloat>("fft_speed", "FFT SPEED", 0.0f, 1.0f, 1.f));
	parameters.push_back(std::make_unique<AudioParameterFloat>("volume_speed", "VOLUME SPEED", 0.0f, 1.0f, 1.f));


	return {parameters.begin(), parameters.end()};
}
