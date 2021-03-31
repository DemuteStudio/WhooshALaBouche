#include "Util.h"

namespace util
{

    double x_to_seconds (
        float x,
        double visibleRegionStartTime,
        double visibleRegionEndTime,
        juce::Rectangle<float> bounds
    )
    {
        double visibleRegionLengthSeconds =
            visibleRegionEndTime - visibleRegionStartTime;

        return (
                ((double) x / (double) bounds.getWidth ()) *
                visibleRegionLengthSeconds
            ) +
            visibleRegionStartTime;
    }

    float seconds_to_x (
        double s,
        double visibleRegionStartTime,
        double visibleRegionEndTime,
        juce::Rectangle<float> bounds
    )
    {
        double visibleRegionLengthSeconds =
            visibleRegionEndTime - visibleRegionStartTime;

        return (float) (
                (s - visibleRegionStartTime) / visibleRegionLengthSeconds
            ) *
            bounds.getWidth ();
    }

    float flatten_x (
        float x,
        juce::Rectangle<float> bounds
    )
    {
        if (x < 0)
        {
            return 0;
        }
        if (x > bounds.getWidth ())
        {
            return bounds.getWidth ();
        }
        return x;
    }

    double flatten_seconds (double s, double totalLength)
    {
        if (s < 0.0f)
        {
            return 0.0f;
        }
        if (s > totalLength)
        {
            return totalLength;
        }
        return s;
    }

    juce::NormalisableRange<float> log_range(const float min, const float max)
    {
	    const auto range{std::log2(max / min)};
	    return {
		    min, max,
		    [=](float min, float, float v) { return std::exp2(v * range) * min; },
		    [=](float min, float, float v) { return std::log2(v / min) / range; },
		    [](float _min, float _max, float v) { return std::clamp(v, _min, _max); }
	    };
    }
}
