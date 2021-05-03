#pragma once
#include "JuceHeader.h"
#include "Parameter.h"
#include "ParameterInterface.h"
#include "Util.h"

using namespace juce;

struct ParameterGui : public juce::Component, public ParameterInterface
{
	ParameterGui(util::Parameter parameter);
	ParameterGui(util::Parameter parameter, const Slider::SliderStyle style);

	virtual void set_parameter_default_value() const override;
	//======================================================================================================
	std::unique_ptr<Slider> slider;
	Label label;
};
