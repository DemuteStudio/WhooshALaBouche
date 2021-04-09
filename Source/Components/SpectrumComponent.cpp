#include "SpectrumComponent.h"


SpectrumComponent::SpectrumComponent(SpectrumAnalyzer* analyzer): analyzer(analyzer)
{
	setOpaque(true);
	startTimerHz(15);
}


void SpectrumComponent::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

	g.setOpacity(1.0f);
	g.setColour(juce::Colours::white);
	draw_frame(g);
}

void SpectrumComponent::timerCallback()
{
	analyzer->calculate_spectrum();
	repaint();
}


void SpectrumComponent::draw_frame(juce::Graphics& g)
{
	for (int i = 1; i < analyzer->scope_size; ++i)
	{
		const auto width = getLocalBounds().getWidth();
		const auto height = getLocalBounds().getHeight();

		g.drawLine({
			(float)juce::jmap(i - 1, 0, analyzer->scope_size - 1, 0, width),
			juce::jmap(analyzer->scopeData[i - 1], 0.0f, 1.0f, (float)height, 0.0f),
			(float)juce::jmap(i, 0, analyzer->scope_size - 1, 0, width),
			juce::jmap(analyzer->scopeData[i], 0.0f, 1.0f, (float)height, 0.0f)
		});
	}
}
