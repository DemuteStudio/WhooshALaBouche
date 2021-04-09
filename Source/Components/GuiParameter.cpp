#include "GuiParameter.h"


	parameter_gui::parameter_gui(const String id, const String text, util::parameter_type type)
	{
		slider = std::make_unique<Slider>();
		slider->setSliderStyle(Slider::LinearHorizontal);
		slider->setTextBoxIsEditable(false);
		addAndMakeVisible(slider.get());
		slider->setName(id);


		addAndMakeVisible(label);

		label.setText(text, NotificationType::dontSendNotification);

		parameter_type_ = type;
	}

	parameter_gui::parameter_gui(const String id, const String text, util::parameter_type type,
	                                            const Slider::SliderStyle style): parameter_gui(id, text, type)
	{
		slider->setSliderStyle(style);
	}
