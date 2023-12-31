#pragma once

#include "ParametersState.h"

class InParametersState: public ParametersState
{
public:
	InParametersState(AudioProcessor* processor, int fft_size);
	~InParametersState();
protected:
	AudioProcessorValueTreeState::ParameterLayout create_parameters() override;
private:
	int fft_size_;
};
