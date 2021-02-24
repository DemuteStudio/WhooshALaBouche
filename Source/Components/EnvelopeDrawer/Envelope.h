#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class envelope
{
public:
	struct envelope_node
	{
		envelope_node(int _sample, int _value): sample(_sample), value(_value)
		{
		}

		int sample = 0;
		float value = 0;
	};

	std::list<envelope_node> list_;
};
