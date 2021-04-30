#pragma once
#include "JuceHeader.h"
#include "Parameter.h"
#include "Util.h"

using namespace juce;

struct ParameterGui : public juce::Component
{
	ParameterGui(util::Parameter parameter);
	ParameterGui(util::Parameter parameter, const Slider::SliderStyle style);


	void set_parameter_default_value() const;
	//======================================================================================================
	std::unique_ptr<Slider> slider;
	Label label;
	util::Parameter parameter_;
};
