#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class envelope
{
public:
	struct node
	{
		node(int _sample, float _value): sample(_sample), value(_value)
		{
		}

		int sample = 0;
		float value = 0;
	};

	envelope();
	envelope(envelope* envelope);
	~envelope();

	std::list<node> list_;
	int get_size();

	node operator[](const int index);

	void add(node new_node);
};
