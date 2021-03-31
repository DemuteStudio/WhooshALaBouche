/*
  ==============================================================================

    TwoValueSliderAttachment.h
    Created: 31 Mar 2021 10:53:41am
    Author:  arnau

  ==============================================================================
*/

#pragma once


#include "JuceHeader.h"

using namespace juce;


struct MultiValueAttachedControlBase : public AudioProcessorValueTreeState::Listener,
                                       public AsyncUpdater
{
	MultiValueAttachedControlBase(AudioProcessorValueTreeState& s, Array<String> p);

	void removeListener();

	void setNewUnnormalisedValue(const String& paramID, float newUnnormalisedValue);

	void sendInitialUpdate();

	void parameterChanged(const String& paramID, float newValue) override;

	void beginParameterChange(const String& paramID);

	void endParameterChange(const String& paramID);

	void handleAsyncUpdate() override;

	virtual void setValue(const String&, float) = 0;

	AudioProcessorValueTreeState& state;
	Array<String> paramIDs;
	NamedValueSet lastValues;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiValueAttachedControlBase)
};


//==============================================================================


class TwoValueSliderAttachment
{
public:
	TwoValueSliderAttachment(
		AudioProcessorValueTreeState& stateToControl,
		const String& parameterID1,
		const String& parameterID2,
		Slider& sliderToControl
	);

	~TwoValueSliderAttachment()
	{
	}

private:
	struct Pimpl : private MultiValueAttachedControlBase,
	               private Slider::Listener
	{
		Pimpl(
			AudioProcessorValueTreeState& s,
			const String& p1,
			const String& p2,
			Slider& sl
		)
			:
			MultiValueAttachedControlBase(s, {p1, p2}),
			minParamID(p1),
			maxParamID(p2),
			slider(sl),
			ignoreCallbacks(false)
		{
			jassert(
				sl.getSliderStyle() == Slider::TwoValueHorizontal ||
				sl.getSliderStyle() == Slider::TwoValueVertical
			);

			NormalisableRange<float> range(
				s.getParameterRange(minParamID).start,

				s.getParameterRange(maxParamID).end,
				s.getParameterRange(maxParamID).interval,
				s.getParameterRange(maxParamID).skew
			);

			slider.setRange(range.start, range.end, range.interval);
			slider.setSkewFactor(range.skew, range.symmetricSkew);

			sendInitialUpdate();
			slider.addListener(this);
		}

		~Pimpl()
		{
			slider.removeListener(this);
			removeListener();
		}

		void setValue(const String& paramID, float newValue) override;

		void sliderValueChanged(Slider* s) override;

		void sliderDragStarted(Slider*) override;

		void sliderDragEnded(Slider*) override;


		String minParamID,
		       maxParamID;
		Slider& slider;
		bool ignoreCallbacks;
		CriticalSection selfCallbackMutex;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Pimpl)
	};

	friend struct ContainerDeletePolicy<Pimpl>;
	ScopedPointer<Pimpl> pimpl;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TwoValueSliderAttachment)
};
