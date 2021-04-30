
#pragma once
#include "ParametersState.h"
#include "Util.h"
#include "AudioParametersString.h"

class OutParametersState: public ParametersState
{
public:
	OutParametersState(AudioProcessor* processor);


protected:
	AudioProcessorValueTreeState::ParameterLayout create_parameters() override;
};
