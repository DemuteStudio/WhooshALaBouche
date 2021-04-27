#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "AudioBufferSource.h"
#include "AudioWaveformComponent.h"


class MyAudioSource : public AudioSource
{
public:


public:
	MyAudioSource();

	~MyAudioSource();

	void prepareToPlay(
		int samplesPerBlockExpected,
		double sampleRate
	) override;

	void releaseResources() override;

	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;

	std::shared_ptr<AudioSampleBuffer> loadRecordingBuffer(int number_of_samples_to_display);

	double getLengthInSeconds() const;

	double getSampleRate() const;

	const CriticalSection* getBufferUpdateLock() const noexcept;

	int get_sample_index() const;


private:
	class BufferPreallocationThread;

private:
	AudioTransportSource masterSource;
	std::unique_ptr<AudioBufferSource> bufferSource;

	std::shared_ptr<AudioSampleBuffer> buffer_;
	std::unique_ptr<AudioSampleBuffer> subregion_buffer_;

	AudioSampleBuffer preallocated_recording_buffer_;
	std::unique_ptr<BufferPreallocationThread> recording_buffer_preallocation_thread_;
	int num_samples_recorded_ = 0;

	double sampleRate = 0.0;
	double inputSampleRate = 0.0;
	int buffer_size;

	bool hasSubregion = false;
	double subregionStartTime = 0.0;
	double subregionEndTime = 0.0;

	class BufferPreallocationThread : public Thread
	{
	public:
		BufferPreallocationThread(
			AudioSampleBuffer& preallocatedRecordingBuffer,
			int& numSamplesRecorded,
			int numSamplesBuffer,
			int numSamplesToAllocate,
			AudioSampleBuffer& buffer
		);

		void run() override;

		const CriticalSection& getLock() const noexcept;

	private:
		AudioSampleBuffer& preallocatedRecordingBuffer;
		int& num_samples_recorded_;
		int num_samples_buffer_;
		int num_samples_to_allocate_;
		AudioSampleBuffer& buffer;
		const CriticalSection bufferUpdateLock;
	};
};
