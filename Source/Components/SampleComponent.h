#pragma once

#include <JuceHeader.h>
#include "FoleyInput.h"
#include "SelectorElement.h"

class SampleComponent : public SelectorElement
{
public:
	SampleComponent(const FoleyInput::FileAudioSource* sample);
	~SampleComponent() override;

	const FoleyInput::FileAudioSource* file_audio_source;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleComponent)
};
