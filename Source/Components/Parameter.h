#pragma once
#include <string>

#include "GuiParameter.h"

class parameter
{
public:
	parameter(std::string osc_label_ );
	~parameter() = default;
	//=========================================================================================================
	virtual void send_value_by_osc();

private:
	std::string osc_label;
	parameter_gui* gui_component;
};
