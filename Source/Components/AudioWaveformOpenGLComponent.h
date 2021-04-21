#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "OpenGLComponent.h"

using namespace juce;

class AudioWaveformOpenGLComponent : public OpenGLComponent
{
public:
	enum ColourIds
	{
		waveformColour = 0,
		waveformBackgroundColour = 1
	};

	struct rectangle
	{
		rectangle(const GLint x, const GLint y, const GLsizei width, const GLsizei height): x(x), y(y), width(width), height(height)
		{
		}

		const GLint x;
		const GLint y;

		const GLsizei width;
		const GLsizei height;
	};

public:
	AudioWaveformOpenGLComponent();

	~AudioWaveformOpenGLComponent();

	void initialise(OpenGLContext& openGLContext) override;

	void shutdown(OpenGLContext& openGLContext) override;
	void initialize_drawing() const;
	void draw_vertices(OpenGLContext& openGLContext, int channel);
	rectangle get_rectangle(OpenGLContext& openGLContext, int channel) const;
	void draw_channel(OpenGLContext& openGLContext, int channel);

	void render(OpenGLContext& openGLContext) override;

	void load(AudioSampleBuffer* buffer, const CriticalSection* bufferUpdateLock);

	void display(int64 startSample, int64 numSamples);

	void refresh();

private:
	void calculateVertices(unsigned int channel);
	void set_vertice(const float* samples, int64 sample, int64 skipSample, int64 endSample, int vertice,
	                 int numVertices,
	                 int channel);


	GLfloat get_peak_sample_value(
		const float* samples, int64 startSample, int64 numSamples
	) const;

private:
	struct Vertex;
	class VertexBuffer;

private:
	std::unique_ptr<OpenGLShaderProgram> shaderProgram;
	std::unique_ptr<OpenGLShaderProgram::Attribute> position;
	std::unique_ptr<OpenGLShaderProgram::Uniform> uniform;
	std::unique_ptr<VertexBuffer> vertexBuffer;

	AudioSampleBuffer* buffer = nullptr;
	int bufferNumChannels = 0;
	int64 visibleRegionEndSample = 0;
	int64 visibleRegionNumSamples = 0;
	unsigned int skipSamples = 8;

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
		VertexBuffer(OpenGLContext& open_gl_context);

		~VertexBuffer();

		void bind(Vertex* vertices, int64 verticesCount) const;

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

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioWaveformOpenGLComponent)
};
