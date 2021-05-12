#pragma once
#include "Analyzer.h"
#include "AudioChainElement.h"


class GainProcess : public AudioChainElement
{
public:

	GainProcess(juce::RangedAudioParameter* parameter, Analyzer* analyzer);
	//========================================================================================================================
	void getNextAudioBlock(juce::AudioBuffer<float>& bufferToFill) override;
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;


private:
	struct Interpolation
	{
		struct point
		{
			point(int x, float y);
			int x;
			float y;
		};

		Interpolation(::GainProcess::Interpolation::point start, ::GainProcess::Interpolation::point end);
		float get_value(int x_position) const;

		float y_range;
		float x_range;

		point start;
		point end;

		float step;
	};
	juce::RangedAudioParameter* parameter_;

	float previous_parameter_value = 0.;

	float gain_value{0};
	Analyzer* analyzer_;
};

