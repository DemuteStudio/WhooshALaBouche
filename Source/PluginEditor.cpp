#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WhooshGeneratorAudioProcessorEditor::WhooshGeneratorAudioProcessorEditor(WhooshGeneratorAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p), buttons_panel_(), parameters_box_(p.getBlockSize(), p.sample_rate), out_parameters_box_(p.get_state())
	  // envelope_array_(p.rms_envelope.get()) 
{
	setLookAndFeel(&tenFtLookAndFeel);
	double time_to_display = 3.;
	number_of_samples_to_display = time_to_display * p.sample_rate;

	audio_source = &audioProcessor.getAudioSource();
	addAndMakeVisible(buttons_panel_);
	addAndMakeVisible(parameters_box_);

	parameters_box_.add_listener(this);

	buttons_panel_.clean_envelope_button.onClick = [this]
	{
		clean_envelope();
	};


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

	bool connected = osc_sender_.connect("127.0.0.1", 8000);
	if (connected)
	{
		DBG("Connected");
	}
	else
	{
		DBG("Not Connected");
	}

	buttons_panel_.sendEnvelopeButton.onClick = [this]
	{
		send_osc_message("");
	};

	//====================================================================
	setSize(1000, 700);
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

	buttons_panel_.setBounds(rectangle.removeFromTop(row_height * 3));

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
		const int threshold_value_db=  Decibels::gainToDecibels(slider->getValue()) ;
		parameters_box_.threshold_value_label.setText(std::to_string(threshold_value_db) + " dB", dontSendNotification);
	}
	if (slider->getName() == "rms_length")
	{
		audioProcessor.rms_blocks_length = slider->getValue();
		const int rms_length_value = slider->getValue()* ((audioProcessor.getBlockSize()/audioProcessor.sample_rate)*1000.);
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
	// volume_envelope_.updateVisibleRegion(end_sample, number_of_samples_to_display);

	// int rms_sample_rate = audioProcessor.getBlockSize()
	// double newEndTime = (double)audioBuffer->getNumSamples() / audio_source->getSampleRate();
	// double nexStartTime = newEndTime - 
	// waveform.updateVisibleRegion(0.0, newEndTime);
	// envelope_.updateVisibleRegion(0.0, newEndTime);

	out_parameters_box_.set_slider_value(out_parameters_box::volume, audioProcessor.last_rms_value);
	out_parameters_box_.set_slider_value(out_parameters_box::frequency, frequency_peak);

	// auto message = juce::MidiMessage::controllerEvent(1, 0, 0.5);
	// message.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001 - startTime);
	// send(message);
}

void WhooshGeneratorAudioProcessorEditor::send_osc_message(String message)
{
	OSCMessage my_message("/blob");
	my_message.addBlob(audioProcessor.get_envelope_memory_block());


	osc_sender_.send(my_message);
	osc_sender_.send<float>("/filter", 1.0);

	// std::FileStorage fs("test.yml", FileStorage::WRITE);


	MemoryBlock memoryBlock = audioProcessor.get_envelope_memory_block();
	for (auto memory_block : memoryBlock)
	{
		DBG(memory_block);
	}
}


void WhooshGeneratorAudioProcessorEditor::enableRecording()
{
	waveform.clearWaveform();
	// volume_envelope_.clearWaveform();
	audio_source->unloadAudio();

	std::shared_ptr<AudioSampleBuffer> tempAudioBuffer = audio_source->loadRecordingBuffer(number_of_samples_to_display);
	waveform.loadWaveform(
		tempAudioBuffer.get(), audio_source->getSampleRate(), audio_source->getBufferUpdateLock()
	);

	envelope* temp_envelope_buffer = audioProcessor.load_new_envelope();
	// volume_envelope_.load_envelope(
	// 	temp_envelope_buffer, tempAudioBuffer.get(), audio_source->getSampleRate(), audio_source->getBufferUpdateLock()
	// );

	audioBuffer = tempAudioBuffer.get();

	startTimer(100);

	buttons_panel_.enableButtons({
		                             &buttons_panel_.clean_envelope_button,  &buttons_panel_.sendEnvelopeButton
	                             }, false);
}



bool node_comparator(envelope::node a, envelope::node b)
{
	return (a.value < b.value);
}

void WhooshGeneratorAudioProcessorEditor::clean_envelope()
{
	// float threshold = 0.3;
	// std::vector<envelope> all_individual_envelopes;
	//
	// auto it = std::find_if(audioProcessor.rms_envelope->list_.begin(), audioProcessor.rms_envelope->list_.end(),
	//                        [threshold](envelope::node node) { return (node.value >= threshold); });
	//
	//
	// decltype(it) first_node;
	// decltype(it) max_node;
	// decltype(it) last_node;
	//
	// while (true)
	// {
	// 	if (it != audioProcessor.rms_envelope->list_.end())
	// 	{
	// 		first_node = it;
	// 	}
	// 	else
	// 	{
	// 		break;
	// 	}
	//
	// 	it = std::find_if_not(first_node, audioProcessor.rms_envelope->list_.end(),
	// 	                      [threshold](envelope::node node) { return (node.value >= threshold); });
	//
	// 	if (it != audioProcessor.rms_envelope->list_.end())
	// 	{
	// 		last_node = it;
	// 	}
	// 	else
	// 	{
	// 		break;
	// 	}
	//
	// 	it = std::max_element(first_node, last_node, node_comparator);
	//
	// 	max_node = it;
	//
	// 	envelope temp_envelope = envelope();
	// 	temp_envelope.add(envelope::node(first_node->sample, first_node->value));
	// 	temp_envelope.add(envelope::node(max_node->sample, max_node->value));
	// 	temp_envelope.add(envelope::node(last_node->sample, last_node->value));
	//
	// 	all_individual_envelopes.emplace_back(temp_envelope);
	//
	//
	// 	it = std::find_if(last_node + 1, audioProcessor.rms_envelope->list_.end(),
	// 	                  [threshold](envelope::node node) { return (node.value >= threshold); });
	// }
	// audioProcessor.rms_envelope_clean.reset(new envelope());
	// for (auto envelope_ : all_individual_envelopes)
	// {
	// 	// for (int index = 0; index < 3; ++index)
	// 	// {
	// 	// 	audioProcessor.rms_envelope_clean->add(envelope_[index]);
	// 	// }
	// 	envelope::node temp_node(envelope_[0].sample, 0);
	// 	audioProcessor.rms_envelope_clean->add(temp_node);
	// 	temp_node = envelope::node(envelope_[1].sample, envelope_[1].value);
	// 	audioProcessor.rms_envelope_clean->add(temp_node);
	// 	temp_node = envelope::node(envelope_[2].sample, 0);
	// 	audioProcessor.rms_envelope_clean->add(temp_node);
	// }
	// volume_envelope_.load_envelope(audioProcessor.rms_envelope_clean.get());
}

