#include "FoleyInput.h"


FoleyInput::FoleyInput()
{
	audio_format_manager_.registerBasicFormats();
	files_audio_sources.reserve(max_samples_);

	scan_libraries_files();
	if (libraries_paths.size() > 0)
	{
		scan_samples_files(libraries_paths.at(0));
		load_samples_into_reader();
	}
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

void FoleyInput::scan_libraries_files()
{
	const juce::File user_documents = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
	const juce::File application_repertory = user_documents.getChildFile("Whoosh");

	auto directory_entrieses = application_repertory.findChildFiles(juce::File::TypesOfFileToFind::findDirectories,
	                                                                false,
	                                                                "*");

	for (const auto& directory : directory_entrieses)
	{
		libraries_paths.emplace_back(directory);
	}
}


void FoleyInput::scan_samples_files(const File library_path)
{
	auto directory_entrieses = library_path.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false,
	                                                       "*.wav");

	int sample_index = 0;
	for (const auto& file : directory_entrieses)
	{
		if (sample_index < max_samples_)
		{
			samples_files.emplace_back(file);
		}
		sample_index++;
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
