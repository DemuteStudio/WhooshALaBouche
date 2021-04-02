#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <algorithm>


namespace util
{

    double x_to_seconds (
        float x,
        double visibleRegionStartTime,
        double visibleRegionEndTime,
        juce::Rectangle<float> bounds
    );

    float seconds_to_x (
        double s,
        double visibleRegionStartTime,
        double visibleRegionEndTime,
        juce::Rectangle<float> bounds
    );

    float flatten_x (
        float x,
        juce::Rectangle<float> bounds
    );

    double flatten_seconds (
        double s,
        double totalLength
    );

	template<typename Type>
    juce::NormalisableRange<Type> log_range(const Type min, const Type max)
    {
		static_assert(std::is_floating_point<Type>::value,
			"Function A can only be instantiated with floating point types");


	    const auto range{std::log2(max / min)};
	    return {
		    min, max,
		    [=](Type min, Type , Type v) { return std::exp2(v * range) * min; },
		    [=](Type min, Type , Type v) { return std::log2(v / min) / range; },
		    [](Type _min, Type _max, Type v) { return std::clamp(v, _min, _max); }
	    };
    }
//===============//===============//===============//===============//===============//===============//===============//===============
	enum parameter_type
	{
		THRESHOLD,
		RMS_LENGTH,
		FFT_ORDER,
		FREQUENCY_BAND,
		FREQUENCY_VARIATION_SPEED,
		VOLUME_VARIATION_SPEED,
		VOLUME,
		FREQUENCY_PEAK
	};

}
