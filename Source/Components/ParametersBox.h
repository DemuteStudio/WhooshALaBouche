#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"

using namespace juce;

//==============================================================================
/*
*/
class ParametersBox : public juce::Component
{
public:
	ParametersBox::ParametersBox(WhooshGeneratorAudioProcessor* processor, int fft_size);
	~ParametersBox() override;

	void paint(juce::Graphics&) override;
	void resized() override;

	void add_listener(Slider::Listener* listener) const;

	std::unique_ptr<Slider> threshold_slider;
	Label threshold_label;
	Label threshold_value_label;

	std::unique_ptr<Slider> rms_length_slider;
	Label rms_length_label;
	Label rms_length_value_label;


	std::unique_ptr<Slider> fft_order_slider;
	Label fft_order_label;
	Label fft_order_value_label;


	std::unique_ptr<Slider> frequency_band_slider;
	Label frequency_band_label;
	Label frequency_band_value_label;


	std::unique_ptr<Slider> frequency_variation_speed_slider;
	Label frequency_variation_speed_label;
	Label frequency_variation_speed__value_label;
private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParametersBox)
};
