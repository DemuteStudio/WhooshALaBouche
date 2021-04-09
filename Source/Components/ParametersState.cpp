#include "ParametersState.h"

AudioProcessorValueTreeState* ParametersState::get_state() const
{
	return state_.get();
}
