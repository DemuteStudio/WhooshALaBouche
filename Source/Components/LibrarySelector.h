/*
  ==============================================================================

    LibrarySelector.h
    Created: 4 May 2021 1:04:26pm
    Author:  arnau

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "FoleyInput.h"
#include "SelectorComponent.h"

//==============================================================================
/*
*/
class LibrarySelector  : public SelectorComponent
{
public:
    LibrarySelector(FoleyInput* foley_input);
    ~LibrarySelector() override;

	std::function<void()> on_library_loaded;
private:
	void element_clicked(SelectorElement*) override ;
	void create_element_components() override;
//==============================================================================
	FoleyInput* foley_input_;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LibrarySelector)
};
