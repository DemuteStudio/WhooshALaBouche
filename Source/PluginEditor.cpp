#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WhooshGeneratorAudioProcessorEditor::WhooshGeneratorAudioProcessorEditor(WhooshGeneratorAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p), recorder_(), parameters_box_(p.getBlockSize(), p.sample_rate),
	  envelope_array_(&p.rms_envelope)
{
	setLookAndFeel(&tenFtLookAndFeel);

	audio_source = &audioProcessor.getAudioSource();
	addAndMakeVisible(recorder_);
	addAndMakeVisible(parameters_box_);

	parameters_box_.add_listener(this);

	//Previous Recorder
	recorder_.recordButton.onClick = [this]
	{
		recordButtonClicked();
	};

	recorder_.playButton.onClick = [this]
	{
		audio_source->playAudio();
	};

	recorder_.stopButton.onClick = [this]
	{
		audio_source->stopAudio();
	};

	recorder_.loopButton.onClick = [this]
	{
		loopButtonClicked();
	};

	recorder_.muteButton.onClick = [this]
	{
		audio_source->muteAudio();
		waveform.refresh();
	};

	recorder_.fadeInButton.onClick = [this]
	{
		audio_source->fadeInAudio();
		waveform.refresh();
	};

	recorder_.fadeOutButton.onClick = [this]
	{
		audio_source->fadeOutAudio();
		waveform.refresh();
	};

	recorder_.normalizeButton.onClick = [this]
	{
		audio_source->normalizeAudio();
		waveform.refresh();
	};

	formatManager.registerBasicFormats();

	audio_source->addListener(&recorder_.clock);
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
	// Make sure you set the size of the component after
	// you add any child components.
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

	recorder_.setBounds(rectangle.removeFromTop(row_height * 3));

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


void WhooshGeneratorAudioProcessorEditor::recordButtonClicked()
{
	!recorder_.recordButton.getToggleState() ? enableRecording() : disableRecording();
}

void WhooshGeneratorAudioProcessorEditor::enableRecording()
{
	waveform.clearWaveform();
	envelope_.clearWaveform();
	audio_source->unloadAudio();
	scroller.disable();

	AudioSampleBuffer* tempAudioBuffer = audio_source->loadRecordingBuffer();
	waveform.loadWaveform(
		tempAudioBuffer, audio_source->getSampleRate(), audio_source->getBufferUpdateLock()
	);
	const float rms_sample_rate = 1000. / parameters_box_.rms_length_slider->getValue();
	envelope* temp_envelope_buffer = audioProcessor.load_new_envelope();
	envelope_.load_envelope(
		temp_envelope_buffer, rms_sample_rate, audio_source->getBufferUpdateLock()
	);

	audioBuffer.reset(tempAudioBuffer);

	startTimer(100);

	recorder_.enableButtons({
		                        &recorder_.playButton, &recorder_.stopButton, &recorder_.loopButton,
		                        &recorder_.muteButton, &recorder_.fadeInButton, &recorder_.fadeOutButton,
		                        &recorder_.normalizeButton
	                        }, false);
	recorder_.recordButton.setButtonText("Stop Recording");
	recorder_.recordButton.setToggleState(true, NotificationType::dontSendNotification);
}

void WhooshGeneratorAudioProcessorEditor::disableRecording()
{
	stopTimer();

	audio_source->stopRecording();

	recorder_.enableButtons({
		                        &recorder_.playButton, &recorder_.stopButton, &recorder_.loopButton,
		                        &recorder_.muteButton, &recorder_.fadeInButton, &recorder_.fadeOutButton,
		                        &recorder_.normalizeButton
	                        }, true);
	recorder_.recordButton.setButtonText("Record");
	recorder_.recordButton.setToggleState(false, NotificationType::dontSendNotification);
}

void WhooshGeneratorAudioProcessorEditor::loopButtonClicked()
{
	audio_source->setLooping(recorder_.loopButton.getToggleState());
}

void WhooshGeneratorAudioProcessorEditor::onAudioSourceStateChange(
	my_audio_source::State state
)
{
	if (state == my_audio_source::Stopped)
	{
		recorder_.setupButton(recorder_.playButton, "Play", true);
		recorder_.setupButton(recorder_.stopButton, "Stop", false);
		waveform.clearSelectedRegion();
	}
	else if (state == my_audio_source::Playing)
	{
		recorder_.setupButton(recorder_.playButton, "Pause", true);
		recorder_.setupButton(recorder_.stopButton, "Stop", true);
	}
	else if (state == my_audio_source::Paused)
	{
		recorder_.setupButton(recorder_.playButton, "Play", true);
		recorder_.setupButton(recorder_.stopButton, "Return To Zero", true);
	}
}
