#pragma once
#include <JuceHeader.h>
#include "Util.h"
using namespace juce;

class Analyzer
{
public:

	Analyzer(AudioParameterFloat* parameter, util::parameter_type type);
	~Analyzer() = default;

	virtual float get_last_value() const = 0;
	virtual String get_osc_address() const =0;
	AudioParameterFloat* parameter;
	util::parameter_type type;
};
