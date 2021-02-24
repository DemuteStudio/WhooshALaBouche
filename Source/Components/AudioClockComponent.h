#pragma once


#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "../JuceLibraryCode/JuceHeader.h"

#include "TenFtAudioSource.h"


using namespace juce;
class AudioClockComponent :    public Component,
                               public TenFtAudioSource::Listener
{
public:
    AudioClockComponent ();

    ~AudioClockComponent ();

    void resized () override;

    void currentPositionChanged (TenFtAudioSource* audioSource) override;

private:
    std::string getCurrentPositionFormatted (double lengthInSeconds, double currentPosition);

    std::string formatTime (double timeInSeconds);

private:
    Label timeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioClockComponent)
};
