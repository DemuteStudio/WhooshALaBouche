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

	void add_sliders_listener(Slider::Listener* listener) const;


	std::vector<std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment>> sliders_attachment_;
	std::vector<std::unique_ptr<TwoValueSliderAttachment>> two_values_sliders_attachment_;


	struct ParameterGuiComponent : public parameter_gui
	{
		ParameterGuiComponent(const String id, const String text, util::parameter_type type): parameter_gui(
			id, text, type)
		{
		}

		ParameterGuiComponent(const String id, const String text, util::parameter_type type,
		                        const Slider::SliderStyle style): parameter_gui(id, text, type, style)
		{
		}

		void resized() override;
	};
	struct TwoValuesParameterGuiComponent : public parameter_gui
	{
		TwoValuesParameterGuiComponent(const String id, const String text, util::parameter_type type): parameter_gui(
			id, text, type, Slider::SliderStyle::TwoValueHorizontal)
		{
			value.setJustificationType(Justification::centred);
			addAndMakeVisible(value);
		}

		Label value;
		void resized() override;
	};


	ParameterGuiComponent threshold;
	TwoValuesParameterGuiComponent frequency_band;
	ParameterGuiComponent frequency_variation_speed;
	ParameterGuiComponent volume_variation_speed;



private:
	void link_sliders_to_parameters();
	void set_parameters_value_to_text() const;

	//==========================================================================================================================================
	std::vector<parameter_gui*> parameter_guis;
	AudioProcessor* processor;
	double frequency_interval;
protected:
	AudioProcessorValueTreeState* parameters_state;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParametersBox)
};
