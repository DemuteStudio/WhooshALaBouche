#include "FoleyInput.h"


FoleyInput::FoleyInput()
{
	audio_format_manager_.registerBasicFormats();
	files_audio_sources_.reserve(10);

	scan_samples_files();
	load_samples_into_reader();
}

FoleyInput::~FoleyInput()
{
}

void FoleyInput::getNextAudioBlock(juce::AudioBuffer<float>& bufferToFill)
{
	if (selected_sample_ != nullptr)
	{
		static_cast<juce::AudioFormatReaderSource*>(selected_sample_)->getNextAudioBlock(
			juce::AudioSourceChannelInfo(bufferToFill));
	}
}

void FoleyInput::set_selected_sample(juce::AudioSource* audio_source)
{
	selected_sample_ = audio_source;
}

void FoleyInput::scan_samples_files_alternative()
{
	//TODO: Uniformize: use std or Juce library
	const std::string path = "/";

	for (const auto& file : std::filesystem::directory_iterator(path))
	{
		samples_files_.emplace_back(file.path().string());
	}
}

void FoleyInput::scan_samples_files()
{
	const juce::File user_documents = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
	const juce::File application_repertory = user_documents.getChildFile("Whoosh");

	auto samples_files = application_repertory.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*.wav");

	for (const auto& file : samples_files)
	{
		samples_files_.emplace_back(file);
	}
}

// std::vector<std::unique_ptr<FoleyInput::FileAudioSource>> FoleyInput::get_samples_information() const
// {
// 	return files_audio_sources_;
// }

void FoleyInput::load_samples_into_reader()
{
	for (auto samples_file : samples_files_)
	{
		auto* reader = audio_format_manager_.createReaderFor(samples_file);

		files_audio_sources_.emplace_back(
			std::make_unique<FoleyInput::FileAudioSource>(
				samples_file, reader));
	}
}
