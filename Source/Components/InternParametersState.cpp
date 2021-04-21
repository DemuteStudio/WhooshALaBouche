#include "InternParametersState.h"

#include "Util.h"

InternParametersState::InternParametersState(AudioProcessor* processor) 
{
	state_ =
		std::make_unique<AudioProcessorValueTreeState>(
			*processor, nullptr, "INTERNAL PARAMETERS",
			InternParametersState::create_parameters());
}

InternParametersState::~InternParametersState()
{
}

AudioProcessorValueTreeState::ParameterLayout InternParametersState::create_parameters()
{
	std::vector<std::unique_ptr<RangedAudioParameter>> parameters;

	parameters.push_back(std::make_unique<AudioParameterBool>(parameters::analyze_on_pause.id, parameters::analyze_on_pause.name, false));
	// parameters.push_back(std::make_unique<AudioParameterFloat>(util::volume_speed.id, util::volume_speed.name, 0.0f, 1.0f, 1.f));


	return {parameters.begin(), parameters.end()};
}
