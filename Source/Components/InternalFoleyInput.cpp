#include "InternalFoleyInput.h"


InternalFoleyInput::InternalFoleyInput()
{
	scan_samples_files2();
}

InternalFoleyInput::~InternalFoleyInput()
{
}


void InternalFoleyInput::scan_samples_files()
{
	//TODO: Uniformize: use std or Juce library
	const std::string path = "/";

	for (const auto& file : std::filesystem::directory_iterator(path))
	{
		samples_files_.emplace_back(file.path().string());
	}
}

void InternalFoleyInput::scan_samples_files2()
{
	const juce::File user_documents = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
	const juce::File application_repertory = user_documents.getChildFile("Whoosh");

	auto samples_files = application_repertory.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*.wav");

	for (const auto& file : samples_files)
	{
		samples_files_.emplace_back(file);
	}
}
std::vector<juce::File> InternalFoleyInput::get_samples_files()
{
	return samples_files_;
}
