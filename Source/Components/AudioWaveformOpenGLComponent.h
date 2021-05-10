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

private:
	struct rectangle
	{
		rectangle(const GLint x, const GLint y, const GLsizei width, const GLsizei height): x(x), y(y), width(width),
			height(height)
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

	void render(OpenGLContext& openGLContext) override;

	void load(AudioSampleBuffer* buffer, const CriticalSection* bufferUpdateLock);

	void display(int64 startSample, int64 numSamples);

	void refresh();
	void calclulate_number_samples_to_skip();

	//===============================================================================================
	void initialize_drawing() const;
	void draw_vertices(OpenGLContext& openGLContext, int channel);
	rectangle get_rectangle(OpenGLContext& openGLContext, int channel) const;
	void draw_channel(OpenGLContext& openGLContext, int channel);

private:
	void calculate_vertices(unsigned int channel);
	int get_samples_interval() const;
	void set_vertice(const float* samples, int64 sample, int64 skipSample, int64 endSample,
	                 int vertice, int numVertices,
	                 int channel);
	GLfloat get_peak_sample_value(
		const float* samples,
		int64 currentStartSample,
		int64 currentNumSamples) const;

private:
	struct Vertex;
	class vertex_buffer;

	std::unique_ptr<OpenGLShaderProgram> shaderProgram;
	std::unique_ptr<OpenGLShaderProgram::Attribute> position;
	std::unique_ptr<OpenGLShaderProgram::Uniform> uniform;
	std::unique_ptr<vertex_buffer> vertexBuffer;

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

	class vertex_buffer
	{
	public:
		vertex_buffer(OpenGLContext& open_gl_context);

		~vertex_buffer();

		void bind(Vertex* vertices, int64 verticesCount) const;

		void unbind();

	private:
		OpenGLContext& openGLContext;
		GLuint id;
	};

	class scoped_nullable_lock
	{
	public:
		inline explicit scoped_nullable_lock(const CriticalSection* lock) noexcept
			: lock_(lock)
		{
			if (lock_ != nullptr)
			{
				lock_->enter();
			}
		}

		inline ~scoped_nullable_lock() noexcept
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
