#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "../OpenGLComponent.h"
#include "Envelope.h"

using namespace juce;

class EnvelopeOpenGLComponent : public OpenGLComponent
{
public:
	enum ColourIds
	{
		waveformColour = 0,
		waveformBackgroundColour = 1
	};

public:
	EnvelopeOpenGLComponent();

	~EnvelopeOpenGLComponent();

	void initialise(OpenGLContext& openGLContext) override;

	void shutdown(OpenGLContext& openGLContext) override;

	void render(OpenGLContext& openGLContext) override;

	void load(envelope* buffer, AudioSampleBuffer* newAudioBuffer, const CriticalSection* bufferUpdateLock);

	void display(int64 startSample, int64 numSamples);

	void refresh();

private:
	void calculateVertices(unsigned int channel);

	GLfloat getAverageSampleValue(
		const float* samples, int64 startSample, int64 numSamples
	);

	GLfloat getPeakSampleValue(
		std::vector<envelope::node> samples,
		int64 startSample, int64 numSamples
	);

private:
	struct Vertex;
	class VertexBuffer;

private:
	std::unique_ptr<OpenGLShaderProgram> shaderProgramLines;
	std::unique_ptr<OpenGLShaderProgram> shaderProgramPoints;
	std::unique_ptr<OpenGLShaderProgram::Attribute> position;
	std::unique_ptr<OpenGLShaderProgram::Uniform> uniformLines;
	std::unique_ptr<OpenGLShaderProgram::Uniform> uniformPoints;
	std::unique_ptr<VertexBuffer> vertexBuffer;

	envelope* envelope_ = nullptr;
	int bufferNumChannels = 0;
	int64 visibleRegionStartSample = 0;
	int64 visibleRegionNumSamples = 0;
	unsigned int skipSamples = 8;
	AudioSampleBuffer* audio_buffer;

	bool calculateVerticesTrigger = false;

	std::vector<std::vector<Vertex>> vertices;

	const CriticalSection* bufferUpdateLock_ = nullptr;

private:
	struct Vertex
	{
		GLfloat x, y;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer(OpenGLContext& openGLContext);

		~VertexBuffer();

		void bind(Vertex* vertices, int64 verticesCount);

		void unbind();

	private:
		OpenGLContext& openGLContext;
		GLuint id;
	};

	class ScopedNullableLock
	{
	public:
		inline explicit ScopedNullableLock(const CriticalSection* lock) noexcept
			: lock_(lock)
		{
			if (lock_ != nullptr)
			{
				lock_->enter();
			}
		}

		inline ~ScopedNullableLock() noexcept
		{
			if (lock_ != nullptr)
			{
				lock_->exit();
			}
		}

	private:
		const CriticalSection* lock_ = nullptr;
	};

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnvelopeOpenGLComponent)
};
