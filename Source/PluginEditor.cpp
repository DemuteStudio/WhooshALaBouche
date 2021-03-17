#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WhooshGeneratorAudioProcessorEditor::WhooshGeneratorAudioProcessorEditor(WhooshGeneratorAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p), parameters_box_(p.getBlockSize(), p.sample_rate),
	  out_parameters_box_(p.get_state())
// envelope_array_(p.rms_envelope.get()) 
{
	setLookAndFeel(&my_look_and_feel_);
	const double time_to_display = 3.;
	number_of_samples_to_display = time_to_display * p.sample_rate;

	audio_source = &audioProcessor.getAudioSource();
	addAndMakeVisible(parameters_box_);

	parameters_box_.add_listener(this);

	formatManager.registerBasicFormats();


	addAndMakeVisible(&waveform);

	addAndMakeVisible(&out_parameters_box_);


	waveform.addListener(audio_source);
	waveform.onPositionChange = [this](double newPosition)
	{
		audio_source->setPosition(newPosition);
	};


	enableRecording();

	//====================================================================
	//OSC

	const bool connected = osc_sender_.connect("10.0.0.34", 8000);
	if (connected)
	{
		DBG("Connected");
	}
	else
	{
		DBG("Not Connected");
	}


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
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

	g.setColour(juce::Colours::white);
	// g.setFont(15.0f);
}

void WhooshGeneratorAudioProcessorEditor::resized()
{
	Rectangle<int> rectangle = getLocalBounds();
	const int height = rectangle.getHeight();
	int width = rectangle.getWidth();

	const int row_height = height / 15;
	int delta = 5;

	parameters_box_.setBounds(rectangle.removeFromBottom(row_height * 2));


	auto main_rectangle = rectangle;
	waveform.setBounds(
		main_rectangle.removeFromTop(main_rectangle.getHeight() / 2).reduced(delta)
	);

	out_parameters_box_.setBounds(main_rectangle.removeFromTop(main_rectangle.getHeight()).reduced(delta));

	// envelope_playback_position_.setBounds(
	// 	volume_envelope_.getBounds()
	// );
	//
	// volume_envelope_.setBounds(main_rectangle.removeFromTop(main_rectangle.getHeight() / 2).reduced(delta));
	// frequency_envelope_.setBounds(main_rectangle.reduced(delta));
}

void WhooshGeneratorAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	if (slider->getName() == "threshold")
	{
		audioProcessor.threshold_value = slider->getValue();
		const int threshold_value_db = Decibels::gainToDecibels(slider->getValue());
		parameters_box_.threshold_value_label.setText(std::to_string(threshold_value_db) + " dB", dontSendNotification);
	}
	if (slider->getName() == "rms_length")
	{
		audioProcessor.rms_blocks_length = slider->getValue();
		const int rms_length_value = slider->getValue() * ((audioProcessor.getBlockSize() / audioProcessor.sample_rate)
			* 1000.);
		parameters_box_.rms_length_value_label.setText(std::to_string(rms_length_value) + " ms", dontSendNotification);

		// audioProcessor.rms_blocks_length = get_number_of_blocks_from_milliseconds(
		// 	processor.getSampleRate(), slider_value, processor.getBlockSize());
	}
}


int WhooshGeneratorAudioProcessorEditor::get_number_of_blocks_from_milliseconds(
	double sample_rate, const float length_in_milliseconds, int samples_per_block)
{
	return (int)(((sample_rate / 1000) * length_in_milliseconds) / samples_per_block);
}

void WhooshGeneratorAudioProcessorEditor::timerCallback()
{
	audioProcessor.calculate_fft();
	int frequency_peak = audioProcessor.get_fft_peak();

	const int end_sample = audio_source->get_sample_index();

	waveform.updateVisibleRegion(end_sample, number_of_samples_to_display);

	out_parameters_box_.set_slider_value(out_parameters_box::volume, audioProcessor.last_rms_value);
	out_parameters_box_.set_slider_value(out_parameters_box::frequency, frequency_peak);

	send_osc_message(out_parameters_box::volume, audioProcessor.last_rms_value);
	send_osc_message(out_parameters_box::frequency, frequency_peak);
}

void WhooshGeneratorAudioProcessorEditor::send_osc_message(out_parameters_box::parameter_type type, float value)
{
	// OSCMessage my_message("/blob");
	// my_message.addBlob(audioProcessor.get_envelope_memory_block());
	//
	//
	// osc_sender_.send(my_message);
	String type_string = "";
	switch (type)
	{
	case out_parameters_box::volume:
		type_string = "/volume";
		break;
	case out_parameters_box::frequency:
		type_string = "/frequency";
		break;
	default:
		break;
	}
	osc_sender_.send<float>(type_string, std::move(value));
}


void WhooshGeneratorAudioProcessorEditor::enableRecording()
{
	waveform.clearWaveform();
	// volume_envelope_.clearWaveform();
	audio_source->unloadAudio();

	const std::shared_ptr<AudioSampleBuffer> temp_audio_buffer = audio_source->
		loadRecordingBuffer(number_of_samples_to_display);
	waveform.loadWaveform(
		temp_audio_buffer.get(), audio_source->getSampleRate(), audio_source->getBufferUpdateLock()
	);

	envelope* temp_envelope_buffer = audioProcessor.load_new_envelope();
	// volume_envelope_.load_envelope(
	// 	temp_envelope_buffer, tempAudioBuffer.get(), audio_source->getSampleRate(), audio_source->getBufferUpdateLock()
	// );

	audioBuffer = temp_audio_buffer.get();

	startTimer(100);
}


bool node_comparator(envelope::node a, envelope::node b)
{
	return (a.value < b.value);
}
