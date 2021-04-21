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

	parameters.push_back(
		std::make_unique<AudioParameterFloat>(util::threshold_strings.id, util::threshold_strings.name, 0.0f, .5f, 0.f));
	parameters.push_back(std::make_unique<AudioParameterFloat>(util::min_frequency_strings.id, util::min_frequency_strings.name,
	                                                           frequency_range, 0.,
	                                                           util::min_frequency_strings.name,
	                                                           AudioProcessorParameter::genericParameter));
	parameters.push_back(std::make_unique<AudioParameterFloat>(util::max_frequency_strings.id, util::max_frequency_strings.name, frequency_range,
	                                                           fft_size_ / 2,
	                                                           util::max_frequency_strings.name,
	                                                           AudioProcessorParameter::genericParameter));
	parameters.push_back(std::make_unique<AudioParameterFloat>(util::frequency_speed_strings.id, util::frequency_speed_strings.name, 0.0f, 1.0f, 1.f));
	parameters.push_back(std::make_unique<AudioParameterFloat>(util::volume_speed.id, util::volume_speed.name, 0.0f, 1.0f, 1.f));


	return {parameters.begin(), parameters.end()};
}
