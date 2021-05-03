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
		parameters::AudioParameterString audio_parameter_string;
		RangedAudioParameter* ranged_parameter = nullptr;
	};
}
