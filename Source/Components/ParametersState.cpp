/*
  ==============================================================================

    ParametersState.cpp
    Created: 6 Apr 2021 12:15:25pm
    Author:  arnau

  ==============================================================================
*/

#include "ParametersState.h"

AudioProcessorValueTreeState* ParametersState::get_state() const
{
	return state_.get();
}
