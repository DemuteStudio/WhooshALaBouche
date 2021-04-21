#pragma once
#include <JuceHeader.h>
#include "ParametersState.h"
#include "Util.h"
#include "Analyzer.h"

using namespace juce;

class OutputTimer : juce::Timer
{
public:
	OutputTimer(ParametersState* internal_parameter, std::vector<Analyzer*>& analyzers);
	~OutputTimer();

	//==============================k========================================================================

private:
	void timerCallback() override;
	//===================================================================
	ParametersState* parameter_;
	std::vector<Analyzer*>& analyzers;
	ParametersState* internal_parameters;

protected:
	bool is_playing_ = false;
	bool is_outputted_on_pause_ = false;

};
