#pragma once
#include <string>
#include <utility>
#include "AudioParametersString.h"

using namespace juce;

namespace util
{
	struct Parameter
	{
		Parameter(parameters::AudioParameterString audio_parameter_string,
		          AudioProcessorValueTreeState* ranged_audio_parameter);

		~Parameter();
		//=========================================================================================================
		parameters::AudioParameterString audio_parameter_string{"empty"};
		RangedAudioParameter* ranged_parameter = nullptr;

	private:
		RangedAudioParameter* get_parameter(AudioProcessorValueTreeState* audio_processor_value_tree_state);


	};
}
