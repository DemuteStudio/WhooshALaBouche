#pragma once

#include "ParametersState.h"
#include "Util.h"

class InParametersState: public ParametersState
{
public:
	InParametersState(AudioProcessor* processor, int fft_size);
protected:
	AudioProcessorValueTreeState::ParameterLayout create_parameters() override;
private:
	int fft_size_;
};
