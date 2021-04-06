#pragma once

#include <JuceHeader.h>


#include "GuiParameter.h"
#include "Util.h"

using namespace juce;

//==============================================================================
/*
*/

class out_parameters_box : public juce::Component, private Button::Listener
{
public:

	out_parameters_box(AudioProcessorValueTreeState* processor_state);
	~out_parameters_box() override;

	void paint(juce::Graphics&) override;
	void resized() override;
	void set_slider_value(util::parameter_type parameter, float value) const;

	void buttonClicked(Button*) override;

	struct parameter_gui_component : public parameter_gui
	{
		parameter_gui_component(const String id, const String text, util::parameter_type type):
			parameter_gui(
				id, text, type, Slider::LinearVertical)
		{
		}

		void resized() override;

		bool show_values = false;
	};

private:
	std::vector<parameter_gui_component*> parameters_components;

	parameter_gui_component volume_out;
	parameter_gui_component frequency_out;

	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> volume_out_attachment_;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> frequency_out_attachment_;

	ToggleButton show_values_button;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(out_parameters_box)
};
