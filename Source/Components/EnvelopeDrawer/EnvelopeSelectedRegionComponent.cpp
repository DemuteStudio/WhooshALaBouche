#include "EnvelopeSelectedRegionComponent.h"

EnvelopeSelectedRegionComponent::EnvelopeSelectedRegionComponent ()
{
    setInterceptsMouseClicks (false, true);
}

EnvelopeSelectedRegionComponent::~EnvelopeSelectedRegionComponent ()
{
}

void EnvelopeSelectedRegionComponent::paint (Graphics& g)
{
    juce::Rectangle<float> thumbnailBounds = getLocalBounds ().toFloat ();
    bool hasIntersectionWithSelectedRegion = !(
        selectedRegionEndTime < visibleRegionStartTime ||
        selectedRegionStartTime > visibleRegionEndTime
    );

    if (hasSelectedRegion && hasIntersectionWithSelectedRegion)
    {
        float notSelectedRegionLeftWidth =
            util::flattenX (
                util::secondsToX (
                    selectedRegionStartTime,
                    visibleRegionStartTime,
                    visibleRegionEndTime,
                    thumbnailBounds
                ),
                thumbnailBounds
            ),
            notSelectedRegionRightWidth =
            util::flattenX (
                util::secondsToX (
                    selectedRegionEndTime,
                    visibleRegionStartTime,
                    visibleRegionEndTime,
                    thumbnailBounds),
                thumbnailBounds
            ),
            selectedRegionWidth =
            util::flattenX (
                notSelectedRegionRightWidth - notSelectedRegionLeftWidth,
                thumbnailBounds
            );
        juce::Rectangle<float> notSelectedRegionLeft =
                thumbnailBounds.removeFromLeft (notSelectedRegionLeftWidth),
            selectedRegion =
                thumbnailBounds.removeFromLeft (selectedRegionWidth),
            notSelectedRegionRight = thumbnailBounds;

        g.setColour (findColour (
            ColourIds::waveformSelectedRegionBackgroundColour
        ).withAlpha(0.3f));
        g.fillRect (selectedRegion);
    }
}

void EnvelopeSelectedRegionComponent::resized ()
{
    setBounds (getLocalBounds ());
}

// ==============================================================================

void EnvelopeSelectedRegionComponent::visibleRegionChanged (
    EnvelopeComponent* waveform
)
{
    visibleRegionStartTime = waveform->getVisibleRegionStartTime ();
    visibleRegionEndTime = waveform->getVisibleRegionEndTime ();
    repaint ();
}

void EnvelopeSelectedRegionComponent::selectedRegionChanged (
    EnvelopeComponent* waveform
)
{
    hasSelectedRegion = waveform->getHasSelectedRegion ();
    selectedRegionStartTime = waveform->getSelectedRegionStartTime ();
    selectedRegionEndTime = waveform->getSelectedRegionEndTime ();
    repaint ();
}