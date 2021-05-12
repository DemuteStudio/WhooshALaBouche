#pragma once
#include "JuceHeader.h"
#include "Parameter.h"
#include "ParameterInterface.h"
#include "Util.h"

using namespace juce;

struct ParameterGui : public juce::Component
{
	ParameterGui(util::Parameter parameter);
    ParameterGui(std::vector<util::Parameter> parameters, parameters::AudioParameterString audio_parameter_string);
	ParameterGui(util::Parameter parameter, const Slider::SliderStyle style);
    ParameterGui(std::vector<util::Parameter> parameters, parameters::AudioParameterString audio_parameter_string,  const Slider::SliderStyle style);

    
    virtual void set_parameters_default_value() const;
	//========================================================================================
	std::unique_ptr<Slider> slider;
	Label label;
    std::vector<std::unique_ptr<ParameterInterface>> parameters;
};
