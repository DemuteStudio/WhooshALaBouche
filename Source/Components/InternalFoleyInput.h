/*
  ==============================================================================

    InternalFoleyInput.h
    Created: 26 Apr 2021 11:06:58am
    Author:  arnau

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <filesystem>


class InternalFoleyInput
{
public:
	InternalFoleyInput();
	~InternalFoleyInput();

	std::vector<juce::File> get_samples_files();

private:
	std::vector<juce::File> samples_files_;
	void scan_samples_files();
	void scan_samples_files2();
};
