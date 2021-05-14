#pragma once
#include "Parameter.h"

class ParameterInterface
{
public:
	ParameterInterface(const util::Parameter parameter): parameter_(parameter)
	{
	}

protected:
	util::Parameter parameter_;

public:
    void set_parameter_default_value() const
    {
        const auto parameter_default_value = parameter_.ranged_parameter->getDefaultValue();
        
        parameter_.ranged_parameter->setValueNotifyingHost(parameter_default_value*0.9);
        parameter_.ranged_parameter->setValueNotifyingHost(parameter_default_value+0.1);
        parameter_.ranged_parameter->setValueNotifyingHost(parameter_default_value);
    }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterInterface)
};
