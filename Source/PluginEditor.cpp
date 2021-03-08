#include "PluginProcessor.h"
#include "PluginEditor.h"
// #include <iostream>

//==============================================================================
WhooshGeneratorAudioProcessorEditor::WhooshGeneratorAudioProcessorEditor(WhooshGeneratorAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p), buttons_panel_(), parameters_box_(p.getBlockSize(), p.sample_rate),
	  envelope_array_(p.rms_envelope.get())
{
	setLookAndFeel(&tenFtLookAndFeel);

	audio_source = &audioProcessor.getAudioSource();
	addAndMakeVisible(buttons_panel_);
	addAndMakeVisible(parameters_box_);

	parameters_box_.add_listener(this);

	//Previous Recorder
	buttons_panel_.recordButton.onClick = [this]
	{
		recordButtonClicked();
	};

	buttons_panel_.playButton.onClick = [this]
	{
		audio_source->playAudio();
	};

	buttons_panel_.stopButton.onClick = [this]
	{
		audio_source->stopAudio();
	};

	buttons_panel_.loopButton.onClick = [this]
	{
		loopButtonClicked();
	};

	buttons_panel_.clean_envelope_button.onClick = [this]
	{
		clean_envelope();
	};

	buttons_panel_.fadeInButton.onClick = [this]
	{
		audio_source->fadeInAudio();
		waveform.refresh();
	};

	buttons_panel_.fadeOutButton.onClick = [this]
	{
		audio_source->fadeOutAudio();
		waveform.refresh();
	};

	buttons_panel_.normalizeButton.onClick = [this]
	{
		audio_source->normalizeAudio();
		waveform.refresh();
	};

	formatManager.registerBasicFormats();

	audio_source->addListener(&buttons_panel_.clock);
	audio_source->addListener((my_audio_source::Listener*)&playbackPosition);
	audio_source->onStateChange = [this](
		my_audio_source::State state
	)
		{
			onAudioSourceStateChange(state);
		};

	addAndMakeVisible(&waveform);
	waveform.addAndMakeVisible(&selectedRegion);
	waveform.addAndMakeVisible(&playbackPosition);

	addAndMakeVisible(envelope_);
	envelope_.addAndMakeVisible(envelope_selected_region_);
	envelope_.addAndMakeVisible(envelope_playback_position_);
	envelope_.addListener(&envelope_selected_region_);
	envelope_.addListener(&envelope_playback_position_);

	addAndMakeVisible(&scroller);

	waveform.addListener(audio_source);
	waveform.addListener(&scroller);
	waveform.addListener(&selectedRegion);
	waveform.addListener(&playbackPosition);
	waveform.onPositionChange = [this](double newPosition)
	{
		audio_source->setPosition(newPosition);
	};

	scroller.addListener(&waveform);
	scroller.addListener(&envelope_);
	scroller.onMouseWheelMove = [this](
		const MouseEvent& event,
		const MouseWheelDetails& wheelDetails
	)
		{
			waveform.mouseWheelMove(event, wheelDetails);
		};

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
	setSize(1000, 900);
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
	scroller.setBounds(rectangle.removeFromBottom(row_height).reduced(delta));


	auto main_rectangle = rectangle;
	waveform.setBounds(
		main_rectangle.removeFromTop(main_rectangle.getHeight() / 2).reduced(delta)
	);
	playbackPosition.setBounds(
		waveform.getBounds()
	);
	envelope_playback_position_.setBounds(
		envelope_.getBounds()
	);
	envelope_.setBounds(main_rectangle.reduced(delta));
	selectedRegion.setBounds(
		rectangle.reduced(delta)
	);
}

void WhooshGeneratorAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	if (slider->getName() == "threshold")
	{
		audioProcessor.threshold_value = slider->getValue();
	}
	if (slider->getName() == "rms_length")
	{
		audioProcessor.rms_blocks_length = get_number_of_blocks_from_milliseconds(
			processor.getSampleRate(), slider->getValue(), processor.getBlockSize());
	}
}


int WhooshGeneratorAudioProcessorEditor::get_number_of_blocks_from_milliseconds(
	double sample_rate, const float length_in_milliseconds, int samples_per_block)
{
	return (int)(((sample_rate / 1000) * length_in_milliseconds) / samples_per_block);
}

void WhooshGeneratorAudioProcessorEditor::timerCallback()
{
	double newEndTime = (double)audioBuffer->getNumSamples() / audio_source->getSampleRate();
	waveform.updateVisibleRegion(0.0, newEndTime);
	envelope_.updateVisibleRegion(0.0, newEndTime);

	// int rms_sample_rate = audioProcessor.getBlockSize()
	// double newEndTime = (double)audioBuffer->getNumSamples() / audio_source->getSampleRate();
	// double nexStartTime = newEndTime - 
	// waveform.updateVisibleRegion(0.0, newEndTime);
	// envelope_.updateVisibleRegion(0.0, newEndTime);
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


void WhooshGeneratorAudioProcessorEditor::recordButtonClicked()
{
	!buttons_panel_.recordButton.getToggleState() ? enableRecording() : disableRecording();
}

void WhooshGeneratorAudioProcessorEditor::enableRecording()
{
	waveform.clearWaveform();
	envelope_.clearWaveform();
	audio_source->unloadAudio();
	scroller.disable();

	std::shared_ptr<AudioSampleBuffer> tempAudioBuffer = audio_source->loadRecordingBuffer();
	waveform.loadWaveform(
		tempAudioBuffer.get(), audio_source->getSampleRate(), audio_source->getBufferUpdateLock()
	);
	// const float rms_sample_rate = 1000. / parameters_box_.rms_length_slider->getValue();

	envelope* temp_envelope_buffer = audioProcessor.load_new_envelope();
	envelope_.load_envelope(
		temp_envelope_buffer, tempAudioBuffer.get(), audio_source->getSampleRate(), audio_source->getBufferUpdateLock()
	);

	audioBuffer = tempAudioBuffer.get();

	startTimer(100);

	buttons_panel_.enableButtons({
		                             &buttons_panel_.playButton, &buttons_panel_.stopButton, &buttons_panel_.loopButton,
		                             &buttons_panel_.clean_envelope_button, &buttons_panel_.fadeInButton,
		                             &buttons_panel_.fadeOutButton,
		                             &buttons_panel_.normalizeButton, &buttons_panel_.sendEnvelopeButton
	                             }, false);
	buttons_panel_.recordButton.setButtonText("Stop Recording");
	buttons_panel_.recordButton.setToggleState(true, NotificationType::dontSendNotification);
}

void WhooshGeneratorAudioProcessorEditor::disableRecording()
{
	stopTimer();

	audio_source->stopRecording();

	buttons_panel_.enableButtons({
		                             &buttons_panel_.playButton, &buttons_panel_.stopButton, &buttons_panel_.loopButton,
		                             &buttons_panel_.clean_envelope_button, &buttons_panel_.fadeInButton,
		                             &buttons_panel_.fadeOutButton,
		                             &buttons_panel_.normalizeButton, &buttons_panel_.sendEnvelopeButton
	                             }, true);
	buttons_panel_.recordButton.setButtonText("Record");
	buttons_panel_.recordButton.setToggleState(false, NotificationType::dontSendNotification);
}

void WhooshGeneratorAudioProcessorEditor::loopButtonClicked()
{
	audio_source->setLooping(buttons_panel_.loopButton.getToggleState());
}

bool node_comparator(envelope::node a, envelope::node b)
{
	return (a.value < b.value);
}

void WhooshGeneratorAudioProcessorEditor::clean_envelope()
{
	float threshold = 0.3;
	std::vector<envelope> all_individual_envelopes;

	auto it = std::find_if(audioProcessor.rms_envelope->list_.begin(), audioProcessor.rms_envelope->list_.end(),
	                       [threshold](envelope::node node) { return (node.value >= threshold); });


	decltype(it) first_node;
	decltype(it) max_node;
	decltype(it) last_node;

	while (true)
	{
		if (it != audioProcessor.rms_envelope->list_.end())
		{
			first_node = it;
		}
		else
		{
			break;
		}

		it = std::find_if_not(first_node, audioProcessor.rms_envelope->list_.end(),
		                      [threshold](envelope::node node) { return (node.value >= threshold); });

		if (it != audioProcessor.rms_envelope->list_.end())
		{
			last_node = it;
		}
		else
		{
			break;
		}

		it = std::max_element(first_node, last_node, node_comparator);

		max_node = it;

		envelope temp_envelope = envelope();
		temp_envelope.add(envelope::node(first_node->sample, first_node->value));
		temp_envelope.add(envelope::node(max_node->sample, max_node->value));
		temp_envelope.add(envelope::node(last_node->sample, last_node->value));

		all_individual_envelopes.emplace_back(temp_envelope);


		it = std::find_if(last_node + 1, audioProcessor.rms_envelope->list_.end(),
		                  [threshold](envelope::node node) { return (node.value >= threshold); });
	}
	audioProcessor.rms_envelope_clean.reset(new envelope());
	for (auto envelope_ : all_individual_envelopes)
	{
		// for (int index = 0; index < 3; ++index)
		// {
		// 	audioProcessor.rms_envelope_clean->add(envelope_[index]);
		// }
		envelope::node temp_node(envelope_[0].sample, 0);
		audioProcessor.rms_envelope_clean->add(temp_node);
		temp_node = envelope::node(envelope_[1].sample, envelope_[1].value);
		audioProcessor.rms_envelope_clean->add(temp_node);
		temp_node = envelope::node(envelope_[2].sample, 0);
		audioProcessor.rms_envelope_clean->add(temp_node);
	}
	envelope_.load_envelope(audioProcessor.rms_envelope_clean.get());
}

void WhooshGeneratorAudioProcessorEditor::onAudioSourceStateChange(
	my_audio_source::State state
)
{
	if (state == my_audio_source::Stopped)
	{
		buttons_panel_.setupButton(buttons_panel_.playButton, "Play", true);
		buttons_panel_.setupButton(buttons_panel_.stopButton, "Stop", false);
		waveform.clearSelectedRegion();
	}
	else if (state == my_audio_source::Playing)
	{
		buttons_panel_.setupButton(buttons_panel_.playButton, "Pause", true);
		buttons_panel_.setupButton(buttons_panel_.stopButton, "Stop", true);
	}
	else if (state == my_audio_source::Paused)
	{
		buttons_panel_.setupButton(buttons_panel_.playButton, "Play", true);
		buttons_panel_.setupButton(buttons_panel_.stopButton, "Return To Zero", true);
	}
}
