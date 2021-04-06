#include "OutParametersState.h"

OutParametersState::OutParametersState(AudioProcessor* processor) 
{
	state_ = std::make_unique<AudioProcessorValueTreeState>(*processor, nullptr, "OUT PARAMETERS", OutParametersState::create_parameters());
}

AudioProcessorValueTreeState::ParameterLayout OutParametersState::create_parameters()
{
	std::vector<std::unique_ptr<RangedAudioParameter>> parameters;

	NormalisableRange<float> frequency_range = util::log_range<float>(50., 20000.);

	float minimum_db = -100.;
	parameters.push_back(std::make_unique<AudioParameterFloat>("volume", "VOLUME", minimum_db, 0.0f, minimum_db));
	parameters.push_back(std::make_unique<AudioParameterFloat>("frequency", "FREQUENCY", frequency_range, 0.,
	                                                           "FREQUENCY", AudioProcessorParameter::genericParameter));


	return {parameters.begin(), parameters.end()};
}
