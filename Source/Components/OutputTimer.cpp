#include "OutputTimer.h"

OutputTimer::OutputTimer(ParametersState* internal_parameter, std::vector<Analyzer*>& analyzers):
	analyzers(analyzers), internal_parameters(internal_parameter)
{
	startTimerHz(60);
}

OutputTimer::~OutputTimer()
{
}


void OutputTimer::timerCallback()
{
	if (internal_parameters != nullptr)
	{
		if (is_playing_ || internal_parameters->get_state()->getParameter(parameters::analyze_on_pause.id)->getValue())
		{
			for (std::vector<Analyzer*>::value_type analyzer : analyzers)
			{
				const float value = analyzer->get_last_value();
				const float normalized_value = analyzer->out_parameter->convertTo0to1(value);

				analyzer->out_parameter->setValueNotifyingHost(normalized_value);
			}
		}
	}
}
