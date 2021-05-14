#include "InParametersState.h"

InParametersState::InParametersState(AudioProcessor* processor, int fft_size): fft_size_(fft_size)
{
	state_ =
		std::make_unique<AudioProcessorValueTreeState>(
			*processor, nullptr, "IN PARAMETERS",
			create_parameters());
}

InParametersState::~InParametersState()
{
}

AudioProcessorValueTreeState::ParameterLayout InParametersState::create_parameters()
{
	std::vector<std::unique_ptr<RangedAudioParameter>> parameters;

	NormalisableRange<float> frequency_range = util::log_range<float>(1., (float)fft_size_ / 2);
	NormalisableRange<float> threshold_range = util::precise_range(0., 0.1);

	parameters.push_back(std::make_unique<AudioParameterFloat>(parameters::threshold.id, parameters::threshold.name,
	                                                           threshold_range, 0.,
	                                                           parameters::threshold.name,
	                                                           AudioProcessorParameter::genericParameter));
	parameters.push_back(std::make_unique<AudioParameterFloat>(parameters::min_frequency.id, parameters::min_frequency.name,
	                                                           frequency_range, 0.,
	                                                           parameters::min_frequency.name,
	                                                           AudioProcessorParameter::genericParameter));
	parameters.push_back(std::make_unique<AudioParameterFloat>(parameters::max_frequency.id, parameters::max_frequency.name, frequency_range,
	                                                           fft_size_ / 2,
	                                                           parameters::max_frequency.name,
	                                                           AudioProcessorParameter::genericParameter));
	parameters.push_back(std::make_unique<AudioParameterFloat>(parameters::frequency_speed.id, parameters::frequency_speed.name, 0.0f, 1.0f, 1.f));
	parameters.push_back(std::make_unique<AudioParameterFloat>(parameters::volume_speed.id, parameters::volume_speed.name, 0.0f, 1.0f, 1.f));


	return {parameters.begin(), parameters.end()};
}
