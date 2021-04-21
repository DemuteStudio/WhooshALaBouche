#include "Util.h"

namespace util
{
	double x_to_seconds(
		float x,
		double visibleRegionStartTime,
		double visibleRegionEndTime,
		juce::Rectangle<float> bounds
	)
	{
		double visibleRegionLengthSeconds =
			visibleRegionEndTime - visibleRegionStartTime;

		return (
				((double)x / (double)bounds.getWidth()) *
				visibleRegionLengthSeconds
			) +
			visibleRegionStartTime;
	}

	float seconds_to_x(
		double s,
		double visibleRegionStartTime,
		double visibleRegionEndTime,
		juce::Rectangle<float> bounds
	)
	{
		double visibleRegionLengthSeconds =
			visibleRegionEndTime - visibleRegionStartTime;

		return (float)(
				(s - visibleRegionStartTime) / visibleRegionLengthSeconds
			) *
			bounds.getWidth();
	}

	float flatten_x(
		float x,
		juce::Rectangle<float> bounds
	)
	{
		if (x < 0)
		{
			return 0;
		}
		if (x > bounds.getWidth())
		{
			return bounds.getWidth();
		}
		return x;
	}

	double flatten_seconds(double s, double totalLength)
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

	AudioParameterString::AudioParameterString(std::string id, std::string name): id(id), name(name)
	{
	}

	AudioParameterString::AudioParameterString(std::string id): id(id)
	{
		name = id;
		if (name.find('_') != std::string::npos)
		{
			std::replace(name.begin(), name.end(), '_', ' ');
		}
		transform(name.begin(), name.end(), name.begin(), ::toupper);
	}
}
