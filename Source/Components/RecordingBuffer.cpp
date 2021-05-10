#include "RecordingBuffer.h"


RecordingBuffer::RecordingBuffer()
{
}

RecordingBuffer::~RecordingBuffer()
{
	masterSource.setSource(nullptr);
	buffer_ = nullptr;
}

void RecordingBuffer::prepareToPlay(
	int samplesPerBlockExpected,
	double masterSourceSampleRate
)
{
	inputSampleRate = masterSourceSampleRate;
	masterSource.prepareToPlay(
		samplesPerBlockExpected, masterSourceSampleRate);
}

void RecordingBuffer::releaseResources()
{
	// auto test = recording_buffer_preallocation_thread_.get();
	if (recording_buffer_preallocation_thread_.get() != nullptr)
	{
		recording_buffer_preallocation_thread_->stopThread(1000);
	}

	masterSource.releaseResources();
}

void RecordingBuffer::getNextAudioBlock(
	const AudioSourceChannelInfo& bufferToFill
)
{
	if (preallocated_recording_buffer_.getNumSamples() > 0)
	{
		for (int channel = 0; channel < 1; channel++)
		{
			jassert(preallocated_recording_buffer_.getNumSamples() <= buffer_size);
			int number_samples_to_copy = (num_samples_recorded_ + bufferToFill.numSamples < buffer_size)
				                             ? bufferToFill.numSamples
				                             : buffer_size - num_samples_recorded_;
			preallocated_recording_buffer_.copyFrom(
				channel,
				num_samples_recorded_,
				*bufferToFill.buffer,
				channel,
				bufferToFill.startSample,
				number_samples_to_copy
			);

			num_samples_recorded_ += number_samples_to_copy;

			if (bufferToFill.numSamples > number_samples_to_copy)
			{
				const int start_sample_to_copy = bufferToFill.startSample + number_samples_to_copy;
				number_samples_to_copy = bufferToFill.numSamples - number_samples_to_copy;

				preallocated_recording_buffer_.copyFrom(
					channel,
					0,
					*bufferToFill.buffer,
					channel,
					start_sample_to_copy,
					number_samples_to_copy
				);

				num_samples_recorded_ = number_samples_to_copy;
			}
		}

		buffer_->setDataToReferTo(
			preallocated_recording_buffer_.getArrayOfWritePointers(),
			preallocated_recording_buffer_.getNumChannels(),
			preallocated_recording_buffer_.getNumSamples()
		);
	}
	else
	{
		DBG("BUFFER NOT SET");
	}
}

std::shared_ptr<AudioSampleBuffer> RecordingBuffer::loadRecordingBuffer(int number_of_samples_to_display)
{
	// TODO fix hardcoded number of channels

	buffer_.reset(new AudioSampleBuffer(1, 0));
	sampleRate = inputSampleRate;
	num_samples_recorded_ = 0;
	buffer_size = number_of_samples_to_display;

	preallocated_recording_buffer_.setSize(
		1,
		(int)(buffer_size)
	);
	for (int sample = 0; sample < number_of_samples_to_display; ++sample)
	{
		preallocated_recording_buffer_.setSample(0, sample, 0);
	}

	recording_buffer_preallocation_thread_.reset(
		new BufferPreallocationThread(
			preallocated_recording_buffer_,
			num_samples_recorded_,
			(int)(number_of_samples_to_display),
			(int)(number_of_samples_to_display),
			*buffer_
		)
	);
	// recording_buffer_preallocation_thread_->startThread();

	std::unique_ptr<AudioBufferSource> tempBufferSource(
		new AudioBufferSource(buffer_.get(), false)
	);

	masterSource.setSource(
		tempBufferSource.get(),
		0,
		nullptr,
		sampleRate
	);

	bufferSource.swap(tempBufferSource);

	jassert(preallocated_recording_buffer_.getNumSamples() <= buffer_size);
	return buffer_;
}

double RecordingBuffer::getLengthInSeconds() const
{
	if (buffer_ != nullptr)
	{
		return buffer_->getNumSamples() / sampleRate;
	}
	else
	{
		return 0.0;
	}
}

double RecordingBuffer::getSampleRate() const
{
	return sampleRate;
}


const CriticalSection* RecordingBuffer::getBufferUpdateLock() const noexcept
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

int RecordingBuffer::get_sample_index() const
{
	return num_samples_recorded_;
}

// ==============================================================================
RecordingBuffer::BufferPreallocationThread::BufferPreallocationThread(
	AudioSampleBuffer& preallocatedRecordingBuffer,
	int& numSamplesRecorded,
	int numSamplesBuffer,
	int numSamplesToAllocate,
	AudioSampleBuffer& buffer
) :
	Thread("BufferPreallocationThread"),
	preallocatedRecordingBuffer(preallocatedRecordingBuffer),
	num_samples_recorded_(numSamplesRecorded),
	num_samples_buffer_(numSamplesBuffer),
	num_samples_to_allocate_(numSamplesToAllocate),
	buffer(buffer)
{
}

void RecordingBuffer::BufferPreallocationThread::run()
{
	while (!threadShouldExit())
	{
		const int preallocatedSamples = preallocatedRecordingBuffer.getNumSamples();
		if (preallocatedSamples - num_samples_recorded_ < num_samples_buffer_)
		{
			const int newNumSamples =
				preallocatedRecordingBuffer.getNumSamples() + num_samples_to_allocate_;
			const ScopedLock scopedLock(bufferUpdateLock);
			preallocatedRecordingBuffer.setSize(
				preallocatedRecordingBuffer.getNumChannels(),
				newNumSamples, true
			);
			buffer.setDataToReferTo(
				preallocatedRecordingBuffer.getArrayOfWritePointers(),
				preallocatedRecordingBuffer.getNumChannels(),
				num_samples_recorded_
			);
		}
		wait(1000);
	}
}

const CriticalSection& RecordingBuffer::BufferPreallocationThread::getLock() const noexcept
{
	return bufferUpdateLock;
}
