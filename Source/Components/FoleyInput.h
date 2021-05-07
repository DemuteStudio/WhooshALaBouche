#pragma once
#include <JuceHeader.h>
#include <filesystem>
#include "AudioChainElement.h"

using namespace juce;

class FoleyInput : public AudioChainElement
{
public:
	FoleyInput();
	~FoleyInput();

	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void getNextAudioBlock(juce::AudioBuffer<float>& bufferToFill) override;

	struct FileAudioSource
	{
		FileAudioSource(File file, AudioFormatReader* format_reader)
			: file(std::move(file)),
			  audio_source(std::make_unique<AudioFormatReaderSource>(format_reader, true))
		{
			audio_source->setLooping(true);
		}

		~FileAudioSource() = default;

		juce::File file;
		std::unique_ptr<AudioFormatReaderSource> audio_source;
	};


	std::vector<std::unique_ptr<FileAudioSource>> files_audio_sources;

	void set_selected_sample(juce::AudioSource*);
	void set_selected_library(const String& library_name);
	//====================================================================================
	String get_current_library_name() const;
	std::vector<File> get_libraries_paths() const;

private:
	struct Library
	{
		Library() = default;

		Library(const File& path, const String& name)
			: path(path),
			  name(name)
		{
		}

		File path;
		String name;
	};

	//====================================================================================
	void scan_libraries_files();
	void scan_samples_files(File library_path);
	void remove_selected_sample();
	void load_samples_into_reader();
	//====================================================================================
	Library* current_library{};
	std::vector<juce::File> samples_files;
	std::vector<Library> libraries_;
	juce::AudioFormatManager audio_format_manager_;
	//====================================================================================
	double sample_rate_{};
	int samples_per_block_{};

	int max_samples_ = 10;

	juce::AudioSource* selected_sample_ = nullptr;
};
