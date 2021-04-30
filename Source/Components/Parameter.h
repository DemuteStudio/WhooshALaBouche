#pragma once
#include <string>
#include <utility>

#include "GuiParameter.h"
#include "AudioParametersString.h"


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
