#include "GuiParameter.h"


	ParameterGui::ParameterGui(util::Parameter parameter): parameter_(std::move(parameter)) {
		slider = std::make_unique<Slider>();
		slider->setSliderStyle(Slider::LinearHorizontal);
		slider->setTextBoxIsEditable(false);
		addAndMakeVisible(slider.get());
		slider->setName(parameter.audio_parameter_string.id);


		addAndMakeVisible(label);

		label.setText(parameter.audio_parameter_string.name, NotificationType::dontSendNotification);
	}

ParameterGui::ParameterGui(util::Parameter parameter,
                           const Slider::SliderStyle style): ParameterGui(parameter)
	{
		slider->setSliderStyle(style);
	}

void ParameterGui::set_parameter_default_value() const
{
	auto parameter_default_value = parameter_.ranged_parameter->getDefaultValue();
	slider->setValue(parameter_default_value);
}


