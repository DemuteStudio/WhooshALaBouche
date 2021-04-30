#include "AudioParametersString.h"


namespace parameters
{
	AudioParameterString::AudioParameterString(std::string id, std::string name): id(id), name(name)
	{
	}

	AudioParameterString::AudioParameterString(std::string id): id(id)
	{
		name = id;
		if (name.find('_') != std::string::npos)
		{
			std::replace(name.begin(), name.end(), '_', ' ');
		}
		transform(name.begin(), name.end(), name.begin(), ::toupper);
	}
}
