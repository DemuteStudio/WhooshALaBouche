/*
  ==============================================================================

    Fft_visualizer.cpp
    Created: 23 Mar 2021 10:42:12am
    Author:  arnau

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Fft_visualizer.h"


//==============================================================================
Fft_visualizer::Fft_visualizer(WhooshGeneratorAudioProcessor* processor):
	window(1024, juce::dsp::WindowingFunction<float>::blackman), processor_(processor)
{
	// In your constructor, you should add any child components, and
	// initialise any special settings that your component needs.
	std::array<float, scope_size> scopeData;
}

Fft_visualizer::~Fft_visualizer()
{
}

void Fft_visualizer::paint(juce::Graphics& g)
{
	/* This demo code just fills the component's background and
	   draws some placeholder text to get you started.

	   You should replace everything in this method with your own
	   drawing code..
	*/

	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId)); // clear the background

	g.setColour(juce::Colours::grey);
	// g.drawRect (getLocalBounds(), 1);   // draw an outline around the component


	drawFrame(g);
}

void Fft_visualizer::resized()
{
	// This method is where you should set the bounds of any child
	// components that your component contains..
}

void Fft_visualizer::drawNextFrameOfSpectrum()
{
	// first apply a windowing function to our data ===========================================================================================================================================================================================================================================================================================================================
	window.multiplyWithWindowingTable(&processor_->fft_data_[0], processor_->fft_size); // [1]


	auto mindB = -100.0f;
	auto maxdB = 0.0f;

	for (int i = 0; i < scope_size; ++i) // [3]
	{
		auto skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)scope_size) * 0.2f);
		auto fftDataIndex = juce::jlimit(0, processor_->fft_size / 2,
		                                 (int)(skewedProportionX * (float)processor_->fft_size * 0.5f));


		auto zer = juce::Decibels::gainToDecibels((float)processor_->fft_size);
		auto zert = juce::Decibels::gainToDecibels(processor_->fft_data_[fftDataIndex] );

		auto level = juce::jmap(juce::jlimit(mindB, maxdB,
		                                     juce::Decibels::gainToDecibels(processor_->fft_data_[fftDataIndex]  )
		                                     // - juce::Decibels::gainToDecibels((float)processor_->fft_size)),
		                        ),
		                        mindB, maxdB, 0.0f, 1.0f);

		scopeData[i] = level; // [4]
	}
}

void Fft_visualizer::drawFrame(juce::Graphics& g)
{
	for (int i = 1; i < scope_size; ++i)
	{
		auto width = getLocalBounds().getWidth();
		auto height = getLocalBounds().getHeight();

		g.drawLine({
			(float)juce::jmap(i - 1, 0, scope_size - 1, 0, width),
			juce::jmap(scopeData[i - 1], 0.0f, 1.0f, (float)height, 0.0f),
			(float)juce::jmap(i, 0, scope_size - 1, 0, width),
			juce::jmap(scopeData[i], 0.0f, 1.0f, (float)height, 0.0f)
		});
	}
}
