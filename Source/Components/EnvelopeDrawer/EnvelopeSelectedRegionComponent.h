#pragma once


#include "../JuceLibraryCode/JuceHeader.h"

#include "EnvelopeComponent.h"
#include "../TenFtUtil.h"


class EnvelopeSelectedRegionComponent : public Component,
                                             public EnvelopeComponent::Listener
{
public:
    enum ColourIds
    {
        waveformSelectedRegionBackgroundColour = 3
    };

public:
    EnvelopeSelectedRegionComponent ();

    ~EnvelopeSelectedRegionComponent ();

    void paint (Graphics& g) override;

    void resized () override;

private:
    void visibleRegionChanged (EnvelopeComponent* waveform) override;

    void selectedRegionChanged (EnvelopeComponent* waveform) override;

private:
    bool hasSelectedRegion = false;
    double selectedRegionStartTime;
    double selectedRegionEndTime;
    double visibleRegionStartTime;
    double visibleRegionEndTime;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopeSelectedRegionComponent)
};
