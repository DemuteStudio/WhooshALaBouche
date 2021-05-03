#pragma once
class ParameterInterface
{
public:
	ParameterInterface(const util::Parameter parameter): parameter(parameter)
	{
	}

	virtual void set_parameter_default_value() const = 0;
protected:
	util::Parameter parameter;
};
