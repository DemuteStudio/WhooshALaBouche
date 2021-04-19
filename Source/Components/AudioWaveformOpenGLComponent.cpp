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
	String vertexShader =
		"attribute vec2 position;\n"
		"\n"
		"void main()\n"
		"{\n"
		"    gl_Position = vec4(position.x, position.y, 1.0, 1.0);\n"
		"};";
	String fragmentShader =
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

		Colour waveformColour = getLookAndFeel().findColour(
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);

	shaderProgram->use();

	double scale = openGLContext.getRenderingScale();
	Component* parent = getParentComponent();
	Rectangle<int> parentBounds = parent->getBounds(),
	               globalBounds = parent->getLocalArea(this, getLocalBounds());
	GLint x = (GLint)(scale * globalBounds.getX());
	GLsizei width = (GLsizei)(scale * globalBounds.getWidth());
	GLsizei height = (GLsizei)(scale * (globalBounds.getHeight() / bufferNumChannels));

	for (int channel = 0; channel < 1; channel++)
	{
		GLint y = (GLint)(scale * (
			parentBounds.getHeight() - globalBounds.getBottom() +
			channel * (globalBounds.getHeight() / bufferNumChannels)
		));

		glViewport(x, y, width, height);

		if (calculateVerticesTrigger)
		{
			calculateVertices(channel);
		}

		vertexBuffer->bind(vertices[channel].data(), vertices[channel].size());

		openGLContext.extensions.glVertexAttribPointer(position->attributeID, 2,
		                                               GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		openGLContext.extensions.glEnableVertexAttribArray(position->attributeID);

		glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)vertices[channel].size());

		openGLContext.extensions.glDisableVertexAttribArray(position->attributeID);

		vertexBuffer->unbind();
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
		vertices.push_back(std::vector<Vertex>());
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
	// Alternative approach:
	// skipSamples = numSamples / (sampleRate * 12);
	// More of a constant UI speed but not very accurate

	int64 numVertices = visibleRegionNumSamples % skipSamples
		                    ? visibleRegionNumSamples / skipSamples + 1
		                    : visibleRegionNumSamples / skipSamples;

	vertices[channel].resize(numVertices + 1);

	const ScopedNullableLock lock(bufferUpdateLock_);


	const float* samples = buffer->getReadPointer(channel);
	int vertice = 0;

	if (visibleRegionEndSample - visibleRegionNumSamples < 0)
	{
		int64 number_of_samples = visibleRegionNumSamples - visibleRegionEndSample;
		int64 startSample = buffer->getNumSamples() - number_of_samples;
		int64 endSample = buffer->getNumSamples();

		for (int64 sample = startSample;
		     sample < endSample;
		     sample += skipSamples, vertice++)
		{
			set_vertice(samples, sample, skipSamples, endSample, vertice, numVertices, channel);
		}


		startSample = 0;
		endSample = visibleRegionEndSample;

		for (int64 sample = startSample;
		     sample < endSample;
		     sample += skipSamples, vertice++)
		{
			set_vertice(samples, sample, skipSamples, endSample, vertice, numVertices, channel);
		}
	}
	else
	{
		int64 startSample = visibleRegionEndSample - visibleRegionNumSamples;
		int64 endSample = visibleRegionEndSample;

		for (int64 sample = startSample, vertice = 0;
		     sample < endSample;
		     sample += skipSamples, vertice++)
		{
			set_vertice(samples, sample, skipSamples, endSample, vertice, numVertices, channel);
		}
	}
}

void AudioWaveformOpenGLComponent::set_vertice(const float* samples, int64 sample, int64 skipSample, int64 endSample,
                                               int vertice, int numVertices,
                                               int channel)
{
	int64 next_sample_interval = jmin((int64)skipSamples, endSample - sample);
	GLfloat sampleValue = getPeakSampleValue(samples, sample, next_sample_interval);

	Vertex vertex;
	// should be in the [-1,+1] range
	vertex.x = (((GLfloat)vertice / (GLfloat)numVertices) * 2) - 1;
	vertex.y = sampleValue;

	vertices[channel][vertice] = vertex;
}

GLfloat AudioWaveformOpenGLComponent::getAverageSampleValue(
	const float* samples,
	int64 currentStartSample,
	int64 currentNumSamples)
{
	GLfloat samplesSum = 0;
	unsigned int samplesCount = 0;
	int64 endSample = currentStartSample + currentNumSamples;

	for (int64 sample = currentStartSample; sample < endSample; sample++)
	{
		samplesSum += samples[sample];
		samplesCount++;
	}

	return samplesSum / samplesCount;
}

GLfloat AudioWaveformOpenGLComponent::getPeakSampleValue(
	const float* samples,
	int64 currentStartSample,
	int64 currentNumSamples)
{
	GLfloat peakValue = 0.0f;
	int64 endSample = currentStartSample + currentNumSamples;

	for (int64 sample = currentStartSample; sample < endSample; sample++)
	{
		float sampleValue = samples[sample];
		if (std::abs(peakValue) < std::abs(sampleValue))
		{
			peakValue = sampleValue;
		}
	}

	return peakValue;
}

// ==============================================================================

AudioWaveformOpenGLComponent::VertexBuffer::VertexBuffer(
	OpenGLContext& openGLContext
) :
	openGLContext(openGLContext)
{
	openGLContext.extensions.glGenBuffers(1, &id);
};

AudioWaveformOpenGLComponent::VertexBuffer::~VertexBuffer()
{
	openGLContext.extensions.glDeleteBuffers(1, &id);
}

void AudioWaveformOpenGLComponent::VertexBuffer::bind(
	Vertex* vertices, int64 verticesCount)
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
