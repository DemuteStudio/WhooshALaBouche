#include "Parameter.h"

namespace util
{
	Parameter::Parameter(parameters::AudioParameterString audio_parameter_string,
	                     AudioProcessorValueTreeState* audio_processor_value_tree_state):
		audio_parameter_string(std::move(audio_parameter_string)),
		ranged_parameter(get_parameter(audio_processor_value_tree_state))
	{
	}

	Parameter::~Parameter()
	{
	}

	RangedAudioParameter* Parameter::get_parameter(AudioProcessorValueTreeState* audio_processor_value_tree_state)
	{
		RangedAudioParameter* parameter = audio_processor_value_tree_state->getParameter(audio_parameter_string.id);
		return parameter;
	}
}
