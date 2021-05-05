#include "LibrarySelector.h"

//==============================================================================
LibrarySelector::LibrarySelector(FoleyInput* foley_input): foley_input_(foley_input)
{
	LibrarySelector::create_element_components();
}

LibrarySelector::~LibrarySelector()
{
}

void LibrarySelector::element_clicked(SelectorElement* selector_element_clicked)
{
	foley_input_->set_selected_library(selector_element_clicked->getName());
	on_library_loaded();
}

void LibrarySelector::create_element_components()
{
	for (const auto& library: foley_input_->get_libraries_paths())
	{
		element_components_.emplace_back(std::make_unique<SelectorElement>(library.getFileName()));
	}
	for (auto& component : element_components_)
	{
		addAndMakeVisible(component.get());
		component->onClick = [this, &component]()
		{
			element_clicked(component.get());
		};
	}
}
