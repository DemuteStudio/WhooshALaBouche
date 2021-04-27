#pragma once
#include <JuceHeader.h>
#include "ParametersState.h"
#include "Util.h"
#include "Analyzer.h"
#include "InParametersState.h"

using namespace juce;

class OutputTimer : juce::Timer
{
public:
	OutputTimer( std::vector<Analyzer*>& analyzers);
	~OutputTimer();

	//==============================k========================================================================

private:
	void timerCallback() override;
	//===================================================================
	ParametersState* internal_parameters_;
	std::vector<Analyzer*>& analyzers;

protected:
	void set_intern_parameters(ParametersState* internal_parameters);
	bool is_playing_ = false;
	bool is_outputted_on_pause_ = false;

};
