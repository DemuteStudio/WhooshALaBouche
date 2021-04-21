#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <algorithm>
#include <string>
#include <cctype>


namespace util
{
	double x_to_seconds(
		float x,
		double visibleRegionStartTime,
		double visibleRegionEndTime,
		juce::Rectangle<float> bounds
	);

	float seconds_to_x(
		double s,
		double visibleRegionStartTime,
		double visibleRegionEndTime,
		juce::Rectangle<float> bounds
	);

	float flatten_x(
		float x,
		juce::Rectangle<float> bounds
	);

	double flatten_seconds(
		double s,
		double totalLength
	);

	template <typename Type>
	juce::NormalisableRange<Type> log_range(const Type min, const Type max)
	{
		static_assert(std::is_floating_point<Type>::value,
			"Function A can only be instantiated with floating point types");


		const auto range{std::log2(max / min)};
		return {
			min, max,
			[=](Type min, Type, Type v) { return std::exp2(v * range) * min; },
			[=](Type min, Type, Type v) { return std::log2(v / min) / range; },
			[](Type _min, Type _max, Type v) { return std::clamp(v, _min, _max); }
		};
	}

	inline juce::NormalisableRange<float> gain_range(const float min, const float max)
	{
		return {
			min, max,
			[=](float min, float, float v) { return v; },
			[=](float min, float, float v) { return v / max; },
			[](float _min, float _max, float v) { return juce::jlimit<float>(_min, _max, v); }
		};
	}

	//===============//===============//===============//===============//===============//===============//===============//===============
	inline float gain_to_decibels(float gain)
	{
		float log;
		if (gain < 0.01)
		{
			log = std::log10(gain * 10) - 1;
		}
		else
		{
			log = std::log10(gain);
		}
		const float decibels = log * 20.0;
		return juce::jmax<float>(-100, decibels);
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

namespace parameters
{
	struct AudioParameterString
	{
		AudioParameterString(std::string id, std::string name);

		AudioParameterString(std::string id);

		std::string id;
		std::string name;
	};

	static AudioParameterString volume_out = AudioParameterString("volume"),
	                            frequency_out = AudioParameterString("frequency"),
	                            threshold = AudioParameterString("threshold"),
	                            min_frequency = AudioParameterString("min_frequency"),
	                            max_frequency = AudioParameterString("max_frequency"),
	                            frequency_speed = AudioParameterString("frequency_speed"),
	                            volume_speed = AudioParameterString("volume_speed"),

	                            analyze_on_pause = AudioParameterString("analyze_on_pause");
}
