#include "GuiParameter.h"


ParameterGui::ParameterGui(util::Parameter parameter):ParameterGui({parameter}, parameter.audio_parameter_string)
{
}

ParameterGui::ParameterGui(const util::Parameter parameter,
                           const Slider::SliderStyle style): ParameterGui(parameter)
{
	slider->setSliderStyle(style);
}

ParameterGui::ParameterGui(std::vector<util::Parameter> _parameters, parameters::AudioParameterString audio_parameter_string,  const Slider::SliderStyle style):
        ParameterGui(_parameters, _parameters.at(0).audio_parameter_string){
    
            slider->setSliderStyle(style);
}
ParameterGui::ParameterGui(std::vector<util::Parameter> in_parameters, parameters::AudioParameterString audio_parameter_string){
    slider = std::make_unique<Slider>();
    slider->setSliderStyle(Slider::LinearHorizontal);
    slider->setTextBoxIsEditable(false);
    addAndMakeVisible(slider.get());
    slider->setName(audio_parameter_string.id);

    addAndMakeVisible(label);

    label.setText(audio_parameter_string.name, NotificationType::dontSendNotification);
    
    for (auto parameter: in_parameters) {
        parameters.emplace_back(std::make_unique<ParameterInterface>(parameter));
    }
}

void ParameterGui::set_parameters_default_value() const
{
    for (auto& parameter : parameters) {
        parameter->set_parameter_default_value(slider.get());
    }
}
