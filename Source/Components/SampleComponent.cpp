#include <JuceHeader.h>
#include "SampleComponent.h"

//==============================================================================
SampleComponent::SampleComponent(const FoleyInput::FileAudioSource* sample) :
	SelectorElement(sample->file.getFileNameWithoutExtension()), file_audio_source(sample)
{
}

SampleComponent::~SampleComponent()
{
}

