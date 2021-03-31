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

    juce::NormalisableRange<float> log_range(const float min, const float max);
}
