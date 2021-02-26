#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class envelope
{
public:
	struct envelope_node
	{
		envelope_node(int _sample, float _value): sample(_sample), value(_value)
		{
		}

		int sample = 0;
		float value = 0;
	};

	envelope();
	~envelope();

	std::vector<envelope_node> list_;
	int get_size();
};
