#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

// #include "AudioWaveformComponent.h"
#include "myAudioSource.h"
#include "EnvelopeDrawer/EnvelopeComponent.h"


class AudioPlaybackPositionComponent :    public Component,
                                          public my_audio_source::Listener,
                                          public AudioWaveformComponent::Listener,
                                          public EnvelopeComponent::Listener
{
public:
    enum ColourIds
    {
        line_colour = 4
    };

public:
    AudioPlaybackPositionComponent ();

    ~AudioPlaybackPositionComponent ();

    void paint (Graphics& g) override;

    void resized () override;

private:
    void currentPositionChanged (my_audio_source* audioSource) override;

    void visibleRegionChanged (AudioWaveformComponent* waveform) override;
    
    void thumbnailCleared (AudioWaveformComponent* waveform) override;

private:
    bool isAudioLoaded = false;
    bool isRecording = false;
    double visibleRegionStartTime;
    double visibleRegionEndTime;
    double currentPosition;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPlaybackPositionComponent)
};
