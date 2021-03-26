#pragma once

#include <JuceHeader.h>

using namespace juce;

//==============================================================================
/*
*/

class out_parameters_box : public juce::Component
{
public:
	enum parameter_type
	{
		volume,
		frequency
	};

	out_parameters_box(AudioProcessorValueTreeState* processor_state);
	~out_parameters_box() override;

	void paint(juce::Graphics&) override;
	void resized() override;
	void set_slider_value(::out_parameters_box::parameter_type parameter, float value);
	void set_slider_range(parameter_type parameter, int new_min, int new_max);


private:
	Slider volume_out_slider_;
	Slider frequency_out_slider_;

	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> volume_out_attachment_;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> frequency_out_attachment_;


	Label volume_out_label_;
	Label frequency_out_label_;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(out_parameters_box)
};
