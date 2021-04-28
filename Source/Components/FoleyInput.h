#pragma once
#include <JuceHeader.h>
#include <filesystem>
#include "AudioChainElement.h"

using namespace juce;

class FoleyInput: public AudioChainElement
{
public:
	FoleyInput();
	~FoleyInput();

	void getNextAudioBlock(juce::AudioBuffer<float>& bufferToFill) override;

	struct FileAudioSource
	{
		FileAudioSource(const File& file, AudioFormatReader* format_reader)
			: file(file),
			audio_source(std::make_unique<AudioFormatReaderSource>(format_reader, true))
		{
		}
		~FileAudioSource()=default;

		juce::File file;
		std::unique_ptr<AudioFormatReaderSource> audio_source;
	};

	void load_samples_into_reader();

	// std::vector<std::unique_ptr<FileAudioSource>> get_samples_information() const;
	std::vector<std::unique_ptr<FileAudioSource>> files_audio_sources_;
	void set_selected_sample(juce::AudioSource*);
private:
	void scan_samples_files_alternative();
	void scan_samples_files();
//====================================================================================
	juce::AudioFormatManager audio_format_manager_;
	std::vector<juce::File> samples_files_;


	juce::AudioSource* selected_sample_ = nullptr;
};

