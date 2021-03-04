#include "EnvelopeOpenGLComponent.h"


EnvelopeOpenGLComponent::EnvelopeOpenGLComponent(): envelope_()
{
	setInterceptsMouseClicks(false, true);
}

EnvelopeOpenGLComponent::~EnvelopeOpenGLComponent()
{
}

void EnvelopeOpenGLComponent::initialise(
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
		shaderProgramPoints.reset(newShaderProgram.release());

		shaderProgramPoints->use();

		Colour waveformColour = getLookAndFeel().findColour(
			ColourIds::waveformColour
		);
		uniformPoints.reset(
			new OpenGLShaderProgram::Uniform(*shaderProgramPoints, "colour")
		);
		uniformPoints->set(waveformColour.getFloatRed(),
		                   0.,
		                   0.,
		                   waveformColour.getFloatAlpha()
		);

		position.reset(
			new OpenGLShaderProgram::Attribute(*shaderProgramPoints, "position")
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

	newShaderProgram.reset(
		new OpenGLShaderProgram(openGLContext)
	);
	if (
		newShaderProgram->addVertexShader(
			OpenGLHelpers::translateVertexShaderToV3(vertexShader)) &&
		newShaderProgram->addFragmentShader(
			OpenGLHelpers::translateFragmentShaderToV3(fragmentShader)) &&
		newShaderProgram->link())
	{
		shaderProgramLines.reset(newShaderProgram.release());

		shaderProgramLines->use();

		Colour waveformColour = getLookAndFeel().findColour(
			ColourIds::waveformColour
		);
		uniformLines.reset(
			new OpenGLShaderProgram::Uniform(*shaderProgramLines, "colour")
		);
		uniformLines->set(waveformColour.getFloatRed(),
		                  waveformColour.getFloatGreen(),
		                  waveformColour.getFloatBlue(),
		                  waveformColour.getFloatAlpha()
		);

		position.reset(
			new OpenGLShaderProgram::Attribute(*shaderProgramLines, "position")
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

void EnvelopeOpenGLComponent::shutdown(OpenGLContext&)
{
	uniformLines.reset();
	uniformPoints.reset();
	position.reset();
	vertexBuffer.reset();
	shaderProgramLines.reset();
	shaderProgramPoints.reset();
}

void EnvelopeOpenGLComponent::render(OpenGLContext& openGLContext)
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
	
	shaderProgramLines->use();
	
	double scale = openGLContext.getRenderingScale();
	Component* parent = getParentComponent();
	Rectangle<int> parentBounds = parent->getBounds(),
	               globalBounds = parent->getLocalArea(this, getLocalBounds());
	GLint x = (GLint)(scale * globalBounds.getX());
	GLsizei width = (GLsizei)(scale * globalBounds.getWidth());
	GLsizei height = (GLsizei)(scale * (globalBounds.getHeight() / bufferNumChannels));
	
	for (int channel = 0; channel < bufferNumChannels; channel++)
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
	
	//*************************************************
	shaderProgramPoints->use();
	
	for (int channel = 0; channel < bufferNumChannels; channel++)
	{
		GLint y = (GLint)(scale * (
			parentBounds.getHeight() - globalBounds.getBottom() +
			channel * (globalBounds.getHeight() / bufferNumChannels)
		));
	
		glViewport(x, y, width, height);
	
		// if (calculateVerticesTrigger)
		// {
		// 	calculateVertices(channel);
		// }
	
		vertexBuffer->bind(vertices[channel].data(), vertices[channel].size());
	
		openGLContext.extensions.glVertexAttribPointer(position->attributeID, 2,
		                                               GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		openGLContext.extensions.glEnableVertexAttribArray(position->attributeID);
	
	
		glDrawArrays(GL_POINTS, 0, (GLsizei)vertices[channel].size());
	
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

void EnvelopeOpenGLComponent::load(envelope* new_envelope, AudioSampleBuffer* newAudioBuffer,
                                   const CriticalSection* bufferUpdateLock)
{
	envelope_ = new_envelope;
	audio_buffer = newAudioBuffer;

	// bufferNumChannels = ->getNumChannels ();
	visibleRegionStartSample = 0;
	// visibleRegionNumSamples = buffer->getNumSamples ();
	bufferUpdateLock_ = bufferUpdateLock;
	bufferNumChannels = 1;
	vertices.clear();
	vertices.reserve(bufferNumChannels);
	for (int i = 0; i < bufferNumChannels; i++)
	{
		vertices.push_back(std::vector<Vertex>());
		vertices.back().reserve(envelope_->get_size());
	}
}

void EnvelopeOpenGLComponent::display(int64 startSample, int64 numSamples)
{
	visibleRegionStartSample = startSample;
	visibleRegionNumSamples = numSamples;
	calculateVerticesTrigger = true;
}

void EnvelopeOpenGLComponent::refresh()
{
	calculateVerticesTrigger = true;
}

// ==============================================================================

void EnvelopeOpenGLComponent::calculateVertices(unsigned int channel)
{
	// More accurate because we depend on the count of the samples 
	// of the current file. The larger the file the less samples 
	// we use when zoomed out

	skipSamples = (unsigned int)(
		visibleRegionNumSamples / (audio_buffer->getNumSamples() * 0.08)
	);
	skipSamples = (skipSamples > 0) ? skipSamples : 1;
	// Alternative approach:
	// skipSamples = numSamples / (sampleRate * 12);
	// More of a constant UI speed but not very accurate

	int64 endSample = visibleRegionStartSample + visibleRegionNumSamples,
	      numVertices = visibleRegionNumSamples % skipSamples
		                    ? visibleRegionNumSamples / skipSamples + 1
		                    : visibleRegionNumSamples / skipSamples;

	envelope my_envelope_ = envelope(envelope_);

	jassert(numVertices <= visibleRegionNumSamples);
	jassert(channel < vertices.size());

	vertices[channel].resize(numVertices);

	// DBG("\nnumVertices " << numVertices <<"   " <<visibleRegionNumSamples << "  visibleRegionNumSamples  " <<
	// 	visibleRegionNumSamples);

	const ScopedNullableLock lock(bufferUpdateLock_);

	// skipSamples = 1;

	for (int64 sample = visibleRegionStartSample, vertice = 0;
	     sample < endSample - skipSamples;
	     sample += skipSamples, vertice++)
	{
		jassert(vertice < my_envelope_.get_size());

		std::vector<envelope::envelope_node> test;
		test.emplace_back(envelope::envelope_node(0, 0.));
		test.emplace_back(envelope::envelope_node(0, 0.));
		test.emplace_back(envelope::envelope_node(0, 0.));
		test.emplace_back(envelope::envelope_node(0, 0.));
		// const GLfloat sampleValue = getPeakSampleValue(test, 0,
		//                                                jmin((int64)2, endSample - sample));
		const GLfloat sampleValue = getPeakSampleValue(my_envelope_.list_, sample,
		                                               jmin((int64)skipSamples, endSample - sample));


		// DBG("SAMPLE: " << envelope_->list_[my_envelope_.get_size()-1].sample);
		// try {
		// DBG("SAMPLE: " << envelope_->list_[0].sample);
		// }
		// catch (int e) {
		// 	DBG(e);
		// }
		Vertex vertex;
		// should be in the [-1,+1] range
		vertex.x = (((GLfloat)envelope_->list_[vertice].sample / (GLfloat)audio_buffer->getNumSamples()) * 2) - 1;
		vertex.y = (sampleValue * 2) - 1;

		jassert(vertice < vertices[channel].size() && vertice >=  0);
		vertices[channel][vertice] = vertex;
	}
}

GLfloat EnvelopeOpenGLComponent::getAverageSampleValue(
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

GLfloat EnvelopeOpenGLComponent::getPeakSampleValue(
	std::vector<envelope::envelope_node> samples,
	int64 currentStartSample,
	int64 currentNumSamples)
{
	// DBG("Start " << currentStartSample << "\nNum Samples " << currentNumSamples << "\nSize " << samples.size() <<
	// 	"\n\n\n");

	jassert((currentStartSample + currentNumSamples) < samples.size());

	GLfloat peakValue = 0.0f;
	int64 endSample = currentStartSample + currentNumSamples;

	for (int64 sample = currentStartSample; sample < endSample; sample++)
	{
		float sampleValue = samples[sample].value;
		if (std::abs(peakValue) < std::abs(sampleValue))
		{
			peakValue = sampleValue;
		}
	}

	return peakValue;
}

// ==============================================================================

EnvelopeOpenGLComponent::VertexBuffer::VertexBuffer(
	OpenGLContext& openGLContext
) :
	openGLContext(openGLContext)
{
	openGLContext.extensions.glGenBuffers(1, &id);
};

EnvelopeOpenGLComponent::VertexBuffer::~VertexBuffer()
{
	openGLContext.extensions.glDeleteBuffers(1, &id);
}

void EnvelopeOpenGLComponent::VertexBuffer::bind(
	Vertex* vertices, int64 verticesCount)
{
	openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, id);
	openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER,
	                                      verticesCount * sizeof(Vertex),
	                                      vertices,
	                                      GL_STATIC_DRAW
	);
}

void EnvelopeOpenGLComponent::VertexBuffer::unbind()
{
	openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
}
