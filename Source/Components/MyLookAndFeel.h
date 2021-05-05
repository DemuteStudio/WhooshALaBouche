#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
using namespace juce;

class MyLookAndFeel : public LookAndFeel_V4
{
public:
	MyLookAndFeel();

	~MyLookAndFeel();


	Colour module_dark_background = Colour(37, 37, 37);
};
