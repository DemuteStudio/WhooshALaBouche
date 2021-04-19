#include "OutputTimer.h"

OutputTimer::OutputTimer(ParametersState* parameter, std::vector<Analyzer*>& analyzers): parameter_(parameter),
	analyzers(analyzers)
{
	const bool connected = osc_sender_.connect("127.0.0.1", 6969);
	if (connected)
	{
		DBG("Connected");
	}
	else
	{
		DBG("Not Connected");
	}
	startTimerHz(60);
}

OutputTimer::~OutputTimer()
{
}


void OutputTimer::timerCallback()
{
	for (std::vector<Analyzer*>::value_type analyzer : analyzers)
	{
		float value = analyzer->get_last_value();
		const float normalized_value = analyzer->out_parameter->convertTo0to1(value);

		analyzer->out_parameter->setValueNotifyingHost(normalized_value);
	}
}
