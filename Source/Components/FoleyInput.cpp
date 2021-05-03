#include "FoleyInput.h"


FoleyInput::FoleyInput()
{
	audio_format_manager_.registerBasicFormats();
	files_audio_sources.reserve(10);

	scan_samples_files();
	load_samples_into_reader();
}

FoleyInput::~FoleyInput()
{
}

void FoleyInput::prepareToPlay(double sampleRate, int samplesPerBlock)

{
	sample_rate_ = sampleRate;
	samples_per_block_ = samplesPerBlock;
}

void FoleyInput::getNextAudioBlock(juce::AudioBuffer<float>& bufferToFill)
{
	if (selected_sample_ != nullptr)
	{
		selected_sample_->getNextAudioBlock(
			juce::AudioSourceChannelInfo(bufferToFill));
	}
}

void FoleyInput::set_selected_sample(juce::AudioSource* audio_source)
{
	if (selected_sample_ != nullptr)
	{
		selected_sample_->releaseResources();
	}

	selected_sample_ = audio_source;
	selected_sample_->prepareToPlay(samples_per_block_, sample_rate_);
}

void FoleyInput::scan_samples_files_alternative()
{
	//TODO: Uniformize: use std or Juce library
	const std::string path = "/";

	for (const auto& file : std::filesystem::directory_iterator(path))
	{
		samples_files.emplace_back(file.path().string());
	}
}

void FoleyInput::scan_samples_files()
{
	const juce::File user_documents = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
	const juce::File application_repertory = user_documents.getChildFile("Whoosh");

	auto directory_entrieses = application_repertory.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false,
	                                                                "*.wav");

	for (const auto& file : directory_entrieses)
	{
		samples_files.emplace_back(file);
	}
}

void FoleyInput::load_samples_into_reader()
{
	for (const auto& samples_file : samples_files)
	{
		auto* reader = audio_format_manager_.createReaderFor(samples_file);

		files_audio_sources.emplace_back(
			std::make_unique<FoleyInput::FileAudioSource>(
				samples_file, reader));
	}
}
