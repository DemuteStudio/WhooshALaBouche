#include "EnvelopeComponent.h"


#include "Envelope.h"
#include "../TenFtUtil.h"


EnvelopeComponent::EnvelopeComponent()
{
	openGLContext.setComponentPaintingEnabled(true);
	openGLContext.setContinuousRepainting(true);
	openGLContext.setRenderer(this);
	openGLContext.attachTo(*this);

	addAndMakeVisible(&envelope_graphic_);

	setInterceptsMouseClicks(true, false);
}

EnvelopeComponent::~EnvelopeComponent()
{
	openGLContext.detach();
	envelope_ = nullptr;
}

void EnvelopeComponent::newOpenGLContextCreated()
{
	envelope_graphic_.initialise(openGLContext);
}

void EnvelopeComponent::openGLContextClosing()
{
	envelope_graphic_.shutdown(openGLContext);
}

void EnvelopeComponent::renderOpenGL()
{
	envelope_graphic_.render(openGLContext);
}

void EnvelopeComponent::paint(Graphics& g)
{
	if (getTotalLength() <= 0)
	{
		paintIfNoFileLoaded(g);
	}
}

void EnvelopeComponent::resized()
{
	envelope_graphic_.setBounds(getLocalBounds());
}

void EnvelopeComponent::mouseWheelMove(
	const MouseEvent& event,
	const MouseWheelDetails& wheelDetails
)
{
	if (getTotalLength() <= 0)
	{
		return;
	}

	juce::Rectangle<float> bounds = getLocalBounds().toFloat();
	float leftRelativeAmmount =
		      (float)(event.getMouseDownX() - bounds.getX())
		      / bounds.getWidth(),
	      rightRelativeAmmount = 1.0f - leftRelativeAmmount;
	double visibleRegionLength = getVisibleRegionLengthInSeconds(),
	       entireRegionLenght = getTotalLength(),
	       visibleRegionToEntireRegionRatio = visibleRegionLength / entireRegionLenght;

	const double scrollAmmount =
		             visibleRegionToEntireRegionRatio * 0.3f * entireRegionLenght,
	             scrollAmmountLeft = scrollAmmount * leftRelativeAmmount,
	             scrollAmmountRight = scrollAmmount * rightRelativeAmmount;

	if (wheelDetails.deltaY < 0)
	{
		// downwards
		updateVisibleRegion(
			visibleRegionStartTime - scrollAmmountLeft,
			visibleRegionEndTime + scrollAmmountRight
		);
	}
	else if (wheelDetails.deltaY > 0)
	{
		// upwards
		updateVisibleRegion(
			visibleRegionStartTime + scrollAmmountLeft,
			visibleRegionEndTime - scrollAmmountRight
		);
	}
}

void EnvelopeComponent::mouseDoubleClick(const MouseEvent& event)
{
	if (getTotalLength() <= 0)
	{
		return;
	}

	double newPosition = util::xToSeconds(
		(float)event.getMouseDownX(),
		visibleRegionStartTime,
		visibleRegionEndTime,
		getLocalBounds().toFloat()
	);

	clearSelectedRegion();

	onPositionChange(newPosition);
}

void EnvelopeComponent::mouseDrag(const MouseEvent& event)
{
	if (getTotalLength() <= 0)
	{
		return;
	}

	juce::Rectangle<float> bounds = getLocalBounds().toFloat();

	if (hasSelectedRegion)
	{
		int mouseDownX = event.getMouseDownX() +
			event.getDistanceFromDragStartX();
		double mouseDownSeconds = util::xToSeconds(
			(float)mouseDownX,
			visibleRegionStartTime,
			visibleRegionEndTime,
			bounds
		);

		updateSelectedRegion(mouseDownSeconds);
	}
	else
	{
		float startOfDragX = util::secondsToX(
			      selectedRegionStartTime,
			      visibleRegionStartTime,
			      visibleRegionEndTime,
			      bounds
		      ),
		      endOfDragX = startOfDragX + event.getDistanceFromDragStartX();
		double newStartTime = selectedRegionStartTime,
		       newEndTime = util::xToSeconds(
			       endOfDragX,
			       visibleRegionStartTime,
			       visibleRegionEndTime,
			       bounds
		       );

		hasSelectedRegion = true;

		// swap the values when we drag left
		if (newStartTime > newEndTime)
		{
			std::swap(newStartTime, newEndTime);
		}

		updateSelectedRegion(newStartTime, newEndTime);
	}

	repaint();
}

void EnvelopeComponent::mouseDown(const MouseEvent& event)
{
	if (getTotalLength() <= 0)
	{
		return;
	}

	int mouseDownX = event.getMouseDownX();
	juce::Rectangle<float> bounds = getLocalBounds().toFloat();

	if (hasSelectedRegion)
	{
		double seconds = util::xToSeconds(
			(float)mouseDownX,
			visibleRegionStartTime,
			visibleRegionEndTime,
			bounds
		);

		updateSelectedRegion(seconds);

		repaint();
	}
	else
	{
		setSelectedRegionStartTime(
			util::xToSeconds(
				(float)mouseDownX,
				visibleRegionStartTime,
				visibleRegionEndTime,
				bounds
			)
		);
	}
}

void EnvelopeComponent::mouseUp(const MouseEvent& event)
{
	if (hasSelectedRegion && event.getNumberOfClicks() == 1)
	{
		listeners.call([this](Listener& l) { l.selectedRegionCreated(this); });
	}
}

void EnvelopeComponent::sliderValueChanged(Slider* slider)
{
	double minValue = slider->getMinValue(),
	       maxValue = slider->getMaxValue();

	if (minValue == maxValue)
	{
		return;
	}

	double leftPositionSeconds = (minValue / 100.0) * getTotalLength(),
	       rightPositionSeconds = (maxValue / 100.0) * getTotalLength();

	updateVisibleRegion(leftPositionSeconds, rightPositionSeconds);
}

void EnvelopeComponent::addListener(Listener* newListener)
{
	listeners.add(newListener);
}

void EnvelopeComponent::removeListener(Listener* listener)
{
	listeners.remove(listener);
}

void EnvelopeComponent::load_envelope(
	envelope* new_envelope,
	AudioSampleBuffer* newAudioBuffer,
	double new_sample_rate,
	const CriticalSection* bufferUpdateLock
)
{
	envelope_ = new_envelope;
	audio_buffer = newAudioBuffer;
	sample_rate_ = new_sample_rate;


	openGLContext.detach();
	envelope_graphic_.load(envelope_, audio_buffer, bufferUpdateLock);
	openGLContext.attachTo(*this);

	// DBG("====  load_envelope");
	updateVisibleRegion(0.0f, getTotalLength());
}

void EnvelopeComponent::clearWaveform()
{
	envelope_ = nullptr;
	sample_rate_ = 0.0;
	clearSelectedRegion();
	updateVisibleRegion(0.0f, 0.0f);
	listeners.call([this](Listener& l) { l.thumbnailCleared(this); });
}

double EnvelopeComponent::getTotalLength()
{
	// DBG("====getTotalLength");
	// if (audio_buffer != nullptr)
	// {
	// 	 DBG(audio_buffer->getNumSamples() / sample_rate_);
	// }
	// if (envelope_ != nullptr)
	// {
	// 	DBG(envelope_->get_size() / sample_rate_);
	// }
	return envelope_ != nullptr ? envelope_->get_size() / sample_rate_ : 0.0;
}

double EnvelopeComponent::get_audio_length() const
{
	return audio_buffer != nullptr ? audio_buffer->getNumSamples() / sample_rate_ : 0.0;
}

void EnvelopeComponent::updateVisibleRegion(
	double newStartTime,
	double newEndTime
)
{
	const double total_length = get_audio_length();
	const double start_time_flattened = util::flattenSeconds(
		newStartTime, total_length);
	const double end_time_flattened = util::flattenSeconds(
		newEndTime, total_length);

	// DBG("===== updateVisibleRegion");
	// DBG(newStartTime);
	// DBG(start_time_flattened);
	// DBG(newEndTime);
	// DBG(end_time_flattened);

	jassert(isVisibleRegionCorrect (start_time_flattened, end_time_flattened));

	if (getSamplesDiff(start_time_flattened, end_time_flattened) < 20)
	{
		return;
	}

	visibleRegionStartTime = start_time_flattened;
	visibleRegionEndTime = end_time_flattened;

	int64 startSample = (int64)(visibleRegionStartTime * sample_rate_),
	      endSample = (int64)get_last_sample(envelope_, newEndTime),
	      numSamples = endSample - startSample;
	envelope_graphic_.display(startSample, numSamples);

	listeners.call([this](Listener& l) { l.visibleRegionChanged(this); });

	repaint();
}

void EnvelopeComponent::updateSelectedRegion(
	double newStartTime, double newEndTime
)
{
	double totalLength = getTotalLength();

	selectedRegionStartTime = util::flattenSeconds(newStartTime, totalLength);
	selectedRegionEndTime = util::flattenSeconds(newEndTime, totalLength);

	listeners.call([this](Listener& l) { l.selectedRegionChanged(this); });
}

void EnvelopeComponent::clearSelectedRegion()
{
	hasSelectedRegion = false;
	selectedRegionStartTime = 0.0;
	selectedRegionEndTime = getTotalLength();

	listeners.call([this](Listener& l) { l.selectedRegionCleared(this); });
	listeners.call([this](Listener& l) { l.selectedRegionChanged(this); });
}

void EnvelopeComponent::refresh()
{
	envelope_graphic_.refresh();
}

double EnvelopeComponent::getVisibleRegionStartTime()
{
	return visibleRegionStartTime;
}

double EnvelopeComponent::getVisibleRegionEndTime()
{
	return visibleRegionEndTime;
}

int EnvelopeComponent::get_last_sample(envelope* _envelope, float end_time)
{
	auto lambda = [this, end_time](envelope::envelope_node node) { return ((node.sample / sample_rate_) <= end_time); };
	auto iterator = std::find_if(envelope_->list_.rbegin(), envelope_->list_.rend(), lambda);
	return (iterator != envelope_->list_.rend())? iterator.base()- envelope_->list_.begin(): 0;
}

double EnvelopeComponent::getSelectedRegionStartTime()
{
	return selectedRegionStartTime;
}

double EnvelopeComponent::getSelectedRegionEndTime()
{
	return selectedRegionEndTime;
}

bool EnvelopeComponent::getHasSelectedRegion()
{
	return hasSelectedRegion;
}

// ==============================================================================
void EnvelopeComponent::paintIfNoFileLoaded(Graphics& g)
{
	juce::Rectangle<float> thumbnailBounds = getLocalBounds().toFloat();
	g.setColour(findColour(
			EnvelopeOpenGLComponent::ColourIds::waveformBackgroundColour)
	);
	g.fillRect(thumbnailBounds);
	g.setColour(findColour(
			EnvelopeOpenGLComponent::ColourIds::waveformColour)
	);
	g.drawFittedText(
		"No File Loaded",
		thumbnailBounds.toNearestInt(),
		Justification::centred,
		1
	);
}

void EnvelopeComponent::setSelectedRegionStartTime(double newStartTime)
{
	updateSelectedRegion(newStartTime, selectedRegionEndTime);
}

void EnvelopeComponent::setSelectedRegionEndTime(double newEndTime)
{
	updateSelectedRegion(selectedRegionStartTime, newEndTime);
}

double EnvelopeComponent::getVisibleRegionLengthInSeconds()
{
	return visibleRegionEndTime - visibleRegionStartTime;
}

void EnvelopeComponent::updateSelectedRegion(double mouseDownSeconds)
{
	double distanceFromStartOfDragSeconds =
		       std::abs(mouseDownSeconds - selectedRegionStartTime),
	       distanceFromEndOfDragSeconds =
		       std::abs(mouseDownSeconds - selectedRegionEndTime);

	if (distanceFromStartOfDragSeconds < distanceFromEndOfDragSeconds)
	{
		setSelectedRegionStartTime(mouseDownSeconds);
	}
	else
	{
		setSelectedRegionEndTime(mouseDownSeconds);
	}
}

bool EnvelopeComponent::isVisibleRegionCorrect(
	double startTime, double endTime
)
{
	bool isAudioLoaded = getTotalLength() > 0.0;
	return
		(!isAudioLoaded &&
			startTime == 0.0f && endTime == 0.0f) ||
		(isAudioLoaded &&
			startTime < endTime &&
			startTime >= 0 &&
			endTime <= get_audio_length());
}

unsigned int EnvelopeComponent::getSamplesDiff(double startTime, double endTime)
{
	int64 startSample = (int64)(startTime * sample_rate_),
	      endSample = (int64)(endTime * sample_rate_);
	return (unsigned int)(endSample - startSample);
}
