#pragma once

#include <JuceHeader.h>
#include <cctype>
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


	enum parameter_type
	{
		THRESHOLD,
		RMS_LENGTH,
		FFT_ORDER,
		FREQUENCY_BAND,
		FREQUENCY_VARIATION_SPEED,
		VOLUME_VARIATION_SPEED
	};

	struct parameter_gui : public juce::Component
	{
		std::unique_ptr<Slider> slider;
		Label label;
		Label value_label;
		parameter_type parameter_type_;

		parameter_gui(const String id, const String text,  parameter_type type);
		void resized() override;

		parameter_gui(const String id, const String text, parameter_type type, const Slider::SliderStyle style);
	};


	parameter_gui threshold;
	parameter_gui rms_length;
	parameter_gui fft_order;
	parameter_gui frequency_band;
	parameter_gui frequency_variation_speed;
	parameter_gui volume_variation_speed;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParametersBox)
};
