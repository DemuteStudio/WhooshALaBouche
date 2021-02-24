#include "Recorder.h"

Recorder::Recorder(TenFtAudioSource& audioSourceToUse): audioSource(audioSourceToUse)
{
	setLookAndFeel(&tenFtLookAndFeel);

	addAndMakeVisible(&recordButton);
	recordButton.setButtonText("Record");
	recordButton.setClickingTogglesState(false);
	recordButton.setToggleState(false,
	                            NotificationType::dontSendNotification
	);
	recordButton.onClick = [this]
	{
		recordButtonClicked();
	};

	addAndMakeVisible(&playButton);
	playButton.setButtonText("Play");
	playButton.onClick = [this]
	{
		audioSource.playAudio();
	};
	playButton.setEnabled(false);

	addAndMakeVisible(&stopButton);
	stopButton.setButtonText("Stop");
	stopButton.onClick = [this]
	{
		audioSource.stopAudio();
	};
	stopButton.setEnabled(false);

	addAndMakeVisible(&loopButton);
	loopButton.setButtonText("Loop");
	loopButton.changeWidthToFitText();
	loopButton.setToggleState(false,
	                          NotificationType::dontSendNotification
	);
	loopButton.onClick = [this]
	{
		loopButtonClicked();
	};
	loopButton.setEnabled(false);

	addAndMakeVisible(&muteButton);
	muteButton.setButtonText("Mute");
	muteButton.onClick = [this]
	{
		audioSource.muteAudio();
		waveform.refresh();
	};
	muteButton.setEnabled(false);

	addAndMakeVisible(&fadeInButton);
	fadeInButton.setButtonText("Fade In");
	fadeInButton.onClick = [this]
	{
		audioSource.fadeInAudio();
		waveform.refresh();
	};
	fadeInButton.setEnabled(false);

	addAndMakeVisible(&fadeOutButton);
	fadeOutButton.setButtonText("Fade Out");
	fadeOutButton.onClick = [this]
	{
		audioSource.fadeOutAudio();
		waveform.refresh();
	};
	fadeOutButton.setEnabled(false);

	addAndMakeVisible(&normalizeButton);
	normalizeButton.setButtonText("Normalize");
	normalizeButton.onClick = [this]
	{
		audioSource.normalizeAudio();
		waveform.refresh();
	};
	normalizeButton.setEnabled(false);

	formatManager.registerBasicFormats();

	audioSource.addListener(&clock);
	audioSource.addListener((TenFtAudioSource::Listener*)&playbackPosition);
	audioSource.onStateChange = [this](
		TenFtAudioSource::State state
	)
		{
			onAudioSourceStateChange(state);
		};

	addAndMakeVisible(&waveform);
	addAndMakeVisible(&scroller);
	addAndMakeVisible(&clock);
	waveform.addAndMakeVisible(&selectedRegion);
	waveform.addAndMakeVisible(&playbackPosition);

	addAndMakeVisible(envelope_);
	envelope_.addAndMakeVisible(envelope_selected_region_);
	envelope_.addAndMakeVisible(envelope_playback_position_);
	envelope_.addListener(&envelope_selected_region_);
	envelope_.addListener((EnvelopeComponent::Listener*)&envelope_playback_position_);
	
	waveform.addListener(&audioSource);
	waveform.addListener(&scroller);
	waveform.addListener(&selectedRegion);
	waveform.addListener((AudioWaveformComponent::Listener*)&playbackPosition);
	waveform.onPositionChange = [this](double newPosition)
	{
		audioSource.setPosition(newPosition);
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
}

Recorder::~Recorder()
{
	setLookAndFeel(nullptr);
}

void Recorder::resized()
{
	juce::Rectangle<int> bounds =
		getLocalBounds().reduced(10);
	float width = bounds.getWidth(),
	      height = bounds.getHeight(),
	      delta = 5.0f;
	int button_row_height = height / 20;
	juce::Rectangle<int> row1 = bounds.removeFromTop(button_row_height),
	                     row2 = bounds.removeFromTop(button_row_height),
	                     row3 = bounds.removeFromTop(button_row_height),
	                     row5 = bounds.removeFromBottom(button_row_height),
	                     row4 = bounds.reduced(delta);

	recordButton.setBounds(
		row1.removeFromLeft(width * 0.5f).reduced(delta)
	);
	playButton.setBounds(
		row2.removeFromLeft(width * 0.42f).reduced(delta)
	);
	stopButton.setBounds(
		row2.removeFromLeft(width * 0.42f).reduced(delta)
	);
	loopButton.setBounds(
		row2.removeFromLeft(width * 0.07f).reduced(delta)
	);
	clock.setBounds(
		row2.reduced(delta)
	);
	muteButton.setBounds(
		row3.removeFromLeft(width / 4.0f).reduced(delta)
	);
	fadeInButton.setBounds(
		row3.removeFromLeft(width / 4.0f).reduced(delta)
	);
	fadeOutButton.setBounds(
		row3.removeFromLeft(width / 4.0f).reduced(delta)
	);
	normalizeButton.setBounds(
		row3.removeFromLeft(width / 4.0f).reduced(delta)
	);
	auto main_rectangle = row4;
	waveform.setBounds(
		main_rectangle.removeFromTop(main_rectangle.getHeight()/2).reduced(delta)
	);
	envelope_.setBounds(main_rectangle.reduced(delta));
	selectedRegion.setBounds(
		row4.reduced(delta)
	);
	playbackPosition.setBounds(
		row4.reduced(delta)
	);
	scroller.setBounds(
		row5.reduced(delta).toNearestInt()
	);
}

void Recorder::paint(Graphics& g)
{
	g.fillAll(findColour(
		AudioWaveformOpenGLComponent::ColourIds::waveformBackgroundColour
	).contrasting(0.2f));
}

// ==============================================================================

void Recorder::recordButtonClicked()
{
	!recordButton.getToggleState() ? enableRecording() : disableRecording();
}

void Recorder::enableRecording()
{
	waveform.clearWaveform();
	audioSource.unloadAudio();
	scroller.disable();

	AudioSampleBuffer* tempAudioBuffer = audioSource.loadRecordingBuffer();
	waveform.loadWaveform(
		tempAudioBuffer, audioSource.getSampleRate(), audioSource.getBufferUpdateLock()
	);
	envelope_.loadWaveform(
		tempAudioBuffer, audioSource.getSampleRate(), audioSource.getBufferUpdateLock()
	);

	audioBuffer.reset(tempAudioBuffer);

	startTimer(100);

	enableButtons({
		              &playButton, &stopButton, &loopButton,
		              &muteButton, &fadeInButton, &fadeOutButton, &normalizeButton
	              }, false);
	recordButton.setButtonText("Stop Recording");
	recordButton.setToggleState(true, NotificationType::dontSendNotification);
}

void Recorder::disableRecording()
{
	stopTimer();

	audioSource.stopRecording();

	enableButtons({
		              &playButton, &stopButton, &loopButton,
		              &muteButton, &fadeInButton, &fadeOutButton, &normalizeButton
	              }, true);
	recordButton.setButtonText("Record");
	recordButton.setToggleState(false, NotificationType::dontSendNotification);
}

void Recorder::loopButtonClicked()
{
	audioSource.setLooping(loopButton.getToggleState());
}

void Recorder::onAudioSourceStateChange(
	TenFtAudioSource::State state
)
{
	if (state == TenFtAudioSource::Stopped)
	{
		setupButton(playButton, "Play", true);
		setupButton(stopButton, "Stop", false);
		waveform.clearSelectedRegion();
	}
	else if (state == TenFtAudioSource::Playing)
	{
		setupButton(playButton, "Pause", true);
		setupButton(stopButton, "Stop", true);
	}
	else if (state == TenFtAudioSource::Paused)
	{
		setupButton(playButton, "Play", true);
		setupButton(stopButton, "Return To Zero", true);
	}
}

void Recorder::setupButton(
	TextButton& button, std::string buttonText, bool enabled
)
{
	button.setButtonText(buttonText);
	button.setEnabled(enabled);
}

void Recorder::enableButtons(
	std::initializer_list<Button*> buttons, bool enable
)
{
	for (Button* button : buttons)
	{
		button->setEnabled(enable);
	}
}

void Recorder::timerCallback()
{
	double newEndTime = (double)audioBuffer->getNumSamples() / audioSource.getSampleRate();
	waveform.updateVisibleRegion(0.0, newEndTime);
	envelope_.updateVisibleRegion(0.0, newEndTime);
}
