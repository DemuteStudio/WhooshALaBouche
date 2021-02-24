#pragma once


#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "../JuceLibraryCode/JuceHeader.h"

#include "myAudioSource.h"


using namespace juce;
class AudioClockComponent :    public Component,
                               public my_audio_source::Listener
{
public:
    AudioClockComponent ();

    ~AudioClockComponent ();

    void resized () override;

    void currentPositionChanged (my_audio_source* audioSource) override;

private:
    std::string getCurrentPositionFormatted (double lengthInSeconds, double currentPosition);

    std::string formatTime (double timeInSeconds);

private:
    Label timeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioClockComponent)
};
