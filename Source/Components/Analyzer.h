#pragma once
#include <JuceHeader.h>


#include "AudioChainElement.h"
#include "ParametersState.h"
#include "Util.h"
using namespace juce;

class Analyzer : public AudioChainElement
{
public:
	Analyzer(AudioParameterFloat* out_parameter, AudioProcessorValueTreeState* in_state, util::parameter_type type);
	~Analyzer() = default;

	virtual float get_last_value() const = 0;
	virtual String get_osc_address() const =0;
	AudioParameterFloat* out_parameter;
	AudioProcessorValueTreeState* in_parameters_state;
	util::parameter_type type;
};
