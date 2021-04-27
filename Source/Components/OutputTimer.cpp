#include "OutputTimer.h"

OutputTimer::OutputTimer(std::vector<Analyzer*>& analyzers): analyzers(analyzers)
{
	startTimerHz(60);
}

OutputTimer::~OutputTimer()
{
}


void OutputTimer::timerCallback()
{
	if (internal_parameters_ != nullptr)
	{
		if (is_playing_ || internal_parameters_->get_state()->getParameter(parameters::analyze_on_pause.id)->getValue())
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

void OutputTimer::set_intern_parameters(ParametersState* interns_parameters)
{
	internal_parameters_ = interns_parameters;
}
