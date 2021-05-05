#pragma once

#include <JuceHeader.h>

using namespace juce;

class SelectorElement  : public juce::TextButton
{
public:
    SelectorElement(String name);
    ~SelectorElement() override;

    void paint (juce::Graphics&) override;


    void unselect();
    void select();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SelectorElement)
};
