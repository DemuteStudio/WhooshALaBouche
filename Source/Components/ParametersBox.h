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
	ParametersBox(AudioProcessor* processor,AudioProcessorValueTreeState* parameters_state,
	              int fft_size);
	~ParametersBox() override;

	void paint(juce::Graphics&) override;
	void resized() override;

	void add_sliders_listener(Slider::Listener* listener) const;
	void set_paramater_value_to_default();


	std::vector<std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment>> sliders_attachment;
	std::vector<std::unique_ptr<TwoValueSliderAttachment>> two_values_sliders_attachment;


	struct ParameterGuiComponent : public ParameterGui
	{
		ParameterGuiComponent(util::Parameter parameter): ParameterGui(parameter)
		{
		}

		void resized() override;
	};

	struct TwoValuesParameterGuiComponent : public ParameterGui
	{
		TwoValuesParameterGuiComponent(util::Parameter parameter): ParameterGui(
			parameter, Slider::SliderStyle::TwoValueHorizontal)
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
	//==========================================================================================================================================
	std::vector<ParameterGui*> parameter_guis;
	std::vector<ParameterGuiComponent*> one_parameter_guis;
	double frequency_interval;
	AudioProcessor* processor;
protected:
	AudioProcessorValueTreeState* parameters_state;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParametersBox)
};
