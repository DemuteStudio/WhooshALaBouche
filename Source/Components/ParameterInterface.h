#pragma once
class ParameterInterface
{
public:
	ParameterInterface(const util::Parameter parameter): parameter(parameter)
	{
	}

    void set_parameter_default_value(Slider* slider) const
    {
        const auto parameter_default_value = parameter.ranged_parameter->getDefaultValue();
//        slider->setValue(parameter_default_value*0.9);
//        slider->setValue(parameter_default_value+0.1);
//        slider->setValue(parameter_default_value);
        
        
        parameter.ranged_parameter->setValueNotifyingHost(parameter_default_value*0.9);
        parameter.ranged_parameter->setValueNotifyingHost(parameter_default_value+0.1);
        parameter.ranged_parameter->setValueNotifyingHost(parameter_default_value);
    }
protected:
	util::Parameter parameter;
};
