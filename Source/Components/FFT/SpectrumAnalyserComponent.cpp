#include "SpectrumAnalyserComponent.h"


SpectrumAnalyserComponent::SpectrumAnalyserComponent():
	SpectrumAnalyzer()
{
	setOpaque(true);
	startTimerHz(15);
}


void SpectrumAnalyserComponent::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

	g.setOpacity(1.0f);
	g.setColour(juce::Colours::white);
	draw_frame(g);
}

void SpectrumAnalyserComponent::timerCallback()
{
	calculate_spectrum();
	repaint();
}


void SpectrumAnalyserComponent::draw_frame(juce::Graphics& g)
{
	for (int i = 1; i < scope_size; ++i)
	{
		const auto width = getLocalBounds().getWidth();
		const auto height = getLocalBounds().getHeight();

		g.drawLine({
			(float)juce::jmap(i - 1, 0, scope_size - 1, 0, width),
			juce::jmap(scopeData[i - 1], 0.0f, 1.0f, (float)height, 0.0f),
			(float)juce::jmap(i, 0, scope_size - 1, 0, width),
			juce::jmap(scopeData[i], 0.0f, 1.0f, (float)height, 0.0f)
		});
	}
}
