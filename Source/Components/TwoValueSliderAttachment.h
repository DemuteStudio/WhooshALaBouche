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


			auto range = s.getParameterRange(minParamID);

			auto convertFrom0To1Function = [range](double currentRangeStart,
			                                       double currentRangeEnd,
			                                       double normalisedValue) mutable
			{
				range.start = (float)currentRangeStart;
				range.end = (float)currentRangeEnd;
				return (double)range.convertFrom0to1((float)normalisedValue);
			};

			auto convertTo0To1Function = [range](double currentRangeStart,
			                                     double currentRangeEnd,
			                                     double mappedValue) mutable
			{
				range.start = (float)currentRangeStart;
				range.end = (float)currentRangeEnd;
				return (double)range.convertTo0to1((float)mappedValue);
			};

			auto snapToLegalValueFunction = [range](double currentRangeStart,
			                                        double currentRangeEnd,
			                                        double mappedValue) mutable
			{
				range.start = (float)currentRangeStart;
				range.end = (float)currentRangeEnd;
				return (double)range.snapToLegalValue((float)mappedValue);
			};

			NormalisableRange<double> newRange{
				(double)range.start,
				(double)range.end,
				std::move(convertFrom0To1Function),
				std::move(convertTo0To1Function),
				std::move(snapToLegalValueFunction)
			};
			newRange.interval = range.interval;
			newRange.skew = range.skew;
			newRange.symmetricSkew = range.symmetricSkew;

			slider.setNormalisableRange(newRange);

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
