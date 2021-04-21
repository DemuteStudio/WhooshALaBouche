#pragma once
#include "ParametersState.h"


class InternParametersState: public ParametersState
{
public:
	InternParametersState(AudioProcessor* processor);
	~InternParametersState();
	AudioProcessorValueTreeState::ParameterLayout create_parameters() override;
};
