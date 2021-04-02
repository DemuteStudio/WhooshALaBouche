#pragma once
#include "JuceHeader.h"
#include "Util.h"

using namespace juce;

struct parameter_gui : public juce::Component
{
	parameter_gui(const String id, const String text, util::parameter_type type);
	parameter_gui(const String id, const String text, util::parameter_type type,
		const Slider::SliderStyle style);

	
	//======================================================================================================
	std::unique_ptr<Slider> slider;
	Label label;
	Label value_label;
	util::parameter_type parameter_type_;

};
