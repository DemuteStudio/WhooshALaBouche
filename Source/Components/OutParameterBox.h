#pragma once

#include <JuceHeader.h>


#include "GuiParameter.h"
#include "Util.h"

using namespace juce;

//==============================================================================
/*
*/

class OutParametersBox : public juce::Component, private Button::Listener
{
public:

	OutParametersBox(AudioProcessorValueTreeState* output_parameters,AudioProcessorValueTreeState* internal_parameters);
	~OutParametersBox() override;

	void paint(juce::Graphics&) override;
	void resized() override;
	void set_slider_value(util::parameter_type parameter, float value) const;

	void buttonClicked(Button*) override;

	struct parameter_gui_component : public ParameterGui
	{
		parameter_gui_component(util::Parameter parameter):
			ParameterGui(
				parameter)
		{
			label.setFont(Font(10));
			label.setJustificationType(Justification::centred);
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
	std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> analyze_on_pause_attachment;

	ToggleButton show_values_button;
	ToggleButton analyze_on_pause_button;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OutParametersBox)
};
