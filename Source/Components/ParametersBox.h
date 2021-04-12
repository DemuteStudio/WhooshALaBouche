#pragma once

#include <JuceHeader.h>

#include "GuiParameter.h"
#include "TwoValueSliderAttachment.h"
#include "Util.h"

using namespace juce;

//==============================================================================
/*
*/
class ParametersBox : public juce::Component
{
public:
	ParametersBox::ParametersBox(AudioProcessor* processor, AudioProcessorValueTreeState* parameters_state,
	                             int fft_size);
	~ParametersBox() override;

	void paint(juce::Graphics&) override;
	void resized() override;

	void add_listener(Slider::Listener* listener) const;


	std::vector<std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment>> sliders_attachment_;
	std::vector<std::unique_ptr<TwoValueSliderAttachment>> two_values_sliders_attachment_;


	struct parameter_gui_component : public parameter_gui
	{
		parameter_gui_component(const String id, const String text, util::parameter_type type): parameter_gui(
			id, text, type)
		{
		}

		parameter_gui_component(const String id, const String text, util::parameter_type type,
		                        const Slider::SliderStyle style): parameter_gui(id, text, type, style)
		{
		}

		void resized() override;
	};


	parameter_gui_component threshold;
	parameter_gui_component rms_length;
	parameter_gui_component fft_order;
	parameter_gui_component frequency_band;
	parameter_gui_component frequency_variation_speed;
	parameter_gui_component volume_variation_speed;


private:
	void link_sliders_to_parameters();
	void set_parameters_value_to_text();

	//==========================================================================================================================================

	AudioProcessor* processor;
protected:
	AudioProcessorValueTreeState* parameters_state;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParametersBox)
};
