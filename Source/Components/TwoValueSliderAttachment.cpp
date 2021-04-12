#include "TwoValueSliderAttachment.h"

MultiValueAttachedControlBase::MultiValueAttachedControlBase(AudioProcessorValueTreeState& s, Array<String> p):
	state(s),
	paramIDs(p)
{
	for (int i = 0; i < paramIDs.size(); ++i)
	{
		state.addParameterListener(paramIDs[i], this);
		lastValues.set(paramIDs[i], 0);
	}
}

void MultiValueAttachedControlBase::removeListener()
{
	for (int i = 0; i < paramIDs.size(); ++i)
	{
		state.removeParameterListener(paramIDs[i], this);
	}
}

void MultiValueAttachedControlBase::setNewUnnormalisedValue(const String& paramID, float newUnnormalisedValue)
{
	if (AudioProcessorParameter* p = state.getParameter(paramID))
	{
		const float newValue = state
		                       .getParameterRange(paramID)
		                       .convertTo0to1(newUnnormalisedValue);

		if (p->getValue() != newValue)
		{
			p->setValueNotifyingHost(newValue);
		}
	}
}

void MultiValueAttachedControlBase::sendInitialUpdate()
{
	for (int i = 0; i < paramIDs.size(); ++i)
	{
		if (float v = state.getRawParameterValue(paramIDs[i])->load(std::memory_order_relaxed))
		{
			parameterChanged(paramIDs[i], v);
		}
	}
}

void MultiValueAttachedControlBase::parameterChanged(const String& paramID, float newValue)
{
	lastValues.set(paramID, newValue);

	if (MessageManager::getInstance()->isThisTheMessageThread())
	{
		cancelPendingUpdate();
		setValue(paramID, newValue);
	}
	else
	{
		triggerAsyncUpdate();
	}
}

void MultiValueAttachedControlBase::beginParameterChange(const String& paramID)
{
	if (AudioProcessorParameter* p = state.getParameter(paramID))
	{
		p->beginChangeGesture();
	}
}

void MultiValueAttachedControlBase::endParameterChange(const String& paramID)
{
	if (AudioProcessorParameter* p = state.getParameter(paramID))
	{
		p->endChangeGesture();
	}
}

void MultiValueAttachedControlBase::handleAsyncUpdate()
{
for (int i = 0; i < paramIDs.size(); ++i)
	{
		const String paramID = paramIDs[i];
		setValue(paramID, lastValues[paramID]);
	}
}

TwoValueSliderAttachment::TwoValueSliderAttachment(AudioProcessorValueTreeState& stateToControl,
                                                   const String& parameterID1, const String& parameterID2,
                                                   Slider& sliderToControl):
	pimpl(new Pimpl(
		stateToControl,
		parameterID1,
		parameterID2,
		sliderToControl
	))
{
}

void TwoValueSliderAttachment::Pimpl::setValue(const String& paramID, float newValue)
{
	const ScopedLock selfCallbackLock(selfCallbackMutex);

	{
		ScopedValueSetter<bool> svs(ignoreCallbacks, true);

		bool allowNudgingOfOtherValues = true;
		if (paramID == minParamID)
		{
			slider.setMinValue(
				newValue,
				sendNotificationSync,
				allowNudgingOfOtherValues
			);
		}
		else if (paramID == maxParamID)
		{
			slider.setMaxValue(
				newValue,
				sendNotificationSync,
				allowNudgingOfOtherValues
			);
		}
		else
		{
			jassertfalse;
		}
	}
}

void TwoValueSliderAttachment::Pimpl::sliderValueChanged(Slider* s)
{
	const ScopedLock selfCallbackLock(selfCallbackMutex);

	if (
		(!ignoreCallbacks) &&
		(!ModifierKeys::getCurrentModifiers().isRightButtonDown())
	)
	{
		setNewUnnormalisedValue(
			minParamID,
			(float)s->getMinValue()
		);

		setNewUnnormalisedValue(
			maxParamID,
			(float)s->getMaxValue()
		);
	}
}

void TwoValueSliderAttachment::Pimpl::sliderDragStarted(Slider*)
{
	beginParameterChange(minParamID);
	beginParameterChange(maxParamID);
}

void TwoValueSliderAttachment::Pimpl::sliderDragEnded(Slider*)
{
	endParameterChange(minParamID);
	endParameterChange(maxParamID);
}
