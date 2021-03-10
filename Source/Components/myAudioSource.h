#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "AudioBufferSource.h"
#include "AudioWaveformComponent.h"


class my_audio_source : public AudioSource,
                         public AudioWaveformComponent::Listener,
                         private ChangeListener,
                         private Timer
{
public:
	class Listener
	{
	public:
		virtual ~Listener()
		{
		}

		virtual void currentPositionChanged(my_audio_source*)
		{
		}

		virtual void stateChanged(my_audio_source*)
		{
		}
	};


public:
	my_audio_source();

	~my_audio_source();

	void prepareToPlay(
		int samplesPerBlockExpected,
		double sampleRate
	) override;

	void releaseResources() override;

	void getNextAudioBlock(
		const AudioSourceChannelInfo& bufferToFill
	) override;

	void unloadAudio();

	std::shared_ptr<AudioSampleBuffer> loadRecordingBuffer();

	void stopRecording();

	void playAudio();

	void stopAudio();

	void pauseAudio();

	void muteAudio();

	void fadeInAudio();

	void fadeOutAudio();

	void normalizeAudio();

	double getCurrentPosition() const;

	double getLengthInSeconds() const;

	double getSampleRate() const;

	void setPosition(double newPosition);

	void setLooping(bool shouldLoop);

	void addListener(Listener* newListener);

	void removeListener(Listener* listener);

	const CriticalSection* getBufferUpdateLock() const noexcept;

private:
	void selectedRegionCreated(AudioWaveformComponent* waveform) override;

	void selectedRegionCleared(AudioWaveformComponent* waveform) override;

	void timerCallback() override;

	void changeListenerCallback(ChangeBroadcaster* broadcaster) override;

	void loadAudioSubregion(
		double startTime,
		double endTime,
		bool subregionSelected,
		bool shouldLoop
	);

private:
	class BufferPreallocationThread;

private:
	AudioTransportSource masterSource;
	std::unique_ptr<AudioBufferSource> bufferSource;

	// AudioSampleBuffer* buffer = nullptr;
	std::shared_ptr<AudioSampleBuffer> buffer;
	std::unique_ptr<AudioSampleBuffer> subregionBuffer;

	AudioSampleBuffer preallocated_recording_buffer_;
	std::unique_ptr<BufferPreallocationThread> recording_buffer_preallocation_thread_;
	int numSamplesRecorded = 0;

	double sampleRate = 0.0;
	double inputSampleRate = 0.0;

	bool hasSubregion = false;
	double subregionStartTime = 0.0;
	double subregionEndTime = 0.0;
	
	ListenerList<Listener> listeners;


private:
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
		int& numSamplesRecorded;
		int numSamplesBuffer;
		int numSamplesToAllocate;
		AudioSampleBuffer& buffer;
		const CriticalSection bufferUpdateLock;
	};
};
