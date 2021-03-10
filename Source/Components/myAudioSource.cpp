#include "myAudioSource.h"


my_audio_source::my_audio_source()
{
	masterSource.addChangeListener(this);
	startTimer(100);
}

my_audio_source::~my_audio_source()
{
	stopTimer();
	masterSource.setSource(nullptr);
	buffer = nullptr;
}

void my_audio_source::prepareToPlay(
	int samplesPerBlockExpected,
	double masterSourceSampleRate
)
{
	inputSampleRate = masterSourceSampleRate;
	masterSource.prepareToPlay(
		samplesPerBlockExpected, masterSourceSampleRate);
}

void my_audio_source::releaseResources()
{
	recording_buffer_preallocation_thread_->stopThread(1000);

	masterSource.releaseResources();
}

void my_audio_source::getNextAudioBlock(
	const AudioSourceChannelInfo& bufferToFill
)
{
	for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); channel++)
	{
		int number_samples_to_copy = (numSamplesRecorded + bufferToFill.numSamples < preallocated_recording_buffer_.
			                             getNumSamples())
			                             ? bufferToFill.numSamples
			                             : preallocated_recording_buffer_.getNumSamples() - numSamplesRecorded;
		preallocated_recording_buffer_.copyFrom(
			channel,
			numSamplesRecorded,
			*bufferToFill.buffer,
			channel,
			bufferToFill.startSample,
			number_samples_to_copy
		);

		numSamplesRecorded += number_samples_to_copy;

		if (bufferToFill.numSamples > number_samples_to_copy)
		{
			int start_sample_to_copy = bufferToFill.startSample + number_samples_to_copy;
			number_samples_to_copy = bufferToFill.numSamples - number_samples_to_copy;

			preallocated_recording_buffer_.copyFrom(
				channel,
				0,
				*bufferToFill.buffer,
				channel,
				start_sample_to_copy,
				number_samples_to_copy
			);

			numSamplesRecorded = number_samples_to_copy;
		}
	}

	numSamplesRecorded += bufferToFill.numSamples;

	buffer->setDataToReferTo(
		preallocated_recording_buffer_.getArrayOfWritePointers(),
		bufferToFill.buffer->getNumChannels(),
		numSamplesRecorded
	);
}

void my_audio_source::unloadAudio()
{
}

std::shared_ptr<AudioSampleBuffer> my_audio_source::loadRecordingBuffer()
{
	// TODO fix hardcoded number of channels
	buffer.reset(new AudioSampleBuffer(2, 0));
	sampleRate = inputSampleRate;
	numSamplesRecorded = 0;

	preallocated_recording_buffer_.setSize(
		1,
		(int)(10 * sampleRate)
	);
	recording_buffer_preallocation_thread_.reset(
		new BufferPreallocationThread(
			preallocated_recording_buffer_,
			numSamplesRecorded,
			(int)(10 * sampleRate),
			(int)(30 * sampleRate),
			*buffer
		)
	);
	recording_buffer_preallocation_thread_->startThread();

	std::unique_ptr<AudioBufferSource> tempBufferSource(
		new AudioBufferSource(buffer.get(), false)
	);

	masterSource.setSource(
		tempBufferSource.get(),
		0,
		nullptr,
		sampleRate
	);

	bufferSource.swap(tempBufferSource);

	return buffer;
}

void my_audio_source::muteAudio()
{
	int startSample = (int)(subregionStartTime * sampleRate),
	    numSamples = (int)((subregionEndTime - subregionStartTime) * sampleRate);

	buffer->clear(startSample, numSamples);
}

void my_audio_source::fadeInAudio()
{
	int startSample = (int)(subregionStartTime * sampleRate),
	    numSamples = (int)((subregionEndTime - subregionStartTime) * sampleRate);
	float magnitude = buffer->getMagnitude(startSample, numSamples),
	      gain = Decibels::decibelsToGain(magnitude);

	buffer->applyGainRamp(startSample, numSamples, 0.0f, gain);
}

void my_audio_source::fadeOutAudio()
{
	int startSample = (int)(subregionStartTime * sampleRate),
	    numSamples = (int)((subregionEndTime - subregionStartTime) * sampleRate);
	float magnitude = buffer->getMagnitude(startSample, numSamples),
	      gain = Decibels::decibelsToGain(magnitude);

	buffer->applyGainRamp(startSample, numSamples, gain, 0.0f);
}

void my_audio_source::normalizeAudio()
{
	int startSample = (int)(subregionStartTime * sampleRate),
	    numSamples = (int)((subregionEndTime - subregionStartTime) * sampleRate);
	float magnitude = buffer->getMagnitude(startSample, numSamples),
	      gain = Decibels::decibelsToGain(magnitude);

	buffer->applyGain(startSample, numSamples, gain);
}

double my_audio_source::getCurrentPosition() const
{
	double currentPosition = masterSource.getCurrentPosition();
	if (hasSubregion)
	{
		currentPosition += subregionStartTime;
	}
	return currentPosition;
}

double my_audio_source::getLengthInSeconds() const
{
	if (buffer != nullptr)
	{
		return buffer->getNumSamples() / sampleRate;
	}
	else
	{
		return 0.0;
	}
}

double my_audio_source::getSampleRate() const
{
	return sampleRate;
}

void my_audio_source::setPosition(double newPosition)
{
	masterSource.setPosition(newPosition);
}

void my_audio_source::setLooping(bool shouldLoop)
{
	bufferSource->setLooping(shouldLoop);
}

void my_audio_source::addListener(Listener* newListener)
{
	listeners.add(newListener);
}

void my_audio_source::removeListener(Listener* listener)
{
	listeners.remove(listener);
}

const CriticalSection* my_audio_source::getBufferUpdateLock() const noexcept
{
	if (recording_buffer_preallocation_thread_)
	{
		return &recording_buffer_preallocation_thread_->getLock();
	}
	else
	{
		return nullptr;
	}
}

// ==============================================================================

// void my_audio_source::selectedRegionCreated(AudioWaveformComponent* waveform)
// {
// 	loadAudioSubregion(
// 		waveform->getSelectedRegionStartTime(),
// 		waveform->getSelectedRegionEndTime(),
// 		true,
// 		bufferSource->isLooping()
// 	);
//
// 	if (state == Playing)
// 	{
// 		masterSource.start();
// 	}
// }
//
// void my_audio_source::selectedRegionCleared(AudioWaveformComponent* waveform)
// {
// 	if (hasSubregion && !waveform->getHasSelectedRegion())
// 	{
// 		loadAudioSubregion(
// 			0.0,
// 			getLengthInSeconds(),
// 			false,
// 			bufferSource->isLooping()
// 		);
//
// 		masterSource.start();
// 	}
// }

void my_audio_source::timerCallback()
{
	listeners.call([this](Listener& l) { l.currentPositionChanged(this); });
}

// void my_audio_source::changeListenerallback(
// 	ChangeBroadcaster*
// )
// {
// 	if (masterSource.isPlaying() && state == StartRecording)
// 	{
// 		changeState(Recording);
// 	}
// 	else if (masterSource.isPlaying() && state != StartRecording)
// 	{
// 		changeState(Playing);
// 	}
// 	else if (state == Pausing)
// 	{
// 		changeState(Paused);
// 	}
// 	else if (hasSubregion)
// 	{
// 		setPosition(0.0);
// 		changeState(Paused);
// 	}
// 	else
// 	{
// 		changeState(Stopped);
// 	}
// }


void my_audio_source::loadAudioSubregion(
	double startTime,
	double endTime,
	bool subregionSelected,
	bool shouldLoop
)
{
	subregionStartTime = startTime;
	subregionEndTime = endTime;
	hasSubregion = subregionSelected;

	double lengthInSeconds = subregionEndTime - subregionStartTime;
	int startSample = (int)(subregionStartTime * sampleRate),
	    numSamples = (int)(lengthInSeconds * sampleRate);

	subregionBuffer.reset(
		new AudioSampleBuffer(
			buffer->getArrayOfWritePointers(),
			buffer->getNumChannels(),
			startSample,
			numSamples
		)
	);

	std::unique_ptr<AudioBufferSource> tempBufferSource(
		new AudioBufferSource(subregionBuffer.get(), shouldLoop)
	);

	masterSource.setSource(
		tempBufferSource.get(),
		0,
		nullptr,
		sampleRate
	);

	bufferSource.swap(tempBufferSource);
}

// ==============================================================================

my_audio_source::BufferPreallocationThread::BufferPreallocationThread(
	AudioSampleBuffer& preallocatedRecordingBuffer,
	int& numSamplesRecorded,
	int numSamplesBuffer,
	int numSamplesToAllocate,
	AudioSampleBuffer& buffer
) :
	Thread("BufferPreallocationThread"),
	preallocatedRecordingBuffer(preallocatedRecordingBuffer),
	numSamplesRecorded(numSamplesRecorded),
	numSamplesBuffer(numSamplesBuffer),
	numSamplesToAllocate(numSamplesToAllocate),
	buffer(buffer)
{
}

void my_audio_source::BufferPreallocationThread::run()
{
	while (!threadShouldExit())
	{
		int preallocatedSamples = preallocatedRecordingBuffer.getNumSamples();
		if (preallocatedSamples - numSamplesRecorded < numSamplesBuffer)
		{
			int newNumSamples =
				preallocatedRecordingBuffer.getNumSamples() + numSamplesToAllocate;
			const ScopedLock scopedLock(bufferUpdateLock);
			preallocatedRecordingBuffer.setSize(
				preallocatedRecordingBuffer.getNumChannels(),
				newNumSamples, true
			);
			buffer.setDataToReferTo(
				preallocatedRecordingBuffer.getArrayOfWritePointers(),
				preallocatedRecordingBuffer.getNumChannels(),
				numSamplesRecorded
			);
		}
		wait(1000);
	}
}

const CriticalSection& my_audio_source::BufferPreallocationThread::getLock() const noexcept
{
	return bufferUpdateLock;
}
