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
}

OutputTimer::~OutputTimer()
{
}

void OutputTimer::timerCallback()
{
	for (std::vector<Analyzer*>::value_type analyzer : analyzers)
	{
		float value = analyzer->get_last_value();

		analyzer->parameter->setValueNotifyingHost(value);

		osc_sender_.send<float>(analyzer->get_osc_address(), std::move(value));
	}
}
