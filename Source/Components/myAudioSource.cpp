/*
==============================================================================

AudioFileTransportSource.cpp
Created: 29 May 2018 11:17:55am
Author:  Nikolay Tsenkov

==============================================================================
*/


#include "myAudioSource.h"


my_audio_source::my_audio_source ()
{
    masterSource.addChangeListener (this);
    startTimer (100);
}

my_audio_source::~my_audio_source ()
{
    stopTimer ();
    masterSource.setSource (nullptr);
    buffer = nullptr;
}

void my_audio_source::prepareToPlay (
    int samplesPerBlockExpected,
    double masterSourceSampleRate
)
{
    inputSampleRate = masterSourceSampleRate;
    masterSource.prepareToPlay (
        samplesPerBlockExpected, masterSourceSampleRate);
}

void my_audio_source::releaseResources ()
{
    if (state == Recording)
    {
        recordingBufferPreallocationThread->stopThread (1000);
    }

    masterSource.releaseResources ();
}

void my_audio_source::getNextAudioBlock (
    const AudioSourceChannelInfo& bufferToFill
)
{
    if (state == NoAudioLoaded)
    {
        bufferToFill.clearActiveBufferRegion ();
    }
    else if (state == Recording)
    {
        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); channel++)
        {
            preallocatedRecordingBuffer.copyFrom (
                channel,
                numSamplesRecorded,
                *bufferToFill.buffer,
                channel,
                bufferToFill.startSample,
                bufferToFill.numSamples
            );
        }

        numSamplesRecorded += bufferToFill.numSamples;

        buffer->setDataToReferTo (
            preallocatedRecordingBuffer.getArrayOfWritePointers (),
            bufferToFill.buffer->getNumChannels (),
            numSamplesRecorded
        );
    }
    else
    {
        masterSource.getNextAudioBlock (bufferToFill);
    }
}

void my_audio_source::loadAudio (
    AudioSampleBuffer* newAudioSampleBuffer,
    double newSampleRate
)
{
    buffer = newAudioSampleBuffer;
    sampleRate = newSampleRate;
    loadAudioSubregion (0.0, getLengthInSeconds (), false, false);
}

void my_audio_source::unloadAudio ()
{
    changeState (NoAudioLoaded);
}

AudioSampleBuffer* my_audio_source::loadRecordingBuffer ()
{
    // TODO fix hardcoded number of channels
    buffer = new AudioSampleBuffer (2, 0); 
    sampleRate = inputSampleRate;
    numSamplesRecorded = 0;

    preallocatedRecordingBuffer.setSize (
        buffer->getNumChannels(),
        (int)(60 * sampleRate)
    );
    recordingBufferPreallocationThread.reset (
        new BufferPreallocationThread (
            preallocatedRecordingBuffer,
            numSamplesRecorded,
            (int)(10 * sampleRate),
            (int)(30 * sampleRate),
            *buffer
        )
    );
    recordingBufferPreallocationThread->startThread ();

    std::unique_ptr<AudioBufferSource> tempBufferSource (
        new AudioBufferSource (buffer, false)
    );

    masterSource.setSource (
        tempBufferSource.get (),
        0,
        nullptr,
        sampleRate
    );

    bufferSource.swap (tempBufferSource);

    changeState (StartRecording);

    return buffer;
}

void my_audio_source::stopRecording ()
{
    recordingBufferPreallocationThread->stopThread (1000);
    changeState (Stopping);
}

void my_audio_source::playAudio ()
{
    if (state == Playing)
    {
        changeState (Pausing);
    }
    else
    {
        changeState (Starting);
    }
}

void my_audio_source::stopAudio ()
{
    if (hasSubregion && state == Paused)
    {
        if (masterSource.getCurrentPosition () == 0.0)
        {
            loadAudioSubregion (
                0.0,
                getLengthInSeconds (),
                false,
                bufferSource->isLooping ()
            );
            changeState (Stopped);
            masterSource.stop ();
        }
        else
        {
            setPosition (0.0);
        }
    }
    else if (state == Paused || state == NoAudioLoaded)
    {
        changeState (Stopped);
    }
    else
    {
        changeState (Stopping);
    }
}

void my_audio_source::pauseAudio ()
{
    changeState (Pausing);
}

void my_audio_source::muteAudio ()
{
    int startSample = (int)(subregionStartTime * sampleRate),
        numSamples = (int)((subregionEndTime - subregionStartTime) * sampleRate);

    buffer->clear (startSample, numSamples);
}

void my_audio_source::fadeInAudio ()
{
    int startSample = (int)(subregionStartTime * sampleRate),
        numSamples = (int)((subregionEndTime - subregionStartTime) * sampleRate);
    float magnitude = buffer->getMagnitude (startSample, numSamples),
        gain = Decibels::decibelsToGain (magnitude);

    buffer->applyGainRamp (startSample, numSamples, 0.0f, gain);
}

void my_audio_source::fadeOutAudio ()
{
    int startSample = (int)(subregionStartTime * sampleRate),
        numSamples = (int)((subregionEndTime - subregionStartTime) * sampleRate);
    float magnitude = buffer->getMagnitude (startSample, numSamples),
        gain = Decibels::decibelsToGain (magnitude);

    buffer->applyGainRamp (startSample, numSamples, gain, 0.0f);
}

void my_audio_source::normalizeAudio ()
{
    int startSample = (int)(subregionStartTime * sampleRate),
        numSamples = (int)((subregionEndTime - subregionStartTime) * sampleRate);
    float magnitude = buffer->getMagnitude (startSample, numSamples),
        gain = Decibels::decibelsToGain (magnitude);

    buffer->applyGain (startSample, numSamples, gain);
}

my_audio_source::State my_audio_source::getState () const
{
    return state;
}

double my_audio_source::getCurrentPosition () const
{
    double currentPosition = masterSource.getCurrentPosition ();
    if (hasSubregion)
    {
        currentPosition += subregionStartTime;
    }
    return currentPosition;
}

double my_audio_source::getLengthInSeconds () const
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

double my_audio_source::getSampleRate () const
{
    return sampleRate;
}

void my_audio_source::setPosition (double newPosition)
{
    masterSource.setPosition (newPosition);
}

void my_audio_source::setLooping (bool shouldLoop)
{
    bufferSource->setLooping (shouldLoop);
}

void my_audio_source::addListener (Listener * newListener)
{
    listeners.add (newListener);
}

void my_audio_source::removeListener (Listener * listener)
{
    listeners.remove (listener);
}

const CriticalSection* my_audio_source::getBufferUpdateLock () const noexcept
{
    if (recordingBufferPreallocationThread)
    {
        return &recordingBufferPreallocationThread->getLock();
    }
    else
    {
        return nullptr;
    }
}

// ==============================================================================

void my_audio_source::selectedRegionCreated (AudioWaveformComponent* waveform)
{
    loadAudioSubregion (
        waveform->getSelectedRegionStartTime (),
        waveform->getSelectedRegionEndTime (),
        true,
        bufferSource->isLooping ()
    );

    if (state == Playing)
    {
        masterSource.start ();
    }
}

void my_audio_source::selectedRegionCleared (AudioWaveformComponent* waveform)
{
    if (hasSubregion && !waveform->getHasSelectedRegion ())
    {
        loadAudioSubregion (
            0.0,
            getLengthInSeconds (),
            false,
            bufferSource->isLooping ()
        );

        masterSource.start ();
    }
}

void my_audio_source::timerCallback ()
{
    listeners.call ([this](Listener& l) { l.currentPositionChanged (this); });
}

void my_audio_source::changeListenerCallback (
    ChangeBroadcaster*
)
{
    if (masterSource.isPlaying () && state == StartRecording)
    {
        changeState (Recording);
    }
    else if (masterSource.isPlaying () && state != StartRecording)
    {
        changeState (Playing);
    }
    else if (state == Pausing)
    {
        changeState (Paused);
    }
    else if (hasSubregion)
    {
        setPosition (0.0);
        changeState (Paused);
    }
    else
    {
        changeState (Stopped);
    }
}

void my_audio_source::changeState (
    my_audio_source::State newState
)
{
    if (state != newState)
    {
        state = newState;

        switch (state)
        {
        case StartRecording:
            masterSource.start ();
            break;

        case Recording:
            break;

        case Starting:
            masterSource.start ();
            break;

        case Playing:
            break;

        case Stopping:
            masterSource.stop ();
            break;

        case Stopped:
            setPosition (0.0);
            break;

        case Pausing:
            masterSource.stop ();
            break;

        case Paused:
            break;

        case NoAudioLoaded:
            masterSource.setSource (nullptr);
            buffer = nullptr;
            sampleRate = 0.0;
            break;
        }

        onStateChange (newState);

        listeners.call ([this](Listener& l) { l.stateChanged (this); });
    }
}

void my_audio_source::loadAudioSubregion (
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
    int startSample = (int) (subregionStartTime * sampleRate),
        numSamples = (int) (lengthInSeconds * sampleRate);

    subregionBuffer.reset (
        new AudioSampleBuffer (
            buffer->getArrayOfWritePointers(),
            buffer->getNumChannels(),
            startSample,
            numSamples
        )
    );
    
    std::unique_ptr<AudioBufferSource> tempBufferSource (
        new AudioBufferSource (subregionBuffer.get (), shouldLoop)
    );

    masterSource.setSource (
        tempBufferSource.get (),
        0,
        nullptr,
        sampleRate
    );

    bufferSource.swap (tempBufferSource);
}

// ==============================================================================

my_audio_source::BufferPreallocationThread::BufferPreallocationThread (
    AudioSampleBuffer& preallocatedRecordingBuffer,
    int& numSamplesRecorded,
    int numSamplesBuffer,
    int numSamplesToAllocate,
    AudioSampleBuffer& buffer
) :
    Thread ("BufferPreallocationThread"),
    preallocatedRecordingBuffer (preallocatedRecordingBuffer),
    numSamplesRecorded (numSamplesRecorded),
    numSamplesBuffer (numSamplesBuffer),
    numSamplesToAllocate (numSamplesToAllocate),
    buffer(buffer)
{
}

void my_audio_source::BufferPreallocationThread::run ()
{
    while (!threadShouldExit ())
    {
        int preallocatedSamples = preallocatedRecordingBuffer.getNumSamples ();
        if (preallocatedSamples - numSamplesRecorded < numSamplesBuffer)
        {
            int newNumSamples =
                preallocatedRecordingBuffer.getNumSamples () + numSamplesToAllocate;
            const ScopedLock scopedLock (bufferUpdateLock);
            preallocatedRecordingBuffer.setSize (
                preallocatedRecordingBuffer.getNumChannels (),
                newNumSamples, true
            );
            buffer.setDataToReferTo (
                preallocatedRecordingBuffer.getArrayOfWritePointers (),
                preallocatedRecordingBuffer.getNumChannels (),
                numSamplesRecorded
            );
        }
        wait (1000);
    }
}

const CriticalSection& my_audio_source::BufferPreallocationThread::getLock () const noexcept
{
    return bufferUpdateLock;
}
