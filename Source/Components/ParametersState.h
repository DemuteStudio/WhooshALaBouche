#pragma once
#include <JuceHeader.h>
#include "Util.h"
#include "AudioParametersString.h"

using namespace juce;

class ParametersState
{
public:
	ParametersState() = default;

	AudioProcessorValueTreeState* get_state() const;
protected:
	virtual AudioProcessorValueTreeState::ParameterLayout create_parameters() = 0;
	std::unique_ptr<AudioProcessorValueTreeState> state_;
};
