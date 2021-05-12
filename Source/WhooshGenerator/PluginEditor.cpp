#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WhooshGeneratorAudioProcessorEditor::WhooshGeneratorAudioProcessorEditor(WhooshGeneratorAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p),
	  in_parameters_box_(&p, p.get_in_parameters()->get_state(), SpectrumAnalyzer::fft_size),
	  out_parameters_box_(p.get_out_parameters()->get_state(), p.get_intern_parameters()->get_state()),
	  fft_visualizer_(p.get_spectrum_analyzer())
{
	setLookAndFeel(&my_look_and_feel_);

	const double time_to_display_in_seconds = 3.;

	number_of_samples_to_display = time_to_display_in_seconds * p.getSampleRate();

	audio_source = &audioProcessor.getAudioSource();

	std::vector<Component*> components = {&in_parameters_box_, &out_parameters_box_, &fft_visualizer_};

	for (std::vector<Component*>::value_type component : components)
	{
		addAndMakeVisible(component);
	}

	enableRecording();

	in_parameters_box_.set_paramater_value_to_default();

	//====================================================================
	setSize(500, 500);
}

WhooshGeneratorAudioProcessorEditor::~WhooshGeneratorAudioProcessorEditor()
{
	setLookAndFeel(nullptr);
}

//==============================================================================
void WhooshGeneratorAudioProcessorEditor::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

	g.setColour(juce::Colours::white);
}

void WhooshGeneratorAudioProcessorEditor::resized()
{
	Rectangle<int> rectangle = getLocalBounds();
	const int height = rectangle.getHeight();
	int width = rectangle.getWidth();

	const int row_height = height / 15;
	int delta = 5;

	in_parameters_box_.setBounds(rectangle.removeFromBottom(row_height * 4));

	auto main_rectangle = rectangle;
	// waveform.setBounds(
	// 	main_rectangle.removeFromTop(main_rectangle.getHeight() / 3).reduced(delta)
	// );
	fft_visualizer_.setBounds(
		main_rectangle.removeFromTop(main_rectangle.getHeight() / 3).reduced(delta)
	);

	out_parameters_box_.setBounds(main_rectangle.removeFromTop(main_rectangle.getHeight()).reduced(delta));
}

int WhooshGeneratorAudioProcessorEditor::get_number_of_blocks_from_milliseconds(
	double sample_rate, const float length_in_milliseconds, int samples_per_block)
{
	return (int)(((sample_rate / 1000) * length_in_milliseconds) / samples_per_block);
}

void WhooshGeneratorAudioProcessorEditor::timerCallback()
{
	const int end_sample = audio_source->get_sample_index();

	// waveform.updateVisibleRegion(end_sample, number_of_samples_to_display);
}


void WhooshGeneratorAudioProcessorEditor::enableRecording()
{
	// waveform.clearWaveform();
	//
	// const std::shared_ptr<AudioSampleBuffer> temp_audio_buffer = audio_source->
	// 	loadRecordingBuffer(number_of_samples_to_display);
	// waveform.loadWaveform(
	// 	temp_audio_buffer.get(), audio_source->getSampleRate(), audio_source->getBufferUpdateLock()
	// );


	// audioBuffer = temp_audio_buffer.get();

	startTimerHz(100);
}
