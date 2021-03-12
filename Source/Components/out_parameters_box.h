/*
  ==============================================================================

    out_parameters_box.h
    Created: 11 Mar 2021 5:15:17pm
    Author:  arnau

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace juce;

//==============================================================================
/*
*/

class out_parameters_box  : public juce::Component
{
public:
    enum parameter_type
    {
	    volume,
    	frequency
    };

    out_parameters_box();
    ~out_parameters_box() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void set_slider_value(::out_parameters_box::parameter_type parameter, float value);


private:
	Slider volume_out_slider_;
	Slider frequency_out_slider_;

	Label volume_out_label_;
	Label frequency_out_label_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (out_parameters_box)
};
