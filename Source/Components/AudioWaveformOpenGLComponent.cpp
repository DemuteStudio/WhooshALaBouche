#include "AudioWaveformOpenGLComponent.h"


AudioWaveformOpenGLComponent::AudioWaveformOpenGLComponent()
{
	setInterceptsMouseClicks(false, true);
}

AudioWaveformOpenGLComponent::~AudioWaveformOpenGLComponent()
{
}

void AudioWaveformOpenGLComponent::initialise(
	OpenGLContext& openGLContext)
{
	const String vertexShader =
		"attribute vec2 position;\n"
		"\n"
		"void main()\n"
		"{\n"
		"    gl_Position = vec4(position.x, position.y, 1.0, 1.0);\n"
		"};";
	const String fragmentShader =
		"uniform vec4 colour;\n"
		"\n"
		"void main()\n"
		"{\n"
		"    gl_FragColor = colour;\n"
		"}";

	std::unique_ptr<OpenGLShaderProgram> newShaderProgram(
		new OpenGLShaderProgram(openGLContext)
	);
	String statusText;

	if (
		newShaderProgram->addVertexShader(
			OpenGLHelpers::translateVertexShaderToV3(vertexShader)) &&
		newShaderProgram->addFragmentShader(
			OpenGLHelpers::translateFragmentShaderToV3(fragmentShader)) &&
		newShaderProgram->link())
	{
		shaderProgram.reset(newShaderProgram.release());

		shaderProgram->use();

		const Colour waveformColour = getLookAndFeel().findColour(
			ColourIds::waveformColour
		);
		uniform.reset(
			new OpenGLShaderProgram::Uniform(*shaderProgram, "colour")
		);
		uniform->set(waveformColour.getFloatRed(),
		             waveformColour.getFloatGreen(),
		             waveformColour.getFloatBlue(),
		             waveformColour.getFloatAlpha()
		);

		position.reset(
			new OpenGLShaderProgram::Attribute(*shaderProgram, "position")
		);

		vertexBuffer.reset(new VertexBuffer(openGLContext));

		statusText = "GLSL: v" +
			String(OpenGLShaderProgram::getLanguageVersion(), 2);
	}
	else
	{
		statusText = newShaderProgram->getLastError();
		Logger::outputDebugString(statusText);
	}
}

void AudioWaveformOpenGLComponent::shutdown(OpenGLContext&)
{
	uniform.reset();
	position.reset();
	vertexBuffer.reset();
	shaderProgram.reset();
}

void AudioWaveformOpenGLComponent::initialize_drawing() const
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);


	shaderProgram->use();
}

void AudioWaveformOpenGLComponent::draw_vertices(OpenGLContext& openGLContext, int channel)
{
	vertexBuffer->bind(vertices[channel].data(), vertices[channel].size());

	openGLContext.extensions.glVertexAttribPointer(position->attributeID, 2,
	                                               GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
	openGLContext.extensions.glEnableVertexAttribArray(position->attributeID);

	glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)vertices[channel].size());

	openGLContext.extensions.glDisableVertexAttribArray(position->attributeID);

	vertexBuffer->unbind();
}

AudioWaveformOpenGLComponent::rectangle AudioWaveformOpenGLComponent::get_rectangle(
	OpenGLContext& openGLContext, const int channel) const
{
	const auto scale = openGLContext.getRenderingScale();

	Component* parent = getParentComponent();
	const auto parent_bounds = parent->getBounds();

	const auto global_bounds = parent->getLocalArea(this, getLocalBounds());

	const auto x = static_cast<GLint>(scale * global_bounds.getX());
	const auto y = static_cast<GLint>(scale * (parent_bounds.getHeight() - global_bounds.getBottom() + channel * (
		global_bounds.getHeight() / bufferNumChannels)));

	const GLint width = static_cast<GLsizei>(scale * global_bounds.getWidth());
	const GLint height = static_cast<GLsizei>(scale * (global_bounds.getHeight() / bufferNumChannels));

	return rectangle(x, y, width, height);
}

void AudioWaveformOpenGLComponent::draw_channel(OpenGLContext& openGLContext, int channel)
{
	const auto rectangle = get_rectangle(openGLContext, channel);

	glViewport(rectangle.x, rectangle.y, rectangle.width, rectangle.height);

	if (calculateVerticesTrigger)
	{
		calculateVertices(channel);
	}

	draw_vertices(openGLContext, channel);
}

void AudioWaveformOpenGLComponent::render(OpenGLContext& openGLContext)
{
	jassert(OpenGLHelpers::isContextActive ());

	if (vertices.empty())
	{
		return;
	}

	OpenGLHelpers::clear(
		getLookAndFeel().findColour(ColourIds::waveformBackgroundColour)
	);

	initialize_drawing();

	for (int channel = 0; channel < 1; channel++)
	{
		draw_channel(openGLContext, channel);
	}

	if (calculateVerticesTrigger)
	{
		calculateVerticesTrigger = false;
	}

	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_BLEND);
}

void AudioWaveformOpenGLComponent::load(AudioSampleBuffer* newBuffer,
                                        const CriticalSection* bufferUpdateLock)
{
	buffer = newBuffer;
	bufferNumChannels = buffer->getNumChannels();
	visibleRegionEndSample = 0;
	visibleRegionNumSamples = buffer->getNumSamples();
	bufferUpdateLock_ = bufferUpdateLock;

	vertices.clear();
	vertices.reserve(bufferNumChannels);
	for (int i = 0; i < bufferNumChannels; i++)
	{
		vertices.emplace_back(std::vector<Vertex>());
		vertices.back().reserve(buffer->getNumSamples());
	}
}

void AudioWaveformOpenGLComponent::display(int64 end_sample, int64 numSamples)
{
	visibleRegionEndSample = end_sample;
	visibleRegionNumSamples = numSamples;
	calculateVerticesTrigger = true;
}

void AudioWaveformOpenGLComponent::refresh()
{
	calculateVerticesTrigger = true;
}

// ==============================================================================

void AudioWaveformOpenGLComponent::calculateVertices(unsigned int channel)
{
	// More accurate because we depend on the count of the samples 
	// of the current file. The larger the file the less samples 
	// we use when zoomed out
	skipSamples = (unsigned int)(
		visibleRegionNumSamples / (buffer->getNumSamples() * 0.04)
	);
	skipSamples = (skipSamples > 0) ? skipSamples : 1;

	const int64 number_of_vertices= visibleRegionNumSamples % skipSamples
		                          ? visibleRegionNumSamples / skipSamples + 1
		                          : visibleRegionNumSamples / skipSamples;

	vertices[channel].resize(number_of_vertices + 1);

	const ScopedNullableLock lock(bufferUpdateLock_);


	const float* samples = buffer->getReadPointer(channel);
	int vertice = 0;

	if (visibleRegionEndSample - visibleRegionNumSamples < 0)
	{
		const int64 number_of_samples = visibleRegionNumSamples - visibleRegionEndSample;
		int64 start_sample = buffer->getNumSamples() - number_of_samples;
		int64 end_sample = buffer->getNumSamples();

		for (int64 sample = start_sample;
		     sample < end_sample;
		     sample += skipSamples, vertice++)
		{
			set_vertice(samples, sample, skipSamples, end_sample, vertice, number_of_vertices, channel);
		}

		start_sample = 0;
		end_sample = visibleRegionEndSample;

		for (int64 sample = start_sample;
		     sample < end_sample;
		     sample += skipSamples, vertice++)
		{
			set_vertice(samples, sample, skipSamples, end_sample, vertice, number_of_vertices, channel);
		}
	}
	else
	{
		const int64 startSample = visibleRegionEndSample - visibleRegionNumSamples;
		const int64 endSample = visibleRegionEndSample;

		for (int64 sample = startSample, vertice = 0;
		     sample < endSample;
		     sample += skipSamples, vertice++)
		{
			set_vertice(samples, sample, skipSamples, endSample, vertice, number_of_vertices, channel);
		}
	}
}

void AudioWaveformOpenGLComponent::set_vertice(const float* samples, int64 sample, int64 skipSample, int64 endSample,
                                               int vertice, int numVertices,
                                               int channel)
{
	const int64 next_sample_interval = jmin((int64)skipSamples, endSample - sample);
	const GLfloat sampleValue = get_peak_sample_value(samples, sample, next_sample_interval);

	Vertex vertex;
	// should be in the [-1,+1] range
	vertex.x = ((static_cast<GLfloat>(vertice) / static_cast<GLfloat>(numVertices)) * 2) - 1;
	vertex.y = sampleValue;

	vertices[channel][vertice] = vertex;
}

GLfloat getAverageSampleValue(
	const float* samples,
	int64 currentStartSample,
	int64 currentNumSamples)
{
	GLfloat samplesSum = 0;
	unsigned int samplesCount = 0;
	const int64 end_sample = currentStartSample + currentNumSamples;

	for (int64 sample = currentStartSample; sample < end_sample; sample++)
	{
		samplesSum += samples[sample];
		samplesCount++;
	}

	return samplesSum / samplesCount;
}

GLfloat AudioWaveformOpenGLComponent::get_peak_sample_value(
	const float* samples,
	int64 currentStartSample,
	int64 currentNumSamples) const 
{
	GLfloat peak_value = 0.0f;
	const int64 end_sample = currentStartSample + currentNumSamples;

	for (int64 sample = currentStartSample; sample < end_sample; sample++)
	{
		const float sample_value = samples[sample];
		if (std::abs(peak_value) < std::abs(sample_value))
		{
			peak_value = sample_value;
		}
	}

	return peak_value;
}

// ==============================================================================

AudioWaveformOpenGLComponent::VertexBuffer::VertexBuffer(
	OpenGLContext& open_gl_context
) :
	openGLContext(open_gl_context)
{
	open_gl_context.extensions.glGenBuffers(1, &id);
};

AudioWaveformOpenGLComponent::VertexBuffer::~VertexBuffer()
{
	openGLContext.extensions.glDeleteBuffers(1, &id);
}

void AudioWaveformOpenGLComponent::VertexBuffer::bind(
	Vertex* vertices, int64 verticesCount) const
{
	openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, id);
	openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER,
	                                      verticesCount * sizeof(Vertex),
	                                      vertices,
	                                      GL_STATIC_DRAW
	);
}

void AudioWaveformOpenGLComponent::VertexBuffer::unbind()
{
	openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
}
