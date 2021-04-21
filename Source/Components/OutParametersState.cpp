#include "OutParametersState.h"

OutParametersState::OutParametersState(AudioProcessor* processor)
{
	state_ = std::make_unique<AudioProcessorValueTreeState>(*processor, nullptr, "OUT PARAMETERS",
	                                                        OutParametersState::create_parameters());
}

AudioProcessorValueTreeState::ParameterLayout OutParametersState::create_parameters()
{
	std::vector<std::unique_ptr<RangedAudioParameter>> parameters;

	NormalisableRange<float> frequency_range = util::log_range<float>(50., 20000.);
	NormalisableRange<float> gain_range = util::gain_range(0., 1.);

	parameters.push_back(std::make_unique<AudioParameterFloat>(util::volume_out_strings.id, util::volume_out_strings.name, gain_range, 0.,
	                                                           util::volume_out_strings.name, AudioProcessorParameter::genericParameter));

	parameters.push_back(std::make_unique<AudioParameterFloat>(util::frequency_out_strings.id, util::frequency_out_strings.name, frequency_range, 0.,
	                                                           util::frequency_out_strings.name, AudioProcessorParameter::genericParameter));

	return {parameters.begin(), parameters.end()};
}
