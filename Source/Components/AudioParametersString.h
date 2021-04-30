#include <JuceHeader.h>
#pragma once

namespace parameters
{
	struct AudioParameterString
	{
		AudioParameterString(std::string id, std::string name);

		AudioParameterString(std::string id);

		std::string id;
		std::string name;
	};

	static AudioParameterString volume_out = AudioParameterString("volume"),
	                            frequency_out = AudioParameterString("frequency"),
	                            threshold = AudioParameterString("threshold"),
	                            min_frequency = AudioParameterString("min_frequency"),
	                            frequency_band = AudioParameterString("frequency_band"),
	                            max_frequency = AudioParameterString("max_frequency"),
	                            frequency_speed = AudioParameterString("frequency_speed"),
	                            volume_speed = AudioParameterString("volume_speed"),

	                            analyze_on_pause = AudioParameterString("analyze_on_pause");
}
