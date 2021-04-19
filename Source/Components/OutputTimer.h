#pragma once
#include <JuceHeader.h>
#include "ParametersState.h"
#include "Util.h"
#include "Analyzer.h"

using namespace juce;

class OutputTimer : juce::Timer
{
public:
	OutputTimer(ParametersState* parameter, std::vector<Analyzer*>& analyzers);
	~OutputTimer();

	//==============================k========================================================================

private:
	void timerCallback() override;
	//===================================================================
	ParametersState* parameter_;
	std::vector<Analyzer*>& analyzers;

	OSCSender osc_sender_;

};
